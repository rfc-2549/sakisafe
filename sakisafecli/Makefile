# sakisafecli makefile

TARGET = sakisafecli
OBJS   = sakisafecli.o funcs.o
CC     = cc
CFLAGSDEF += -MD -std=c11 -Wall -Wextra -lcurl -lconfig
CFLAGS += -O2 -ggdb

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(CFLAGSDEF) $(OBJS) -o $(TARGET)

%.o: %.c
	$(CC) -c $(CFLAGS) $(CFLAGSDEF) $< -o $@

-include *.d

install: all
	@cp sakisafecli /usr/local/bin/

.PHONY: clean install
clean:
	rm -f *.d *.o sakisafecli
