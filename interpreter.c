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

#include "interpreter.h"

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "context.h"

void
ident_error(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);

	fprintf(stderr, "interpreter: ");
	vfprintf(stderr, fmt, ap);

	va_end(ap);
}

ident_t *
ident_add(context_t *context, const token_t *token, IDENT type)
{
	ident_t *id;

	if (token->type != ident) {
		ident_error("cannot add ident of \"%s\" type\n",
			    sym2human(token->type));
		return NULL;
	}

	if (ident_find(context, token->value)) {
		ident_error("cannot add ident \"%s\" duplicately\n",
			    token->value);
		return NULL;
	}

	if (context->id_num < PREALLOC_ID_NUM)
		id = context->idents + context->id_num;
	else {
		id = malloc(sizeof(ident_t));
		if (!id) {
			fprintf(stderr, "Out of memory\n");
			exit(1);
		}
	}

	id->name = token->value;
	id->type = type;
	id->value = NULL;

	context->id_tail->next = id;
	context->id_tail = id;

	context->id_num++;

	return id;
}

ident_t *
ident_find(context_t *context, const char *name)
{
	for (context_t *c = context; c != NULL; c = c->prev) {
		ident_t *ptr = c->idents;
		for (int i = 0; i < c->id_num; i++) {
			if (!strcmp(ptr->name, name))
				return ptr;
			ptr = ptr->next;
		}
	}

	return NULL;
}

int
ident_assign(const context_t *context, ident_t *id, int value)
{
	/* Skip if condition not match */
	if (!context->excute)
		return 0;

	if (id->value) {
		if (id->type == constvar) {
			ident_error("cannot assign value to const %s\n",
				    id->name);
			return -1;
		}
	} else
		id->value = malloc(sizeof(int));

	*id->value = value;

	return 0;
}

int
ident_value(const context_t *context, ident_t *id)
{
	if (!id->value) {
		ident_error("variable %s not initialed, default to 0\n",
			    id->name);
		return 0;
	}

	return *id->value;
}

void
ident_dump(context_t *context)
{
	const ident_t *ptr = context->idents;

	printf("+---------------+----------+\n"
	       "|          name |    value |\n"
	       "+---------------+----------+\n");
	for (int i = 0; i < context->id_num; i++) {
		if (ptr->value)
			printf("|%14s |%9d |\n", ptr->name, *ptr->value);
		else
			printf("|%14s |%9s |\n", ptr->name, "(null)");
		ptr = ptr->next;
	}
	printf("+---------------+----------+\n");
}

int
operate(int m, SYMBOL opt, int n)
{
	switch (opt) {
	case plus:
		return m + n;
	case minus:
		return m - n;
	case times:
		return m * n;
	case slash:
		return m / n;
	default:
		ident_error("invalid operation: \"%s\"\n", sym2human(opt));
		return 0;
	}
}

bool
condition(int m, SYMBOL opt, int n)
{
	switch (opt) {
	case eql:
		return m == n;
	case neq:
		return m != n;
	case lss:
		return m < n;
	case leq:
		return m <= n;
	case gtr:
		return m > n;
	case geq:
		return m >= n;
	default:
		ident_error("invalid operation token: \"%s\"\n",
			    sym2human(opt));
		return false;
	}
}