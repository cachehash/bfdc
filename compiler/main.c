#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "main.h"
#include "hash/hash.h"

#include "optimize.h"
#include "interp.h"
#include "dynarec.h"
#include "comp.h"
#include "main.h"

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

int eofType = EOF_M1;
char* eofStr = "-1";
char* cell_t_str = NULL;

char* progName;

int main(int argc, char** argv) {
	progName = argv[0];
	int interpret = 0;
	char* outname = NULL;
	char* ifile = NULL;
	char opt;
	//TODO check if it's safe to not strdup optarg
	while ((opt = getopt (argc, argv, "jdthio:c:E:C:")) != -1) {
		switch (opt) {
			case 'j':
			case 'd':
				dynarec();
				exit(0);
			break;
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
				free(outname);
				outname = strdup(optarg);
			break;
			case 'c':
				numCells = atoi(optarg);
			break;
			case 'C':
				free(cell_t_str);
				cell_t_str = strdup(optarg);
			break;
			case 'E':
				if (strcmp("unchanged", optarg) == 0 || strcmp("nc", optarg) == 0) {
					eofType = EOF_NC;
					eofStr = "prev";
				} else if (strcmp("-1", optarg) == 0) {
					eofType = EOF_M1;
					eofStr = "-1";
				} else if (strcmp("0", optarg) == 0) {
					eofType = EOF_0;
					eofStr = "0";
				} else {
					usage(INVALID_ARG);
				}
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
	if ((outname && interpret) || (cell_t_str && interpret)) {
		usage(INVALID_COMBINATION);
	}
	cell_t_str = strdup("uint8_t");
	if (ifile == NULL) {
		usage(NO_INPUT);
	}
	if (strcmp(ifile, "-") == 0) {
		int in = dup(0);
		close(0);
		yyin = fdopen(in, "r");
	} else {
		yyin = fopen(ifile, "r");
	}
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
   -j -d     use dynamic recompilation to run the program\n\
   -o FILE   output name for the compiled brainfuck\n\
   -c NUM    number of cells to use\n\
   -C TYPE   data type to use for cells\n\
   -E EOF    format for EOF to use\n\
   -h        display this help text\n\
\n\
Examples:\n\
   %s -i hello.b            interpret the contents of hello.b\n\
   %s hello.b -o hello.c    compile hello.b and output hello.c\n\
"
, progName, progName);
	exit(status);
}
CELL_T readChar(int prev) {
	int x = getchar();
	if (x == EOF) {
		switch (eofType) {
		case EOF_NC:
			return prev;
		case EOF_M1:
			return -1;
		case EOF_0:
			return 0;
		}
	}
	return x;
}
