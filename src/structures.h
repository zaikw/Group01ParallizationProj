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

typedef struct ValList ValList;

typedef struct {
  union { //Last two bits are type identifier
    intptr_t intval;
    struct ValList* listStart;
    char* identifier;
  } value;
  char type;
} Val;

typedef struct {
  Val value;
  char* ident;
} ArgName;

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

#endif
