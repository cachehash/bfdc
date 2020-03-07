#include <stdlib.h>

extern char* ifile;
extern char* progName;

void runFast() {
	setenv("ifile", ifile, 1);
	setenv("arg0", progName, 1);
	system(" \
		tmp=`mktemp -d`; \
		trap 'rm -rf \"$tmp\"' EXIT; \
		\"$arg0\" -O3 \"$ifile\" -o \"$tmp/prog.c\"; \
		cc -O2 \"$tmp/prog.c\" -o \"$tmp/prog\"; \
		\"$tmp/prog\"; \
	");
}
