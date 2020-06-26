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

#ifndef PARSER_H
#define PARSER_H

#include "symbols.h"

/* Scan in next token */
const token_t *next_token();

/**
 * Check if token->type matches the given SYMBOLs,
 * abort if no one matches.
*/
void assert_multi(const token_t *token, int num, ...);
#define assert(token, sym) assert_multi(token, 1, sym)

/**
 * program = block "." .
*/

/**
 * block = [ "const" ident "=" number {"," ident "=" number} ";"]
 * 	[ "var" ident {"," ident} ";"]
 * 	{ "procedure" ident ";" block ";" } statement .
*/
void parse();
/**
 * statement = [ ident ":=" expression | "call" ident
 * 	| "?" ident | "!" expression
 * 	| "begin" statement {";" statement } "end"
 * 	| "if" condition "then" statement
 * 	| "while" condition "do" statement ].
*/
void parse_statement(const token_t *token);
/**
 * expression = [ "+"|"-"] term { ("+"|"-") term}.
*/
void parse_expression(const token_t *token);
/**
 * condition = "odd" expression |
 * 	expression ("="|"#"|"<"|"<="|">"|">=") expression .
*/
void parse_condition(const token_t *token);
/**
 * term = factor {("*"|"/") factor}.
*/
void parse_term(const token_t *token);
/**
 * factor = ident | number | "(" expression ")".
*/
void parse_factor(const token_t *token);

#endif /* PASER_H */