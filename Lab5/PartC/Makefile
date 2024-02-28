UNAME=$(shell uname)

CCFLAGS=-Wall -g -std=gnu99
CC=gcc

all: sjf

sjf: main.o queue.o sjf.o ctest.h sjftests.o
	$(CC) $(LDFLAGS) main.o queue.o sjf.o sjftests.o -o shortestjobfirst

remake: clean all

%.o: %.c ctest.h
	$(CC) $(CCFLAGS) -c -o $@ $<

clean:
	rm -f shortestjobfirst *.o
