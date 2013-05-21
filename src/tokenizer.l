%{
#include "parser.tab.h"
#include <stdint.h>
%}
%%
fun			return FUNCTION;
val 			return VALUE;
quit			return QUIT;
if			return IF;
then 			return THEN;
else			return ELSE;
\[			return LBRACKET;
\]			return RBRACKET;
\(			return LPARENS;
\)			return RPARENS;
;			return COLON;
[0-9]+			{yylval.i=(intptr_t)atoi(yytext); return NUMBER;}
[a-zA-Z][0-9a-zA-Z]* 	{yylval.cval=strdup(yytext); return NAME;}
\+			{yylval.cval=strdup("plus"); return PLUS;}
-			{yylval.cval=strdup("minus"); return MINUS;}
\*			{yylval.cval=strdup("mult"); return MULT;}
\/			{yylval.cval=strdup("div"); return DIV;}
\<                      {yylval.cval=strdup("lesser"); return LESSER;}
\>                      {yylval.cval=strdup("greater"); return GREATER;}
=			{yylval.cval=strdup("equals"); return EQUAL;}
,			return COMMA;
\n			|
\t			|
.			;
