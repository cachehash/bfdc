#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
		imem = realloc(imem, amt);
		prev = amt;
	}
}
int push(inst_t* i) {
	int sz = sizeof(*imem);
	grow(pc+sz);
	char* buff = (void*) imem;
	char* dest = buff+pc;
	memcpy(dest, i, sz);
	pc += sz;
	return pc-sz;
}

#define I_ZERO 0x80
void mkInsts(Node* n) {
	inst_t inst;
	if (n == NULL) {
		return;
	}
	switch (n->type) {
	case LOOP: {
		int dst = pc/sizeof(*imem);
		inst.type = n->type;
		inst.imm = 1;

		int offset = push(&inst);

		mkInsts(n->n[0].n);

		char* buff = (void*) imem;
		inst_t* begin = (void*) (buff+offset);
		begin->amt = pc/sizeof(*imem);

		inst.type = n->type;
		inst.amt = dst;
		inst.imm = 0;
		push(&inst);
	}
	break;

	case STMTS:
		mkInsts(n->n[0].n);
		mkInsts(n->n[1].n);
	break;

	case SUM:
	case SHIFT:
		inst.type = n->type;
		inst.amt = n->n[0].i;
		push(&inst);
	break;

	case OUT:
	case IN:
		inst.type = n->type;
		push(&inst);
	break;

	case SET:
		for (int z = 0; z < n->sz; z++) {
			Point *p = n->n[z].p;
			inst.type = n->type;
			inst.amt = p->x;
			inst.imm = p->y;
			inst.imm2 = p->z;
			push(&inst);
		}
		inst.type = I_ZERO;
		push(&inst);
	break;
	}
}
void interpret(size_t end, CELL_T * m) {
	size_t i = 0;
	I = &i;
	pc /= sizeof(*imem);
	for (size_t k = 0; k < end; k++) {
		inst_t* inst = &imem[k];
		switch (inst->type) {
		case LOOP: {
			int go = (m[i] != 0);
			if (inst->imm) {
				go = !go;
			}
			if (go) {
				k = inst->amt-1;
				continue;
			}
		}
		break;
		case SUM:
			m[i] += inst->amt;
		break;
		case SHIFT:
			i += inst->amt;
		break;
		case OUT:
			putchar(m[i]);
			fflush(stdout);
		break;
		case IN:
			m[i] = readChar(m[i]);
		break;
		case SET: {
			int x = inst->amt;
			int y = inst->imm;
			int scale = inst->imm2;
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
