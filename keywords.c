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

#include "keywords.h"

const keyword_t keywords[14] = {
	{ .value = "begin", .symbol = beginsym },
	{ .value = "call", .symbol = callsym },
	{ .value = "const", .symbol = constsym },
	{ .value = "do", .symbol = dosym },
	{ .value = "end", .symbol = endsym },
	{ .value = "if", .symbol = ifsym },
	{ .value = "odd", .symbol = oddsym },
	{ .value = "procedure", .symbol = proceduresym },
	{ .value = "read", .symbol = readsym },
	{ .value = "then", .symbol = thensym },
	{ .value = "var", .symbol = varsym },
	{ .value = "while", .symbol = whilesym },
	{ .value = "write", .symbol = writesym },
	{ .value = "", .symbol = ident } // fallback to ident
};

SYMBOL
key2sym(const char *id)
{
	const keyword_t *ptr;
	for (ptr = keywords; ptr - keywords < 13; ptr++) {
		if (!strcmp(ptr->value, id))
			break;
	}
	return ptr->symbol;
}
