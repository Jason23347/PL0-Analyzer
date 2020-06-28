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

char prompt_buff[MAX_PROMPT_SIZE];

void
prompt_fork(prompt_t *prompt, prompt_t *parent, const char *str)
{
	prompt->prev = parent;
	strcat(prompt->buffer, str);
	prompt->len = parent->len + strlen(str);
}

void
prompt_unfork(prompt_t *prompt)
{
	if (prompt->prev) {
		prompt_t *prev = prompt->prev;
		prompt->buffer[prev->len] = 0;
	}
	free(prompt);
}

void
prompt_setup(prompt_t *prompt, const char *str)
{
	prompt->len = strlen(str);
	memcpy(prompt_buff, str, prompt->len);
    prompt->buffer = prompt_buff;
}