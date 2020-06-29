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

#include "keywords.h"
#include "context.h"

/* Current position and error position */
pos_t cur, err;

/* Max length of an ident */
#define MAX_IDENT 20

/* Ident variable */
char id[MAX_IDENT] = "";
/* Ident length */
int id_len;

int
get_char(context_t *context)
{
	int ch = fgetc(context->instream);
	cur.col++;
	if (ch == '\n') {
		cur.row++;
		cur.col = 0;
	}
	return ch;
}

int
unget_char(context_t *context, int ch)
{
	ungetc(ch, context->instream);
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
		sprintf(context_top(context)->message,                         \
			"lex:%d:%d: invalid symbol: %s", err.row, err.col,     \
			id);                                                   \
		return nul;                                                    \
	}

SYMBOL
getsym(context_t *context)
{
	int ch;

	while ((ch = get_char(context)) != EOF && ch <= ' ')
		;

	err.row = cur.row;
	err.col = cur.col;

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
		ch = get_char(context);
		if (ch == '=') {
			SAVE_SYM(":=", becomes)
		} else
			invalid_symbol();
	case '=':
		SAVE_SYM("=", eql)
	case '#':
		SAVE_SYM("#", neq)
	case '<':
		ch = get_char(context);
		if (ch == '=') {
			SAVE_SYM("<=", leq)
		}
		unget_char(context, ch);
		SAVE_SYM("<", lss)
	case '>':
		ch = get_char(context);
		if (ch == '=')
			SAVE_SYM(">=", geq)

		unget_char(context, ch);
		SAVE_SYM(">", gtr)
	default:
		if (isdigit(ch)) {
			int num = 0, count = 0;
			do {
				num = 10 * num + ch - '0';
				count++;
				ch = get_char(context);
			} while (ch != EOF && isdigit(ch));
			sprintf(id, "%d", num);

			if (isalpha(ch)) {
				id[count] = ch;
				for (ch = get_char(context); isalnum(ch);
				     ch = get_char(context))
					id[++count] = ch;

				invalid_symbol();
			}
			unget_char(context, ch);
			return number;
		} else if (isalpha(ch)) {
			id_len = 0;
			do {
				if (id_len < MAX_IDENT)
					id[id_len++] = ch;
				ch = get_char(context);
			} while (ch != EOF && isalnum(ch));
			id[id_len] = 0;

			unget_char(context, ch);
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
}

void
token_add(context_t *context, int flag)
{
	token_t *t;
	if (context->token_num < PREALLOC_SYM_NUM) {
		t = context->tokens + context->token_num;
	} else {
		t = malloc(sizeof(token_t));
		if (!t) {
			sprintf(context_top(context)->message, "Out of memory");
			exit(1);
		}
	}

	int len = strlen(id);
	t->value = malloc(len + 1);
	if (!t->value) {
		fprintf(stderr, "Out of mempry\n");
		exit(1);
	}
	t->type = flag;
	memcpy(t->value, id, len + 1);

	context->token_tail->next = t;
	context->token_tail = t;

	context->token_num++;
}

void
token_dump(context_t *context)
{
	const token_t *t = context->tokens;

	printf("+-----+--------------------+--------------------+\n"
	       "|%4s |%19s |%19s |\n"
	       "+-----+--------------------+--------------------+\n",
	       "No", "Symbol", "Symbol Type");
	for (int i = 1; i < context->token_num; i++) {
		printf("|%4d |%19s |%19s |\n", i, t->value, sym2human(t->type));
		t = t->next;
	}
	printf("+-----+--------------------+--------------------+\n");
}