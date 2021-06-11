/**
 * @file sudoku.c
 * @author Juan Gonzalez
 * @brief main sudoku code
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>
#include "sudoku.h"
#include "version.h"

enum moves_t {
	ZERO = 0x0,
	ONE = 0x1,
	TWO = 0x2,
	THREE = 0x4,
	FOUR = 0x8,
	FIVE = 0x10,
	SIX = 0x20,
	SEVEN = 0x40,
	EIGHT = 0x80,
	NINE = 0x100,
};

static bool _show_intermediate = false;
static const uint16_t masks[]
	= {ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE};

static struct option _long_options[] = {
	{"intermediate", no_argument, 0, 'i'},
	{"help", no_argument, 0, 'h'},
	{"version", no_argument, 0, 'v'},
	{0, 0, 0, 0}
};

/**
 * prints a sudoku puzzle
 *
 * @param sp a sudoku_puzzle struct pointer
 */
void sudoku_puzzle_print(struct sudoku_puzzle *sp)
{
	if(sp == NULL) {
		printf("no solution\n");
		return;
	}
	int row, column;
	for(row = 0; row < 9; row++) {
		printf("[");
		for(column = 0; column < 9; column++) {
			if(sp->element[row][column] == 0) {
				printf("_");
			} else {
				printf("%"PRIu16"", sp->element[row][column]);
			}
			if(column < 8)
				printf(" ");
		}
		printf("]\n");
	}
}

/**
 * advances the current element to the next zero element
 *
 * @param sp a sudoku_puzzle struct pointer
 */
static void _go_to_next(struct sudoku_puzzle *sp)
{
	int idx, jdx;
	for(idx = sp->x; idx < 9; idx++) {
		for(jdx = sp->y; jdx < 9; jdx++) {
			if(sp->element[idx][jdx] == 0) {
				sp->x = idx;
				sp->y = jdx;
				return;
			}
		}
		if(jdx == 9 && idx < 9) {
			jdx = 0;
			sp->x = idx;
			sp->y = jdx;
		}
	}
}

/**
 * given an integer 'val' between 0-9, return the mask for the 'val' bit
 *
 * @param val unsigned 16-bit integer
 * @return unsigned 16-bit integer
 */
static uint16_t _get_val(uint16_t val)
{
	switch(val) {
	case 1:
		return ONE;
	case 2:
		return TWO;
	case 3:
		return THREE;
	case 4:
		return FOUR;
	case 5:
		return FIVE;
	case 6:
		return SIX;
	case 7:
		return SEVEN;
	case 8:
		return EIGHT;
	case 9:
		return NINE;
	default:
		return ZERO;
	}
}

/**
 * get the numbers that are valid for a puzzle's current position in
 * the form of a bitmask
 *
 * @param sp a sudoku_puzzle struct pointer
 * @return unsigned 16-bit integer
 */
static uint16_t _get_possible_moves(struct sudoku_puzzle *sp)
{
	uint16_t ret = 0;

	int idx, jdx, ide, jde;

	uint64_t index[10] = {0};

	for(idx = 0; idx < 9; idx++) {
		index[sp->element[idx][sp->y]]++;
		index[sp->element[sp->x][idx]]++;
	}
	idx = sp->x / 3 * 3;
	jdx = sp->y / 3 * 3;
	ide = idx + 3;
	jde = jdx + 3;

	for(; idx < ide; idx++) {
		for(; jdx < jde; jdx++) {
			index[sp->element[idx][jdx]]++;
		}
		jdx = sp->y / 3 * 3;
	}

	for(idx = 1; idx <= 9; idx++) {
		if(index[idx] == 0) {
			ret |= _get_val((uint16_t)idx);
		}
	}

	return ret;
}

/**
 * solve the sudoku puzzle (recursive backtracking algorithm)
 *
 * @param sp a sudoku_puzzle struct
 * @return a sudoku_puzzle struct pointer
 */
static struct sudoku_puzzle *_sudoku_puzzle_solve_bt(struct sudoku_puzzle sp)
{
	_go_to_next(&sp);
	uint16_t moves = _get_possible_moves(&sp);
	uint8_t tmp = 0;
	struct sudoku_puzzle *solution = NULL;
	int idx, jdx;

	if(_show_intermediate) {
		printf("intermediate:\n");
		sudoku_puzzle_print(&sp);
		printf("====================\n");
	}

	// base case
	if(moves == 0) {
		bool no_zeroes = true;
		for(idx = 0; idx < 9; idx++) {
			for(jdx = 0; jdx < 9; jdx++) {
				if(sp.element[idx][jdx] == 0) {
					no_zeroes = false;
				}
			}
		}
		if(no_zeroes) {
			solution = malloc(sizeof(sp));
			memcpy(solution, &sp, sizeof(sp));
			return solution;
		}
	}
	for(idx = 0; idx < 9; idx++) {
		if(moves & masks[idx]) {
			tmp = sp.element[sp.x][sp.y];
			sp.element[sp.x][sp.y] = idx + 1;
			solution = _sudoku_puzzle_solve_bt(sp);
			if(solution) {
				return solution;
			} else {
				sp.element[sp.x][sp.y] = tmp;
			}
		}
	}

	return NULL;
}

/**
 * solve the sudoku puzzle
 *
 * @param sp a sudoku_puzzle struct pointer
 * @return a sudoku_puzzle struct pointer
 */
struct sudoku_puzzle *sudoku_puzzle_solve(struct sudoku_puzzle *sp)
{
	return _sudoku_puzzle_solve_bt(*sp);
}

/**
 * load a sudoku puzzle from stdin
 *
 * @param sp a sudoku_puzzle struct pointer
 */
static void _sudoku_puzzle_load(struct sudoku_puzzle *sp)
{
	char buf[1024] = {0};

	fread(&buf, 1, sizeof(buf), stdin);

	int idx, jdx, count;
	idx = jdx = count = 0;

	while(buf[count]) {
		if(idx > 8)
			break;
		if(jdx > 8) {
			idx++;
			jdx = 0;
			continue;
		}
		if(buf[count] < '0' && buf[count] > '9' && buf[count] != '_') {
			count++;
			continue;
		}
		if((buf[count] >= '0' && buf[count] <= '9') || buf[count] == '_') {
			if(buf[count] == '_') {
				sp->element[idx][jdx] = 0;
			} else {
				sp->element[idx][jdx] = buf[count] - '0';
			}
			jdx++;
			count++;
			continue;
		}
		count++;
	}
}

/**
 * print help
 */
static void _print_help(void)
{
	printf("Usage: sudoku [OPTION]...\n"
	    "Print solved sudoku puzzle for a given sudoku puzzle from stdin\n\n"
	    "  -i, --intermediate\tshow intermediate results\n"
	    "  -h, --help\tdisplay this help and exit\n");
}

/**
 * print version
 */
static void _print_version(void)
{
	if(commit_id == NULL) {
		printf("sudoku solver: version %s\n", version);
	} else {
		printf("sudoku solver: version %s-%s\n", version, commit_id);
	}
}

/**
 * main.
 *
 * @param argc an integer for number of args
 * @param argv a char** for the args
 */
int main(int argc, char **argv)
{
	struct sudoku_puzzle sp = {0};
	struct sudoku_puzzle *solution;

	int c;
	while(true) {
		int option_index = 0;
		c = getopt_long(argc, argv, "hi", _long_options, &option_index);

		if(c == -1)
			break;
		switch(c) {
		case 'h':
			_print_help();
			return 0;
		case 'v':
			_print_version();
			return 0;
		case 'i':
			_show_intermediate = true;
			break;
		}
	}

	_sudoku_puzzle_load(&sp);

	printf("original:\n");
	sudoku_puzzle_print(&sp);

	solution = sudoku_puzzle_solve(&sp);

	printf("solution:\n");
	sudoku_puzzle_print(solution);

	free(solution);

	return 0;
}
