#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"

typedef struct inst_t {
	uint8_t type;
	int amt;
	int imm;
	int imm2;
	int imm3;
}
#ifndef NO_PACK
__attribute__((packed))
#endif
inst_t;
char* buff = NULL;

static inst_t dummy;
static const size_t TYPE_END = ((ptrdiff_t)&dummy.amt) - (ptrdiff_t)&dummy;
static const size_t AMT_END = ((ptrdiff_t)&dummy.imm) - (ptrdiff_t)&dummy;
static const size_t IMM_END = ((ptrdiff_t)&dummy.imm2) - (ptrdiff_t)&dummy;
static const size_t IMM2_END = ((ptrdiff_t)&dummy.imm3) - (ptrdiff_t)&dummy;
static const size_t IMM3_END = sizeof(dummy);

int pc;
static void grow(int amt) {
	static int prev = 0;
	if (amt > prev) {
		buff = realloc(buff, amt);
		prev = amt;
	}
}
static int push(inst_t* i, size_t sz) {
	grow(pc+sz);
	char* dest = buff+pc;
	memcpy(dest, i, sz);
	int ret = pc;
	pc += sz;
	return ret;
}

#define I_ZERO 0x80
#define I_LOOP_BACK 0x81
#define I_SHORT_SUM 0x82
#define I_SHORT_IN 0x83
#define I_SHORT_OUT 0x84
#define I_SHORT_ZERO 0x85

#define I_DIV_SET 0x86
#define I_OFF_SET 0x87
#define I_SHORT_SET 0x88
#define I_FULL_SET SET
void mkInsts(Node* n) {
	inst_t inst;
	//TODO why is this faster?
	memset(&inst, 0, sizeof(inst));
	if (n == NULL) {
		return;
	}
	int sz = 0;
	switch (n->type) {
	case LOOP: {
		sz = AMT_END;
		int dst = pc+sz;
		inst.type = n->type;

		int offset = push(&inst, sz);

		mkInsts(n->n[0].n);

		inst_t* begin = (void*) (buff+offset);
		begin->amt = pc+sz;

		inst.type = I_LOOP_BACK;
		inst.amt = dst;
		push(&inst, sz);
	}
	break;
	case STMTS:
		mkInsts(n->n[0].n);
		mkInsts(n->n[1].n);
	break;
	case SUM:
		sz = IMM_END;
		inst.type = n->type;
		inst.amt = n->n[0].i;
		inst.imm = n->n[1].i;
		if (inst.imm == 0) {
			inst.type = I_SHORT_SUM;
			sz = AMT_END;
		}
		push(&inst, sz);
	break;
	case SHIFT:
		inst.type = n->type;
		inst.amt = n->n[0].i;
		push(&inst, AMT_END);
	break;
	case OUT:
		inst.amt = n->n[0].i;
		if (inst.amt == 0) {
			inst.type = I_SHORT_OUT;
			push(&inst, TYPE_END);
		} else {
			inst.type = n->type;
			push(&inst, AMT_END);
		}
	break;
	case IN:
		inst.amt = n->n[0].i;
		if (inst.amt == 0) {
			inst.type = I_SHORT_IN;
			push(&inst, TYPE_END);
		} else {
			inst.type = n->type;
			push(&inst, AMT_END);
		}
	break;
	case SET: {
		int off = n->n[0].i;
		for (int z = 1; z < n->sz; z++) {
			Point *p = n->n[z].p;
			inst.amt = p->x;
			inst.imm = p->y;
			inst.imm2 = p->z;
			inst.imm3 = off;
			if (p->z == 1) {
				if (off == 0) {
					inst.type = I_SHORT_SET;
					push(&inst, IMM_END);
				} else {
					inst.imm2 = off;
					inst.type = I_OFF_SET;
					push(&inst, IMM2_END);
				}
			} else {
				if (off == 0) {
					inst.type = I_DIV_SET;
					push(&inst, IMM2_END);
				} else {
					inst.type = I_FULL_SET;
					push(&inst, IMM3_END);
				}
			}
		}
		if (off == 0) {
			inst.type = I_SHORT_ZERO;
			push(&inst, TYPE_END);
		} else {
			inst.type = I_ZERO;
			inst.amt = off;
			push(&inst, AMT_END);
		}
	}
	break;
	}
}
void interpret(size_t end, CELL_T * m) {
	size_t i = 0;
	I = &i;
	for (size_t k = 0; k < end;) {
		inst_t* inst = (void*) (buff+k);
		switch (inst->type) {
		case LOOP: {
			if (m[i] == 0) {
				k = inst->amt;
				continue;
			}
			k += AMT_END;
		}
		break;
		case I_LOOP_BACK: {
			if (m[i] != 0) {
				k = inst->amt;
				continue;
			}
			k += AMT_END;
		}
		break;
		case I_SHORT_SUM:
			m[i] += inst->amt;
			k += AMT_END;
		break;
		case SUM: {
			int off = inst->imm;
			m[i+off] += inst->amt;
			k += IMM_END;
		}
		break;
		case SHIFT:
			i += inst->amt;
			k += AMT_END;
		break;
		case I_SHORT_OUT:
			putchar(m[i]);
			fflush(stdout);
			k += TYPE_END;
		break;
		case I_SHORT_IN:
			m[i] = readChar(m[i]);
			k += TYPE_END;
		break;
		case OUT: {
			int off = inst->amt;
			putchar(m[i+off]);
			fflush(stdout);
			k += AMT_END;
		}
		break;
		case IN: {
			int indx = i + inst->amt;
			m[indx] = readChar(m[indx]);
			k += AMT_END;
		}
		break;
		case I_DIV_SET: {
			int x = inst->amt;
			int y = inst->imm;
			int scale = inst->imm2;
			m[i+x] += (y*m[i])/scale;
			k += IMM2_END;
		}
		break;
		case I_SHORT_SET: {
			int x = inst->amt;
			int y = inst->imm;
			m[i+x] += (y*m[i]);
			k += IMM_END;
		}
		break;
		case I_FULL_SET: {
			int x = inst->amt;
			int y = inst->imm;
			int scale = inst->imm2;
			int off = inst->imm3;
			m[i+x+off] += (y*m[i+off])/scale;
			k += IMM3_END;
		}
		break;
		case I_OFF_SET: {
			int x = inst->amt;
			int y = inst->imm;
			int off = inst->imm2;
			m[i+x+off] += (y*m[i+off]);
			k += IMM2_END;
		}
		break;
		case I_ZERO: {
			int off = inst->amt;
			m[i+off] = 0;
			k += AMT_END;
		}
		break;
		case I_SHORT_ZERO: {
			m[i] = 0;
			k += TYPE_END;
		}
		break;
		default:
			fprintf(stderr, "Internal error! unrecognized instruction %X\n", inst->type);
			exit(1);
		}
	}
}
void interp() {
	CELL_T m[numCells];
	for (int z = 0; z < numCells; z++) {
		m[z] = 0;
	}
	buff = malloc(0);
	pc = 0;
	size_t *isz = 0;
	mkInsts(root);
	size_t end = pc;
	pc = 0;
	interpret(end, m);
	free(buff);
	buff = NULL;
}
