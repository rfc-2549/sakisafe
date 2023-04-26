#!/usr/bin/perl

# This file is part of sakisafe.

use if $^O eq "openbsd", OpenBSD::Pledge, qw(pledge);
use Mojolicious::Lite -signatures;
use Mojolicious::Routes::Pattern;
use Mojo::Log;
use Time::HiRes;
use List::MoreUtils qw(uniq);
use Term::ANSIColor;
use English;
use MIME::Types;
use Path::Tiny;
use warnings;
use experimental 'signatures';
use feature 'say';
plugin 'RenderFile';

# OpenBSD promises.
pledge("stdio cpath rpath wpath inet flock fattr") if $^O eq "openbsd";

# 100 MBs
my $MAX_SIZE = 1024 * 1024 * 100;

my @BANNED = eval { path('banned.txt')->slurp_utf8 } || qw();    # Add banned IP addresses here
my $dirname;
my $link;

mkdir "f";

# Mojo logger
my $log = Mojo::Log->new(path => 'sakisafe.log', level => 'trace');
$log->color(1);

# Forward logs to STDERR or STDOUT while also writing to the `sakisafe.log` file.
$log->on(
     message => sub ($l, $level, @lines) {
          my $time = time;
          my ($s, $m, $h, $day, $month, $year) = localtime time;
          $time = sprintf(
               '%04d-%02d-%02d %02d:%02d:%08.5f',
               $year + 1900,
               $month + 1, $day, $h, $m, "$s." . ((split '.', $time)[1] // 0)
          );
          my $log_to_print = '[' . $time . '] ' . '[' . $level . '] ' . join(' ', @lines);
          if ($level eq 'trace' || $level eq 'info') {
               say $log_to_print;
          }
          else {
               print \*STDERR, $log_to_print . "\n";
          }
     }
);

# Function to handle file uploads
sub handle_file {
     my $c        = shift;
     my $filedata = $c->param("file");
     if ($filedata->size > $MAX_SIZE) {
          return $c->render(text => "Max upload size: $MAX_SIZE", status => 400);
     }

     if (List::MoreUtils::any { $c->tx->remote_address } uniq @BANNED) {
          $log->info("Attempt to upload by banned IP: " . $c->tx->remote_address);
          $c->render(
               text => "Hi! Seems like the server admin added your IP address to the banned IP array."
                 . "As the developer of sakisafe, I can't do anything.",
               status => 403
          );
          return;
     }

     # Generate random string for the directory
     my @chars = ('0' .. '9', 'a' .. 'Z');
     $dirname .= $chars[rand @chars] for 1 .. 5;
     my $filename = $filedata->filename;
     $log->warn("sakisafe warning: could not create directory: $ERRNO") unless mkdir("f/" . $dirname);
     $filename .= ".txt" if $filename eq "-";

     # TODO: get whether the server is http or https
     # There's a CGI ENV variable for that.
     my $host = $c->req->url->to_abs->host;
     my $ua   = $c->req->headers->user_agent;
     $filedata->move_to("f/" . $dirname . "/" . $filename);
     $link = "http://$host/f/$dirname/$filename";
     $c->stash(link => $link, host => $host, dirname => $dirname);

     $c->res->headers->header('Location' => "$link" . $filename);

     # Only give the link to curl, html template for others.
     if ($ua =~ m/curl/) {
          $c->render(text => $link . "\n", status => 201,);

          $dirname = "";
     }
     else {
          $c->render(template => 'file', status => 201,);
     }

     $log->info($c->tx->remote_address . " " . $dirname . "/" . $filename);
     $dirname = "";
}

# Function to log uploaded files

get '/' => 'index';
post '/' => sub ($c) { handle_file($c) };

# Allow files to be downloaded.

get '/f/:dir/#name' => sub ($c) {
     my $dir  = $c->param("dir");
     my $file = $c->param("name");
     my $ext  = $file;
     $ext =~ s/.*\.//;
     my $path = "f/" . $dir . "/" . $file;

     #carp "sakisafe warning: could not get file: $ERRNO" unless
     $c->render(text => "file not found", status => 404) unless -e $path;
     $c->render_file(filepath => $path, format => $ext, content_disposition => 'inline');
};

app->log($log);
app->max_request_size(1024 * 1024 * 100);

post '/upload' => sub ($c) { handle_file($c) };

app->start;

# Index template

# By default Mojolicious gets the "directory root" from the "public"
# directory, so the css and the favicon from the "public" directory,
# in the root of this repo.

# Not sure why I have to do this filthy hack, could not get Mojolicious
# to get the template here. So a TODO is to fix this.

__DATA__
@@ file.html.ep
  <!DOCTYPE html>
  <html lang="en">
  <head>
  <title>sakisafe</title>
  <link rel="stylesheet" type="text/css" href="index.css"/>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  </head>
  <body>
  <center>
  <h1>sakisafe</h1>
  <h2>shitless file upload, pastebin and url shorter</h2>
  <img src="saki.png"/>
  <h2>LINK</h2>
  <code><%= $link %></code>
  </center>
  <p>Running sakisafe 2.4.0</p>
  </body>
  </html>

@@ index.html.ep
  <!DOCTYPE html>
  <html lang="en">
  <head>
  <title>sakisafe</title>
  <link rel="stylesheet" type="text/css" href="index.css"/>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  </head>
  <body>
  <center>
  <h1>sakisafe</h1>
  <h2>shitless file upload, pastebin and url shorter</h2>
  <img src="saki.png"/>
  <h2>USAGE</h2>
  <p>POST a file:</p>
  <code>curl -F 'file=@yourfile.png' https://<%= $c->req->url->to_abs->host; %></code>
  <p>Post your text directly</p>
  <code>curl -F 'file=@-' https://<%= $c->req->url->to_abs->host; %></code>
  </center>
  <p>Running sakisafe 2.4.0</p>
  <div class="left">
  <h2>Or just upload a file here</h2>
  <form ENCTYPE='multipart/form-data' method='post' action='/upload'>
  <input type='file' name='file' size='30' required/>
  <input type='submit' value='upload'/>
  </form>
  </div>
  </body>
  </html>
__END__

=pod

=head1 sakisafe

sakisafe is a web application using the Mojolicious framework which
allow users to simply upload and share files.

=head2 synopsis

C<./sakisafe.pl daemon -m production>

This will start sakisafe in port 3000. Which should be proxied with
nginx or any reverse proxy software.

=head2 license

The Unlicense.

=head2 author

Raoul Vaughn

=cut
