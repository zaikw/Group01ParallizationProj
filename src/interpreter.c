#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include "structures.h"
#include "parser.h"
#include "hashmap.h"

map_t symbolmap;

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
  //assert(getType(arg1) == getType(arg2) == ValueType_INT);
  return createVal(ValueType_INT, getIntVal(arg1)+getIntVal(arg2));
}

Val evalMinus(Val arg1, Val arg2) {
  //assert(getType(arg1) == getType(arg2) == ValueType_INT);
  return createVal(ValueType_INT, getIntVal(arg1)-getIntVal(arg2));
}

Val evalDiv(Val arg1, Val arg2) {
  //assert(getType(arg1) == getType(arg2) == ValueType_INT);
  return createVal(ValueType_INT, getIntVal(arg1)/getIntVal(arg2));
}

Val evalMult(Val arg1, Val arg2) {
  //assert(getType(arg1) == getType(arg2) == ValueType_INT);
  return createVal(ValueType_INT, getIntVal(arg1)*getIntVal(arg2));
}

Val evalEqual(Val arg1, Val arg2) {
  return createVal(ValueType_INT, getCharVal(arg1) == getCharVal(arg2));
}

Val evalHead(Val arg) {
  assert(getType(arg) == ValueType_LIST);
  return getListVal(arg)->value;
}

Val evalTail(Val arg) {
  //assert(getType(arg) == ValueType_LIST);
  return createVal(ValueType_LIST, (intptr_t) getListVal(arg)->next);
}

Val evalCons(Val arg1, Val arg2) {
  //assert(getType(arg2) == ValueType_LIST);
  ValList* newNode = malloc(sizeof(ValList));
  newNode->value = arg1;
  newNode->next = getListVal(arg2);
  return createVal(ValueType_LIST, (intptr_t) newNode);
}

Val seqEval(TreeNode* curr, ArgName args[], int argNum) {
  switch (getType(curr->value)) {
  case ValueType_CONSTANT:
    for (int k=0; k < argNum; k++) {
      if (!strcmp(getCharVal(curr->value),args[k].ident)) {
	return args[k].value;
      }
    }
  case ValueType_FUNCTION:
    if (!strcmp(getCharVal(curr->value),"plus")) {
      return evalPlus(seqEval(getArgNode(curr,0), args, argNum),
		      seqEval(getArgNode(curr,1), args, argNum));
    } else if (!strcmp(getCharVal(curr->value),"minus")) {
      return evalMinus(seqEval(getArgNode(curr,0), args, argNum),
		      seqEval(getArgNode(curr,1), args, argNum));
    } else if (!strcmp(getCharVal(curr->value),"mult")) {
      return evalMult(seqEval(getArgNode(curr,0), args, argNum),
		      seqEval(getArgNode(curr,1), args, argNum));
    } else if (!strcmp(getCharVal(curr->value),"div")) {
      return evalDiv(seqEval(getArgNode(curr,0), args, argNum),
		     seqEval(getArgNode(curr,1), args, argNum));
    } else if (!strcmp(getCharVal(curr->value),"equals")) {
      return evalEqual(seqEval(getArgNode(curr,0), args, argNum),
		       seqEval(getArgNode(curr,1), args, argNum));
    } else if (!strcmp(getCharVal(curr->value),"hd")) {
      return evalHead(seqEval(getArgNode(curr,0), args, argNum));
    } else if (!strcmp(getCharVal(curr->value),"tl")) {
      return evalTail(seqEval(getArgNode(curr,0), args, argNum));
    } else if (!strcmp(getCharVal(curr->value),"cons")) {
      return evalCons(seqEval(getArgNode(curr,0), args, argNum),
		      seqEval(getArgNode(curr,1), args, argNum));
    } else if (!strcmp(getCharVal(curr->value),"ite")) {
      Val branchBool = seqEval(getArgNode(curr,0), args, argNum);
      if (branchBool.value.intval)
	return seqEval(getArgNode(curr,1),args,argNum);
      else
	return seqEval(getArgNode(curr,2),args,argNum);
    } else {
      SymbolIdent* temp;
      hashmap_get(symbolmap, getCharVal(curr->value),&temp);
      int i = 0;
      NameListNode* count_temp = temp->argNames;
      while (count_temp) {
	i++;
	count_temp = count_temp->next;
      }
      count_temp = temp->argNames;
      ArgName arguments[i];
      for (int j = 0; j < i; j++) {
	arguments[j].value = seqEval(getArgNode(curr,j), args, argNum);
	arguments[j].ident = count_temp->name;
	count_temp = count_temp->next;
      }
      return seqEval(temp->parseTree,arguments,i);
    }
    break;
  case ValueType_INT:
  case ValueType_LIST:
    return curr->value;
  }
}

int main(int argv, char** argc){
  symbolmap = hashmap_new();
  SymbolIdent* it = NULL;
  void* olololo;
  while(1){
    it = parse(0);
    if (it == 5) {
      return 0;
    }
    else if(it){
      if(it->name){
	if(hashmap_get(symbolmap, it->name, &olololo) == MAP_OK){
	    printf("redefinition is not alowed");
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
      }
    }
  }
  return 0;
}
