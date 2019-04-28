CC = gcc
CFLAGS = -Wall -std=c11 -pedantic

console_checkers: checkers.c console.c
	$(CC) $(CFLAGS) $^ -o $@
