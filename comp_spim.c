#include "main.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "comp.h"



void compileSpim(Node* n, int* labelId) {
	if (n == NULL) {
		return;
	}
	switch (n->type) {
	case LOOP: {
		int li = *labelId;
		*labelId += 1;
		iprintfln("lb $t0, ($sp)");
		iprintfln("beq $t0, $0, e%03d", li);
		level--;
		iprintfln("s%03d:", li);
		level++;
		compileSpim(n->n[0].n, labelId);
		iprintfln("lb $t0, ($sp)");
		iprintfln("bne $t0, $0, s%03d", li);
		level--;
		iprintfln("e%03d:", li);
		level++;
	}
	break;
	case STMTS:
		compileSpim(n->n[0].n, labelId);
		compileSpim(n->n[1].n, labelId);
	break;
	case SUM: {
		int off = n->n[1].i;
		iprintfln("lb $t0, %d($sp)", -off);
		iprintfln("addiu $t0, $t0, %d", n->n[0].i);
		iprintfln("sb $t0, %d($sp)", -off);
	}
	break;
	case SHIFT:
		iprintfln("addiu $sp, $sp, %d", -n->n[0].i);
	break;
	case OUT: {
		int off = n->n[0].i;
		iprintfln("li $v0, 11");
		iprintfln("lb $a0, %d($sp)", -off);
		iprintfln("syscall");
	}
	break;
	case IN: {
		int off = n->n[0].i;
		iprintfln("li $v0, 12");
		iprintfln("syscall");
		iprintfln("sb $v0, %d($sp)", -off);
	}
	break;
	case SET: {
		int off = n->n[0].i;
		if (n->sz > 1) {
			iprintfln("lb $t3, %d($sp)", -off);
		}
		for (int i = 1; i < n->sz; i++) {
			Point *p = n->n[i].p;
			int x = p->x;
			int y = p->y;
			int scale = p->z;
			//"m[i+%d] += (%d*m[i+%d])/%d;", x+off, y, off, scale
			iprintfln("lb $t0, %d($sp)", -x-off);
			//TODO check if y or scale is a power of 2 and use a shift
			int reg = 3;
			if (y != 1) {
				iprintfln("li $t1, %d", y);
				iprintfln("mult $t3, $t1");
				iprintfln("mflo $t1");
				reg = 1;
			}
			if (scale != 1) {
				iprintfln("li $t2, %d", scale);
				iprintfln("div $t%d, $t2", reg);
				iprintfln("mflo $t1");
				reg = 1;
			}
			iprintfln("add $t0, $t0, $t%d", reg);
			iprintfln("sb $t0, %d($sp)", -x-off);
		}
		iprintfln("sb $0, %d($sp)", -off);
	}
	break;
	}
}
void compSpim() {
	iprintfln(".text");
	iprintfln("main:	addiu $sp, $sp, -1");
	level++;
	int i = 0;
	compileSpim(root, &i);
	iprintfln("li $v0, 10");
	iprintfln("syscall");
}
