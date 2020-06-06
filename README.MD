# Lainsafe
### simple file upload.

### Installation:

1. Configure your webserver to run CGI
2. If running nginx, set ```client_max_body_size``` to the max size of
   the file
2. There you go.

## lainsafecli

```lainsafecli``` is a simple interface for lainsafe. 

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
          
https://lainsafe.delegao.moe/files/EWwEnBHk.png

~~~
