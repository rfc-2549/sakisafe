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
use Digest::MD5 qw(md5_hex);


my $q = CGI->new;
print $q->header();
# TODO: fix 502

my $upload_dir = "files/";
my $filename = $q->param("file");
if($filename eq "") {
    print("What are you looking for?");
    exit;
}

my $extension = $filename;
$extension =~ s/.*\.//;



my @chars = ("A".."Z", "a".."z");
my $string;
$string .= $chars[rand @chars] for 1..8;
my $upload_filehandle = $q->upload("file");

$filename = $string . "." . $extension;

open(FILE,">$upload_dir/$filename");
binmode(FILE);
while(<$upload_filehandle>)
{
    print FILE;
}

close FILE;

print $ENV{HTTP_REFERER} . "$upload_dir$filename";
