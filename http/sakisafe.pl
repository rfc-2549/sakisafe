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
use File::Basename;
use warnings;
use experimental 'signatures';
use feature 'say';
use Encode qw(decode encode);
plugin 'RenderFile';


my $openbsd = 0;
$openbsd = 1 if $^O eq "openbsd";
pledge("stdio cpath rpath wpath inet flock fattr") if $openbsd;

my $MAX_SIZE = 1024 * 1024 * 100;
my @BANNED = qw();			  # Add banned IP addresses here
my $RANDOMIZE_FILENAME = 1;   # Enable/Disable filename randomization

my $dirname;
my $host;

mkdir "f";

# Function to handle file uploads

sub logger ( $level, $address, $message ) {
	open( my $fh, ">>", "sakisafe.log" );
	printf( $fh "[%s]: %s has uploaded file %s\n", $level, $address, $message );
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
	if ( List::MoreUtils::any { /$c->tx->remote_address/ } uniq @BANNED ) {
		$c->render(
				 text =>
				 "Error: Your IP is banned." .
				 "To resolve this issue, contact the server's administrator.",
				 status => 403
				);
		return;
	}

	# Generate random string for the directory
	my @chars = ( '0' .. '9', 'a' .. 'Z' );
	$dirname .= $chars[ rand @chars ] for 1 .. 5;
	my $filename;
	$filename = $filedata->filename;
	my $enc = encode( "UTF-8", $filename );
	$filename = $enc;
	if ( $RANDOMIZE_FILENAME = 1 ) {
		my $extension = $filename;
		$extension =~ s/.*\.//;
		$filename = "";
		$filename .= $chars[ rand @chars ] for 1 .. 5;
		$filename = $filename . "." . $extension;
	}
	carp(color("bold yellow"), "sakisafe warning: could not create directory: $ERRNO", color("reset")) unless
	  mkdir( "f/" . $dirname );
	$filename .= ".txt" if $filename eq "-";

	$filedata->move_to( "f/" . $dirname . "/" . $filename );
	my $host = $c->req->url->to_abs->host;
	$c->res->headers->header(
						'Location' => "http://$host/$dirname/" . $filename );
	$c->render(
			 text   => "http://$host/f/$dirname/" . $filename,
			 status => 201,
			);
	logger( "INFO", $c->tx->remote_address, $dirname . "/" . $filename );
	$dirname = "";

}

# Function to log uploaded files

get '/' => 'index';
post '/' => sub ($c) { handle_file($c) };

# Allow files to be downloaded.

get '/f/:dir/#name' => sub ($c) {
	my $dir =  $c->param("dir");
	my $file =  $c->param("name");
	my $ext = $file;
	$ext =~ s/.*\.//;
	my $path = "f/".$dir . "/" . $file;
	#carp "sakisafe warning: could not get file: $ERRNO" unless
	$c->render(text => "file not found", status => 404) unless -e $path;
	$c->render_file( filepath => $path,
				  format   => $ext,
				  content_disposition => 'inline'
				)

}
;
app->max_request_size( 1024 * 1024 * 100 );

post '/upload' => sub ($c) { handle_file($c) };

app->start;

# Index template

#By default Mojolicious gets the "directory root" from the "public"
# directory, so the css and the favicon from the "public" directory,
# in the root of this repo.

__DATA__

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
  <h2>shitless file upload, pastebin and url shortener</h2>
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
  <form ENCTYPE='multipart/form-data' charset='utf-8' method='post' action='/upload'>
  <input type='file' name='file' size='30'/>
  <input type='submit' value='upload'/>
  </form>
  </div>
  </body>
  </html>
