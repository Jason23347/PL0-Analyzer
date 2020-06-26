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

const char *keywords[13] = { "begin", "call",  "const",	    "do",   "end",
			     "if",    "odd",   "procedure", "read", "then",
			     "var",   "while", "write" };

int
sym2key(const char *id)
{
	for (const char **ptr = keywords; ptr - keywords < 13; ptr++) {
		if (!strcmp(*ptr, id))
			return (ptr - keywords);
	}
	return -1;
}

SYMBOL
key2num(int num)
{
	switch (num) {
	case 0:
		return beginsym;
	case 1:
		return callsym;
	case 2:
		return constsym;
	case 3:
		return dosym;
	case 4:
		return endsym;
	case 5:
		return ifsym;
	case 6:
		return oddsym;
	case 7:
		return proceduresym;
	case 8:
		return readsym;
	case 9:
		return thensym;
	case 10:
		return varsym;
	case 11:
		return whilesym;
	case 12:
		return writesym;
	default:
		return nul;
	}
}
