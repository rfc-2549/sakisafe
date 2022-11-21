#!/usr/bin/perl
# This file is part of sakisafe.

use Mojolicious::Lite -signatures;
use Mojolicious::Routes::Pattern;
use v5.36;
plugin 'RenderFile';

my $MAX_SIZE = 1024 * 1024 * 100;

my $dirname;
my $host;

sub handle_file {
    my $c        = shift;
    my $filedata = $c->param("file");
    if ( $filedata->size > $MAX_SIZE ) {
	   return $c->render(
		  text   => "Max upload size: $MAX_SIZE",
		  status => 400
	   );
    }

    # Generate random string for the directory
    my @chars = ( '0' .. '9', 'a' .. 'Z' );
    $dirname .= $chars[ rand @chars ] for 1 .. 5;
    my $filename = $filedata->filename;
    mkdir( "f/" . $dirname );
    $filename .= ".txt" if $filename eq "-";
    $filedata->move_to( "f/" . $dirname . "/" . $filename );
    my $host = $c->req->url->to_abs->host;
    $c->res->headers->header(
	   'Location' => "http://$host/$dirname/" . $filedata->filename );
    $c->render(
	   text   => "http://$host/f/$dirname/" . $filename,
	   status => 201,
    );
    $dirname = "";

}

get '/' => 'index';
post '/' => sub ($c) { handle_file($c) };

# Allow files to be downloaded.

get '/f/:dir/:name' => sub ($c) {
    my $captures = $c->req->url;
    $captures =~ s/^.//;
    my $filerender = Mojolicious::Plugin::RenderFile->new;
    $c->render_file( filepath => $captures );
};

app->max_request_size( 1024 * 1024 * 100 );

post '/upload' => sub ($c) { handle_file($c) };

app->start;

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
		<h2>shitless file upload, pastebin and url shorter</h2>
		<img src="saki.png"/>
		<h2>USAGE</h2>
		<p>POST a file:</p>
		<code>curl -F 'file=@yourfile.png' <%= $c->req->url->to_abs->host; %></code>
		<p>Post your text directly</p>
		<code>curl -F 'file=@-' <%= $c->req->url->to_abs->host; %></code>
      </center>
		<div class="left">
			<h2>Or just upload a file here</h2>
			<form ENCTYPE='multipart/form-data' method='post' action='/upload'>
				<input type='file' name='file' size='30'/>
				<input type='submit' value='upload'/>
			</form>
		</div>
	</body>
</html>
