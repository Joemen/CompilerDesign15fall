#include "offset.h"

#if 0
  #define SPAM(a) printf a
#else
  #define SPAM(a) (void)0
#endif

//int Offset ;


/*
	So I can do this all over my code
	SPAM(("foo: %d\n", 42));
	and then disable all of them by changing 1 to 0 in #if above.
*/
/*	funtion block */
/*	funtion block */

char *getIdName(AST_NODE *idNode)
{
	return idNode->semantic_value.identifierSemanticValue.identifierName;
}

void setOffsetInSymboltableEntry(AST_NODE* node,int offset)
{
	node->semantic_value.identifierSemanticValue.symbolTableEntry->offset = offset;
	return ;
}

SymbolTableEntry* getSymboltableEntry(AST_NODE *node) {
	return node->semantic_value.identifierSemanticValue.symbolTableEntry;
}
void countParaOffset(AST_NODE *node)
{
	
	AST_NODE* i;
	for (i = node->child; i != NULL; i = i->rightSibling)
	{
		AST_NODE *idNode = i->child->rightSibling;
		Offset = Offset - 4 ;
		
		if(idNode->semantic_value.identifierSemanticValue.kind  == ARRAY_ID)
		{			
			Offset = Offset - 4 ;
		}
		setOffsetInSymboltableEntry(idNode,Offset);
		SPAM(("parameter %s offset is %d\n", getIdName(idNode), Offset));		
	}
}

AST_TYPE getAstType(AST_NODE* node)
{
	return node->nodeType;
}

DECL_KIND getDeclKind(AST_NODE* node)
{
	return node->semantic_value.declSemanticValue.kind;
}	

int countArraySize(SymbolTableEntry *entry) {
	int dim;
	int count = 1;
	for (dim = 0; dim < entry->attribute->attr.typeDescriptor->properties.arrayProperties.dimension; dim++) {
		count *= entry->attribute->attr.typeDescriptor->properties.arrayProperties.sizeInEachDimension[dim];
	}
	return count;
}

void countVarOffset(AST_NODE* node)
{
	AST_NODE* tempNode;
	AST_NODE* iNode;
	
	/*	*/
	if(getAstType(node) == VARIABLE_DECL_LIST_NODE)
	{
		
		for(tempNode = node->child;tempNode !=NULL; tempNode = tempNode -> rightSibling)
		{
				if(getDeclKind(tempNode) == VARIABLE_DECL){
					for(iNode = tempNode->child->rightSibling ; iNode!= NULL ; iNode = iNode->rightSibling )
					{
							SymbolTableEntry *entry = getSymboltableEntry(iNode);
							if (entry->attribute->attr.typeDescriptor->kind == ARRAY_TYPE_DESCRIPTOR)
							{
								Offset -= (countArraySize(entry) * 4);
								SPAM(("ARRAY %s offset is %d\n", entry->name, Offset));	
							}		
							else
							{
								Offset -= 4;
								SPAM(("VARIABLE %s offset is %d\n", getIdName(iNode), Offset));	
							}
							entry->offset = Offset;
							
					}
				
				}
			
		}
		
		
	}
	/* no variable declaration */
	return ;	
}
void visitchildNode(AST_NODE* node)
{
	if(node->child ==  NULL)	return ;
	AST_NODE *iNode = node->child;
	AST_NODE *i ;
	//SPAM(("%d\n",getAstType(node)));

	if(getAstType(node) == DECLARATION_NODE && getDeclKind(node) == FUNCTION_DECL)
	{
		Offset = 0; 
		
		/* Go into first line of fuction */
		for(i=iNode; i!=NULL; i=i->rightSibling)
		{
			visitchildNode(i);
		}
		
	}
	else if(getAstType(node) == PARAM_LIST_NODE)
	{
		countParaOffset(node);
		return ;
	}
	else if(getAstType(node) == BLOCK_NODE)
	{
		if(node->child != NULL){
			countVarOffset(node->child);
		}
		for(i=iNode; i!=NULL; i=i->rightSibling)
		{
			visitchildNode(i);
		}
	}
	else
	{
		/*	no decl so no need to set offset*/
		for(i=iNode; i!=NULL; i=i->rightSibling)
		{
			visitchildNode(i);
		}
	}	
}


/* visit child of prog node */
void setoffset(AST_NODE* prog)
{
	visitchildNode(prog);
}

