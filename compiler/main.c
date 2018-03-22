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

Node* root;

int yyparse();
void optimize(Node*);
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

int callAll(Node* n, int(*f)(Node**)) {
	int changed = 0;
	if (n == NULL) {
		return 0;
	}
	switch (n->type) {
	case LOOP:
		return f(&n->n[0].n);
	break;
	case STMTS:
		changed |= f(&n->n[0].n);
		changed |= f(&n->n[1].n);
		return changed;
	break;
	case SUM:
	case SHIFT:
	case OUT:
	case IN:
	case SET:
		return 0;
	break;
	}
	return 0;
}
/*
 * concatenate multiple adds/subs together
 */
int join(Node** np) {
	Node *n = *np;
	if (n == NULL) {
		return 0;
	}
	int changed = 0;
	if (n->type == STMTS) {
		Node* lchild = n->n[0].n;
		Node* rtmp = n;
		while (1) {
			rtmp = rtmp->n[1].n;
			if (rtmp == NULL) {
				break;
			}
			Node* rlchild = rtmp->n[0].n;
			if (lchild->type == rlchild->type && (lchild->type == SUM || lchild->type == SHIFT)) {
				lchild->n[0].i += rlchild->n[0].i;
				n->n[1].n = rtmp->n[1].n;
				changed = 1;
			} else {
				break;
			}
		}
	}
	changed |= callAll(n, join);
	return changed;
}
int nullify(Node** np) {
	Node* n = *np;
	if (n == NULL) {
		return 0;
	}
	int changed = 0;
	Node* left = n->n[0].n;
	if (n->type == STMTS) {
		//eliminate +- and ><
		if (((left->type == SUM || left->type == SHIFT) && left->n[0].i == 0)) {
			*np = n->n[1].n;
			changed = 1;
		}
	}
	changed |= callAll(n, nullify);
	return changed;
}
int useSet(Node** np) {
	Node *n = *np;
	if (n == NULL) {
		return 0;
	}
	int changed = 0;
	if (n->type == LOOP) {
		if (n->n[0].n == NULL) {
			*np = NULL;
			return 1;
		}
		Node* body = n->n[0].n;
		if (body->type != STMTS) {
			goto ret;
		}
		/*
		 * perform static analysis to see if it's only composed of 
		 * +-<> and always lands on the same cell.
		 */
		int netMove = 0;
		for (Node* node = body; node != NULL; node = node->n[1].n) {
			Node* left = node->n[0].n;
			if (!(left && (left->type == SHIFT || left->type == SUM))) {
				goto ret;
			}
			if (left->type == SHIFT) {
				netMove += left->n[0].i;
			}
		}
		if (netMove == 0) {
			/*
			 * the cell the loop starts on/ends on must be zero to exit the loop.
			 * This means we can set it to 0 and add it/subtract it to the other
			 * cells.
			 */
			Map* m = newIntPtrMap(100);
			/*
			 * guarenty that if someone does [>+<] we don't
			 * assume wrongly that zero is in the map
			 */
			int* zero = calloc(1, sizeof(int));
			mPut(m, zero, zero);
			for (Node* node = body; node != NULL; node = node->n[1].n) {
				Node* left = node->n[0].n;
				if (left->type == SHIFT) {
					netMove += left->n[0].i;
				} else {
					int* n = mGet(m, &netMove);
					if (n == NULL) {
						n = calloc(1, sizeof(int));
						mPut(m, &netMove, n);
					}
					*n += left->n[0].i;
				}
			}
			void* keys[m->size];
			mGetKeys(m, keys);
			/*
			 * first prune the map of redundant values
			 */
			for (int i = 0; i < m->size; i++) {
				int* k = keys[i];
				int* val = mGet(m, k);
				if (*k != 0 && *val == 0) {
					mDel(m, k);
				}
			}
			mGetKeys(m, keys);
			int quantity = m->size-1;
			Node* new = mkNode(quantity, SET);
			int newIndx = 0;
			for (int i = 0; i < m->size; i++) {
				int* k = keys[i];
				int* val = mGet(m, k);
				if (*k == 0) {
					//TODO warn if loop is infinite
				} else {
					Point *p = &new->n[newIndx].p;
					p->x = *k;
					p->y = *val;
					newIndx++;
				}
			}
			destroyMap(m);
			*np = new;
		}

	}
ret:
	changed |= callAll(*np, useSet);
	return changed;
}
void optimize(Node* n) {
	int changed = 1;
	while (changed) {
		changed = 0;
		changed |= join(&n);
		changed |= nullify(&n);
		if (!changed) {
			changed |= useSet(&n);
		}
	}
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
