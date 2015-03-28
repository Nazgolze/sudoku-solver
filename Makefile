CC=gcc
#CC=clang
CFLAGS=-O2 -pipe -std=gnu11 -g -Wall -Wunused-variable -Wuninitialized -pedantic

all: sudoku

sudoku: sudoku.o Makefile
	$(CC) $(CFLAGS) $(INCFLAGS) $(LDFLAGS) -fPIC -o sudoku sudoku.o

%.o: %.c Makefile
	$(CC) $(CFLAGS) -fPIC $(INCFLAGS) -c $<

clean:
	(rm -f *.o sudoku)
