%{
#include <stdio.h>
int yylex();
int lines;
int yyerror(const char* c) {
	fprintf(stderr, "ERROR on line %d: %s\n", lines, c);
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


op	: '+'		{$$ = '+';}
    	| '-'		{$$ = '-';}
	| '>'		{$$ = '>';}
	| '<'		{$$ = '<';}
	| ','		{$$ = ',';}
	| '.'		{$$ = '.';}

%%
