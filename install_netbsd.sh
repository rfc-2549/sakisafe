#!/bin/sh

set -e

name="sakisafe"

error_out()
{
    echo "\n"
    echo "$1"
    echo "\n"
    exit 1
}

if ! type "pkgin" > /dev/null; then
    error_out "You need to install 'pkgin' to install ${name}"
fi

if $(id -u) != 0; then
    error_out "You need to be root to run install ${name}"
fi

# Create sakisafe share directory, this holds the PIDFILE, and LOGFILE
mkdir -p "/usr/pkg/share/${name}/run/"

echo "Installing dependencies..."

# Install NetBSD pre-reqs
pkgin -y install git \
      perl \
      p5-App-cpanminus

# Clone project
git clone "https://github.com/rfc-2549/${name}" "/usr/pkg/etc/${name}"

echo "Installing CPAN (Perl) dependencies, this can take a while"
cpanm -n List::MoreUtils Path::Tiny MIME::Types Mojolicious Mojolicious::Plugin::RenderFile Mojolicious::Routes::Pattern

echo "Symlinking rc script to rc.d"
ln -sf "/usr/pkg/etc/${name}/rc.d/netbsd/${name}" "/etc/rc.d/${name}"

echo "DONE!"
