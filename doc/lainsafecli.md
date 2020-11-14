NAME
====

lainsafecli - Command line interface for lainsafe

SYNOPSIS
========

lainsafecli *\[\--tor \| \--i2p\] \[\--server\]* file

DESCRIPTION
===========

*lainsafecli* uploads a file to a given lainsafe server. This server may
be specified with the *\--server* flag. If *\--server* is not given. The
content of the variable *\$DEFAULT_SERVER* will be used instead. This
variable is in the script. In the official packages. this is
https://lainsafe.delegao.moe

OPTIONS
=======

*\--server* Sets the server to use. If it is not given \$DEFAULT_SERVER
will be used instead.

*\--help* Displays a simple help message and exits. This also specify
which server will be used if no *\--server* is given.

*\--tor* Uses tor to upload the file. This requires
*LWP::Protocol::socks* to be installed on your system. You also need to
have tor running at 127.0.0.1:9050 If LWP::Protocol::socks is not
installed, \--tor is ignored, so don\'t try it

*\--i2p* Routes the traffic through I2P. This uses the HTTP proxy (which
is normally in port 4444). So *LWP::Protocol::socks* is not necessary.

*\--get-response* If the server returned an error, \--get-response will
make lainsafecli print the content, so it can help you find out why
lainsafecli isn\'t working!

CONFIGURATION
=============

there are configuration values in *lainsafecli* line 34.

*\$DISPLAY_ASCII* If true (Any number but 0), It will display an ASCII
art if no error returned.

*\$STORE_LINKS* If true, lainsafecli will store the links on a file,
specified by *\$LINKS_FILE*

TROUBLESHOOTING
===============

whatever is not running lainsafe. But works in the browser.
-----------------------------------------------------------

This only happens with https lainsafe instances. To solve that. Make
sure that the module *LWP::UserAgent::https* is installed. You can
install it using *cpan(1)*

HTTP 413 is returned
--------------------

The file you specified is too big for the instance. Try using another
instance. By default lainsafe supports up to 100MBs. But this can be
configured in lainsafe **AND** the web server. If the file exceds
whatever your **server** allows. lainsafe can set another limit. If the
lainsafe limit is less than the server limit. lainsafe will return its
own error message.

AUTHORS
=======

qorg11 \<qorg\@vxempire.xyz\>

LICENSE
=======

GPLv3 or later.
