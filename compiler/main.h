#define LOOP 0
#define STMTS 1
#define SUM 2
#define SHIFT 3
#define OUT 4
#define IN 5
#define SET 6
typedef struct Point {
	int x;
	int y;
} Point;
typedef struct Node {
	int sz;
	int type;
	union {
		struct Node* n;
		void* v;
		int i;
		Point p;
	} n[];
} Node;

extern Node* mkNode(int,int);