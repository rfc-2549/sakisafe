NAME
====

lainsafe - Simple file upload

DESCRIPTION
===========

lainsafe is a simple file upload server written in Perl + CGI.

It runs in a Web server (i.e. nginx) with a CGI handler (i.e. fcgiwrap)

You also have to install the dependencies, in Debian:

**apt install nginx libcgi-pm-perl**

INSTALLATION
============

You can use any web server for running lainsafe. However, nginx is
recommended. The following configuration should work for nginx:

*/etc/nginx/sites-enabled/lainsafe.conf*

    server
    {
    	server_name lainsafe.foo.tld;

    	listen 80;
    	listen [::]:80;
    	client_max_body_size 100m; # max size 100MBs, change 10 to 100
    				   # in upload.cgi in line 30
    	root /var/www/lainsafe;

    	location ~ .cgi$ {
    		gzip off;
    		include /etc/nginx/fastcgi_params;
    		fastcgi_pass unix:/var/run/fcgiwrap.socket;
    		fastcgi_index index.cgi;
    		fastcgi_param SCRIPT_FILENAME /var/www/lainsafe/$fastcgi_script_name;
    	}
    }

The document root will be /var/www/lainsafe (be sure to change it in
*root* and *fastcgi\_param SCRIPT\_FILENAME if you\'re going to use
another directory*

You also have to give the .cgi files +x permissions:

    chmod +x *.cgi

And also, create the \"files\" directory manually, and give it
permissions so the user that is running nginx (usually) *www-data* can
write on it.

TROUBLESHOOTING
===============

**502 Bad Gateway**

Probably you\'re missing the *CGI* dependence, so install it with CPAN
or apt

    cpan -i CGI
    apt install libcgi-pm-perl

If you still get this problem, run

    perl <file>.cgi

so perl can help you debug the problem.

**403 is returned**

You probably forgot to give the files execution permissions.
