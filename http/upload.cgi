#!/usr/bin/perl
# This file is part of lainsafe.

# lainsafe is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# lainsafe is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with lainsafe.  If not, see <https://www.gnu.org/licenses/>.

use CGI;
use CGI::Carp qw(fatalsToBrowser);
use Time::HiRes qw(gettimeofday);
my $q = CGI->new;

my $filename = $q->param('file');

my $upload_dir = "files/";
print $q->header();
$size    = $ENV{CONTENT_LENGTH};

# Configuration

our $MAX_SIZE = 1024*1024*10; # Change for your size
our $MAX_SIZE_MB = $MAX_SIZE / 1024 / 1024; # Don't change this
our @not_allowed_extensions = qw(sh out exe);

if($filename eq "" || $ENV{REQUEST_METHOD} eq "GET")
{
    print("What are you looking for?");
    exit;
}

if($size > $MAX_SIZE)
{
    print("Max size for a file is $MAX_SIZE_MB MBs");
    exit;
}

my $extension = $filename;
$extension =~ s/.*\.//; # tar.gz sucks with this
$extension = "notcgi" if $extension eq "cgi";
# Get unix time in miliseconds
my $string;
$string = gettimeofday; # perl, what?
$string =~ s/\.//g;

my $upload_filehandle = $q->upload("file");

# onion urls will be http
my $prot = length $ENV{HTTPS} ? "https" : "http";


$filename = $string . "." . $extension;
my $allowed_extension = 1;

foreach(@not_allowed_extensions)
{
    if($filename =~ /\.$_$/i)
    {
        $allowed_extension = 0;
        last;
    }

}

if($allowed_extension)
{

    open(FILE,">$upload_dir/$filename");
    binmode(FILE);

    while(<$upload_filehandle>)
    {
        print FILE;
    }
    
    close FILE;

    print $prot. "://" . $ENV{HTTP_HOST} . "/$upload_dir$filename";
}

else
{
    print "The file extension .$extension is not allowed in this instance.";
}
