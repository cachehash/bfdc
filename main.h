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

#define EOF_M1 0
#define EOF_0 1
#define EOF_NC 2

#define READ_CHAR_FMT \
"CELL_T readChar(int prev) {\n\t"\
	"int x = getchar();\n\t"\
	"return (x == EOF) ? %s : x;\n}"

#define EOF_0_STR READ_CHAR_HEAD 
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
CELL_T readChar();

extern char* eofStr;
extern int eofType;
extern char* cell_t_str;

#endif
