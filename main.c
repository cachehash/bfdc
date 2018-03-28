#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <signal.h>

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
extern void reg_segv();

int usage(int);
FILE* outfile;

Node* root = NULL;

int numCells = NUM_CELLS;

int eofType = EOF_M1;
char* cell_t_str = NULL;

char* progName = NULL;

size_t* I = 0;

char* target = NULL;
int main(int argc, char** argv) {
	reg_segv();
	progName = argv[0];
	int interpret = 0;
	char* outname = NULL;
	char* ifile = NULL;
	char opt;
	int optLevel = 3;
	//TODO check if it's safe to not strdup optarg
	static struct option long_opts[] = {
		{"jit",		no_argument,		0, 'j'},
		{"dynarec",	no_argument,		0, 'd'},
		{"traverse",	no_argument,		0, 'T'},
		{"help",	no_argument,		0, 'h'},
		{"interpret",	no_argument,		0, 'i'},
		{"output",	required_argument,	0, 'o'},
		{"num-cells",	required_argument,	0, 'c'},
		{"eof",		required_argument,	0, 'E'},
		{"cell-type",	required_argument,	0, 'C'},
		{"optimize",	required_argument,	0, 'O'},
		{"target",	required_argument,	0, 't'},
		{0, 0, 0, 0}
	};
	while ((opt = getopt_long(argc, argv, "jdThio:c:C:E:O:t:", long_opts, NULL)) != -1) {
		switch (opt) {
			case 'j':
			case 'd':
				dynarec();
				exit(0);
			break;
			case 'T':
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
				} else if (strcmp("-1", optarg) == 0) {
					eofType = EOF_M1;
				} else if (strcmp("0", optarg) == 0) {
					eofType = EOF_0;
				} else {
					usage(INVALID_ARG);
				}
			break;
			case 'O':
				optLevel = atoi(optarg);
			break;
			case 't':
				free(target);
				target = strdup(optarg);
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
	if (ifile == NULL) {
		usage(NO_INPUT);
	}
	if (strcmp(ifile, "-") == 0) {
		int in = dup(0);
		close(0);
		yyin = fdopen(in, "r");
	} else {
		yyin = fopen(ifile, "r");
		if (yyin == NULL) {
			fprintf(stderr, "ERROR: couldn't open file `%s'\n", ifile);
			exit(1);
		}
	}
	yyparse();
	optimize(&root, optLevel);
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
		comp(outname);
		free(outname);
		outname = NULL;
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
   -i, --interpret              interpret the brainfuck code specified in FILE\n\
   -T, --traverse               interpret the parse tree used for compilation instead of compiling\n\
   -j, -d, --jit, --dynarec     use dynamic recompilation to run the program\n\
   -o, --output=FILE            output name for the compiled brainfuck\n\
   -O, --optimize=NUM           optimization level\n\
   -c, --num-cells=NUM          number of cells to use\n\
   -C, --cell-type=TYPE         data type to use for cells\n\
   -E, --eof=TYPE               format for EOF to use\n\
   -t, --target=FORMAT		output language for compiled code\n\
   -h, --help                   display this help text\n\
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

void sig_segv(int arg0) {
	if (*I < 0 || *I > numCells) {
		fprintf(stderr, "\nOut of bounds at cell %zd! Quitting.\n", *I);
	} else {
		SIG_DFL(arg0);
	}
	exit(SIGSEGV);
}
void reg_segv() {
	struct sigaction sigact[1];
	memset(sigact, 0, sizeof(*sigact));

	sigact->sa_handler = sig_segv;
	sigaction(SIGSEGV, sigact, NULL);
	//sigaction(SIGBUS, sigact, NULL);
}
