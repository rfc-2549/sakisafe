# clainsafecli makefile

TARGET = clainsafecli
OBJS   = clainsafecli.o
CC     = cc
CFLAGS = -MD -Wall -Wextra -O2 -march=native -lcurl

$(TARGET): clainsafecli.o
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

-include *.d

.PHONY: clean
clean:
	rm -f *.d *.o clainsafecli
