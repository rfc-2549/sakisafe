#!/usr/bin/perl
# This file is part of sakisafe.

use if $^O eq "openbsd", OpenBSD::Pledge, qw(pledge);
use Mojolicious::Lite -signatures;
use Mojolicious::Routes::Pattern;
use List::MoreUtils qw(uniq);
use Carp;
use Term::ANSIColor;
use English;
use MIME::Types;
use Path::Tiny;
use warnings;
use experimental 'signatures';
use feature 'say';
plugin 'RenderFile';

# OpenBSD promises.
pledge("stdio prot_exec cpath rpath wpath inet flock fattr")
  if $^O eq "openbsd";

# 500 MBs

my $MAX_SIZE = 1024 * 1024 * 1000;

my @BANNED = eval { path('banned.txt')->slurp_utf8 }
  or qw();    # Add banned IP addresses here
my @BANNED_EXTS = eval { path('banned_exts.txt')->slurp_utf8 }
  or qw();    # Add forbidden files extensions here
my $dirname;
my $link;

mkdir "f";

# Function to handle file uploads

sub logger ( $level, $address, $message ) {

    open( my $fh, ">>", "sakisafe.log" );
    printf( $fh "[%s]: %s has uploaded file %s\n", $level, $address, $message );
    printf("[%s]: %s has uploaded file %s\n", $level, $address, $message );
    close($fh);
}

sub handle_file {
    my $c        = shift;
    my $filedata = $c->param("file");
    if ( $filedata->size > $MAX_SIZE ) {
        return $c->render(
            text   => "Max upload size: $MAX_SIZE",
            status => 400
        );
    }

    if ( List::MoreUtils::any { /$c->req->headers->header('X-Forwarded-For')/ } uniq @BANNED ) {
        $c->render(
            text =>
"Hi! Seems like the server admin added your IP address to the banned IP array."
              . "As the developer of sakisafe, I can't do anything.",
            status => 403
        );
        return;
    }

    # Generate random string for the directory
    my @chars = ( '0' .. '9', 'a' .. 'Z' );
    $dirname .= $chars[ rand @chars ] for 1 .. 5;
    my $filename = $filedata->filename;
    my ($ext) = $filename =~ /(\.[^.]+)$/;
    if ( List::MoreUtils::any { /$ext/ } uniq @BANNED_EXTS ) {
	    $c->render( text => "You cannot this filetype.\n", status => 415 );
	    say $ext;
        logger( "WARN", $c->req->headers->header('X-Forwarded-For'), $dirname . "/" . $filename );
        return;
    }
    carp( color("bold yellow"),
        "sakisafe warning: could not create directory: $ERRNO",
        color("reset") )
      unless mkdir( "f/" . $dirname );
    $filename .= ".txt" if $filename eq "-";

    # TODO: get whether the server is http or https
    # There's a CGI ENV variable for that.
    my $host = $c->req->url->to_abs->host;
    my $ua   = $c->req->headers->user_agent;
    $filedata->move_to( "f/" . $dirname . "/" . $filename );
    $link = "http://$host/f/$dirname/$filename";
    $c->stash( link => $link, host => $host, dirname => $dirname );

    $c->res->headers->header( 'Location' => "$link" . $filename );

    # Only give the link to curl, html template for others.

    if ( $ua =~ m/curl/ || $ua eq "" ) {
        $c->render(
            text   => $link . "\n",
            status => 201,
        );

        $dirname = "";
    }
    else {
        $c->render(
            template => 'file',
            status   => 201,
        );
    }
    logger( "INFO", $c->req->headers->header('X-Forwarded-For'), $dirname . "/" . $filename );
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
    $c->render( text => "file not found", status => 404 ) unless -e $path;
    $c->render_file(
        filepath            => $path,
        format              => $ext,
        content_disposition => 'inline'
    );
};

app->max_request_size( 1024 * 1024 * 100 );

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

  __END__


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
  <code>curl -F 'file=@-' https://<%= $c->req->url->to_abs->host; %></code><br/>
  <a href="https://git.suragu.net/svragv/sakisafe">Git repository</a>
  </center>
  <p>Running sakisafe 2.4.0</p>
  <h2>FAQ</h2>
  <p>(No one has ever asked these questions)</p>
  <p><b>How long are the files stored?</b> Until the heat death of the universe</b></p>
  <p><b>Do you log IP addresses?</b> Yes. Blame the people uploading illegal stuff to this</p>
  <div class="left">
  <h2>Or just upload a file here</h2>
  <form ENCTYPE='multipart/form-data' method='post' action='/upload'>
  <input type='file' name='file' size='30'/>
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
