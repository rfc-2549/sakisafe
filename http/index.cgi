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

my $currently_used = `du -h files`;
$currently_used =~ s/files//;
my $total = "32GB"; # Put here what you use.

print "<!DOCTYPE html>
<html lang=\"en\">
  <head>
    <title>lainsafe</title>
  </head>
  <body>

    <h1>lainsafe</h1>

    <img src='https://lainchan.org/static/lain_is_cute_datass_small.png'
    width=200 alt='cool lain' />
    <form ENCTYPE='multipart/form-data' method='post' action='upload.cgi'>
      <input type='file' name='file' size='30'>
      <input type='submit' value='upload'>
    </form>


  </body>
    </html>";

print "$currently_used available from $total";
