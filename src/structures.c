#include "structures.h"
#include <stdint.h>
#include <stdlib.h>

ValueType getType (Val v) {
  switch (v.type) {
  case 0:
    return ValueType_INT;
  case 1:
    return ValueType_LIST;
  case 2:
    return ValueType_CONSTANT;
  case 3:
    return ValueType_FUNCTION;
  }
  return ValueType_INT;
}

intptr_t getIntVal(Val v) {
  return v.value.intval;
}

ValList* getListVal(Val v) {
  return v.value.listStart;
}

int getListLength(Val v) {
  ValList* tempList = getListVal(v);  
  if(!tempList){
    return 0;
  }
  else{
    int i = 1;
    while(tempList->next){
      tempList = tempList->next;
      i++;
    }
    return i;
  }
}

int getListsEqual(Val arg1, Val arg2) {
  ValList* tempList1 = getListVal(arg1);
  ValList* tempList2 = getListVal(arg2);
  while(tempList1 && tempList2){
    if(getIntVal(tempList1->value) != getIntVal(tempList2->value))
      return 0;
    tempList1 = tempList1->next;
    tempList2 = tempList2->next;
  }
  if((!tempList1 && !tempList2)){
    return 1;
  }
  else{
    return 0;
  }
}
     


char* getCharVal(Val v) {
  return v.value.identifier;
}

TreeNode* getArgNode (TreeNode* node, int ArgNum) {
  PointerListNode* argument = node->argList;
  for (int i = 0; i < ArgNum; i++) {argument = argument->next;}
  return argument->target;
}

char* getArgName (SymbolIdent* node, int ArgIndex) {
  NameListNode* argument = node->argNames;
  for (int i = 0; i < ArgIndex ; i++) {argument = argument->next;}
  return argument->name;
}

Val createVal(ValueType type, intptr_t value) {
  Val returnVal;
  returnVal.value.intval = value; //dubbelkod?
  switch (type) {
  case ValueType_INT:
    returnVal.type = 0;
    break;
  case ValueType_LIST:
    returnVal.type = 1;
    break;
  case ValueType_CONSTANT:
    returnVal.type = 2;
    break;
  case ValueType_FUNCTION:
    returnVal.type = 3;
    break;
  }
  returnVal.value.intval = value;
  return returnVal;
}

void freeVal(Val target) {
  //if (getType(target) == ValueType_LIST)
    //freeValList(target.value.listStart);
  if (getType(target) == ValueType_CONSTANT ||
      getType(target) == ValueType_FUNCTION)
    if (target.value.identifier)
      free(target.value.identifier);
}

void freeValList(ValList* target) {
  if (target) {
    freeVal(target->value);
    freeValList(target->next);
    free(target);
  }
}

void freeNameList(NameListNode* target) {
  if (target) {
    if (target->name)
      free(target->name);
    freeNameList(target->next);
    free(target);
  }
}

void freePointerList(PointerListNode* target) {
  if (target) {
    freeTree(target->target);
    freePointerList(target->next);
    free(target);
  }
}

void freeTree(TreeNode* target) {
  if (target) {
    freePointerList(target->argList);
    freeVal(target->value);
    free(target);
  }
}

void freeSymbol(SymbolIdent* target) {
  if (target) {
    if (target->name)
      free(target->name);
    freeNameList(target->argNames);
    freeTree(target->parseTree);
    free(target);
  }
}
