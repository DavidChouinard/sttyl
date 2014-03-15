#
# Author: David Chouinard
#   Date: March 5 2014
#   Desc: Simple Makefile for compiling sttyl
#  Usage: make sttyl
#

# -D_XOPEN_SOURCE necessary because standard C99 is buggy and incorrectly
# doesn't include isascii() from ctype.h
CFLAGS = -Wall -std=c99 -D_XOPEN_SOURCE
CC = gcc
OBJECTS = sttyl.o tables.o

all: sttyl

sttyl: $(OBJECTS)
	$(CC) $(OBJECTS) -o sttyl $(CFLAGS)

sttyl.o: sttyl.c tables.h
	$(CC) -c $(CFLAGS) sttyl.c

tables.o: tables.c tables.h
	$(CC) -c $(CFLAGS) tables.c

clean:
	rm -f *.o core sttyl
