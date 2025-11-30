CC     = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
TARGETS = myls mytail

.PHONY: all clean

all: $(TARGETS)

myls: myls.c
	$(CC) $(CFLAGS) -o myls myls.c

mytail: mytail.c
	$(CC) $(CFLAGS) -o mytail mytail.c

clean:
	rm -f $(TARGETS) *.o
