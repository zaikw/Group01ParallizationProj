%code requires {
#include "structures.h"
#include <stdio.h>

extern char* strdup(const char*);
}

%{
#include <stdio.h>
#include <assert.h>
#include "structures.h"

TreeNode* lastTree = NULL;

void yyerror(const char *str)
{
        fprintf(stderr,"error: %s\n",str);
}
 
int yywrap()
{
        return 1;
} 

void valPrint(Val* curr) {
  if (curr->next == curr)
    printf("%d",(int)curr->val);
  else {
    printf("[");
    valPrint(curr->val);
    if (curr->next) {
      printf(",");
      valPrint(curr->next);
    }
    printf("]");
  }
}

void walkPrint(TreeNode* curr) {
  if (curr->name)
    printf("Found %s: ",curr->name);
  if (curr->value)
    valPrint(curr->value);
  PointerListNode* temp = curr->argList;
  do {
    if (temp->target)
      walkPrint(temp->target);
    else
      assert(0);
    temp = temp->next;
  } while (temp);
}

main()
{
	if (!yyparse()) {
	   walkPrint(lastTree);
	}
}
%}

%union {
       char* cval;
       TreeNode* TNval;
       NameListNode* NLNval;
       SymbolIdent* SIval;
       PointerListNode* PLNval;
       Val* Vval;
       int i;
}      
%type <TNval> expression 
%type <SIval> function constant declaration
%type <Vval> value list nodes
%type <cval> functionname argument
%type <NLNval> arguments
%type <PLNval> expressionlist
%token <cval> NAME
%token <i> NUMBER
%token FUNCTION VALUE LBRACKET RBRACKET LPARENS RPARENS COLON PLUS MINUS MULT DIV EQUAL
%%

declaration: function {lastTree=$1; printf("Made function\n");}
	   | constant {lastTree=$1; printf("Made value\n");}
	   ;

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
		returnPointer->name = strdup($1);
		returnPointer->value = NULL;
		$$ = returnPointer;
		printf("Made expression function call\n");
	    }
	  | NAME
	    {		
	    	TreeNode* returnPointer = malloc(sizeof(TreeNode));
		returnPointer->argList = NULL;
		returnPointer->name = strdup($1);
		returnPointer->value = NULL;
		$$ = returnPointer;
		printf("Made expression symbol reference\n");
	    }
          | value
	    {
		TreeNode* returnPointer = malloc(sizeof(TreeNode));
		returnPointer->argList = NULL;
		returnPointer->name = NULL;
		returnPointer->value = $1;
		$$ = returnPointer;
		printf("Made expression constant value\n");
	    }
	    ;

functionname:  PLUS		{$$=strdup("plus");}
	     | MINUS		{$$=strdup("minus");}
	     | MULT		{$$=strdup("mult");}
	     | DIV		{$$=strdup("div");}
	     | NAME		{$$=strdup($1);}
	     ;

value: list 		{$$=$1;}
     | NUMBER 		{Val* returnPointer = malloc(sizeof(Val));
       			 printf("%d",sizeof(Val*));
       	       		 returnPointer->val = (Val*)$1;
			 returnPointer->next = returnPointer;
	       		 $$=returnPointer;
			 printf("Made number\n");}
     ;

list: LBRACKET nodes RBRACKET {$$=$2;
			       printf("Made list\n");}
    ;


nodes:	       	     	{Val* returnVal = malloc(sizeof(Val));
			 returnVal->val=0;
			 returnVal->next=0;
			 $$=returnVal;}
     | value nodes	{Val* returnVal = malloc(sizeof(Val));
			 returnVal->val=$1;
			 returnVal->next=$2;
			 $$=returnVal;}
     ;
