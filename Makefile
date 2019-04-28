CC = gcc
CFLAGS = -Wall -std=c11 -pedantic
CHECKERS = checkers.c checkers.h

console_checkers: $(CHECKERS) console.c
	$(CC) $(CFLAGS) $(filter %.c,$^) -o $@

ncurses_checkers: $(CHECKERS) ncurses.c
	$(CC) $(CFLAGS) $(filter %.c,$^) -lncurses -o $@
