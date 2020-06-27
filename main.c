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
#include <time.h>
#include <string.h>
#include <getopt.h>

#include "context.h"

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
	printf("Usage: %s [options] infile\n"
	       "Options:\n"
	       "  -o\tSpecify output file",
	       argv[0]);
}

int
main(int argc, char *argv[])
{
	char *infile, *outfile = "";
	int len;
	context_t *context;

	for (int option; (option = getopt(argc, argv, "o:")) != -1;) {
		switch (option) {
		case 'o':
			len = strlen(optarg);
			outfile = malloc(len);
			if (!outfile) {
				fprintf(stderr, "Out of memory\n");
				return 1;
			}
			memcpy(outfile, optarg, len);
			break;
		default:
			print_help(argv);
			return 1;
		}
	}

	if (argc - optind < 1) {
		print_help(argv);
		return 1;
	}
	print_version();

	/* Open input file */
	infile = argv[optind];
	if (!(infile[0] == '-' && infile[1] == 0))
		if ((freopen(infile, "r", stdin)) == 0) {
			perror(infile);
			return 1;
		}

	/**
	 * Generate a default filename
	 * file		-> file.out
	 * file.pl0	-> file.out
	 */
	if (strcmp(infile, "-") && !outfile) {
		len = strlen(infile);
		outfile = malloc(len + 4);
		memcpy(outfile, infile, len);

		char *t = 0;
		char *p = 0, *q = 0;

		for (t = outfile; (t = strpbrk(t + 1, ".")) != 0; p = t)
			;
		for (t = outfile; (t = strpbrk(t + 1, "/")) != 0; q = t)
			;

		if (p < q) /* Input file has no extension */
			strcat(outfile, ".out");
		else
			memcpy(p, ".out", 5);
	}

	/* "-" for stdout */
	if (outfile[0] == '-' && outfile[1] == 0) {
		printf("Redirecting output to file: %s", outfile);
		if (freopen(outfile, "w+", stdout) == 0) {
			perror(outfile);
			return 1;
		}
	}

	/* Initialize variables */
	token_init();
	context = context_init();
	while (!feof(stdin)) {
		/* FIXME: hanle returing code instead of exit on errors */
		parse(context);
	}

	ident_dump(context);
	context_free(context);

	return 0;
}
