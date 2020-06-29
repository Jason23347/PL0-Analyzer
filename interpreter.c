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
ident_error(const context_t *context, const char *fmt, ...)
{
	extern pos_t err;
	va_list ap;

	va_start(ap, fmt);

	sprintf(context_top_restrict(context)->message, "interpreter:%d:%d: ", err.row,
		err.col);
	vsprintf(context_top_restrict(context)->message, fmt, ap);

	va_end(ap);

	exit(1);
}

ident_t *
ident_add(context_t *context, const token_t *token, IDENT type)
{
	ident_t *id;

	if (token->type != ident) {
		ident_error(context, "cannot declare ident of \"%s\" type",
			    sym2human(token->type));
		return NULL;
	}

	if (ident_find(context, token->value)) {
		ident_error(context, "cannot declare ident \"%s\" duplicately",
			    token->value);
		return NULL;
	}

	if (context->id_num > MAX_IDENT_NUM) {
		sprintf(context_top_restrict(context)->message, "Out of memory");
		exit(1);
	}

	id = context->idents + (context->id_num);

	strcpy(id->name, token->value);
	id->type = type;
	id->value = 0;

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
			ptr++;
		}
	}

	return NULL;
}

int
ident_assign(const context_t *context, ident_t *id, void *value)
{
	/* Skip if condition not match */
	if (!context->excute && id->type != procvar)
		return 0;

	if (id->value) {
		if (id->type == constvar) {
			ident_error(context, "cannot assign value to const %s",
				    id->name);
			return -1;
		}
	}

	if (id->type == procvar)
		id->value = (size_t)value;
	else
		id->value = *(size_t *)value;

	return 0;
}

void
ident_dump(context_t *context)
{
	const ident_t *ptr = context->idents;

	printf("+---------------+----------+\n"
	       "|          name |    value |\n"
	       "+---------------+----------+\n");
	for (size_t i = 0; i < context->id_num; i++) {
		if (ptr->type == procvar) {
			if (ptr->value)
				printf("|%14s |%9s |\n", ptr->name, "(addr)");
			else
				printf("|%14s |%9p |\n", ptr->name,
				       (void *)ptr->value);
		} else {
			printf("|%14s |%9ld |\n", ptr->name, ptr->value);
		}
		ptr++;
	}
	printf("+---------------+----------+\n");
}

int
operation(const context_t *context, int m, SYMBOL opt, int n)
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
		ident_error(context, "invalid operation: \"%s\"",
			    sym2human(opt));
		return 0;
	}
}

bool
condition(const context_t *context, int m, SYMBOL opt, int n)
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
		ident_error(context, "invalid operation token: \"%s\"",
			    sym2human(opt));
		return false;
	}
}