#include "main.h"
#include <stdarg.h>
#include <stdio.h>

int level = 0;
void iprintf(const char* fmt,...) {
	va_list ap;
	for (int i = 0; i < level; i++) {
		fprintf(outfile, "\t");
	}
	va_start(ap, fmt);
	vfprintf(outfile, fmt, ap);
	fprintf(outfile, "\n");
}
void iprintfnln(const char* fmt,...) {
	va_list ap;
	for (int i = 0; i < level; i++) {
		fprintf(outfile, "\t");
	}
	va_start(ap, fmt);
	vfprintf(outfile, fmt, ap);
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
		iprintf("putchar(m[i]);");
		iprintf("fflush(stdout);");
	break;
	case IN:
		iprintf("m[i] = getchar();");
	break;
	case SET: {
		for (int i = 0; i < n->sz; i++) {
			Point *p = n->n[i].p;
			int x = p->x;
			int y = p->y;
			int scale = p->z;
			//"m[i+%d] += (%d*m[i])/%d;", x, y, scale
			iprintfnln("m[i");
			if (x >= 0) {
				fprintf(outfile, "+");
			}
			fprintf(outfile, "%d] += ", x);
			if (y == 1) {
				fprintf(outfile, "m[i]");
			} else {
				fprintf(outfile, "%d*m[i]", y);
			}
			if (scale == 1) {
				fprintf(outfile, ";\n");
			} else {
				fprintf(outfile, "/%d;\n", scale);
			}
		}
		iprintf("m[i] = 0;");
	}
	break;
	}
}
void comp() {
	iprintf("#include <stdio.h>");
	iprintf("#include <stddef.h>");
	iprintf("#include <stdint.h>");
	iprintf("");
	iprintf("#ifndef CELL_T");
	iprintf("#define CELL_T uint8_t");
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
