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

#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "symbols.h"
#include "keywords.h"

/* Current position and error position */
pos_t cur, err;
/* Preallocated symbol chain */
token_t tokens[PREALLOC_SYM_NUM];
/* Head and tail of the chain, the chain itself is hidden to others */
token_t *token_tail = tokens;

/* Max length of an ident */
#define MAX_IDENT 20

/* Ident variable */
char id[MAX_IDENT] = "";
/* Ident length */
int id_len;

/* Count for symbols */
int token_num;

int
get_char()
{
	int ch = fgetc(stdin);
	cur.col++;
	if (ch == '\n') {
		cur.row++;
		cur.col = 0;
	}
	return ch;
}

int
unget_char(int ch)
{
	ungetc(ch, stdin);
	cur.col--;
	if (ch == '\n')
		cur.row--;
	return 0;
}

/* Save symbol to id and return */
#define SAVE_SYM(str, sym)                                                     \
	{                                                                      \
		strcpy(id, str);                                               \
		return sym;                                                    \
	}

/* Print error info if symbol cannot be recongnized */
#define invalid_symbol()                                                       \
	{                                                                      \
		fprintf(stderr, "lex:%d:%d: invalid symbol: %s\n", err.row,    \
			err.col, id);                                          \
		return nul;                                                    \
	}

SYMBOL
getsym()
{
	int ch;

	while ((ch = get_char()) != EOF && ch <= ' ')
		;

	err.row = cur.row;
	err.col = cur.col;
	token_num++;

	switch (ch) {
	case EOF:
		SAVE_SYM("EOF", eof)
	case '+':
		SAVE_SYM("+", plus)
	case '-':
		SAVE_SYM("-", minus)
	case '*':
		SAVE_SYM("*", times)
	case '/':
		SAVE_SYM("/", slash)
	case '(':
		SAVE_SYM("(", lparen)
	case ')':
		SAVE_SYM(")", rparen)
	case ',':
		SAVE_SYM(",", comma)
	case ';':
		SAVE_SYM(";", semicolon)
	case '.':
		SAVE_SYM(".", period)
	case ':':
		ch = get_char();
		if (ch == '=') {
			SAVE_SYM(":=", becomes)
		} else
			invalid_symbol();
	case '=':
		SAVE_SYM("=", eql)
	case '#':
		SAVE_SYM("#", neq)
	case '<':
		ch = get_char();
		if (ch == '=') {
			SAVE_SYM("<=", leq)
		}
		unget_char(ch);
		SAVE_SYM("<", lss)
	case '>':
		ch = get_char();
		if (ch == '=')
			SAVE_SYM(">=", geq)

		unget_char(ch);
		SAVE_SYM(">", gtr)
	default:
		if (isdigit(ch)) {
			int num = 0, count = 0;
			do {
				num = 10 * num + ch - '0';
				count++;
				ch = get_char();
			} while (ch != EOF && isdigit(ch));
			sprintf(id, "%d", num);

			if (isalpha(ch)) {
				id[count] = ch;
				for (ch = get_char(); isalnum(ch);
				     ch = get_char())
					id[++count] = ch;

				invalid_symbol();
			}
			unget_char(ch);
			return number;
		} else if (isalpha(ch)) {
			id_len = 0;
			do {
				if (id_len < MAX_IDENT)
					id[id_len++] = ch;
				ch = get_char();
			} while (ch != EOF && isalnum(ch));
			id[id_len] = 0;

			unget_char(ch);
			return key2sym(id);
		}

		invalid_symbol();
	}
}

const char *
sym2human(SYMBOL sym)
{
	switch (sym) {
	case eof:
		return "EOF";
	case plus:
		return "+";
	case minus:
		return "-";
	case times:
		return "*";
	case slash:
		return "/";
	case lparen:
		return "(";
	case rparen:
		return ")";
	case comma:
		return ",";
	case semicolon:
		return ";";
	case period:
		return ".";
	case becomes:
		return ":=";
	case eql:
		return "=";
	case neq:
		return "#";
	case lss:
		return "<";
	case gtr:
		return ">";
	case leq:
		return "<=";
	case geq:
		return ">=";
	case number:
		return "number";
	case ident:
		return "ident";
	case beginsym:
		return "begin";
	case callsym:
		return "end";
	case constsym:
		return "const";
	case dosym:
		return "do";
	case endsym:
		return "end";
	case ifsym:
		return "if";
	case oddsym:
		return "odd";
	case proceduresym:
		return "proc";
	case readsym:
		return "read";
	case thensym:
		return "then";
	case varsym:
		return "var";
	case whilesym:
		return "while";
	case writesym:
		return "write";
	case nul:
	default:
		return "(null)";
	}
}

void
token_init()
{
	cur.row = 1;
	cur.col = 0;
	token_num = 0;
}

void
token_add(int flag)
{
	token_t *t;
	if (token_num < PREALLOC_SYM_NUM) {
		t = tokens + token_num - 1;
	} else {
		t = malloc(sizeof(token_t));
		if (!t) {
			fprintf(stderr, "Out of memory\n");
			exit(1);
		}
	}

	int len = strlen(id);
	t->no = token_num;
	t->value = malloc(len);
	t->type = flag;
	memcpy(t->value, id, len);

	token_tail->next = t;
	token_tail = t;
}

void
token_dump()
{
	printf("+-----+--------------------+--------------------+\n"
	       "|%4s |%19s |%19s |\n"
	       "+-----+--------------------+--------------------+\n",
	       "No", "Symbol", "Symbol Type");
	for (token_t *t = tokens; t != NULL; t = t->next) {
		printf("|%4d |%19s |%19s |\n", t->no, t->value,
		       sym2human(t->type));
	}
	printf("+-----+--------------------+--------------------+\n");
}