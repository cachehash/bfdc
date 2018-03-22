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
		m[*i] += n->n[0].i;
	break;
	case SHIFT:
		*i += n->n[0].i;
	break;
	case OUT:
		putchar(m[*i]);
		fflush(stdout);
	break;
	case IN:
		m[*i] = readChar(m[*i]);
	break;
	case SET: {
		for (int k = 0; k < n->sz; k++) {
			Point *p = n->n[k].p;
			int x = p->x;
			int y = p->y;
			int scale = p->z;
			m[*i+x] += (y*m[*i])/scale;
		}
		m[*i] = 0;
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
