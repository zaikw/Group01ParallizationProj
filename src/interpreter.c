#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "structures.h"
#include "parser.h"
#include "hashmap.h"
#include <pthread.h>
#define DPRINT(...) if (debug) {fprintf(debug,__VA_ARGS__);}


char* DEF_FUN[] = {"plus","minus","mult", "div", "equals", "greater", "lesser", "hd", "tl", "cons", "length"};
int DEF_NUM = 11;

map_t symbolmap;
FILE* debug = NULL;
int sequential = 0;

typedef struct {
  pthread_t id;
  Val value;
} ThreadTuple;

typedef struct {
  Val value;
  char* ident;
} ArgName;

typedef struct {
  TreeNode* target;
  ArgName* args;
  int num;
  Val* returnVal;
} ForkArgs;

pthread_t checkFork(ForkArgs*);

void valPrint(Val curr) {
  switch (getType(curr)) {
  case ValueType_INT:
    printf("%ld",getIntVal(curr));
    break;
  case ValueType_LIST:
    printf("[");
    ValList* tempNode = getListVal(curr);
    while (tempNode) {
      valPrint(tempNode->value);
      if (tempNode->next)
	printf(",");
      tempNode = tempNode->next;
    }
    printf("]");
    break;
  case ValueType_CONSTANT:
    printf("NI");
    break;
  case ValueType_FUNCTION:
    printf("NI");
    break;
  }
}

Val evalPlus(Val arg1, Val arg2) {
  return createVal(ValueType_INT, getIntVal(arg1)+getIntVal(arg2));
}

Val evalMinus(Val arg1, Val arg2) {
  return createVal(ValueType_INT, getIntVal(arg1)-getIntVal(arg2));
}

Val evalDiv(Val arg1, Val arg2) {
  return createVal(ValueType_INT, getIntVal(arg1)/getIntVal(arg2));
}

Val evalMult(Val arg1, Val arg2) {
  return createVal(ValueType_INT, getIntVal(arg1)*getIntVal(arg2));
}

Val evalEqual(Val arg1, Val arg2) {
  if(getType(arg1) == ValueType_INT && getType(arg2) == ValueType_INT){
    return createVal(ValueType_INT, getCharVal(arg1) == getCharVal(arg2));
  }
  else if(getType(arg1) == ValueType_LIST && getType(arg2) == ValueType_LIST){
    return createVal(ValueType_INT, getListsEqual(arg1, arg2));
  }
  else{
    return createVal(ValueType_INT, 0);
  }
}

Val evalHead(Val arg) {
  return getListVal(arg)->value;
}

Val evalTail(Val arg) {
  return createVal(ValueType_LIST, (intptr_t) getListVal(arg)->next);
}

Val evalLength(Val arg) {
  return createVal(ValueType_INT, getListLength(arg));
}

Val evalCons(Val arg1, Val arg2) {
  ValList* newNode = malloc(sizeof(ValList));
  newNode->value = arg1;
  newNode->next = getListVal(arg2);
  return createVal(ValueType_LIST, (intptr_t) newNode);
}


Val evalLesser(Val arg1, Val arg2) {
  if(getType(arg1) == ValueType_INT && getType(arg2) == ValueType_INT){
    return createVal(ValueType_INT, (getIntVal(arg1) < getIntVal(arg2)));
  }
  else if(getType(arg1) == ValueType_LIST && getType(arg2) == ValueType_LIST){
    return createVal(ValueType_INT, (getListLength(arg1) < getListLength(arg2)));
  }
  else{
    return createVal(ValueType_INT, 0);
  }
}

Val evalGreater(Val arg1, Val arg2) {
  if(getType(arg1) == ValueType_INT && getType(arg2) == ValueType_INT){
    return createVal(ValueType_INT, (getIntVal(arg1) > getIntVal(arg2)));
  }
  else if(getType(arg1) == ValueType_LIST && getType(arg2) == ValueType_LIST){
    return createVal(ValueType_INT, (getListLength(arg1) > getListLength(arg2)));
  }
  else{
    return createVal(ValueType_INT, 0);
  }
}

int exists(const char* str) {
  for (int i = 0; i < DEF_NUM; i++) {
    if (!strcmp(str,DEF_FUN[i]))
      return 1;
  }
  return 0;
}

Val seqEval(TreeNode* curr, ArgName args[], int argNum) {
  DPRINT("%ld: entering eval\n", pthread_self());
  switch (getType(curr->value)) {
  case ValueType_CONSTANT:
    for (int k=0; k < argNum; k++) {
      if (!strcmp(getCharVal(curr->value),args[k].ident)) {
	DPRINT("%ld: evaluated a symbol from arguments\n", pthread_self());
	return args[k].value;
      }
    }
  case ValueType_FUNCTION:
    if (!strcmp(getCharVal(curr->value),"ite")) {
      DPRINT("%ld: evaluated a if-then-else case\n", pthread_self());
      Val branchBool = seqEval(getArgNode(curr,0), args, argNum);
      if (branchBool.value.intval)
	return seqEval(getArgNode(curr,1),args,argNum);
      else
	return seqEval(getArgNode(curr,2),args,argNum);
    } else { //Execute arguments
      int i = 0;
      PointerListNode* temp = curr->argList;
      while (temp) {temp = temp->next; i++;}
      ThreadTuple* argList = malloc(sizeof(ThreadTuple)*i);
      ForkArgs forkArgs[i];
      temp = curr->argList;
      for (int j = 0; j < i; j++) {
	forkArgs[j].target = temp->target;
	forkArgs[j].args = args;
	forkArgs[j].num = argNum;
	forkArgs[j].returnVal = &(argList[j].value);
	argList[j].id = checkFork(&(forkArgs[j]));
	temp = temp->next;
      }
      temp = curr->argList;
      for (int j = 0; j < i; j++) {
	if (!argList[j].id) {
	  argList[j].value = seqEval(temp->target,args,argNum);
	}
	temp = temp->next;
      }
      void* bogus;
      for (int j = 0; j < i; j++) {
	if (argList[j].id) {
	  pthread_join(argList[j].id, &bogus);
	}
      }
      
      if (!strcmp(getCharVal(curr->value),"plus")) {
	free(argList);
	return evalPlus(argList[0].value,argList[1].value);
      } else if (!strcmp(getCharVal(curr->value),"minus")) {
	free(argList);
	return evalMinus(argList[0].value,argList[1].value);
      } else if (!strcmp(getCharVal(curr->value),"mult")) {
	free(argList);
	return evalMult(argList[0].value,argList[1].value);
      } else if (!strcmp(getCharVal(curr->value),"div")) {
	free(argList);
	return evalDiv(argList[0].value,argList[1].value);
      } else if (!strcmp(getCharVal(curr->value),"equals")) {
	free(argList);
      return evalEqual(argList[0].value,argList[1].value);
      } else if (!strcmp(getCharVal(curr->value),"hd")) {
	free(argList);
	return evalHead(argList[0].value);
      } else if (!strcmp(getCharVal(curr->value),"tl")) {
	free(argList);
	return evalTail(argList[0].value);
      } else if (!strcmp(getCharVal(curr->value),"length")) {
	free(argList);
	return evalLength(argList[0].value);
      } else if (!strcmp(getCharVal(curr->value),"cons")) {
	free(argList);
	return evalCons(argList[0].value,argList[1].value);
      } else if (!strcmp(getCharVal(curr->value),"lesser")) {
	free(argList);
	return evalLesser(argList[0].value,argList[1].value);
      } else if (!strcmp(getCharVal(curr->value),"greater")) {
	free(argList);
	return evalGreater(argList[0].value,argList[1].value);
      } else {
	SymbolIdent* symbolGot;
	hashmap_get(symbolmap, getCharVal(curr->value),&symbolGot);
	int k = 0;
	NameListNode* count_temp = symbolGot->argNames;
	while (count_temp) {
	  k++;
	  count_temp = count_temp->next;
	}
	count_temp = symbolGot->argNames;
	ArgName arguments[k];
	for (int l = 0; l < k; l++) {
	  arguments[l].value = argList[l].value;
	  arguments[l].ident = count_temp->name;
	  count_temp = count_temp->next;
	}
	DPRINT("%ld: evaluated user-defined symbol %s\n", pthread_self(), getCharVal(curr->value));
	free(argList);
	return seqEval(symbolGot->parseTree,arguments,k);
      }
      break;
    }
  case ValueType_INT:
  case ValueType_LIST:
    DPRINT("%ld: evaluated a constant value\n", pthread_self());
    return curr->value;
  }
}

void* prepSeqEval(void* arguments) {
  ForkArgs* args = (ForkArgs*) arguments;
  *(args->returnVal) = seqEval(args->target, args->args, args-> num);
  return 0;
}

pthread_t checkFork(ForkArgs* args)
{
  pthread_t tid = 0;
  if (!sequential && getType(args->target->value) == ValueType_FUNCTION) {
    if (!exists(args->target->value.value.identifier)) {
      pthread_create(&tid, NULL, prepSeqEval, args);
      DPRINT("%ld: Created thread %ld\n", pthread_self(), tid);
    }
  }
  return tid; 
}

int interpretate (FILE* in) {
  SymbolIdent* it = NULL;
  void* olololo;
  while(1){
    it = parse(in, debug);
    if (it == 5) {
      if (debug != stdin)
	fclose(debug);
      return 0;
    }
    else if(it){
      if(it->name){
	if(exists(it->name) || 
	   hashmap_get(symbolmap, it->name, &olololo) == MAP_OK){
	  printf("redefinition is not allowed");
	}
	else {
	  if (it->argNames) {
	    hashmap_put(symbolmap, it->name, it);
	    printf("Defined function %s\n",it->name);
	  }
	  else {
	    SymbolIdent* newIdent = malloc(sizeof(SymbolIdent));
	    newIdent -> name = it->name;
	    newIdent -> argNames = NULL;
	    TreeNode* newNode = malloc(sizeof(TreeNode));
	    newNode->value = seqEval(it->parseTree,NULL,0);
	    newNode->argList = NULL;
	    newIdent -> parseTree = newNode;
	    hashmap_put(symbolmap, it->name, newIdent);
	    printf("Defined %s = ",it->name);
	    valPrint(newNode->value);
	    printf("\n");
	  } 
	}
      }
      else{
	Val calced = seqEval(it->parseTree, NULL,0);
	valPrint(calced);
	printf("\n");
	freeSymbol(it);
	freeVal(calced);
      }
    }
  }
  return 0;
}

int main(int argv, char* argc[]) {
  symbolmap = hashmap_new();
  FILE* in = stdin;
  if (argv > 1) {
    for (int n = 1; n < argv; n++) {
      if (!strcmp(argc[n],"-f")) {
	in = fopen(argc[n+1],"r");
	if (!in) {
	  printf("Failed to open %s\n", argc[n+1]);
	  in = stdin;
	}
	n++;
      } else if (!strcmp(argc[n],"-d")) {
	if ((argv == n+1) || argc[n+1][0] == '-') {
	  debug = stdout;
	}
	else {
	  debug = fopen(argc[n+1],"w");
	  printf("%s\n",argc[n+1]);
	  if (!debug)
	    printf("Failed to open debug file %s\n", argc[n+1]);
	  n++;
	}
      } else if (!strcmp(argc[n],"-s")) {
	sequential = 1;
      }
    }
  }
  return interpretate(in);
}
  


