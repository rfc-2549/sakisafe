# sakisafecli makefile

TARGET = sakisafecli
OBJS   = sakisafecli.o funcs.o
CC     = cc
CFLAGSDEF = -MD -std=c11 -Wall -Wextra -lcurl
CFLAGS = -O2 -march=native


$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(CFLAGSDEF) $(OBJS) -o $(TARGET)

%.o: %.c
	$(CC) -c $(CFLAGS) $(CFLAGSDEF) $< -o $@

-include *.d

.PHONY: clean
clean:
	rm -f *.d *.o sakisafecli
