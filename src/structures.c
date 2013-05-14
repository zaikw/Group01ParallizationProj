#include "structures.h"
#include <stdint.h>

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
