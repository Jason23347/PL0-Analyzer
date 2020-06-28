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

#ifndef CONTEXT_H
#define CONTEXT_H

#include "symbols.h"
#include "interpreter.h"

#define PREALLOC_SYM_NUM 0x040

#define PREALLOC_ID_NUM 0x10

/* Context tree node of each block */
typedef struct {
	/* Preallocated token chain, used by lex and syntax */
	token_t tokens[PREALLOC_SYM_NUM];
	/* Tail of token chain*/
	token_t *token_tail;
	/* Count for tokens */
	int token_num;

	/* Preallocated ident table, used by interpreter */
	ident_t idents[PREALLOC_ID_NUM];
	ident_t *id_tail;
	int id_num;

	/* For conditions */
	bool excute;
	/* Specify if scan next token from input */
	bool scan;

	/* Tow-way linked list */
	void *prev;
	void *next;
} context_t;

context_t *context_init();
void context_free(context_t *context);
context_t *context_fork(context_t *parent);

/**
 * Functions of tokens
*/

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
void token_add(context_t *context, int ch);
/* Print token info */
void token_dump(context_t *context);

/**
 * Functions of syntax
*/

/* Scan in next token */
context_t *context_next(context_t *context);

/**
 * Check if token->type matches the given SYMBOLs,
 * abort if no one matches.
*/
void assert_multi(const context_t *context, int num, ...);
#define assert(context, sym) assert_multi(context, 1, sym)

/**
 * program = block "." .
*/

/**
 * block = [ "const" ident "=" number {"," ident "=" number} ";"]
 * 	[ "var" ident {"," ident} ";"]
 * 	{ "procedure" ident ";" block ";" } statement .
*/
void parse(context_t *context);
/**
 * statement = [ ident ":=" expression | "call" ident
 * 	| "?" ident | "!" expression
 * 	| "begin" statement {";" statement } "end"
 * 	| "if" condition "then" statement
 * 	| "while" condition "do" statement ].
*/
void parse_statement(context_t *context);
/**
 * expression = [ "+"|"-"] term { ("+"|"-") term}.
*/
int parse_expression(context_t *context);
/**
 * condition = "odd" expression |
 * 	expression ("="|"#"|"<"|"<="|">"|">=") expression .
*/
bool parse_condition(context_t *context);
/**
 * term = factor {("*"|"/") factor}.
*/
int parse_term(context_t *context);
/**
 * factor = ident | number | "(" expression ")".
*/
int parse_factor(context_t *context);

/**
 * Functions of interpreters
*/
ident_t *ident_add(context_t *context, const token_t *token, IDENT type);
int ident_assign(const context_t *context, ident_t *id, void *value);

ident_t *ident_find(context_t *context, const char *name);

void ident_prompt(const ident_t *id);
void ident_dump(context_t *context);

#endif /* CONTEXT_H */
