%{

%}
%%

%union {
	char c;
}
%type<c> op
%type loop

%%

prog	: stmt prog
	| 

stmt	: loop
	| op

loop	: '[' ops ']'

ops 	: op ops
	| 

op	: '+'		{$$ = '+';}
    	| '-'		{$$ = '-';}
	| '>'		{$$ = '>';}
	| '<'		{$$ = '<';}
	| ','		{$$ = ',';}
	| '.'		{$$ = '.';}

%%
