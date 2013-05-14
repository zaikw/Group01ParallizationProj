%code requires {
#include "structures.h"
#include <stdio.h>
#include <stdint.h>

extern char* strdup(const char*);
}

%{
#include <stdio.h>
#include "structures.h"
#include "parser.h"

SymbolIdent* it = NULL;

void yyerror(const char *str)
{
  //fprintf(stderr,"error: %s\n",str);
}
 
int yywrap()
{
        return 1;
} 

SymbolIdent* parse(int inStream) {
  //Setup insteam if neccecary
  if (!yyparse())
    return it;
  else //Parsing failed for whatever reason
    return NULL;
}

%}

%union {
       char* cval;
       TreeNode* TNval;
       NameListNode* NLNval;
       SymbolIdent* SIval;
       PointerListNode* PLNval;
       ValList* VLval;
       Val Vval;
       intptr_t i;
}      
%type <TNval> expression 
%type <SIval> function constant base_expr
%type <Vval> value
%type <VLval> nodes list
%type <cval> functionname argument
%type <NLNval> arguments
%type <PLNval> expressionlist
%token <cval> NAME
%token <i> NUMBER
%token END FUNCTION VALUE LBRACKET RBRACKET LPARENS RPARENS COLON PLUS MINUS MULT DIV EQUAL
%%

declaration: function COLON {it=$1; printf("Made function\n"); YYACCEPT;}
	     | constant COLON {it=$1; printf("Made value\n"); YYACCEPT;}
	     | base_expr {it=$1; printf("Made base expression\n"); YYACCEPT;}
             | error COLON {printf("Syntax error\n"); YYABORT;}
	     | COLON {YYABORT;}
	     ;

base_expr: expression
	   {
	    SymbolIdent* returnPointer = malloc(sizeof(SymbolIdent));
	    returnPointer->name = NULL;
	    returnPointer->argNames = NULL;
	    returnPointer->parseTree = $1;
	    $$ = returnPointer;
	   }	    

function: FUNCTION NAME LPARENS arguments RPARENS EQUAL expression
	  {
	    SymbolIdent* returnPointer = malloc(sizeof(SymbolIdent));
	    returnPointer->name = strdup($2);
	    returnPointer->argNames = $4;
	    returnPointer->parseTree = $7;
	    $$ = returnPointer;
	  }
	  ;

constant: VALUE NAME EQUAL expression
	  {
	    SymbolIdent* returnPointer = malloc(sizeof(SymbolIdent));
	    returnPointer->name = strdup($2);
	    returnPointer->argNames = NULL;
	    returnPointer->parseTree = $4;
	    $$ = returnPointer;
	  }
	  ;

arguments:		     {$$ = NULL;}
	 | argument arguments
	 {
	    NameListNode* returnPointer = malloc(sizeof(NameListNode));
	    returnPointer->name = $1;
	    returnPointer->next = $2;
	    $$ = returnPointer;
	 }
	 ;

argument: NAME		{$$ = strdup($1);}
	 ;

expressionlist:		{$$=NULL;}
	      | expression expressionlist
	    {
		PointerListNode* returnPointer = malloc(sizeof(PointerListNode));
		returnPointer->next = $2;
		returnPointer->target = $1;
		$$ = returnPointer;
	    }	 
	    ;

expression: functionname LPARENS expressionlist RPARENS 
	    {
		TreeNode* returnPointer = malloc(sizeof(TreeNode));
		returnPointer->argList = $3;
		returnPointer->value = 
		createVal(ValueType_FUNCTION, (intptr_t) $1);
		$$ = returnPointer;
		printf("Made expression function call\n");
	    }
	  | NAME
	    {		
	    	TreeNode* returnPointer = malloc(sizeof(TreeNode));
		returnPointer->argList = NULL;
		returnPointer->value = 
		createVal(ValueType_CONSTANT, (intptr_t) $1);
		$$ = returnPointer;
		printf("Made expression symbol reference\n");
	    }
          | value
	    {
		TreeNode* returnPointer = malloc(sizeof(TreeNode));
		returnPointer->argList = NULL;
		returnPointer->value = $1;
		$$ = returnPointer;
		printf("Made expression constant value\n");
	    }
	    ;

functionname:  PLUS		{$$=strdup("plus");}
	     | MINUS		{$$=strdup("minus");}
	     | MULT		{$$=strdup("mult");}
	     | DIV		{$$=strdup("div");}
	     | EQUAL		{$$=strdup("equals");}
	     | NAME		{$$=strdup($1);}
	     ;

value: list 		{$$=createVal(ValueType_LIST,(intptr_t) $1);
       			 printf("Made list\n");}
     | NUMBER 		{$$=createVal(ValueType_INT,(intptr_t) $1);
			 printf("Made number\n");}
     ;

list: LBRACKET nodes RBRACKET {$$=$2;}
    ;


nodes:	       	     	{$$=NULL;}
     | value nodes	{ValList* returnVal = malloc(sizeof(ValList));
			 returnVal->value=$1;
			 returnVal->next=$2;
			 $$=returnVal;}
     ;
