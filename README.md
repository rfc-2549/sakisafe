# Sakisafe (formerly lainsafe)

## Simple file upload

## Features

* Written in perl
* Can be used as pastebin
* Easy installation (just a simple daemon and use a reverse proxy)
* Does not bully Tor users ;)


# Installation

sakisafe is written in Perl, so it is its first dependency. But if you're
using a real operating system you should have it by default.

1. install the dependencies using `cpan`:

~~~
cpan -i Mojolicious::Lite Mojolicious::Routes::Pattern Mojoliciuos::Plugin::RenderFile
~~~

If you're running Debian or FreeBSD you can install the dependencies with
your package manager:

~~~bash
apt install libmojolicious-perl libmojolicious-plugin-renderfile-perl liblist-moreutils-perl # Debian, Ubuntu...
pkg install p5-Mojolicious p5-List-MoreUtils p5-Path-Tiny # FreeBSD
# You'll have to run cpan to install the RenderFile plugin in FreeBSD
cpan -Ti Mojolicious::Plugin::Renderfile
~~~

2. Clone the repo and start the daemon:

~~~
git clone https://git.suragu.net/sosa/sakisafe
cd sakisafe/http
./sakisafe.pl daemon -m production
~~~
You can also use `hypnotoad` to run the thing:
~~~
hypnotoad sakisafe.pl # This will daemonize the thing in port 8080.
~~~

2.2. Installing on BSD:

In *FreeBSD*, you can install using the `install_freebsd.sh` script. Run
the following:

~~~bash
su root - -c 'curl -L https://raw.githubusercontent.com/rfc-2549/sakisafe/refs/heads/master/install_freebsd.sh | sh'
service sakisafe enable
service sakisafe start
~~~

In *NetBSD*, you can install using the `netbsd_install.sh` script.
Run the following:

~~~bash
su root - -c 'curl -L https://raw.githubusercontent.com/rfc-2549/sakisafe/refs/heads/master/install_netbsd.sh | sh'
service sakisafe enable
service sakisafe start
~~~

Please contribute more init scripts for sakisafe!!! (for systemd,
openbsd...)

2.3. You can also use Docker. I am not a Docker advocate. But apparently
running `docker build -t sakisafe . && docker run -it sakisafe` or something like that just works.

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
			 proxy_set_header    X-Real-IP       $remote_addr; # Important for logging!
    		 proxy_set_header    X-Forwarded-for $remote_addr; # Idem
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
* As far as I know, runs in any *NIX operating system.

## Installation

### Compilation

Clone the repo and run `make`.

#### Linux-specific

Use `bmake` instead of `make`, and you'll also need these deps:

* `libcurl-devel`
* `libconfig-dev`
* `libnghttp2-dev`

### Repositories

![](https://repology.org/badge/vertical-allrepos/sakisafe.svg)

TODO: Add the thing to Debian repositories ;)


# Contributing

Just create a pull request in GitHub or send a diff file.

# Donate

Thanks!

**Bitcoin**: bc1qghl6f27dpgktynpvkrxte2s3gm9pcv8vlwuzum

**Monero**: 47QTumjtqJabbo1s9pLDdXeJarLVLfs1AaEcbi1xrEiV852mqcbe5AHLNXTk7tH9MscxcxQDfJQnvH5LpxvfgwSJQZ3zbS6

