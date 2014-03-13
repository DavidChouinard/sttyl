#
# Author: David Chouinard
#   Date: March 5 2014
#   Desc: Simple Makefile for compiling sttyl
#  Usage: make sttyl
#

sttyl: sttyl.c tables.h tables.c
	gcc sttyl.c tables.c -o sttyl -Wall -std=gnu99

clean:
	rm -f *.o core sttyl
