PROG   += sakisafecli
SRCS   += funcs.c sakisafecli.c config.c
MAN    += sakisafecli.1 sakisafeclirc.5
LDADD  += -lpthread -lcurl -lconfig -L/usr/local/lib -fPIC
PREFIX = /usr/local

# Use libbsd features if wanted
use-libbsd ?= no
.if ${use-libbsd} == "yes"
CFLAGS += -Duse_libbsd
LDADD  += -lbsd
.endif

BINMODE = 755
BINDIR = $(PREFIX)/bin
MANDIR = $(PREFIX)/man/man

.include <bsd.prog.mk>
