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

#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <stdbool.h>

#include "symbols.h"

typedef enum {
	constvar, // value unchangeable
	variable, // value changeable
	procedure // function
} IDENT;

typedef struct {
	const char *name;
	int *value;
	IDENT type;

	void *next;
} ident_t;

void ident_error(const char *fmt, ...);
#define ident_undefined(name)                                                    \
	ident_error("variable \"%s\" used but undefined\n", name)

int operation(int m, SYMBOL opt, int n);
bool condition(int m, SYMBOL opt, int n);

#endif /* INTERPRETER_H */