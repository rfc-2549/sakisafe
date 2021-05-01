# Lainsafe
### simple file upload.

### Installation:

1. Configure your webserver to run CGI
2. If running nginx, set ```client_max_body_size``` to the max size of
   the file
2. There you go.

For more detailed information, look at the
[manpage](https://codeberg.org/qorg11/lainsafe/src/branch/master/doc/lainsafe.md)

# clainsafecli

`clainsafecli` is the command line interface for lainsafe (and other
filesharing services)

## usage

`clainsafecli --server=http://server <file>`

(for more info see -h)

# Automatically removing files

Try [lsd](https://git.kalli.st/czar/lsd) (lainsafe daemon)

usage: `lsd <path> days`

lsd wont work with versions before 8a557ca

# but perl sucks!!

No problem, [use this
then](https://ech1.github.io/blog/servers/phpfilesafe/index.html)

You don't have to use lainsafecli either, you can just use curl:

You can also use clainsafecli for other servers, such as u.kalli.st and 0x0.st

# Donate

Thanks!

Bitcoin: bc1qghl6f27dpgktynpvkrxte2s3gm9pcv8vlwuzum
Monero: 47QTumjtqJabbo1s9pLDdXeJarLVLfs1AaEcbi1xrEiV852mqcbe5AHLNXTk7tH9MscxcxQDfJQnvH5LpxvfgwSJQZ3zbS6
