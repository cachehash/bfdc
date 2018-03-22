%{
extern int lines;
extern int chars;
%}

%option noyywrap
%%

[][><,.+-]	{chars++; return *yytext;}
.		{chars++;}
\n		{lines++; chars = 0;}
