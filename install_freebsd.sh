#!/bin/sh

set -e

name="sakisafe"

echo "Installing Perl and modules..."
pkg install perl p5-Mojolicious p5-List-MoreUtils p5-Path-Tiny
cpan -Ti Mojolicious::Plugin::Renderfile

git clone "http://github.com/rfc-2549/${name}" "/usr/local/share/${name}"
ln -sf "/usr/local/share/${name}/rc.d/freebsd/${name}" "/usr/local/etc/rc.d/${name}"
touch "/var/run/${name}.pid"
chown saki:saki "/var/run/${name}.pid"

echo "${name} installed. Now run:"
echo ""
echo "service ${name} enable"
echo "service ${name} start"
echo ""
echo "To start ${name}."
