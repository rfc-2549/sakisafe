# lainsafecli documentation

## History

[delegao](https://kill-9.xyz/rocks/people/delegao) and I were talking about how much I hate the modern web. He runs <https://safe.delegao.moe> which is the same but with JS, so I think it sucked. I started to make lainsafe to replace lolisafe.

## Lainsafe installation

Just put index.html and upload.cgi in a HTTP directory. I use fcgi to run my cgi scripts. So it's what i recommend. To install fcgi on Debian, run: ```# apt install fcgiwrap```

here's an example configuration for lainsafe, using nginx.

~~~
server
{
	server_name lainsafe.foo.tld;

	listen 80;
	listen [::]:80;
	client_max_body_size 100m; # max size 10MBs, change 10 to 100 in upload.cgi in line 30
	root /var/www/lainsafe;

	location ~ \.cgi$ {
		gzip off;
		include /etc/nginx/fastcgi_params;
		fastcgi_pass unix:/var/run/fcgiwrap.socket;
		fastcgi_index index.cgi;
		fastcgi_param SCRIPT_FILENAME /var/www/lainsafe/$fastcgi_script_name;
	}
}
~~~

## lainsafecli

lainsafecli is a command line interface for lainsafe. It can be used in whatever that runs Perl. Installation is simple: if you're running Debian install the libwww-perl package. If you are not running Debian, search for that package in your repositories. Or use ```# cpan -i LWP::UserAgent```

### Installation

I haven't made a package for lainsafecli yet. So installation is basically:

~~~
# wget https://raw.githubusercontent.com/qorg11/lainsafe/master/lainsafecli \
-O /bin/lainsafecli && chmod +x /bin/lainsafecli
~~~

That will install lainsafecli, run ```lainsafecli``` to make sure it is installed.

### Configuration

lainsafecli is a simple software. So it has just 2 configurable
parameters. lainsafecli do not have a config file. So you have to
change these parameters in the executable file.

These parameters are located in the line 34 of lainsafecli.

```$DEFAULT_SERVER``` the server that will be used if --server is not
specified.
```$DISPLAY_ASCII``` By default is true, change to 0 if you don't want
the Lain ASCII art to appear when you upload a file.

the most "official" lainsafe instance is <https://lainsafe.delegao.moe>.

### Usage

Before 1b8373

~~~
USAGE: lainsafecli [--server] --file=FILE
~~~

After 1b8373

~~~
USAGE: lainsafecli [--server] FILE
~~~


for example:

```lainsafe --server=https://lainsafe.foo.bar sicp.pdf```

will upload sicp.pdf to lainsafe.foo.bar

### Sample output

In this output, ```$DEFAULT_SERVER``` is lainsafe.delegao.moe. and ```
$DISPLAY_ASCII``` equals 1 (true)

~~~
lainsafecli sicp.pdf

          _..-- ----- --.._
        ,-''                `-.
       ,                       \
      /                         \
     /        ` .                \
    '         /  ||               ;
    ;      ^/|   |/        |      |
    |      /v    /\`-'v√\'-|\     ,
    |   /v` ,---      ---- .^.|    ;
    :  |   /´@@`,     ,@@`\  |    ;
    '  |  '. @@ /     \@@ /  |\  |;
    | ^|    -----      ---   | \/||
    ` |`                     | /\ /
    \    \                   |/ |,
    ' ;   \                 /|  |
      `    \      --       / |  |
      `     `.          .-'  |  /
       v,-    `;._   _.;     | |
          `'`\ |-_      -^'^'| |
                  ------     |/
          
https://lainsafe.delegao.moe/files/nzOhDKtB.pdf
~~~

# upload.cgi

```upload.cgi``` is the perl script that do all the works. You could
delete index.cgi from your sever and just upload.cgi. You'd upload
files through ```lainsafecli```.

## Configuration

Only one parameter: ```$MAX_SIZE``` specify, in megabytes, the max size of files allowed. So for example. If you want to set 100MBs as max size . ```$MAX_SIZE``` would be like this: ```$MAX_SIZE = 1024*1024*100```

If someone tries to upload a file bigger than 100MBs. It will return
Max size for a file is 100 MBs, then, upload.cgi will die.
