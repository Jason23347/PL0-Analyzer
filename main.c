#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <getopt.h>

#include "symbols.h"
#include "keywords.h"
#include "parser.h"

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
	char *infile, *outfile = 0;
	clock_t start, finish;
	int len;

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
	if (!outfile) {
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

	/* Almost ready for lexical analysis */
	printf("Lexical analysis result:\n\n");

	/* "" or "-" for stdout */
	if (!(outfile[0] == 0 || (outfile[0] == '-' && outfile[1] == 0))) {
		printf("Redirecting output to file: %s", outfile);
		if (freopen(outfile, "w+", stdout) == 0){
			perror(outfile);
			return 1;
		}
	}

	/* Record start time */
	start = clock();

	/* Initialize variables */
	token_init();
	while (!feof(stdin)) {
		/* Break on yass errors but print results of lex */
		parse();
	}

	/* Record end time */
	finish = clock();

	/* Print results of lex */
	token_dump();

	/* Calculate time spent */
	printf("\n"
	       "Lexical and syntax analysis completed (%2.3f secs)\n",
	       (double)(finish - start) / CLOCKS_PER_SEC);

	return 0;
}
