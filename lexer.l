%{
#include "main.h"
#include "parse.tab.h"
extern int lines;
extern int chars;
static int count(int* i) {
	int ret = 0;
	int count = 0;
	static int vals[8*sizeof(char)] = {1};
	if (vals[0] == 1) {
		vals[0] = 0;
		vals['+'] = 1;
		vals['-'] = -1;
		vals['>'] = 1;
		vals['<'] = -1;
	}
	for (int x = 0; yytext[x]; x++) {
		count += vals[yytext[x]];
		ret++;
	}
	*i = count;
	return ret;
}
%}

%option noyywrap
%%

[+-]+		{chars += count(&yylval.i); return *yytext;}
[><]+		{chars += count(&yylval.i); return *yytext;}
[][,.]		{chars++; return *yytext;}
.		{chars++;}
\n		{lines++; chars = 0;}
