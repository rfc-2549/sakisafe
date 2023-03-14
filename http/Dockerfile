FROM perl:latest AS build

EXPOSE 3000

COPY . /sakisafe
COPY ./public /sakisafe/public

WORKDIR /sakisafe

RUN perl "Makefile.PL"
RUN make install
RUN cpan List::MoreUtils Path::Tiny MIME::Types Mojolicious Mojolicious::Plugin::RenderFile Mojolicious::Routes::Pattern

FROM build
CMD ["perl", "sakisafe.pl", "daemon"]
