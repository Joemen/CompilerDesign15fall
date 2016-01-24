#include <stdio.h>
#include "header.h"
#include "symbolTable.h"


void setoffset(AST_NODE* prog);
void visitchildNode(AST_NODE* node);
AST_TYPE getAstType(AST_NODE* node);
DECL_KIND getDeclKind(AST_NODE* node);
void countParaOffset(AST_NODE* node);
void setOffsetInSymboltableEntry(AST_NODE* node,int offset);
char *getIdName(AST_NODE* idNode);
void countVarOffset(AST_NODE* node);
int countArraySize(SymbolTableEntry* entry);
SymbolTableEntry *getSymboltableEntry(AST_NODE *node);
