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

my $disk_size = `df -h | awk 'NR==2 {print \$2; exit}'`;
my $disk_usage = `df -h | awk 'NR==2 {print \$3; exit}'`;
my $disk_free = `df -h | awk 'NR==2 {print \$4; exit}'`;
my $disk_percentage = `df -h | awk 'NR==2 {print \$5; exit}'`;

if ($ENV{REQUEST_METHOD} eq "POST") {
  do "./upload.cgi";
  exit;
}

print "Content-type: text/html\n\n";
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
    <a href='https://github.com/dimethyltriptamine/lainsafe'>star me</a>
   <h2>Terminal usage</h2>
   <p>I recommend using clainsafecli to upload files to lainsafe, however, there is more than one way to do it.</p>
   <code>curl -F 'file=@1605388889.png' $ENV{HTTP_HOST}$ENV{REQUEST_URI}</code><br/>
  </body>
    </html>";
