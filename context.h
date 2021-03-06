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
#include "prompt.h"
#include "interpreter.h"

#define PREALLOC_SYM_NUM 0x040
#define MAX_IDENT_NUM 0x40

#define MAX_CONTEXT_MSG_SIZE 128
#define MAX_TOKEN_BUFFER_SIZE 2048

/* Context tree node of each block */
typedef struct {
	/* I/O stream */
	FILE *instream;
	FILE *outstream;

	prompt_t prompt[1];

	/* Preallocated token chain, used by lex and syntax */
	token_t tokens[PREALLOC_SYM_NUM];
	/* Tail of token chain*/
	token_t *token_tail;
	token_t *token_last_tail;
	/* Count for tokens */
	int token_num;

	/* Ident table, used by interpreter */
	ident_t idents[MAX_IDENT_NUM];
	size_t id_num;

	/* For conditions */
	bool excute;
	/* Specify if scan next token from input */
	bool scan;

	/* Error message */
	char *message;

	int depth;

	/* Point to previous context */
	void *prev;
} context_t;

void context_init(context_t *context, FILE *instream, FILE *outstream);
context_t *context_fork(context_t *parent);

const context_t *context_top_restrict(const context_t *context);
context_t *context_top(context_t *context);

/**
 * Functions of tokens
*/

/* Wrapper for fgetc/ungetc */
int get_char(context_t *context);
int unget_char(context_t *context, int ch);

/* Get symbol from input file */
SYMBOL getsym(context_t *context);
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
context_t *context_prev(context_t *context);

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

void ident_error(const context_t *context, const char *fmt, ...);
#define ident_undefined(name)                                                  \
	ident_error(context, "variable \"%s\" used but undefined", name)
#define ident_uninitialized(name)                                              \
	ident_error(context, "variable \"%s\" used but not initialized", name)

int operation(const context_t *context, int m, SYMBOL opt, int n);
bool condition(const context_t *context, int m, SYMBOL opt, int n);

#endif /* CONTEXT_H */
