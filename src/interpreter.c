#include "parser.h"
#include "structures.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

Val* seqWalkCalc(TreeNode* curr) {
  if (curr->argList) {
    if (!strcmp(curr->name,"plus")) {
      return (intptr_t) seqWalkCalc(curr->argList[0].target)->val +
	(intptr_t) seqWalkCalc(curr->argList[1].target)->val;
    } else if (!strcmp(curr->name,"minus")) {
      return (intptr_t) seqWalkCalc(curr->argList[0].target)->val -
	(intptr_t) seqWalkCalc(curr->argList[1].target)->val;
    }
  }
  return curr->value;
}

int main (int argv, char** argc) {
  SymbolIdent* it = NULL;
  //Possible initial read from file here, otherwise:
  while (1) {
    it = parse(0);
    if (it) {
      Val* calced = seqWalkCalc(it->parseTree);
      printf("%lu\n",calced->val);
    }
  }
}

void valPrint(Val* curr) {
  if (curr->next == curr)
    printf("%lu",(intptr_t)curr->val);
  else {
    printf("[");
    if (curr->val)
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
  while (temp) {
    if (temp->target)
      walkPrint(temp->target);
    else
      assert(0);
    temp = temp->next;
  }
}
