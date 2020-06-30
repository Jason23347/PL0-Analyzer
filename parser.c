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
#include <string.h>

#include "interpreter.h"
#include "prompt.h"
#include "context.h"

static inline void
invalid_token_tail(const context_t *context, SYMBOL assumed)
{
	extern pos_t err;

	sprintf(context_top_restrict(context)->message,
		"syntax:%d:%d: syntax error, expected \"%s\" but got \"%s\"",
		err.row, err.col, sym2human(assumed),
		sym2human(context->token_tail->type));
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
	invalid_token_tail(context, sym);

	va_end(ap);
}

void
parse(context_t *context)
{
	ident_t *id;

	if (context->token_tail->type == constsym) { // const
		do {
			assert(context_next(context), ident); // id
			id = ident_add(context, context->token_tail, constvar);

			assert(context_next(context), eql); // =

			assert(context_next(context), number); // 123
			size_t tmp = atoi(context->token_tail->value);
			ident_assign(context, id, &tmp);
		} while (context_next(context)->token_tail->type == comma); // ,

		assert(context, semicolon); // ;
		if (context_next(context)->token_tail->type == period) {
			context_prev(context);
			if (context_top(context)->depth)
				context_next(context);
		}
	}

	if (context->token_tail->type == varsym) { // var
		do {
			assert(context_next(context), ident); // id
			id = ident_add(context, context->token_tail, variable);
		} while (context_next(context)->token_tail->type == comma); // ,

		assert(context, semicolon); // ;
		if (context_next(context)->token_tail->type == period) {
			context_prev(context);
			if (context_top(context)->depth)
				context_next(context);
		}
	}

	for (; context->token_tail->type == proceduresym;) { // procedure
		bool is_multi_lined = false;
		assert(context_next(context), ident); // id
		ident_t *id = ident_add(context, context->token_tail, procvar);

		assert(context_next(context), semicolon); // ;
		if (context_next(context)->token_tail->type == period) {
			prompt_step_in(context_top(context)->prompt, "proc> ");
			context_top(context)->depth++;
			is_multi_lined = true;
			context_prev(context);
			context_next(context);
		}

		context_t *new_context = context_fork(context);
		if (!new_context)
			return;
		new_context->excute = false;
		ident_assign(context, id, new_context);

		strcpy(new_context->tokens[0].value,
		       context->token_tail->value);
		new_context->tokens[0].type = context->token_tail->type;
		new_context->token_num++;
		context_prev(context);

		parse(new_context); // block

		if (is_multi_lined) {
			context_top(context)->depth--;
			prompt_step_out(context_top(context)->prompt);
		}

		assert(context, semicolon); // ;
		context_next(context);
		new_context->excute = true;
		new_context->scan = false;
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
		size_t ret = parse_expression(context_next(context));

		ident_assign(context, id, &ret);
	}

	else if (context->token_tail->type == callsym) { // call
		assert(context_next(context), ident); // id
		ident_t *id = ident_find(context, context->token_tail->value);
		if (id) {
			context_t *tmp = (context_t *)id->value;
			/* TODO not so elegant */
			token_t t = { .next = tmp->tokens };
			tmp->token_tail = &t;
			parse(tmp);
		}
		context_next(context);
	}

	else if (context->token_tail->type == beginsym) { // begin
		bool is_multi_lined = 0;
		if (context_next(context)->token_tail->type == period) {
			is_multi_lined = 1;
			context_top(context)->depth++;
			prompt_step_in(context_top(context)->prompt, ">> ");
			context_prev(context);
			context_next(context);
		}
		parse_statement(context); // a := 1
		if (is_multi_lined && context->token_tail->type == period) {
			context_prev(context);
			context_next(context);
		}

		/* Return when got an 'end' symbol,
			or assumed to be a semicolon with afterward other statements */
		if (context->token_tail->type == endsym) { // end
			context_next(context);
			return;
		}

		do {
			if (!(context->token_tail->type == semicolon)) // ;
				invalid_token_tail(context, endsym);

			if (is_multi_lined &&
			    context_next(context)->token_tail->type == period) {
				context_prev(context);
				context_next(context);
			}

			parse_statement(context); // a := 1

			if (is_multi_lined &&
			    context->token_tail->type == period) {
				context_prev(context);
				context_next(context);
			}
		} while (context->token_tail->type != endsym); // end

		if (is_multi_lined) {
			context_top(context)->depth--;
			prompt_step_out(context_top(context)->prompt);
		}
	}

	else if (context->token_tail->type == ifsym) { // if
		bool excute = context->excute;
		context->excute &= parse_condition(context_next(context));
		assert(context, thensym); // then
		/* FIXME this should only be valid in CLI mode */
		if (context_next(context)->token_tail->type == period) {
			context_prev(context);
			prompt_step_in(context_top(context)->prompt, "if> ");
			context_top(context)->depth++;
			parse_statement(context_next(context));
			context_top(context)->depth--;
			prompt_step_out(context_top(context)->prompt);
		} else
			parse_statement(context); // a := 1

		context->excute = excute;
	}

	else if (context->token_tail->type == whilesym) { // while
		token_t *token_hook = context->token_tail;
		bool excute = context->excute;
		bool is_multi_lined = false;
		context->excute &= parse_condition(context_next(context));

		assert(context, dosym); // do
		if (context_next(context)->token_tail->type == period) {
			is_multi_lined = true;
			context_prev(context);
			prompt_step_in(context_top(context)->prompt, "loop >");
			context_top(context)->depth++;
			context_next(context);
		}

		parse_statement(context);
		context->scan = false;

		if (is_multi_lined) {
			context_top(context)->depth--;
			prompt_step_out(context_top(context)->prompt);
		}

		if (!context->excute) {
			context->excute = excute;
			return;
		}

		context->token_tail = token_hook;
		while (context->excute &&
		       parse_condition(context_next(context))) {
			assert(context, dosym); // do
			parse_statement(context_next(context));
			context->token_tail = token_hook;
		};
		context->scan = true;
		for (; context->token_tail->next;
		     context->token_tail = context->token_tail->next)
			;
	}

	else if (context->token_tail->type == readsym) { // read
		assert(context_next(context), lparen); // (

		do {
			assert(context_next(context), ident); // id
			size_t tmp;
			ident_t *id =
				ident_find(context, context->token_tail->value);
			if (!id) {
				ident_undefined(context->token_tail->value);
			} else {
				if (scanf("%ld", &tmp))
					ident_assign(context, id, &tmp);
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
		ret = (int)id->value;
	}

	else if (context->token_tail->type == number) // 1
		ret = atoi(context->token_tail->value);

	else {
		extern pos_t err;
		sprintf(context_top_restrict(context)->message,
			"syntax:%d:%d: invalid factor", err.col, err.row);
		exit(1);
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
			ret = operation(context, ret, opt,
					parse_factor(context_next(context)));
			continue;
		}
		break;
	}

	for (;;) {
		if (context->token_tail->type == plus ||
		    context->token_tail->type == minus) { // + and -
			SYMBOL opt = context->token_tail->type;
			ret = operation(context, ret, opt,
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
	return condition(context, ret, opt,
			 parse_expression(context_next(context)));
}
