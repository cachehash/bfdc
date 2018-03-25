#include <stdio.h>
#include "main.h"

void interpretRaw(Node* n, CELL_T* m, size_t* i) {
	if (n == NULL) {
		return;
	}
	switch (n->type) {
	case LOOP:
		while (m[*i]) {
			interpretRaw(n->n[0].n, m, i);
		}
	break;
	case STMTS:
		interpretRaw(n->n[0].n, m, i);
		interpretRaw(n->n[1].n, m, i);
	break;
	case SUM:
		m[*i + n->n[1].i] += n->n[0].i;
	break;
	case SHIFT:
		*i += n->n[0].i;
	break;
	case OUT: {
		int off = n->n[0].i;
		putchar(m[*i + off]);
		fflush(stdout);
	}
	break;
	case IN: {
		int off = n->n[0].i;
		m[*i + off] = readChar(m[*i + off]);
	}
	break;
	case SET: {
		int off = n->n[0].i;
		for (int k = 1; k < n->sz; k++) {
			Point *p = n->n[k].p;
			int x = p->x;
			int y = p->y;
			int scale = p->z;
			m[*i+x+off] += (y*m[*i+off])/scale;
		}
		m[*i+off] = 0;
	}
	break;
	}
}
void interpRaw() {
	CELL_T m[numCells];
	for (int z = 0; z < numCells; z++) {
		m[z] = 0;
	}
	size_t i[1] = {0};
	I = i;
	interpretRaw(root, m, i);
}
