%{
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "main.h"
#define YYMAXDEPTH 10000

int yylex();
int lines = 1;
int chars = 0;
int yyerror(const char* c) {
	return fprintf(stderr, "ERROR on line %d, col %d: %s\n", lines, chars, c);
}

Node* mkNode(int sz, int type) {
	Node* n = calloc(1, sizeof(Node) + sz*sizeof(Node*));
	n->sz = sz;
	n->type = type;
	return n;
}
Node* buildNode(int sz, int type, ...) {
	Node* n = mkNode(sz, type);

	va_list ap;
	va_start(ap, type);
	for (int i = 0; i < sz; i++) {
		n->n[i] = va_arg(ap, Child);
	}
	return n;
}
%}
%union {
	int i;
	char c;
	Node* n;
}

%type<n> op
%type<n> loop stmt stmts
%type<i> inc shift
%type<i> incs shifts
%token<i> '+' '-' '>' '<'

%define parse.error verbose 
%%

prog	: stmts			{root = $1;}


stmts	: stmt stmts		{$$ = buildNode(2, STMTS, $1, $2);}
	| 			{$$ = NULL;}

stmt	: loop			{$$ = $1;}
	| op			{$$ = $1;}

loop	: '[' stmts ']'		{$$ = buildNode(1, LOOP, $2);}


inc	: '+'			{$$ = $1;}
    	| '-'			{$$ = $1;}

shift	: '>'			{$$ = $1;}
    	| '<'			{$$ = $1;}

incs	: inc			{$$ = $1;}
	| incs inc		{$$ = $1 + $2;}
	

shifts	: shift			{$$ = $1;}
	| shifts shift		{$$ = $1 + $2;}
	

op	: incs			{$$ = buildNode(2, SUM, $1, 0);}
	| shifts		{$$ = buildNode(1, SHIFT, $1);}
	| ','			{$$ = buildNode(1, IN, 0);}
	| '.'			{$$ = buildNode(1, OUT, 0);}

%%
