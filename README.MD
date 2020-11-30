# Lainsafe
### simple file upload.

### Installation:

1. Configure your webserver to run CGI
2. If running nginx, set ```client_max_body_size``` to the max size of
   the file
2. There you go.

For more detailed information, look at the
[manpage](https://codeberg.org/qorg11/lainsafe/src/branch/master/doc/lainsafe.md)

## lainsafecli

### Installation

#### Arch
[AUR - `lainsafecli`](https://aur.archlinux.org/packages/lainsafecli/)

### lainsafecli configuration

edit the file with your favorite text editor and change these
variables:

```$DEFAULT_SERVER``` This is the server where lainsafecli will try to
upload the files.

```$DISPLAY_ASCII``` By default 1, change to 0 if you don't want the
Lain ascii art to appear when you upload a file.

### Command line arguments

```--server``` If given, it will be used instead of $DEFAULT_SERVER

```--help``` Displays a simple help message and exits.

```--file``` the file you want to upload (useless in newer versions)

### Example

~~~
$ lainsafecli --server https://lainsafe.delegao.moe sicp.png
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

https://lainsafe.delegao.moe/files/160572227741108.png
~~~

# Automatically removing files

Try [lsd](https://git.kalli.st/czar/lsd) (lainsafe daemon)

usage: `lsd <path> days`

lsd wont work with versions before 8a557ca

# but perl sux!!

No problem, [Lainsafe rewritten in
PHP](https://ech1.github.io/blog/servers/phpfilesafe/index.html)
(Not sure if it is compatible with lainsafecli, probably not)


# Donate

Thanks!

[Liberapay](https://liberapay.com/qorg11)

Bitcoin: bc1qghl6f27dpgktynpvkrxte2s3gm9pcv8vlwuzum
Monero: 47QTumjtqJabbo1s9pLDdXeJarLVLfs1AaEcbi1xrEiV852mqcbe5AHLNXTk7tH9MscxcxQDfJQnvH5LpxvfgwSJQZ3zbS6
