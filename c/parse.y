%{
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
int yylex();
int lines;
int yyerror(const char* c) {
	fprintf(stderr, "ERROR on line %d: %s\n", lines, c);
}

typedef struct Node {
	int sz;
	int type;
	union {
		struct Node* n;
		void* v;
	} child[];
} Node;
Node* mkNode(int sz, int type, ...) {
	Node* n = calloc(1, sizeof(Node) + sz*sizeof(Node*));
	n->sz = sz;
	n->type = type;

	va_list ap;
	va_start(ap, type);
	for (int i = 0; i < sz; i++) {
		n->child[i].v = va_arg(ap, void*);
	}
	return n;
}
%}

%union {
	char c;
}

%type<c> op

%define parse.error verbose 
%%

prog	: stmts


stmts	: stmt stmts
	| 

stmt	: loop
	| op

loop	: '[' stmts ']'


op	: '+'			{$$ = '+';}
    	| '-'			{$$ = '-';}
	| '>'			{$$ = '>';}
	| '<'			{$$ = '<';}
	| ','			{$$ = ',';}
	| '.'			{$$ = '.';}

%%
