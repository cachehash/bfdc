#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "main.h"
#include "hash/hash.h"

#include "optimize.h"
#include "interp.h"
#include "comp.h"

#define INVALID_ARG 8
#define EXTRA_ARGS 7
#define INVALID_COMBINATION 6
#define NO_INPUT 2

extern int yyparse();
extern FILE* yyin;

int usage(int);
FILE* outfile;

Node* root;

int numCells = NUM_CELLS;

char* progName;

int main(int argc, char** argv) {
	progName = argv[0];
	int interpret = 0;
	char* outname = NULL;
	char* ifile = NULL;
	char opt;
	while ((opt = getopt (argc, argv, "thio:c:")) != -1) {
		switch (opt) {
			case 't':
				interpret = 2;
			break;
			case 'h':
				usage(0);
			break;
			case 'i':
				interpret = 1;
			break;
			case 'o':
				outname = strdup(optarg);
			break;
			case 'c':
				numCells = atoi(optarg);
			break;
			default:
				usage(INVALID_ARG);
		}
	}
	for (int i = optind; i < argc; i++) {
		if (ifile) {
			usage(EXTRA_ARGS);
		}
		ifile = argv[i];
	}
	if (outname && interpret) {
		usage(INVALID_COMBINATION);
	}
	if (ifile == NULL) {
		usage(NO_INPUT);
	}
	yyin = fopen(ifile, "r");
	yyparse();
	optimize(root);
	if (interpret) {
		if (interpret == 2) {
			interpRaw();
		} else {
			interp();
		}
	} else {
		if (outname == NULL) {
			outname = strdup("a.c");
		}
		outfile = fopen(outname, "w");
		free(outname);
		outname = NULL;
		comp();
	}
	clipBranch(&root);
}
int usage(int status) {
	FILE* f = stdout;
	if (status) {
		//f = stderr;
	}
	fprintf(f, "Usage: %s [OPTION]... FILE\n", progName);
	fprintf(f, "\
\n\
   -i        interpret the brainfuck code specified in FILE\n\
   -t        interpret the parse tree used for compilation instead of compiling\n\
   -o FILE   output name for the compiled brainfuck\n\
   -c NUM    number of cells to use\n\
   -h        display this help text\n\
\n\
Examples:\n\
   %s -i hello.b            interpret the contents of hello.b\n\
   %s hello.b -o hello.c    compile hello.b and output hello.c\n\
"
, progName, progName);
	exit(status);
}

