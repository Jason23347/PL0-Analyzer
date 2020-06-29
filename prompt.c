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

#include "prompt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void
prompt_setup(prompt_t *prompt, const char *str)
{
    int len = strlen(str);
    memcpy(prompt->buffer, str, len + 1);
    prompt->length[0] = len;
    prompt->depth = 0;
}

void
prompt_step_in(prompt_t *prompt, const char *str)
{
	int depth = prompt->depth;
	strcat(prompt->buffer, str);
	prompt->length[depth + 1] = prompt->length[depth] + strlen(str);

	int len = prompt->length[depth + 1];
	prompt->buffer[len + 1] = 0;
	prompt->depth++;
}

void
prompt_step_out(prompt_t *prompt)
{
	int depth = prompt->depth;
	int len = prompt->length[depth - 1];
	prompt->buffer[len] = 0;
	prompt->depth--;
}