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

#include <stdarg.h>
#include <stdlib.h>

#include "interpreter.h"
#include "context.h"

static inline void
invalid_token(const token_t *token, SYMBOL assumed)
{
	extern pos_t err;

	fprintf(stderr,
		"syntax:%d:%d: syntax error, expected \"%s\" but got \"%s\".\n",
		err.row, err.col, sym2human(assumed), sym2human(token->type));
	exit(1);
}

void
assert_multi(const context_t *context, int num, ...)
{
	SYMBOL sym;
	va_list ap;

	va_start(ap, num);

	for (int i = 0; i < num; i++) {
		sym = va_arg(ap, SYMBOL);
		if (context->token_tail->type == sym)
			return;
	}
	invalid_token(context->token_tail, sym);

	va_end(ap);
}

void
parse(context_t *context)
{
	ident_t *id;

	context_next(context);

	if (context->token_tail->type == constsym) { // const
		do {
			assert(context_next(context), ident); // id
			id = ident_add(context, context->token_tail, constvar);

			assert(context_next(context), eql); // =

			assert(context_next(context), number); // 123
			ident_assign(context, id,
				     atoi(context->token_tail->value));
		} while (context_next(context)->token_tail->type == comma); // ,

		assert(context, semicolon); // ;
		context_next(context);
	}

	if (context->token_tail->type == varsym) { // var
		do {
			assert(context_next(context), ident); // id
			id = ident_add(context, context->token_tail, variable);
		} while (context_next(context)->token_tail->type == comma); // ,

		assert(context, semicolon); // ;
		context_next(context);
	}

	for (; context->token_tail->type == proceduresym;) { // procedure
		assert(context_next(context), ident); // id
		assert(context_next(context), semicolon); // ;

		context_t *new_context = context_init(1, sizeof(context_t));
		/* FIXME multi procedures */
		new_context->prev = context;
		context->next = new_context;
		parse(new_context); // block

		assert(context, semicolon); // ;
		context_next(context);
	}

	parse_statement(context); // a := 1

	/* End of program */
	if (context->token_tail->type == period) // .
		return;
}

void
parse_statement(context_t *context)
{
	if (context->token_tail->type == ident) { // id
		ident_t *id = ident_find(context, context->token_tail->value);
		if (!id)
			ident_undefined(context->token_tail->value);

		assert(context_next(context), becomes); // :=
		// a + 1
		int ret = parse_expression(context_next(context));

		if (id)
			ident_assign(context, id, ret);
	}

	else if (context->token_tail->type == callsym) { // call
		assert(context_next(context), ident); // id
		context_next(context);
	}

	else if (context->token_tail->type == beginsym) { // begin
		parse_statement(context_next(context)); // a := 1
		/* Return when got an 'end' symbol,
			or assumed to be a semicolon with afterward other statements */
		if (context->token_tail->type == endsym) { // end
			context_next(context);
			return;
		}

		do {
			if (context->token_tail->type == semicolon) // ;
				parse_statement(
					context_next(context)); // a := 1
			else
				invalid_token(context->token_tail, endsym);

		} while (context->token_tail->type != endsym); // end
	}

	else if (context->token_tail->type == ifsym) { // if
		bool tmp = context->excute;
		context->excute &= parse_condition(context_next(context));
		assert(context, thensym); // then
		parse_statement(context_next(context)); // a := 1
		context->excute = tmp;
	}

	else if (context->token_tail->type == whilesym) { // while
		token_t *hook = context->token_tail;
		while (context->excute &&
		       parse_condition(context_next(context))) {
			assert(context, dosym); // do
			parse_statement(context_next(context)); // a := 1
			context->scan = false;
			context->token_tail = hook;
		}
		context->scan = true;
		for (; context->token_tail->next;
		     context->token_tail = context->token_tail->next)
			;
	}

	else if (context->token_tail->type == readsym) { // read
		assert(context_next(context), lparen); // (

		do {
			assert(context_next(context), ident); // id
			int tmp;
			ident_t *id =
				ident_find(context, context->token_tail->value);
			if (!id) {
				ident_undefined(context->token_tail->value);
			} else {
				if (scanf("%d", &tmp))
					ident_assign(context, id, tmp);
			}
		} while (context_next(context)->token_tail->type == comma); // ,

		assert(context, rparen); // )
		context_next(context);
	}

	else if (context->token_tail->type == writesym) { // write
		assert(context_next(context), lparen); // (

		context_next(context);
		do {
			parse_expression(context); // a + 1
		} while (context->token_tail->type == comma); // ,

		assert(context, rparen); // )
		context_next(context);
	}
}

int
parse_factor(context_t *context)
{
	int ret;

	if (context->token_tail->type == lparen) { // (
		ret = parse_expression(context_next(context));
		assert(context, rparen); // )
	}

	else if (context->token_tail->type == ident) { // a
		ident_t *id = ident_find(context, context->token_tail->value);
		if (!id) {
			ident_undefined(context->token_tail->value);
			return 0;
		}
		ret = ident_value(context, id);
	}

	else if (context->token_tail->type == number) // 1
		ret = atoi(context->token_tail->value);

	else {
		extern pos_t err;
		fprintf(stderr, "syntax:%d:%d: invalid factor\n", err.col,
			err.row);
	}

	return ret;
}

int
parse_term(context_t *context)
{
	int ret = parse_factor(context);

	for (;;) {
		context_next(context);
		if (context->token_tail->type == times ||
		    context->token_tail->type == slash) { // * or /
			SYMBOL opt = context->token_tail->type;
			ret = operation(ret, opt,
					parse_factor(context_next(context)));
			continue;
		}
		break;
	}

	for (;;) {
		if (context->token_tail->type == plus ||
		    context->token_tail->type == minus) { // + and -
			SYMBOL opt = context->token_tail->type;
			ret = operation(ret, opt,
					parse_term(context_next(context)));
			continue;
		}
		break;
	}

	return ret;
}

int
parse_expression(context_t *context)
{
	if (context->token_tail->type == plus ||
	    context->token_tail->type == minus) // + and -
		return parse_term(context_next(context)); // a + 1
	else
		return parse_term(context); // a + 1
}

bool
parse_condition(context_t *context)
{
	int ret;
	if (context->token_tail->type == oddsym) // odd
		return 0 == parse_expression(context_next(context)); // a + 1

	ret = parse_expression(context); // a + 1

	// = # < <= > >=
	assert_multi(context, 6, eql, neq, lss, leq, gtr, geq);

	SYMBOL opt = context->token_tail->type;
	return condition(ret, opt, parse_expression(context_next(context)));
}