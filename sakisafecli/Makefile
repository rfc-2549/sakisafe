PROG = sakisafecli
SRCS += funcs.c sakisafecli.c
MAN  = sakisafecli.1 sakisafeclirc.5
LDADD  = -lssl -lz -lpthread -lnghttp2 -lcurl -lconfig -lcrypto -L/usr/local/lib

BINDIR=/usr/local/bin
MANDIR=/usr/local/man
.include <bsd.prog.mk>
