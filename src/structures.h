#ifndef STRUCTURE_HEADER
#define STRUCTURE_HEADER

struct PointerListNode;

typedef struct Val {
  struct Val* val;
  struct Val* next;
} Val;

typedef struct TreeNode {
  struct PointerListNode* argList;
  char* name;
  struct Val* value;
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

#endif
