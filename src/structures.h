#ifndef STRUCTURE_HEADER
#define STRUCTURE_HEADER
#include <stdint.h>
struct PointerListNode;

typedef enum ValueType {
  ValueType_INT, 
  ValueType_LIST, 
  ValueType_CONSTANT, 
  ValueType_FUNCTION
} ValueType;

typedef struct ValList;

typedef struct {
  union {
    intptr_t intval;
    struct ValList* listStart;
    char* identifier;
  } value;
  char type;
} Val;

typedef struct ValList {
  Val value;
  struct ValList* next;
} ValList;

typedef struct TreeNode {
  struct PointerListNode* argList;
  Val value;
} TreeNode;

typedef struct NameListNode {
  char* name;
  struct NameListNode* next;
} NameListNode;

typedef struct SymbolIdent {
  char* name;
  struct NameListNode* argNames;
  struct TreeNode* parseTree;
} SymbolIdent;

typedef struct PointerListNode {
  struct TreeNode* target;
  struct PointerListNode* next;
} PointerListNode;

ValueType getType (Val v);
TreeNode* getArgNode (TreeNode* t, int argnum);
char* getArgName (SymbolIdent* t, int argnum);
Val createVal(ValueType type, intptr_t value);
intptr_t getIntVal (Val v);
char* getCharVal(Val v);
ValList* getListVal (Val v);
int getListLength(Val v);
int getListsEqual(Val arg1, Val arg2);
void freeSymbol(SymbolIdent*);
void freeTree(TreeNode*);
void freePointerList(PointerListNode*);
void freeNameList(NameListNode*);
void freeValList(ValList*);
void freeVal(Val v);

#endif
