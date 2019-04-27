CC = gcc
CFLAGS = -Wall -std=c11 -pedantic

checkers: checkers.c
	$(CC) $(CFLAGS) checkers.c -o checkers
