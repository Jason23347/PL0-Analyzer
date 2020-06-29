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

/* Max length of an ident */
#define MAX_IDENT_SIZE 20

/* Enumulate of all the symbols */
typedef enum {
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
	char value[MAX_IDENT_SIZE];
	SYMBOL type;
	void *next;
} token_t;

#endif /* SYMBOLS_H */
