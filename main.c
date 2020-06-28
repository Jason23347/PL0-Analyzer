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

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <pthread.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/sem.h>

#include "context.h"

// #define NDEBUG

/* Print version and copyright */
static inline void
print_version()
{
	printf("PL/0 Analizer Copyright 2020\tShuaiCheng Zhu\n\n"
	       "This program is distributed in the hope that it will be useful,\n"
	       "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
	       "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
	       "GNU General Public License for more details.\n\n");
}

/* Print help and usage */
static inline void
print_help(char **argv)
{
	printf("Usage: %s [options] [infile]\n", argv[0]);
}

void
cli_run(char *filename)
{
	char *line = 0;
	int fd[2];
	int status, process_count = 0;
	FILE *instream;
	pid_t pid;
	struct timeval start, end;
	key_t key;
	int sem;
	context_t *context;
	extern context_t *context_tail;

	if (!(key = ftok(filename, 0))) {
		perror("generate key");
		exit(1);
	}

	/* CLI mode readline */
	while ((line = readline("PL0> ")) != NULL) {
		/* Chile thread not setup */
		if (!process_count) {
			/* Setup pipe */
			if (pipe(fd) == -1) {
				perror("pipe creatation");
				continue;
			}
			if (!(instream = fdopen(fd[0], "r"))) {
				perror("pipe input");
				continue;
			}

			/* Setup semaphore */
			if (!(sem = semget(key, 1, IPC_EXCL))) {
				perror("semaphore");
				continue;
			}

			/* Initialize semaphore */
			semctl(sem, 1, SETVAL);

			/* Initialize interpreter */
			token_init();
			context = context_init(instream, stdout);
			prompt_setup(context->prompt, "PL0> ");
			context_tail = context;

			/* Create a child thread */
			pid = fork();
			if (pid == 0) { /* Child thread */
				/* Close output pipe */
				close(fd[1]);
				/* Run interpreter */
				parse(context);
				/* Exit with 0 if no error,
					or 1 handled by interpreter */
				exit(0);
			}

			/* thread creatation failed */
			if (pid < 0) {
				perror("thread create");
				exit(1);
			}

			/* Close input pipe for father thread */
			close(fd[0]);
		}

		/* Deal with input string */
		if (strcmp(line, "")) {
			/* Write line to pipe */
			if (!write(fd[1], line, strlen(line)) ||
			    !write(fd[1], "\n", 1)) {
				perror("pipe write error");
				exit(1);
			}
		}

		/* Father thread */
		process_count++;
		/* Set start time for timeout */
		gettimeofday(&start, 0);
		/* initialize status for waitpid */
		status = -1;
		/* non-blocking hanlding with timeout */
		while (waitpid(pid, &status, WNOHANG)) {
			gettimeofday(&end, 0);
			/* Set timeout for 1 sec */
			if ((end.tv_sec - start.tv_sec) * 1000000 +
				    (end.tv_usec - start.tv_usec) >
			    100000) {
				fprintf(stderr, "interpreter timeout\n");
				break;
			}
			/* Child thread exited */
			if (WIFEXITED(status)) {
				/* Child thread exited with non-zero value */
				if (WEXITSTATUS(status))
					/* Print error message */
					context_error(context);
				break;
			}
		}
		process_count = 0;

#if !defined(NDEBUG)
		/* Debug info */
		printf("\nIdent table:\n");
		ident_dump(context);
#endif
	}
}

int
main(int argc, char *argv[])
{
	const char *infile = 0;
	int is_cli_mode = 1;
	FILE *instream;

	for (int option; (option = getopt(argc, argv, "hv")) != -1;) {
		switch (option) {
		case 'v':
			print_version();
			break;
		case 'h':
		default:
			print_help(argv);
			return 1;
		}
	}

	/* Open input file */
	infile = argv[optind];
	if (infile) {
		is_cli_mode = 0;
		if (!(instream = fopen(infile, "r")))
			perror(infile);
	}

	if (is_cli_mode)
		cli_run(argv[0]);

	return 0;
}
