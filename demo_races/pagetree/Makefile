CC     ?= gcc
CFLAGS  = -std=c99 -O2 -g -Wall -Wextra

.PHONY: all clean

all: pagetree

pagetree: pagetree.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	-rm -f pagetree
