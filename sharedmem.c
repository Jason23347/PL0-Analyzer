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

#include "sharedmem.h"

#include <sys/shm.h>

#define NULL (void *)0

int
shm_setup(shm_t *shm)
{
	shm->id = shmget(0, shm->len, IPC_CREAT | 0600);
	return shm->id;
}

void *
shm_attach(shm_t *shm)
{
	shm->ptr = shmat(shm->id, NULL, 0);
	return shm->ptr;
}

int
shm_detach(shm_t *shm)
{
	return shmdt(shm->ptr);
}