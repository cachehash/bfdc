#define LOOP 0
#define STMTS 1
#define SUM 2
#define SHIFT 3
#define OUT 4
#define IN 5
typedef struct Node {
	int sz;
	int type;
	union {
		struct Node* n;
		void* v;
		int i;
	} n[];
} Node;

