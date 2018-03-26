#include "main.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "comp.h"



void compileC(Node* n) {
	if (n == NULL) {
		return;
	}
	switch (n->type) {
	case LOOP:
		iprintfln("while(m[i]) {");
		level++;
		compileC(n->n[0].n);
		level--;
		iprintfln("}");
	break;
	case STMTS:
		compileC(n->n[0].n);
		compileC(n->n[1].n);
	break;
	case SUM: {
		int off = n->n[1].i;
		iprintf("m[i");
		iprintAdd(off);
		fprintf(outfile, "] += %d;\n", n->n[0].i);
	}
	break;
	case SHIFT:
		iprintfln("i += %d;", n->n[0].i);
	break;
	case OUT: {
		int off = n->n[0].i;
		iprintf("putchar(m[i");
		iprintAdd(off);
		fprintf(outfile, "]);\n");
		iprintfln("fflush(stdout);");
	}
	break;
	case IN: {
		int off = n->n[0].i;
		iprintf("readChar(&m[i");
		iprintAdd(off);
		fprintf(outfile, "]);\n");
	}
	break;
	case SET: {
		int off = n->n[0].i;
		for (int i = 1; i < n->sz; i++) {
			Point *p = n->n[i].p;
			int x = p->x;
			int y = p->y;
			int scale = p->z;
			//"m[i+%d] += (%d*m[i+%d])/%d;", x+off, y, off, scale
			iprintf("m[i");
			iprintAdd(x);
			iprintAdd(off);
			fprintf(outfile, "] += ");
			iprintCoeff(y);
			fprintf(outfile, "m[i");
			iprintAdd(off);
			fprintf(outfile, "]");
			iprintDiv(scale);
			fprintf(outfile, ";\n");
		}
		iprintf("m[i");
		iprintAdd(off);
		fprintf(outfile, "] = 0;\n");
	}
	break;
	}
}
void compC() {
	char* eofStr = "-1";
	if (eofType == EOF_NC) {
		eofStr = "*cell";
	} else if (eofType == EOF_M1) {
		eofStr = "-1";
	} else if (eofType == EOF_0) {
		eofStr = "0";
	}
	if (cell_t_str == NULL) {
		cell_t_str = strdup("uint8_t");
	}
	iprintfln("#include <stdio.h>");
	iprintfln("#include <stddef.h>");
	iprintfln("#include <stdint.h>");

	iprintfln("");

	iprintfln("#ifndef CELL_T");
	iprintfln("#define CELL_T %s", cell_t_str);
	iprintfln("#endif");

	iprintfln("");

	iprintfln("#ifndef NUM_CELLS");
	iprintfln("#define NUM_CELLS %d", numCells);
	iprintfln("#endif");

	iprintfln("");

	iprintfln("void readChar(CELL_T* cell) {");
	level++;
	iprintfln("int x = getchar();");
	iprintfln("*cell = (x == EOF) ? %s : x;", eofStr);
	level--;
	iprintfln("}");

	iprintfln("");

	iprintfln("int main() {");
	level++;
	iprintfln("size_t i = 0;");
	iprintfln("CELL_T m[NUM_CELLS] = {0};");
	compileC(root);
	level--;
	iprintfln("}");
}
