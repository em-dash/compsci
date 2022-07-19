CC = clang

CFLAGS = -Wall -std=c99 -pedantic

SOURCES = hash.c

make:
	$(CC) $(CFLAGS) -o hash $(SOURCES)

test: make
	python test.py
