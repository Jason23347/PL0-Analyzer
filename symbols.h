/*
    PL0-Analyzer -- A simple PL0 lexical & syntex analyzer
    Copyright 2020  Shuaicheng Zhu

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SYMBOLS_H
#define SYMBOLS_H

#include <stdio.h>

/* Enumulate of all the symbols */
typedef enum SYMBOL {
	nul, // null
	eof, // EOF
	plus, // +
	minus, // -
	times, // *
	slash, // /
	lparen, // (
	rparen, // )
	comma, // ,
	semicolon, // ;
	period, // .
	becomes, // :=
	eql, // =
	neq, // #
	lss, // <
	gtr, // >
	leq, // <=
	geq, // >=
	number, // 123
	ident, // abc
	beginsym, // begin
	callsym, // end
	constsym, // const
	dosym, // do
	endsym, // end
	ifsym, // if
	oddsym, // odd
	proceduresym, // proc
	readsym, // read
	thensym, // then
	varsym, // var
	whilesym, // while
	writesym // write
} SYMBOL;

/* Record position of the file */
typedef struct {
	int row;
	int col;
} pos_t;

/* Chain node of symbols */
typedef struct {
	char *value;
	SYMBOL type;
	void *next;
} token_t;

#define PREALLOC_SYM_NUM 0x040

/* Wrapper for fgetc/ungetc */
int get_char();
int unget_char(int ch);

/* Get symbol from input file */
SYMBOL getsym();
/* Convert SYMBOL to human readable string */
const char *sym2human(SYMBOL sym);

/* Initialize variables */
void token_init();
/* Add a symbol to end of chain */
void token_add(int ch);
/* Print token info */
void token_dump();

#endif /* SYMBOLS_H */
