
#include "parser.h"

#include <stdarg.h>
#include <stdlib.h>

extern token_t *token_tail;
extern pos_t err;

static inline void
invalid_token(const token_t *token, SYMBOL assumed)
{
	fprintf(stderr,
		"syntax:%d:%d: syntax error, expected \"%s\" but got \"%s\".\n",
		err.row, err.col, sym2human(assumed), sym2human(token->type));
	exit(1);
}

/* Get next token, abort on error */
const token_t *
next_token()
{
	/* Get a symbol from input */
	int flag = getsym();
	/* Abort if get an invalid symbol */
	if (!flag)
		exit(1);
	/* Or add it into chain */
	token_add(flag);

	return token_tail;
}

void
assert_multi(const token_t *token, int num, ...)
{
	SYMBOL sym;
	va_list ap;

	va_start(ap, num);

	for (int i = 0; i < num; i++) {
		sym = va_arg(ap, SYMBOL);
		if (token->type == sym)
			return;
	}
	invalid_token(token, sym);

	va_end(ap);
}

void
parse()
{
	next_token();

	if (token_tail->type == constsym) { // const
		do {
			assert(next_token(), ident); // id
			assert(next_token(), eql); // =
			assert(next_token(), number); // 123
		} while (next_token()->type == comma); // ,

		assert(token_tail, semicolon); // ;
		next_token();
	}

	if (token_tail->type == varsym) { // var
		do {
			assert(next_token(), ident); // id
		} while (next_token()->type == comma); // ,

		assert(token_tail, semicolon); // ;
		next_token();
	}

	for (; token_tail->type == proceduresym;) { // procedure
		assert(next_token(), ident); // id
		assert(next_token(), semicolon); // ;

		parse(); // block

		assert(token_tail, semicolon); // ;
		next_token();
	}

	parse_statement(token_tail); // a := 1

	/* End of program */
	if (token_tail->type == period) // .
		return;
}

void
parse_statement(const token_t *token)
{
	if (token->type == ident) { // id
		assert(next_token(), becomes); // :=
		parse_expression(next_token()); // a + 1
	}

	else if (token->type == callsym) { // call
		assert(next_token(), ident); // id
		next_token();
	}

	else if (token->type == beginsym) { // begin
		parse_statement(next_token()); // a := 1
		/* Return when got an 'end' symbol,
			or assumed to be a semicolon with afterward other statements */
		if (token_tail->type == endsym) { // end
			next_token();
			return;
		}

		do {
			if (token_tail->type == semicolon) // ;
				parse_statement(next_token()); // a := 1
			else
				invalid_token(token_tail, semicolon);

		} while (token_tail->type != endsym); // end
	}

	else if (token->type == ifsym) { // if
		parse_condition(next_token()); // a > 1
		assert(token_tail, thensym); // then
		parse_statement(next_token()); // a := 1
	}

	else if (token->type == whilesym) { // while
		parse_condition(next_token()); // a > 1
		assert(token_tail, dosym); // do
		parse_statement(next_token()); // a := 1
	}

	else if (token->type == readsym) { // read
		assert(next_token(), lparen); // (

		do {
			assert(next_token(), ident); // id
		} while (next_token()->type == comma); // ,

		assert(token_tail, rparen); // )
		next_token();
	}

	else if (token->type == writesym) { // write
		assert(next_token(), lparen); // (

		next_token();
		do {
			parse_expression(token_tail); // a + 1
		} while (token_tail->type == comma); // ,

		assert(token_tail, rparen); // )
		next_token();
	}
}

void
parse_factor(const token_t *token)
{
	if (token->type == lparen) { // (
		parse_expression(next_token());
		assert(token_tail, rparen); // )
	} else
		assert_multi(token, 2, ident, number); // a 1
}

void
parse_term(const token_t *token)
{
	parse_factor(token);
	for (;;) {
		next_token();
		if (token_tail->type == times ||
		    token_tail->type == slash) { // * or /
			parse_factor(next_token());
			continue;
		}
		break;
	}

	for (;;) {
		if (token_tail->type == plus ||
		    token_tail->type == minus) { // + and -
			parse_term(next_token());
			continue;
		}
		break;
	}
}

void
parse_expression(const token_t *token)
{
	if (token->type == plus || token->type == minus) // + and -
		parse_term(next_token()); // a + 1
	else
		parse_term(token); // a + 1
}

void
parse_condition(const token_t *token)
{
	if (token->type == oddsym) // odd
		parse_expression(next_token()); // a + 1
	else {
		parse_expression(token); // a + 1

		// = # < <= > >=
		assert_multi(token_tail, 6, eql, neq, lss, leq, gtr, geq);

		parse_expression(next_token()); // b * 2
	}
}