#ifndef SUDOKU_H
#define SUDOKU_H

#include <stdint.h>

struct sudoku_puzzle {
	uint8_t element[9][9];
	int x;
	int y;
};

void sudoku_puzzle_print(struct sudoku_puzzle *);
struct sudoku_puzzle *sudoku_puzzle_solve(struct sudoku_puzzle *);

void sudoku_puzzle_load(struct sudoku_puzzle *, FILE *);

#endif
