%{
#include "parser.tab.h"
#include <stdint.h>
%}
%%
fun			return FUNCTION;
val 			return VALUE;
\[			return LBRACKET;
\]			return RBRACKET;
\(			return LPARENS;
\)			return RPARENS;
;			return COLON;
[0-9]+			{yylval.i=(intptr_t)atoi(yytext); return NUMBER;}
[a-zA-Z][0-9a-zA-Z]* 	{yylval.cval=yytext; return NAME;}
\+			return PLUS;
-			return MINUS;
\*			return MULT;
\\			return DIV;
=			return EQUAL;
,			|
\n			|
\t			|
.			;
