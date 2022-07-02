PROG   = sakisafecli
SRCS   = parse.y
SRCS  += funcs.c sakisafecli.c config.c lex.yy.c
MAN    = sakisafecli.1 sakisafeclirc.5
LDADD  = -lssl -lz -lpthread -lnghttp2 -lcurl -lconfig -lcrypto -L/usr/local/lib
YFLAGS = -d
PREFIX = /usr/local
BINDIR = $(PREFIX)/bin
MANDIR = $(PREFIX)/man/man

lex.yy.c:
	lex parse.l

.include <bsd.prog.mk>
