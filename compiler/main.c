#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include "main.h"
#include "hash/hash.h"

#ifndef CELL_T
#define CELL_T char
#endif

#include "optimize.h"

Node* root;

int yyparse();
void comp();
void interp();
void interpTree();
int usage();
FILE* outfile;
extern FILE* yyin;


typedef struct inst_t {
	int type;
	int amt;
	Node* n;
} inst_t;
inst_t* imem = NULL;
int pc;


int numCells = 30000;
int main(int argc, char** argv) {
	int interpret = 0;
	char* outname = NULL;
	char* ifile = NULL;
	char opt;
	while ((opt = getopt (argc, argv, "tio:c:")) != -1) {
		switch (opt) {
			case 'i':
				interpret = 1;
			break;
			case 't':
				interpret = 2;
			break;
			case 'o':
				outname = strdup(optarg);
			break;
			case 'c':
				numCells = atoi(optarg);
			break;
			default:
				usage();
		}
	}
	for (int i = optind; i < argc; i++) {
		if (ifile) {
			usage();
		}
		ifile = argv[i];
	}
	if (outname && interpret) {
		usage();
	}
	if (ifile == NULL) {
		usage();
	}
	yyin = fopen(ifile, "r");
	yyparse();
	optimize(root);
	if (interpret) {
		if (interpret == 2) {
			interpTree();
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
}
int usage() {
	printf("invalid args\n");
	exit(1);
}

int level = 0;
int iprintf(const char* fmt,...) {
	va_list ap;
	for (int i = 0; i < level; i++) {
		fprintf(outfile, "\t");
	}
	va_start(ap, fmt);
	vfprintf(outfile, fmt, ap);
	fprintf(outfile, "\n");
}
void compile(Node* n) {
	if (n == NULL) {
		return;
	}
	switch (n->type) {
	case LOOP:
		iprintf("while(m[i]) {");
		level++;
		compile(n->n[0].n);
		level--;
		iprintf("}");
	break;
	case STMTS:
		compile(n->n[0].n);
		compile(n->n[1].n);
	break;
	case SUM:
		iprintf("m[i] += %d;", n->n[0].i);
	break;
	case SHIFT:
		iprintf("i += %d;", n->n[0].i);
	break;
	case OUT:
		//iprintf("%s", "printf(\"%c\", m[i]);");
		iprintf("putchar(m[i]);");
	break;
	case IN:
		iprintf("m[i] = getchar();");
	break;
	case SET:
		for (int i = 0; i < n->sz; i++) {
			Point *p = &n->n[i].p;
			iprintf("m[i+%d] += %d*m[i];", p->x, p->y);
		}
		iprintf("m[i] = 0;");
	break;
	}
}
void comp() {
	iprintf("#include <stdio.h>");
	iprintf("");
	iprintf("#ifndef CELL_T");
	iprintf("#define CELL_T char");
	iprintf("#endif");
	iprintf("");
	iprintf("#ifndef NUM_CELLS");
	iprintf("#define NUM_CELLS %d", numCells);
	iprintf("#endif");
	iprintf("");
	iprintf("int main() {");
	level++;
	iprintf("size_t i = 0;");
	iprintf("CELL_T m[NUM_CELLS] = {0};");
	compile(root);
	level--;
	iprintf("}");
}
void interpretTree(Node* n, CELL_T* m, size_t* i) {
	if (n == NULL) {
		return;
	}
	switch (n->type) {
	case LOOP:
		while (m[*i]) {
			interpretTree(n->n[0].n, m, i);
		}
	break;
	case STMTS:
		interpretTree(n->n[0].n, m, i);
		interpretTree(n->n[1].n, m, i);
	break;
	case SUM:
		m[*i] += n->n[0].i;
	break;
	case SHIFT:
		*i += n->n[0].i;
	break;
	case OUT:
		putchar(m[*i]);
		fflush(stdout);
	break;
	case IN:
		m[*i] = getchar();
	break;
	case SET:
		for (int k = 0; k < n->sz; k++) {
			Point *p = &n->n[k].p;
			int x = p->x;
			int y = p->y;
			m[*i+x] += y*m[*i];
		}
		m[*i] = 0;
	break;
	}
}
void interpTree() {
	CELL_T m[numCells];
	for (int z = 0; z < numCells; z++) {
		m[z] = 0;
	}
	size_t i[1] = {0};
	interpretTree(root, m, i);
}
void mkInsts(Node* n) {
	if (n == NULL) {
		return;
	}
	switch (n->type) {
	case LOOP: {
		int dst = pc;
		imem = realloc(imem, (pc+1)*sizeof(*imem));
		imem[pc].type = n->type;
		imem[pc].n = n;

		pc++;

		mkInsts(n->n[0].n);

		imem = realloc(imem, (pc+1)*sizeof(*imem));
		imem[dst].amt = pc;

		imem[pc].type = n->type;
		imem[pc].amt = dst;
		imem[pc].n = NULL;
		pc++;
	}
	break;

	case STMTS:
		mkInsts(n->n[0].n);
		mkInsts(n->n[1].n);
	break;

	case SUM:
	case SHIFT:
		imem = realloc(imem, (pc+1)*sizeof(*imem));
		imem[pc].type = n->type;
		imem[pc].amt = n->n[0].i;
		pc++;
	break;

	case OUT:
	case IN:
		imem = realloc(imem, (pc+1)*sizeof(*imem));
		imem[pc].type = n->type;
		pc++;
	break;

	case SET:
		imem = realloc(imem, (pc+1)*sizeof(*imem));
		imem[pc].type = n->type;
		imem[pc].n = n;
		pc++;
	break;
	}
}
void interpret(size_t end, CELL_T * m) {
	size_t i = 0;
	for (size_t k = 0; k < end; k++) {
		switch (imem[k].type) {
		case LOOP: {
			int go = (m[i] != 0);
			if (imem[k].n) {
				go = !go;
			}
			if (go) {
				k = imem[k].amt-1;
				continue;
			}
		}
		break;
		case SUM:
			m[i] += imem[k].amt;
		break;
		case SHIFT:
			i += imem[k].amt;
		break;
		case OUT:
			putchar(m[i]);
			fflush(stdout);
		break;
		case IN:
			m[i] = getchar();
		break;

		case SET: {
			Node* n = imem[k].n;
			for (int z = 0; z < n->sz; z++) {
				Point *p = &n->n[z].p;
				int x = p->x;
				int y = p->y;
				m[i+x] += y*m[i];
			}
			m[i] = 0;
		}
		break;
		}
	}
}
void interp() {
	CELL_T m[numCells];
	for (int z = 0; z < numCells; z++) {
		m[z] = 0;
	}
	free(imem);
	imem = malloc(0);
	pc = 0;
	size_t *isz = 0;
	mkInsts(root);
	size_t end = pc;
	pc = 0;
	interpret(end, m);
}
