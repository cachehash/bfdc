#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"

typedef struct inst_t {
	uint8_t type;
	int amt;
	int imm;
	int imm2;
}
#ifndef NO_PACK
__attribute__((packed))
#endif
inst_t;
inst_t* imem = NULL;

static inst_t dummy;
#define TYPE_END (((ptrdiff_t)&dummy.amt) - (ptrdiff_t)&dummy)
#define AMT_END (((ptrdiff_t)&dummy.imm) - (ptrdiff_t)&dummy)
#define IMM_END (((ptrdiff_t)&dummy.imm2) - (ptrdiff_t)&dummy)
#define IMM2_END (sizeof(dummy))

/*
#define TYPE_END (sizeof(dummy.type))
#define AMT_END (sizeof(dummy) - sizeof(dummy.imm) - sizeof(dummy.imm2))
#define IMM_END (sizeof(dummy) - sizeof(dummy.imm2))
#define IMM2_END (sizeof(dummy))
*/
int pc;
void grow(int amt) {
	static int prev = 0;
	if (amt > prev) {
		imem = realloc(imem, amt);
		prev = amt;
	}
}
int push(inst_t* i, size_t sz) {
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
		int dst = pc+IMM_END;
		inst.type = n->type;
		inst.imm = 1;

		int offset = push(&inst, IMM_END);

		mkInsts(n->n[0].n);

		char* buff = (void*) imem;
		inst_t* begin = (void*) (buff+offset);
		begin->amt = pc+IMM_END;

		inst.type = n->type;
		inst.amt = dst;
		inst.imm = 0;
		push(&inst, IMM_END);
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
		push(&inst, AMT_END);
	break;

	case OUT:
	case IN:
		inst.type = n->type;
		push(&inst, TYPE_END);
	break;

	case SET:
		for (int z = 0; z < n->sz; z++) {
			Point *p = n->n[z].p;
			inst.type = n->type;
			inst.amt = p->x;
			inst.imm = p->y;
			inst.imm2 = p->z;
			push(&inst, sizeof(inst_t));
		}
		inst.type = I_ZERO;
		push(&inst, TYPE_END);
	break;
	}
}
void interpret(size_t end, CELL_T * m) {
	size_t i = 0;
	I = &i;
	char* buff = (void*) imem;
	for (size_t k = 0; k < end;) {
		inst_t* inst = (void*) (buff+k);
		switch (inst->type) {
		case LOOP: {
			int go = (m[i] != 0);
			if (inst->imm) {
				go = !go;
			}
			if (go) {
				k = inst->amt;
				continue;
			}
			k += IMM_END;
		}
		break;
		case SUM:
			m[i] += inst->amt;
			k += AMT_END;
		break;
		case SHIFT:
			i += inst->amt;
			k += AMT_END;
		break;
		case OUT:
			putchar(m[i]);
			fflush(stdout);
			k += TYPE_END;
		break;
		case IN:
			m[i] = readChar(m[i]);
			k += TYPE_END;
		break;
		case SET: {
			int x = inst->amt;
			int y = inst->imm;
			int scale = inst->imm2;
			m[i+x] += (y*m[i])/scale;
			k += sizeof(inst_t);
		}
		break;
		case I_ZERO: {
			m[i] = 0;
			k += TYPE_END;
		}
		break;
		default:
			exit(1);
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
