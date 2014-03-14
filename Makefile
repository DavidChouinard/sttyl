#
# Author: David Chouinard
#   Date: March 5 2014
#   Desc: Simple Makefile for compiling sttyl
#  Usage: make sttyl
#

CC = gcc
CFLAGS = -Wall -std=c99 -D_XOPEN_SOURCE

all: sttyl

# -D_XOPEN_SOURCE necessary because standard C99 is buggy and incorrectly
# doesn't include isascii() from ctype.h
sttyl: sttyl.c tables.h tables.c
	$(CC) sttyl.c tables.c -o sttyl $(CFLAGS)

clean:
	rm -f *.o core sttyl
