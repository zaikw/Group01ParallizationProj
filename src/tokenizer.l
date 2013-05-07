%{
#include "parser.tab.h"
%}
%%
fun			return FUNCTION;
val 			return VALUE;
\[			return LBRACKET;
\]			return RBRACKET;
\(			return LPARENS;
\)			return RPARENS;
,
\;			return EOF;
[0-9]+			yylval.i=atoi(yytext); return NUMBER;
[a-zA-Z][0-9a-zA-Z]* 	yylval.cval=yytext; return NAME;
\+			return PLUS;
-			return MINUS;
\*			return MULT;
\\			return DIV;
=			return EQUAL;
