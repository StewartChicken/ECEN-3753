UNAME=$(shell uname)

CCFLAGS=-Wall -g -std=gnu99
CC=gcc

all: rr

rr: main.o queue.o rr.o ctest.h rrtests.o
	$(CC) $(LDFLAGS) main.o queue.o rr.o rrtests.o -o roundrobin

remake: clean all

%.o: %.c ctest.h
	$(CC) $(CCFLAGS) -c -o $@ $<

clean:
	rm -f roundrobin *.o
