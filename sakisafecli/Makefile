PROG = sakisafecli
SRCS += funcs.c sakisafecli.c
MAN  = sakisafecli.1 
LDADD  = -lssl -lz -lpthread -lnghttp2 -lcurl -lconfig -lcrypto -L/usr/local/lib
CPPFLAGS = -I/usr/local/include 
BINDIR=/usr/local/bin
.include <bsd.prog.mk>
