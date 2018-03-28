#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include "main.h"
#include "comp.h"
int level = 0;
void iprintfln(const char* fmt,...) {
	va_list ap;
	for (int i = 0; i < level; i++) {
		fprintf(outfile, "\t");
	}
	va_start(ap, fmt);
	vfprintf(outfile, fmt, ap);
	fprintf(outfile, "\n");
}
void iprintf(const char* fmt,...) {
	va_list ap;
	for (int i = 0; i < level; i++) {
		fprintf(outfile, "\t");
	}
	va_start(ap, fmt);
	vfprintf(outfile, fmt, ap);
}
void iprintAdd(int x) {
	if (x == 0) {
		return;
	}
	if (x > 0) {
		fprintf(outfile, "+");
	}
	fprintf(outfile, "%d", x);
}
void iprintCoeff(int x) {
	if (x == 1) {
		return;
	}
	fprintf(outfile, "%d*", x);
}
void iprintDiv(int x) {
	if (x == 1) {
		return;
	}
	fprintf(outfile, "/%d", x);
}
void comp(char* outname) {
	int i = 0;
	for (int x = 0; outname[x]; x++) {
		if (outname[x] == '.') {
			i = x;
		}
	}
	outname += i+1;
	if (target == NULL) {
		if (strcmp(outname, "go") == 0) {
			compGo();
		} else if (strcmp(outname, "s") == 0) {
			//TODO target native assembly
			compMips();
		} else {
			compC();
		}
	} else {
		if (strcmp(target, "c") == 0) {
			compC();
		} else if (strcmp(target, "go") == 0) {
			compGo();
		} else if (strcmp(target, "mips") == 0) {
			compMips();
		} else if (strcmp(target, "spim") == 0) {
			compSpim();
		}
	}
}
