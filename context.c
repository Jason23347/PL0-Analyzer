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

#include "context.h"

#include <stdlib.h>

/* Get next token, abort on error */
context_t *
context_next(context_t *context)
{
	if (!context->scan) {
		context->token_tail = context->token_tail->next;
		return context;
	}

	/* Get a symbol from input */
	int flag = getsym(context);
	/* Abort if get an invalid symbol */
	if (!flag)
		exit(1);
	/* Or add it into chain */
	token_add(context, flag);

	return context;
}

context_t *
context_prev(context_t *context)
{
	if (!context->token_num)
		return context;

	context->token_tail = context->token_last_tail;
	context->token_num--;

	return context;
}

void
context_init(context_t *context, FILE *instream, FILE *outstream)
{
	context->instream = instream;
	context->outstream = outstream;

	context->token_tail = context->tokens;
	context->token_num = 0;

	context->prev = 0;

	context->excute = true;
	context->scan = true;
}

context_t *
context_fork(context_t *parent)
{
	context_t *context;
	if (!(context = calloc(1, sizeof(context_t))))
		goto no_mem;

	/* Basic setups */
	context_init(context, parent->instream, parent->outstream);

	/* prev for ident table */
	context->prev = parent;

	return context;

no_mem:
	sprintf(context_top(context)->message, "Out of memory");
	exit(1);
}

const context_t *
context_top(const context_t *context)
{
	const context_t *tmp;
	for (tmp = context; tmp->prev; tmp = context->prev)
		;
	return tmp;
}
