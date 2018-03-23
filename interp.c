#include <stdio.h>
#include <stdlib.h>
#include "main.h"

typedef struct inst_t {
	int type;
	int amt;
	int imm;
	int imm2;
} inst_t;
inst_t* imem = NULL;
int pc;
void grow(int amt) {
	static int prev = 0;
	if (amt > prev) {
		imem = realloc(imem, amt*sizeof(*imem));
		prev = amt;
	}
}

#define I_ZERO 0x80
void mkInsts(Node* n) {
	if (n == NULL) {
		return;
	}
	switch (n->type) {
	case LOOP: {
		int dst = pc;
		grow(pc+1);
		imem[pc].type = n->type;
		imem[pc].imm = 1;

		pc++;

		mkInsts(n->n[0].n);

		grow(pc+1);
		imem[dst].amt = pc;

		imem[pc].type = n->type;
		imem[pc].amt = dst;
		imem[pc].imm = 0;
		pc++;
	}
	break;

	case STMTS:
		mkInsts(n->n[0].n);
		mkInsts(n->n[1].n);
	break;

	case SUM:
	case SHIFT:
		grow(pc+1);
		imem[pc].type = n->type;
		imem[pc].amt = n->n[0].i;
		pc++;
	break;

	case OUT:
	case IN:
		grow(pc+1);
		imem[pc].type = n->type;
		pc++;
	break;

	case SET:
		for (int z = 0; z < n->sz; z++) {
			Point *p = n->n[z].p;
			grow(pc+1);
			imem[pc].type = n->type;
			imem[pc].amt = p->x;
			imem[pc].imm = p->y;
			imem[pc].imm2 = p->z;
			pc++;
		}
		grow(pc+1);
		imem[pc].type = I_ZERO;
		pc++;
	break;
	}
}
void interpret(size_t end, CELL_T * m) {
	size_t i = 0;
	I = &i;
	for (size_t k = 0; k < end; k++) {
		switch (imem[k].type) {
		case LOOP: {
			int go = (m[i] != 0);
			if (imem[k].imm) {
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
			m[i] = readChar(m[i]);
		break;
		case SET: {
			int x = imem[k].amt;
			int y = imem[k].imm;
			int scale = imem[k].imm2;
			m[i+x] += (y*m[i])/scale;
		}
		break;
		case I_ZERO: {
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
