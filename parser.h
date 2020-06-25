#ifndef PARSER_H
#define PARSER_H

#include "symbols.h"

void parse();
void parse_statement(const token_t *token);
void parse_expresion(const token_t *token);
void parse_condition(const token_t *token);

#endif /* PASER_H */