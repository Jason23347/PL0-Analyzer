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

context_t *context_tail;

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
context_init(FILE *instream, FILE *outstream)
{
	context_t *context = calloc(1, sizeof(context_t));
	if (!context) {
		fprintf(stderr, "Out of memory\n");
		return NULL;
	}

	context->instream = instream;
	context->outstream = outstream;

	context->token_tail = context->tokens;
	context->id_tail = context->idents;

	context->excute = true;
	context->scan = true;

	return context;
}

context_t *
context_fork(context_t *parent)
{
	context_t *context = NULL;
	if (!(context = context_init(context->instream, context->outstream)))
		return NULL;

	/* prev fot ident table, next for free */
	context->prev = parent;
	context_tail->next = context;
	context_tail = context;

	return context;
}

void
context_free(context_t *context)
{
	int i;
	ident_t *id = context->idents;
	token_t *token = context->tokens;

	for (i = 0; i < context->id_num; i++) {
		free(id->value);
		id = id->next;
	}

	if (context->id_num >= PREALLOC_ID_NUM) {
		for (id = context->tokens[PREALLOC_ID_NUM - 1].next; id != NULL;
		     id++) {
			ident_t *tmp = id;
			id = id->next;
			free(tmp);
		}
	}

	/* FIXME free(): invalid next size (fast) */
	for (i = 0; i < context->token_num - 1; i++) {
		free(token->value);
		token = token->next;
	}

	if (context->token_num >= PREALLOC_SYM_NUM) {
		for (token = context->tokens[PREALLOC_SYM_NUM - 1].next;
		     token != NULL; token++) {
			token_t *tmp = token;
			token = token->next;
			free(tmp);
		}
	}
}
