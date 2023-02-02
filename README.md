# Sakisafe (formerly lainsafe)

## Simple file upload

## Features

* Written in perl
* Can be used as pastebin
* Easy installation (just a simple daemon and use a reverse proxy)

1. install the dependencies using `cpan`:

~~~
cpan -i Mojolicious::Lite Mojolicious::Routes::Pattern Mojoliciuos::Plugin::RenderFile
~~~

2. Clone the repo and start the daemon:

~~~
git clone https://git.suragu.net/dudemanweed/sakisafe
cd sakisafe/http
./sakisafe.pl daemon -m production
~~~

3. Create a 'f' directory in the directory sakisafe will run with
`mkdir f`. Make sure that the user which will run sakisafe.pl can
write in that directory.

By default, sakisafe will bind in 127.0.0.1 port 3000. Because that's
the default bind Mojolicious uses.

4. Create a proxy rule in nginx configuration (If you're using another
HTTP server, you're on your own.)

~~~conf

server {
	  server_name sakisafe.whatever.tld;
	  listen 443 ssl;

	  # ssl configuration here

	  location / {
			 proxy_set_header    Host            $host;
			 proxy_set_header    X-Real-IP       $remote_addr;
    			 proxy_set_header    X-Forwarded-for $remote_addr;
			 proxy_pass http://127.0.0.1:3000$request_uri;
	  }
}
~~~

And restart nginx. Going to `sakisafe.whatever.tld` should work.

# sakisafecli

sakisafecli is a command line interface for sakisafe servers (and
other file upload services). It also supports file uploading via scp
(ssh).

## Features

* Written in C so is fast.
* HTTP and SCP support (FTP support is planned)
* Highly configurable
* Lightweight
* If using OpenBSD, it will run `pledge()` for security reasons.

## Installation

### Compilation

Clone the repo and run `make` (or `bmake` in linux systems)

### Repositories

![](https://repology.org/badge/vertical-allrepos/sakisafe.svg)

# Contributing

Just create a pull request in GitHub or send a diff file.

# Donate

Thanks!

**Bitcoin**: bc1qghl6f27dpgktynpvkrxte2s3gm9pcv8vlwuzum

**Monero**: 47QTumjtqJabbo1s9pLDdXeJarLVLfs1AaEcbi1xrEiV852mqcbe5AHLNXTk7tH9MscxcxQDfJQnvH5LpxvfgwSJQZ3zbS6
