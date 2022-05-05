#!/usr/bin/perl

# This file is part of sakisafe.
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

if ($ENV{REQUEST_METHOD} eq "POST") {
  do "./upload.cgi";
  exit;
}

my $SITE_URL = $ENV{REMOTE_ADDR};

print "Content-type: text/html\n\n";

open(my $fh, "<index.html");

print while <$fh>;
