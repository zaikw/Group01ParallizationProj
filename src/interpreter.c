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
    printf("%lu",getIntVal(curr));
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

Val seqEval(TreeNode* curr, ArgName args[]) {
  switch (getType(curr->value)) {
  case ValueType_CONSTANT:
  case ValueType_FUNCTION:
    if (!strcmp(getCharVal(curr->value),"plus")) {
      return evalPlus(seqEval(getArgNode(curr,0), args),
		      seqEval(getArgNode(curr,1), args));
    } else if (!strcmp(getCharVal(curr->value),"minus")) {
      return evalMinus(seqEval(getArgNode(curr,0), args),
		      seqEval(getArgNode(curr,1), args));
    } else if (!strcmp(getCharVal(curr->value),"mult")) {
      return evalMult(seqEval(getArgNode(curr,0), args),
		      seqEval(getArgNode(curr,1), args));
    } else if (!strcmp(getCharVal(curr->value),"div")) {
      return evalDiv(seqEval(getArgNode(curr,0), args),
		     seqEval(getArgNode(curr,1), args));
    } else if (!strcmp(getCharVal(curr->value),"equals")) {
      return evalEqual(seqEval(getArgNode(curr,0), args),
		       seqEval(getArgNode(curr,1), args));
    } else if (!strcmp(getCharVal(curr->value),"hd")) {
      return evalHead(seqEval(getArgNode(curr,0), args));
    } else if (!strcmp(getCharVal(curr->value),"tl")) {
      return evalTail(seqEval(getArgNode(curr,0), args));
    } else if (!strcmp(getCharVal(curr->value),"cons")) {
      return evalCons(seqEval(getArgNode(curr,0), args),
		      seqEval(getArgNode(curr,1), args));
    } else {
      SymbolIdent* temp;
      hashmap_get(symbol_map, getCharVal(curr->value),&temp);
      int i = 0;
      NameListNode* count_temp = temp->argNames;
      while (count_temp) {
	i++;
	count_temp = count_temp->next;
      }
      count_temp = temp->argNames;
      ArgName arguments[i];
      for (int j = 0; j < i; j++) {
	arguments[j].value = seqEval(getArgNode(curr,j), args);
	arguments[j].ident = count_temp->name;
	count_temp = count_temp->next;
      }
      return seqEval(temp->parseTree,args);
    }
    break;
  case ValueType_INT:
  case ValueType_LIST:
    return curr->value;
  }
}

int old_main (int argv, char** argc) {
  SymbolIdent* it = NULL;
  //Possible initial read from file here, otherwise:
  while (1) {
    it = parse(0);
    if (it) {
      Val calced = seqEval(it->parseTree,NULL);
      printf("it = ");
      valPrint(calced);
      printf("\n");
    }
  }
}

int main(int argv, char** argc){
  symbolmap = newMap();
  SymbolIdent* it = NULL;
  while(1){
    it = parse(0);
    if(it){
      if(it->name != NULL && it->argNames != NULL){
	if(hashmap_get(m, it->name, NULL) == MAP_OK){
	  fprintf("redefinition is not alowed");
	}
	else {
	  hashmap_put(m, it->name, it); 
	}
      }
      else{
	if(hashmap_get(m, it->name, NULL) == MAP_OK){
	  fprintf("redefinition is not alowed");
	}
	else {
	  it->parseTree->value = seqEval(it->parseTree);
	  hashmap_put(m, it->name, it); 
	}
      }
    }
  }
  return 0;
}
