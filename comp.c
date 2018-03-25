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
	case SUM: {
		int off = n->n[1].i;
		iprintfnln("m[i");
		if (off != 0) {
			fprintf(outfile, "+%d", off);
		}
		fprintf(outfile, "] += %d;\n", n->n[0].i);
	}
	break;
	case SHIFT:
		iprintf("i += %d;", n->n[0].i);
	break;
	case OUT: {
		int off = n->n[0].i;
		iprintfnln("putchar(m[i");
		if (off != 0) {
			fprintf(outfile, "+%d", off);
		}
		fprintf(outfile, "]);\n");
		iprintf("fflush(stdout);");
	}
	break;
	case IN: {
		int off = n->n[0].i;
		if (off != 0) {
			iprintf("m[i+%d] = readChar(m[i+%d]);", off, off);
		} else {
			iprintf("m[i] = readChar(m[i]);");
		}
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
			iprintfnln("m[i");
			if (x+off >= 0) {
				fprintf(outfile, "+");
			}
			fprintf(outfile, "%d] += ", x+off);
			if (y == 1) {
				fprintf(outfile, "m[i");
			} else {
				fprintf(outfile, "%d*m[i", y);
			}
			fprintf(outfile, "+%d]", off);
			if (scale == 1) {
				fprintf(outfile, ";\n");
			} else {
				fprintf(outfile, "/%d;\n", scale);
			}
		}
		iprintf("m[i+%d] = 0;", off);
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
	iprintf("#define CELL_T %s", cell_t_str);
	iprintf("#endif");
	iprintf("");
	iprintf("#ifndef NUM_CELLS");
	iprintf("#define NUM_CELLS %d", numCells);
	iprintf("#endif");
	iprintf("");
	iprintf(READ_CHAR_FMT, eofStr);
	iprintf("int main() {");
	level++;
	iprintf("size_t i = 0;");
	iprintf("CELL_T m[NUM_CELLS] = {0};");
	compile(root);
	level--;
	iprintf("}");
}
