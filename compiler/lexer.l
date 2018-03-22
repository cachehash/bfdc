%{
extern int lines;
%}

%option noyywrap
%%

[][><,.+-]	{return *yytext;}
.		{}
\n		{lines++;}
