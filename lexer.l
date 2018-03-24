%{
#include "main.h"
#include "parse.tab.h"
extern int lines;
extern int chars;
%}

%option noyywrap
%%

[+]+		{int c = strlen(yytext); chars+=c; yylval.i = c; return *yytext;}
[-]+		{int c = strlen(yytext); chars+=c; yylval.i = -c; return *yytext;}
[>]+		{int c = strlen(yytext); chars+=c; yylval.i = c; return *yytext;}
[<]+		{int c = strlen(yytext); chars+=c; yylval.i = -c; return *yytext;}
[][,.]		{chars++; return *yytext;}
.		{chars++;}
\n		{lines++; chars = 0;}
