CC=gcc
#CC=clang
CFLAGS=-O2 -pipe -std=gnu11 -Wall -Wunused-variable -Wuninitialized -pedantic

all: prod

debug: sudoku.o Makefile
	$(CC) $(CFLAGS) -ggdb $(INCFLAGS) $(LDFLAGS) -fPIC -o sudoku sudoku.o

prod: sudoku.o Makefile
	$(CC) $(CFLAGS) $(INCFLAGS) $(LDFLAGS) -fPIC -o sudoku sudoku.o

doc:
	(doxygen)

%.o: %.c Makefile
	$(CC) $(CFLAGS) -fPIC $(INCFLAGS) -c $<

clean:
	(rm -rf *.o sudoku documentation)
