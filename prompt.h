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

#ifndef PROMPT_H
#define PROMPT_H

#include <stddef.h>

#define MAX_PROMPT_SIZE 64

typedef struct {
	char *buffer;
	size_t len;
	void *prev;
} prompt_t;

void prompt_setup(prompt_t *prompt, const char *str);
void prompt_fork(prompt_t *prompt, prompt_t *parent, const char *str);
void prompt_unfork(prompt_t *prompt);

#endif /* PROMPT_H */
