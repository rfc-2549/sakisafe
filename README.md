# Sakisafe (formerly lainsafe)

## Simple file upload

## Features

* Written in perl
* Can be used as pastebin
* Or as a URL shorter
* Easiy installation (just use fcgi)
* Runs with CGI

## Installation:

1. Configure your webserver to run CGI
2. If running nginx, set `client_max_body_size` to the max size of
   the file
2. There you go.

# sakisafecli

sakisafecli is a command line interface for sakisafe servers (and
other file upload services). It also supports file uploading via scp
(ssh).

## Features

* Written in C so is fast.
* HTTP and SCP support (FTP support is planned!)
* Highly configurable
* Lightweight
* If using OpenBSD, it will run `pledge()` for security reasons.

## Installation

### Compilation

Clone the repo and run `make` (or `bmake` in linux systems)

### Repositories

![](https://repology.org/badge/vertical-allrepos/sakisafe.svg)



# Donate

Thanks!

**Bitcoin**: bc1qghl6f27dpgktynpvkrxte2s3gm9pcv8vlwuzum

**Monero**: 47QTumjtqJabbo1s9pLDdXeJarLVLfs1AaEcbi1xrEiV852mqcbe5AHLNXTk7tH9MscxcxQDfJQnvH5LpxvfgwSJQZ3zbS6
