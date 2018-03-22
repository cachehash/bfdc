#include <stdio.h>
#include <stdlib.h>
#include "main.h"

typedef struct inst_t {
	int type;
	int amt;
	Node* n;
} inst_t;
inst_t* imem = NULL;
int pc;

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
			int scale = n->n[0].i;
			for (int z = 1; z < n->sz; z++) {
				Point *p = &n->n[z].p;
				int x = p->x;
				int y = p->y;
				m[i+x] += (y*m[i])/scale;
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
	imem = malloc(0);
	pc = 0;
	size_t *isz = 0;
	mkInsts(root);
	size_t end = pc;
	pc = 0;
	interpret(end, m);
	free(imem);
	imem = NULL;
}
