UNAME=$(shell uname)

CCFLAGS=-Wall -g -std=gnu99
CC=gcc

all: pri

pri: main.o queue.o priority.o ctest.h prioritytests.o
	$(CC) $(LDFLAGS) main.o queue.o priority.o prioritytests.o -o priority

remake: clean all

%.o: %.c ctest.h
	$(CC) $(CCFLAGS) -c -o $@ $<

clean:
	rm -f priority *.o
