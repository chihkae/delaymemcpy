CC=gcc
CFLAGS=-Wall -g -O1
LDFLAGS=

all: memcpy-test memcpy-performance

memcpy-test: memcpy-test.o delaymemcpy.o
memcpy-performance: memcpy-performance.o delaymemcpy.o

memcpy-performance.o: memcpy-test.c delaymemcpy.h
memcpy-test.o: memcpy-test.c delaymemcpy.h
delaymemcpy.o: delaymemcpy.c delaymemcpy.h

clean:
	-rm -rf memcpy-test.o memcpy-performance.o delaymemcpy.o memcpy-test memcpy-performance
