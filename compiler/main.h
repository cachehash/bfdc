#ifndef MAIN_H
#define MAIN_H
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#define LOOP 0
#define STMTS 1
#define SUM 2
#define SHIFT 3
#define OUT 4
#define IN 5
#define SET 6

#ifndef CELL_T
#define CELL_T uint8_t
#endif

#ifndef NUM_CELLS
#define NUM_CELLS 30000
#endif

typedef struct Point {
	int x;
	int y;
	int z;
} Point;
typedef union Child {
	struct Node* n;
	void* v;
	//TODO why can't this be ptrdiff_t
	int i;
	Point* p;
} Child;
typedef struct Node {
	int sz;
	int type;
	Child n[];
} Node;
extern int numCells;
extern Node* mkNode(int,int);
extern Node* root;
extern FILE* outfile;

#endif
