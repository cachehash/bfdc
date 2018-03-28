#include "main.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "comp.h"
#include <math.h>



static int a0 = 0;
static int a0valid = 0;
static void loada0(int off) {
	if (!a0valid || a0 != off) {
		a0 = off;
		a0valid = 1;
		iprintfln("lb $a0, %d($sp)", off);
	} else {
		//iprintfln("#lb $a0, %d($sp)", off);
	}
}
//return -1 if not a power of 2, otherwise return lg(x)
int getExp(int x) {
	double l = log2(x);
	if (!isnan(l) && ceil(l) == floor(l)) {
		return (int) l;
	}
	return -1;
}
void compileSpim(Node* n, int* labelId) {
	if (n == NULL) {
		return;
	}
	switch (n->type) {
	case LOOP: {
		int li = *labelId;
		*labelId += 1;
		loada0(0);
		a0valid = 0;
		iprintfln("beq $a0, $0, e%03d", li);
		level--;
		iprintfln("s%03d:", li);
		level++;
		compileSpim(n->n[0].n, labelId);
		loada0(0);
		//in code generation it is safe to assume a0 will still be loaded because 
		//the next instructions generated only execute after the loop is over
		//a0valid = 0;
		iprintfln("bne $a0, $0, s%03d", li);
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
		loada0(-off);
		iprintfln("addiu $a0, $a0, %d", n->n[0].i);
		iprintfln("sb $a0, %d($sp)", -off);
	}
	break;
	case SHIFT: {
		int shamt = n->n[0].i;
		iprintfln("addiu $sp, $sp, %d", -shamt);
		//move n to the right, a0 now holds m[i-n]
		a0 += shamt;
	}
	break;
	case OUT: {
		int off = n->n[0].i;
		iprintfln("li $v0, 11");
		loada0(-off);
		iprintfln("syscall");
	}
	break;
	case IN: {
		int off = n->n[0].i;
		iprintfln("li $v0, 12");
		iprintfln("syscall");
		iprintfln("sb $v0, %d($sp)", -off);
		//invalidate because we just updated memory, not a0 itself
		//if (a0 == off)
		a0valid = 0;
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
			loada0(-x-off);
			//TODO check if y or scale is a power of 2 and use a shift
			int reg = 3;
			if (y != 1) {
				int exp = getExp(y);
				if (exp != -1) {
					iprintfln("sll, $t1, $t3, %d", exp);
				} else {
					iprintfln("li $t1, %d", y);
					iprintfln("multu $t3, $t1");
					iprintfln("mflo $t1");
				}
				reg = 1;
			}
			if (scale != 1) {
				int exp = getExp(scale);
				if (exp != -1) {
					iprintfln("srl, $t1, $t%d, %d", reg, exp);
				} else {
					iprintfln("li $t2, %d", scale);
					iprintfln("divu $t%d, $t2", reg);
					iprintfln("mflo $t1");
				}
				reg = 1;
			}
			iprintfln("add $a0, $a0, $t%d", reg);
			iprintfln("sb $a0, %d($sp)", -x-off);
		}
		iprintfln("sb $0, %d($sp)", -off);
		if (off == a0) {
			a0valid = 0;
		}
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
