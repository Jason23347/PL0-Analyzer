#include "parser.h"

#include <stdarg.h>
#include <stdlib.h>

extern token_t *token_tail;
extern pos_t err;
extern const char *symtype[33];

static inline void
invalid_token(const token_t *token, SYMBOL assumed)
{
	fprintf(stderr,
		"syntax:%d:%d: syntax error, expected \"%s\" but got \"%s\".\n",
		err.row, err.col, symtype[assumed], symtype[token->type]);
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

static void
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

#define assert(token, sym) assert_multi(token, 1, sym)

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
	}

	if (token_tail->type == varsym) { // var
		do {
			assert(next_token(), ident); // id
		} while (next_token()->type == comma); // ,

		assert(token_tail, semicolon); // ;
	}

	if (token_tail->type == proceduresym) { // procedure
		assert(next_token(), ident); // id
		assert(next_token(), semicolon); // ;

		assert(next_token(), beginsym); // begin
		next_token();
		do {
			parse_statement(token_tail); // a := 1
			assert(token_tail, semicolon); // ;
		} while (next_token()->type != endsym); // end
	}

	/* End of program */
	if (token_tail->type == period) // .
		return;

	parse_statement(token_tail); // a := 1
}

void
parse_statement(const token_t *token)
{
	if (token->type == ident) { // id
		assert(next_token(), becomes); // :=
		parse_expresion(next_token()); // a + 1
	}

	else if (token->type == callsym) {// call
		assert(next_token(), ident); // id
		next_token();
	}

	else if (token->type == beginsym) { // begin
		next_token();
		do {
			parse_statement(token_tail); // a := 1
			assert(token_tail, semicolon); // ;
		} while (next_token()->type != endsym); // end
		next_token();
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
			parse_expresion(token_tail); // a + 1
		} while (token_tail->type == comma); // ,

		assert(token_tail, rparen); // )
		next_token();
	}
}

static void
parse_factor(const token_t *token)
{
	if (token->type == lparen) { // (
		parse_expresion(next_token());
		assert(token_tail, rparen); // )
	} else
		assert_multi(token, 2, ident, number); // a 1
}

static inline void
parse_item(const token_t *token)
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
			parse_item(next_token());
			continue;
		}
		break;
	}
}

inline void
parse_expresion(const token_t *token)
{
	if (token->type == plus || token->type == minus) // + and -
		parse_item(next_token()); // a + 1
	else
		parse_item(token); // a + 1
}

void
parse_condition(const token_t *token)
{
	if (token->type == oddsym) // odd
		parse_expresion(next_token()); // a + 1
	else {
		parse_expresion(token); // a + 1

		// = # < <= > >=
		assert_multi(token_tail, 6, eql, neq, lss, leq, gtr, geq);

		parse_expresion(next_token()); // b * 2
	}
}