#include "main.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "comp.h"



void compileGo(Node* n) {
	if (n == NULL) {
		return;
	}
	switch (n->type) {
	case LOOP:
		iprintfln("for m[i] != 0 {");
		level++;
		compileGo(n->n[0].n);
		level--;
		iprintfln("}");
	break;
	case STMTS:
		compileGo(n->n[0].n);
		compileGo(n->n[1].n);
	break;
	case SUM: {
		int off = n->n[1].i;
		iprintf("m[i");
		iprintAdd(off);
		fprintf(outfile, "] += %d\n", n->n[0].i);
	}
	break;
	case SHIFT:
		iprintfln("i += %d", n->n[0].i);
	break;
	case OUT: {
		int off = n->n[0].i;
		iprintf("fmt.Printf(\"%%c\", m[i");
		iprintAdd(off);
		fprintf(outfile, "])\n");
	}
	break;
	case IN: {
		int off = n->n[0].i;
		iprintf("readChar(&m[i");
		iprintAdd(off);
		fprintf(outfile, "])\n");
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
			fprintf(outfile, "\n");
		}
		iprintf("m[i");
		iprintAdd(off);
		fprintf(outfile, "] = 0\n");
	}
	break;
	}
}
void compGo() {
	char* eofStr = "-1";
	if (eofType == EOF_NC) {
		eofStr = "*cell";
	} else if (eofType == EOF_M1) {
		eofStr = "-1";
	} else if (eofType == EOF_0) {
		eofStr = "0";
	}
	if (cell_t_str == NULL) {
		cell_t_str = strdup("int8");
	}
	iprintfln("package main");
	iprintfln("import \"fmt\"");
	iprintfln("//#include <stdio.h>");
	iprintfln("import \"C\"");

	iprintfln("");

	iprintfln("type CELL_T %s", cell_t_str);

	iprintfln("");

	iprintfln("const NUM_CELLS = %d", numCells);

	iprintfln("");

	iprintfln("func readChar(cell* CELL_T) {");
	level++;
	iprintfln("x := C.getchar();");
	iprintfln("if x == C.EOF {");
	level++;
	iprintfln("*cell = %s", eofStr);
	level--;
	iprintfln("} else {");
	level++;
	iprintfln("*cell = CELL_T(x)");
	level--;
	iprintfln("}");
	level--;
	iprintfln("}");

	iprintfln("");

	iprintfln("func main() {");
	level++;
	iprintfln("i := 0");
	iprintfln("m := make([]CELL_T, NUM_CELLS)");
	compileGo(root);
	level--;
	iprintfln("}");
}
