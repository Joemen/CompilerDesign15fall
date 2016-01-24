#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"
#include "symbolTable.h"

#if 0
  #define SPAM(a) printf a
#else
  #define SPAM(a) (void)0
#endif
/*
	So I can do this all over my code
	SPAM(("foo: %d\n", 42));
	and then disable all of them by changing 1 to 0 in #if above.
*/


/*	define by b01902103*/
#define MAX_ARRAY_DIM_SIZE 10

// This file is for reference only, you are not required to follow the implementation. //
// You only need to check for errors stated in the hw4 assignment document. //
int g_anyErrorOccur = 0;

DATA_TYPE getBiggerType(DATA_TYPE dataType1, DATA_TYPE dataType2);
void processProgramNode(AST_NODE *programNode);
void processDeclarationNode(AST_NODE* declarationNode);
void declareIdList(AST_NODE* typeNode, SymbolAttributeKind isVariableOrTypeAttribute, int ignoreArrayFirstDimSize);
void declareFunction(AST_NODE* returnTypeNode);
void processDeclDimList(AST_NODE* variableDeclDimList, TypeDescriptor* typeDescriptor, int ignoreFirstDimSize);
void processTypeNode(AST_NODE* typeNode);
void processBlockNode(AST_NODE* blockNode);
void processStmtNode(AST_NODE* stmtNode);
void processGeneralNode(AST_NODE *node);
void checkAssignOrExpr(AST_NODE* assignOrExprRelatedNode);
void checkWhileStmt(AST_NODE* whileNode);				//function param
void checkForStmt(AST_NODE* forNode);					//function param
void checkAssignmentStmt(AST_NODE* assignmentNode);		//type confliction
void checkIfStmt(AST_NODE* ifNode);						//function param
void checkWriteFunction(AST_NODE* functionCallNode);	//function param
void checkFunctionCall(AST_NODE* functionCallNode);		//function param
void processExprRelatedNode(AST_NODE* exprRelatedNode);
void checkParameterPassing(Parameter* formalParameter, AST_NODE* actualParameter);
void checkReturnStmt(AST_NODE* returnNode);
void processExprNode(AST_NODE* exprNode);
void processVariableLValue(AST_NODE* idNode);
void processVariableRValue(AST_NODE* idNode);
void processConstValueNode(AST_NODE* constValueNode);
void getExprOrConstValue(AST_NODE* exprOrConstNode, int* iValue, float* fValue);
void evaluateExprValue(AST_NODE* exprNode);
/*	Add by b01902103 */
void visit(AST_NODE *node);
void visitChild(AST_NODE* node);
void processVarDeclList(AST_NODE *node);
char* getIDNodeName(AST_NODE* node);
IDENTIFIER_KIND getIDKind(AST_NODE* node);
DATA_TYPE getDataType(AST_NODE* node);
EXPR_KIND getExprKind(AST_NODE* node);

void setIdSymtabEntry(AST_NODE *node, SymbolTableEntry *sym);
TypeDescriptorKind getTypekind(SymbolTableEntry* sym);
/*  ADD by b01902098 */
void checkVariableDimensions(AST_NODE* node);
int checkArrayIsScalar(AST_NODE* array_node);
/*	ADD end */


/*	useful function add by b01902103 */

char* getIDNodeName(AST_NODE* node)
{
	return node->semantic_value.identifierSemanticValue.identifierName;
}

IDENTIFIER_KIND getIDKind(AST_NODE* node)
{
	return node->semantic_value.identifierSemanticValue.kind;
}

void setIdSymtabEntry(AST_NODE *node, SymbolTableEntry *sym)
{
    node->semantic_value.identifierSemanticValue.symbolTableEntry = sym;
}

TypeDescriptorKind getTypekind(SymbolTableEntry* sym)
{	
	return sym->attribute->attr.typeDescriptor->kind;
}

EXPR_KIND getExprKind(AST_NODE* node)
{
	return 	node->semantic_value.exprSemanticValue.kind ;
}
/*	useful function END add by b01902103 */

typedef enum ErrorMsgKind
{
    SYMBOL_IS_NOT_TYPE,
    SYMBOL_REDECLARE,
    SYMBOL_UNDECLARED,
    NOT_FUNCTION_NAME,
    TRY_TO_INIT_ARRAY,
    EXCESSIVE_ARRAY_DIM_DECLARATION,
    RETURN_ARRAY,
    VOID_VARIABLE,
    TYPEDEF_VOID_ARRAY,
    PARAMETER_TYPE_UNMATCH,
    TOO_FEW_ARGUMENTS,
    TOO_MANY_ARGUMENTS,
    RETURN_TYPE_UNMATCH,
    INCOMPATIBLE_ARRAY_DIMENSION,
    NOT_ASSIGNABLE,
    NOT_ARRAY,
    IS_TYPE_NOT_VARIABLE,
    IS_FUNCTION_NOT_VARIABLE,
    STRING_OPERATION,
    ARRAY_SIZE_NOT_INT,
    ARRAY_SIZE_NEGATIVE,
    ARRAY_SUBSCRIPT_NOT_INT,
    ARRAY_SUBSCRIPT_NEGATIVE,
    PASS_ARRAY_TO_SCALAR,
    PASS_SCALAR_TO_ARRAY
} ErrorMsgKind;

void printErrorMsgSpecial(AST_NODE* node, char* name2, ErrorMsgKind errorMsgKind)
{
    g_anyErrorOccur = 1;
    printf("Error found in line %d\n", node->linenumber);
    
    switch(errorMsgKind)
    {
    	case PASS_ARRAY_TO_SCALAR:
			printf("Array \'%s\' passed to scalar parameter \'%s\'.\n",getIDNodeName(node),name2);
			break;
		case PASS_SCALAR_TO_ARRAY:
			printf("Scalar \'%s\' passed to array parameter \'%s\'.\n",getIDNodeName(node),name2);
			break;		
		default:
        	printf("Unhandled case in void printErrorMsg(AST_NODE* node, ERROR_MSG_KIND* errorMsgKind)\n");
        	break;
    }
    
}

/*TODO*/
// need to modify to correct one
void printErrorMsg(AST_NODE* node, ErrorMsgKind errorMsgKind)
{
    g_anyErrorOccur = 1;
    printf("Error found in line %d\n", node->linenumber);
    switch(errorMsgKind)
    {	
		case SYMBOL_IS_NOT_TYPE:
			printf("ID \'%s\' is not a type name.\n",getIDNodeName(node));
			break;
		case SYMBOL_REDECLARE:
			printf("ID \'%s\' redeclared.\n",getIDNodeName(node));
			break;
		case SYMBOL_UNDECLARED:
			printf("ID \'%s\' undeclared.\nn",getIDNodeName(node));
			break;
		case NOT_FUNCTION_NAME:
			printf("ID \'%s\' is not a function.\n",getIDNodeName(node));
			break;
		case TRY_TO_INIT_ARRAY:
			printf("Cannot initialize array \'%s\'.\n",getIDNodeName(node));
			break;
		case EXCESSIVE_ARRAY_DIM_DECLARATION:
			printf("ID \'%s\' array dimension cannot be greater than %d\n",getIDNodeName(node),MAX_ARRAY_DIM_SIZE);
			break;
		case RETURN_ARRAY:
			printf("Function \'%s\' cannot return array.\n",getIDNodeName(node->rightSibling));
			break;
		case VOID_VARIABLE:
			printf("Type \'%s\' cannot be a variable's type.\n",getIDNodeName(node));
			break;
		case TYPEDEF_VOID_ARRAY:
			printf("Declaration of \'%s\' as array of voids.\n",getIDNodeName(node));
			break;
		case PARAMETER_TYPE_UNMATCH:
			printf("Parameter is incompatible with parameter type.\n");
			break;
		case TOO_FEW_ARGUMENTS:
			printf("Too few arguments to function \'%s\'.\n",getIDNodeName(node));
			break;
    	case TOO_MANY_ARGUMENTS:
			printf("Too many arguments to function \'%s\'.\n",getIDNodeName(node));
			break;
    	case RETURN_TYPE_UNMATCH:
			printf("Incompatible return type.\n");
			break;
   		case INCOMPATIBLE_ARRAY_DIMENSION:
			printf("Incompatible array dimensions.\n");
			break;
		case NOT_ASSIGNABLE:
			printf("ID \'%s\' is not assignable.\n",getIDNodeName(node));
			break;
		case NOT_ARRAY:
			printf("ID \'%s\' is not array.\n",getIDNodeName(node));
			break;
		case IS_TYPE_NOT_VARIABLE:
			printf("ID \'%s\' is a type, not a variable's name.\n",getIDNodeName(node));
			break;
    	case IS_FUNCTION_NOT_VARIABLE:
			printf("ID \'%s\' is a function, not a variable's name.\n",getIDNodeName(node));
			break;
		case STRING_OPERATION:
			printf("String operation is unsupported.\n");
			break;
		case ARRAY_SIZE_NOT_INT:	//when declaration
			printf("Size of array \'%s\' has non-integer type.\n",getIDNodeName(node));
			break;
		case ARRAY_SIZE_NEGATIVE:	//when decalration
			printf("Size of array \'%s\' is negative.\n",getIDNodeName(node));
			break;
		case ARRAY_SUBSCRIPT_NOT_INT:
			printf("Array subscript is not an integer.\n");
			break;
		case ARRAY_SUBSCRIPT_NEGATIVE:
			printf("ARRAY_SUBSCRIPT_NEGATIVE\n");
			break;
    	default:
			printf("Unhandled case in void printErrorMsg(AST_NODE* node, ERROR_MSG_KIND* errorMsgKind)\n");
        break;
    }
    
}

DATA_TYPE getBiggerType(DATA_TYPE dataType1, DATA_TYPE dataType2)
{
    if(dataType1 == FLOAT_TYPE || dataType2 == FLOAT_TYPE) {
        return FLOAT_TYPE;
    } else {
        return INT_TYPE;
    }
}


void semanticAnalysis(AST_NODE *root)
{
    processProgramNode(root);
}


void processProgramNode(AST_NODE *programNode)
{
	/*	Visit Declaration list */
	visitChild(programNode);
}

void visitChild(AST_NODE* node)
{
	AST_NODE *m_node;
	/*	Visit ALL children of given node*/
	for( m_node = node -> child ; m_node != NULL ; m_node = m_node ->rightSibling)
	{
		visit(m_node);
	}


}

void visit(AST_NODE *node)
{
	/*	AST_TYPE */
	switch(node->nodeType)
	{
		case PROGRAM_NODE:
			SPAM(("PROGRAM_NODE\n"));
			processProgramNode(node);
			break;
		case DECLARATION_NODE:
			SPAM(("DECLARATION_NODE\n"));
			processDeclarationNode(node);
			break;
		case IDENTIFIER_NODE:
			SPAM(("IDENTIFIER_NODE\n"));
			break;
		case PARAM_LIST_NODE:
			SPAM(("Param_list_node\n"));
			break;
		case NUL_NODE:
			SPAM(("NUL_NODE\n"));
			break;
		case BLOCK_NODE:
			SPAM(("BLOCK_NODE\n"));
			processBlockNode(node);
			break;
		case VARIABLE_DECL_LIST_NODE:
			SPAM(("VARIABLE_DECL_LIST_NODE\n"));
			processVarDeclList(node);
			break;
		case STMT_LIST_NODE:
			SPAM(("STMT_LIST_NODE\n"));
			visitChild(node);
			break;
		case STMT_NODE:
			SPAM(("STMT_NODE\n"));
			processStmtNode(node);
			break;
		case EXPR_NODE:
			SPAM(("EXPR_NODE\n"));
			break;
		case CONST_VALUE_NODE: //ex:1, 2, "constant string"
			SPAM(("CONST_VALUE_NODE\n"));
			break;
		case NONEMPTY_ASSIGN_EXPR_LIST_NODE:
			SPAM(("NONEMPTY_ASSIGN_EXPR_LIST_NODE\n"));
			break;
		case NONEMPTY_RELOP_EXPR_LIST_NODE:
			SPAM(("NONEMPTY_RELOP_EXPR_LIST_NODE\n"));
			break;
		default:
			SPAM(("In visit Default\n"));
			break;

	}
}


void processDeclarationNode(AST_NODE* declarationNode)
{
	processTypeNode(declarationNode->child);
	switch(declarationNode->semantic_value.declSemanticValue.kind )
	{
		case VARIABLE_DECL:
			declareIdList(declarationNode->child,VARIABLE_ATTRIBUTE,0);
			break;
		case TYPE_DECL:
			declareIdList(declarationNode->child,TYPE_ATTRIBUTE,0);
			break;
		case FUNCTION_DECL:
			declareFunction(declarationNode->child);
			break;
		case FUNCTION_PARAMETER_DECL:
			declareIdList(declarationNode->child,VARIABLE_ATTRIBUTE,1);
			break;
		default:
			SPAM(("In processDeclarationNode Default\n"));
			break;
	
	}
}

void processTypeNode(AST_NODE* idNodeAsType)
{
	char* name = getIDNodeName(idNodeAsType);
	SPAM(("typename = %s\n",name));	
	SymbolTableEntry *symbolinfo = retrieveSymbol(name);
	if( (symbolinfo == NULL) || (symbolinfo->attribute->attributeKind != TYPE_ATTRIBUTE))
	{
		printErrorMsg(idNodeAsType,SYMBOL_IS_NOT_TYPE);
		idNodeAsType -> dataType = ERROR_TYPE;
	}
	else
	{
		setIdSymtabEntry(idNodeAsType, symbolinfo);
		switch(getTypekind(symbolinfo))
		{
			case SCALAR_TYPE_DESCRIPTOR:
				idNodeAsType -> dataType = symbolinfo ->attribute->attr.typeDescriptor->properties.dataType;
				break;
			case ARRAY_TYPE_DESCRIPTOR:
				idNodeAsType -> dataType = symbolinfo ->attribute->attr.typeDescriptor->properties.arrayProperties.elementType;
				break;
			default:
				SPAM(("In processTypeNode default"));
				break;
		
		}
	}
}

void declareFunction(AST_NODE* declarationNode)
{
	SPAM(("In declareFunction\n"));
	char* typename = getIDNodeName(declarationNode);
	SymbolTableEntry *symbolinfo = retrieveSymbol(typename);
	DATA_TYPE data_type;
	if( (symbolinfo == NULL) || (symbolinfo->attribute->attributeKind != TYPE_ATTRIBUTE))
	{
		printErrorMsg(declarationNode,SYMBOL_IS_NOT_TYPE);
		declarationNode -> dataType = ERROR_TYPE;
	}
	else{
		data_type = symbolinfo->attribute->attr.typeDescriptor->properties.dataType;
	}

	AST_NODE* function_name_node = declarationNode->rightSibling;
	AST_NODE* parameter_list_node = function_name_node->rightSibling;
	AST_NODE* block_node = parameter_list_node->rightSibling;
	char *name = getIDNodeName(function_name_node);
	SPAM(("	function ID = %s, type = %d\n", name, data_type));

	if(declaredLocally(name) == 1){
		printErrorMsg(function_name_node, SYMBOL_REDECLARE);
	}
	SymbolAttribute *symattr = malloc(sizeof(SymbolAttribute));
	FunctionSignature *funcsig = malloc(sizeof(FunctionSignature));
	symattr->attributeKind = FUNCTION_SIGNATURE;
	symattr->attr.functionSignature = funcsig;
	funcsig->parametersCount = 0;
	funcsig->returnType = data_type;
	parameter_list_node->semantic_value.identifierSemanticValue.symbolTableEntry = enterSymbol(name, symattr);
	
	openScope();
	
	AST_NODE* param_node = parameter_list_node->child;
	AST_NODE* param_name_node;
	Parameter* parameterList;
	Parameter* new_parameter;
	SymbolTableEntry* idEntry;
	int first = 0;
	while(param_node != NULL){
		new_parameter = (Parameter*)malloc(sizeof(Parameter));
		if(first == 0){
			funcsig->parameterList = new_parameter;
			first += 1;
		}
		else
			parameterList->next = new_parameter;
		parameterList = new_parameter;
		funcsig->parametersCount += 1;

		processDeclarationNode(param_node);
		param_name_node = param_node->child->rightSibling;
		idEntry = param_name_node->semantic_value.identifierSemanticValue.symbolTableEntry; 
		parameterList->type = idEntry->attribute->attr.typeDescriptor;
		parameterList->parameterName = idEntry->name;
			
		param_node = param_node->rightSibling;	
	}
	processBlockNode(block_node);

	closeScope();
}

DATA_TYPE getDataType(AST_NODE* node)
{
	char *name = getIDNodeName(node);	
	if(strcmp(name,"int") == 0)
		return INT_TYPE;
	else if(strcmp(name,"float") == 0)
		return FLOAT_TYPE;
	else if(strcmp(name,"void") == 0)
		return VOID_TYPE;
	else
		return -1;
}

void declareIdList(AST_NODE* declarationNode, SymbolAttributeKind isVariableOrTypeAttribute, int ignoreArrayFirstDimSize)
{
	
	AST_NODE *node;	
	TypeDescriptor *td = declarationNode->semantic_value.identifierSemanticValue.symbolTableEntry->attribute->attr.typeDescriptor;

	/*	type -> int or float */
	DATA_TYPE type = getDataType(declarationNode);
	SPAM(("declarationNode's type = %d\n",type));	
	
	if((isVariableOrTypeAttribute == VARIABLE_ATTRIBUTE) &&
       (td->kind == SCALAR_TYPE_DESCRIPTOR) && (td->properties.dataType == VOID_TYPE))
    {
        printErrorMsg(declarationNode, VOID_VARIABLE);
        declarationNode->dataType = ERROR_TYPE;
		return;
	}
	
	for(node = declarationNode->rightSibling; node != NULL ; node = node->rightSibling)
	{
		char *name = getIDNodeName(node);	
		SPAM(("name = %s dataType = %d\n",name,node->dataType));
		if(declaredLocally(name))
		{
			printErrorMsg(node, SYMBOL_REDECLARE);
			node->dataType = ERROR_TYPE;
			declarationNode->parent->dataType = ERROR_TYPE;
		}
		else if( declaredLocally(name) == 0  && retrieveSymbol(name) != NULL && retrieveSymbol(name)->attribute->attributeKind == FUNCTION_SIGNATURE )
		{
			printErrorMsg(node, IS_FUNCTION_NOT_VARIABLE );
			node->dataType = ERROR_TYPE;
			declarationNode->parent->dataType = ERROR_TYPE;					
		}
		else
		{
			SymbolAttribute *symattr = (SymbolAttribute* )malloc(sizeof(SymbolAttribute));
			symattr->attributeKind = isVariableOrTypeAttribute;
			SPAM(("Kind = %d\n",getIDKind(node)));
			/* array ID node */
			if(getIDKind(node) == ARRAY_ID)
			{
				if((isVariableOrTypeAttribute == TYPE_ATTRIBUTE) &&
                   (td->kind == SCALAR_TYPE_DESCRIPTOR) && (td->properties.dataType == VOID_TYPE))
                {
                    printErrorMsg(node, TYPEDEF_VOID_ARRAY);
                    node->dataType = ERROR_TYPE;
                    declarationNode->parent->dataType = ERROR_TYPE;
                    break;
                }
                symattr->attr.typeDescriptor = (TypeDescriptor*)malloc(sizeof(TypeDescriptor));
                symattr->attr.typeDescriptor->kind = ARRAY_TYPE_DESCRIPTOR;
				processDeclDimList(node->child, symattr->attr.typeDescriptor, ignoreArrayFirstDimSize);
                if(node->dataType == ERROR_TYPE)
                {
                    free(symattr->attr.typeDescriptor);
                    declarationNode->parent->dataType = ERROR_TYPE;
                }
                else if(declarationNode->semantic_value.identifierSemanticValue.symbolTableEntry->attribute->attr.typeDescriptor->kind == SCALAR_TYPE_DESCRIPTOR)
                {
                    symattr->attr.typeDescriptor->properties.arrayProperties.elementType =
                        declarationNode->semantic_value.identifierSemanticValue.symbolTableEntry->attribute->attr.typeDescriptor->properties.dataType;
                }
                else if(declarationNode->semantic_value.identifierSemanticValue.symbolTableEntry->attribute->attr.typeDescriptor->kind == ARRAY_TYPE_DESCRIPTOR)
                {
                    int typeArrayDim = declarationNode->semantic_value.identifierSemanticValue.symbolTableEntry->attribute->attr.typeDescriptor->properties.arrayProperties.dimension;
                    int idArrayDim = symattr->attr.typeDescriptor->properties.arrayProperties.dimension;
                 	if((typeArrayDim + idArrayDim) > MAX_ARRAY_DIM_SIZE)
					{
						printErrorMsg(node,EXCESSIVE_ARRAY_DIM_DECLARATION);
						free(symattr->attr.typeDescriptor);
                    	declarationNode->parent->dataType = ERROR_TYPE;
						node->dataType =ERROR_TYPE;
					}	
					else
					{
						symattr->attr.typeDescriptor->properties.arrayProperties.elementType = 	declarationNode->semantic_value.identifierSemanticValue.symbolTableEntry->attribute->attr.typeDescriptor->properties.arrayProperties.elementType;
						symattr->attr.typeDescriptor->properties.arrayProperties.dimension = typeArrayDim + idArrayDim;
						int indexType = 0;
						int indexId = 0;
						for(indexType = 0, indexId = idArrayDim; indexId < idArrayDim + typeArrayDim; ++indexType, ++indexId)
						{
							symattr->attr.typeDescriptor->properties.arrayProperties.sizeInEachDimension[indexId] =	declarationNode->semantic_value.identifierSemanticValue.symbolTableEntry->attribute->attr.typeDescriptor->properties.arrayProperties.sizeInEachDimension[indexType];
						}
                	}
				}				
			}
			else if(getIDKind(node) == NORMAL_ID )
			{
				symattr->attr.typeDescriptor = declarationNode->semantic_value.identifierSemanticValue.symbolTableEntry->attribute->attr.typeDescriptor;
			}
			else if(getIDKind(node) == WITH_INIT_ID)
			{
				if(declarationNode->semantic_value.identifierSemanticValue.symbolTableEntry->attribute->attr.typeDescriptor->kind == ARRAY_TYPE_DESCRIPTOR)
                {
                    printErrorMsg(node, TRY_TO_INIT_ARRAY);
                    node->dataType = ERROR_TYPE;
                    declarationNode->parent->dataType = ERROR_TYPE;
                }
                else
                {
                    symattr->attr.typeDescriptor = declarationNode->semantic_value.identifierSemanticValue.symbolTableEntry->attribute->attr.typeDescriptor;
					processExprRelatedNode(node->child);
                }				
			}
			SPAM(("name = %s datatype = %d\n",getIDNodeName(node),node->dataType));
			if(node->dataType == ERROR_TYPE)
            {
                free(symattr);
                declarationNode->dataType = ERROR_TYPE;
            }
            else
            {
                setIdSymtabEntry(node,enterSymbol(name, symattr));
            }
		}
	}
	
}

void checkAssignOrExpr(AST_NODE* assignOrExprRelatedNode)
{
}

void checkWhileStmt(AST_NODE* whileNode)
{
	AST_NODE* condition_node = whileNode->child;
	AST_NODE* block_node = condition_node->rightSibling;
	char* idname;
	SymbolTableEntry* ID;
	switch(condition_node->nodeType){
		case CONST_VALUE_NODE:
			SPAM(("Warning: There might be infinity loop.\n"));
			break;
		case IDENTIFIER_NODE:
			idname = getIDNodeName(condition_node);
			ID = retrieveSymbol(idname);
			if(ID == NULL){
				printErrorMsg(condition_node, SYMBOL_UNDECLARED);
				break;
			}
			if(ID->attribute->attributeKind == TYPE_ATTRIBUTE){
				if(condition_node->semantic_value.identifierSemanticValue.kind == ARRAY_ID &&
						ID->attribute->attr.typeDescriptor->kind == SCALAR_TYPE_DESCRIPTOR){
					printErrorMsg(condition_node, NOT_ARRAY);
					break;
				}
			}
			else if(ID->attribute->attributeKind == FUNCTION_SIGNATURE)
				printErrorMsg(condition_node, IS_FUNCTION_NOT_VARIABLE);
			else if(ID->attribute->attributeKind == TYPE_ATTRIBUTE)
				printErrorMsg(condition_node, IS_TYPE_NOT_VARIABLE);
			break;
		case EXPR_NODE:
			processExprNode(condition_node);
			break;
		case STMT_NODE:
			if(condition_node->semantic_value.stmtSemanticValue.kind != FUNCTION_CALL_STMT){
				SPAM(("Error: This should not exist in if condition\n"));
				break;
			}
			idname = getIDNodeName(condition_node->child);
			ID = retrieveSymbol(idname);
			if(ID == NULL){
				printErrorMsg(condition_node, SYMBOL_UNDECLARED);
				break;
			}
			if(ID->attribute->attributeKind == VARIABLE_ATTRIBUTE)
				printErrorMsg(condition_node, NOT_FUNCTION_NAME);
			else if(ID->attribute->attributeKind == TYPE_ATTRIBUTE)
				printErrorMsg(condition_node, NOT_FUNCTION_NAME);
			break;
		default:
			SPAM(("Error: This should not exist in if condition.\n"));
			break;
	}
	openScope();
	visitChild(block_node);
	closeScope();
}


void checkForStmt(AST_NODE* forNode)
{
	AST_NODE* init_assign_expr_list = forNode->child;
	AST_NODE* relop_expr_list = init_assign_expr_list->rightSibling;
	AST_NODE* assign_expr_list = relop_expr_list->rightSibling;
	AST_NODE* block_node = assign_expr_list->rightSibling;
	if(init_assign_expr_list->nodeType != NUL_NODE)
		visitChild(init_assign_expr_list);
	if(relop_expr_list->nodeType != NUL_NODE)
		visitChild(init_assign_expr_list);
	if(assign_expr_list->nodeType != NUL_NODE)
		visitChild(init_assign_expr_list);
	openScope();
	visitChild(block_node);
	closeScope();
}


void checkAssignmentStmt(AST_NODE* assignmentNode)
{
	AST_NODE* left_node = assignmentNode->child;
	DATA_TYPE left_data_type;
	AST_NODE* right_node = left_node->rightSibling;
	DATA_TYPE right_data_type;
	char* idname;
	SymbolTableEntry* retID;
	char* leftname = getIDNodeName(left_node);
	SymbolTableEntry* leftID = retrieveSymbol(leftname);
	AST_NODE* dimension_node;
	int dimension, dim_count, scalar;
	int left, right;//0 for scalar, 
	if(leftID == NULL){
		printErrorMsg(left_node, SYMBOL_UNDECLARED);
	}
	else if(leftID->attribute->attributeKind == VARIABLE_ATTRIBUTE){
		if(left_node->semantic_value.identifierSemanticValue.kind == ARRAY_ID &&
			leftID->attribute->attr.typeDescriptor->kind == SCALAR_TYPE_DESCRIPTOR){
			printErrorMsg(left_node, NOT_ARRAY);
		}
		else if(leftID->attribute->attr.typeDescriptor->kind == ARRAY_TYPE_DESCRIPTOR){
//			checkVariableDimensions(left_node);
			left_data_type = leftID->attribute->attr.typeDescriptor->properties.arrayProperties.elementType;
			//NOT A SCALAR: NOT_ASSIGNABLE
			scalar = checkArrayIsScalar(left_node);
			if(scalar == 0)
				printErrorMsg(left_node, NOT_ASSIGNABLE);
		}
		else
			left_data_type = leftID->attribute->attr.typeDescriptor->properties.dataType;
	}
	else if(leftID->attribute->attributeKind == FUNCTION_SIGNATURE)
		printErrorMsg(left_node, IS_FUNCTION_NOT_VARIABLE);
	else
		printErrorMsg(left_node, IS_TYPE_NOT_VARIABLE);
	
	switch(right_node->nodeType){
		case IDENTIFIER_NODE:
			idname = getIDNodeName(right_node);
			retID = retrieveSymbol(idname);
			if(retID == NULL)
				printErrorMsg(right_node, SYMBOL_UNDECLARED);
			if(retID->attribute->attributeKind == VARIABLE_ATTRIBUTE){
				if(right_node->semantic_value.identifierSemanticValue.kind == ARRAY_ID &&
						retID->attribute->attr.typeDescriptor->kind == SCALAR_TYPE_DESCRIPTOR){
					printErrorMsg(right_node, NOT_ARRAY);
					break;
				}
				if(retID->attribute->attr.typeDescriptor->kind == ARRAY_TYPE_DESCRIPTOR){
					right_data_type = retID->attribute->attr.typeDescriptor->properties.arrayProperties.elementType;
					dimension_node = right_node->child;
					dimension = retID->attribute->attr.typeDescriptor->properties.arrayProperties.dimension;
					for(dim_count = 0; dim_count < dimension; dim_count++){
						if(dimension_node == NULL)
							break;
						dimension_node = dimension_node->rightSibling;
					}
					if(dim_count != dimension)
						printErrorMsgSpecial(right_node, getIDNodeName(left_node), PASS_ARRAY_TO_SCALAR);
				}
				else
					right_data_type = retID->attribute->attr.typeDescriptor->properties.dataType;
			}
			else if(retID->attribute->attributeKind == FUNCTION_SIGNATURE)
				printErrorMsg(right_node, IS_FUNCTION_NOT_VARIABLE);
			else
				printErrorMsg(right_node, IS_TYPE_NOT_VARIABLE);
			break;
		case EXPR_NODE:
			processExprNode(right_node);
			right_data_type = right_node->dataType;
			break;
		case CONST_VALUE_NODE:
			right_data_type = right_node->semantic_value.const1->const_type;
			break;
		case STMT_NODE:
			if(right_node->semantic_value.stmtSemanticValue.kind != FUNCTION_CALL_STMT){
				SPAM(("Error: This should not exist in assign\n"));
				break;
			}
			idname = getIDNodeName(right_node->child);
			retID = retrieveSymbol(idname);
			if(retID == NULL){
				printErrorMsg(right_node, SYMBOL_UNDECLARED);
				break;
			}
			if(retID->attribute->attributeKind == VARIABLE_ATTRIBUTE)
				printErrorMsg(right_node, NOT_FUNCTION_NAME);
			else if(retID->attribute->attributeKind == FUNCTION_SIGNATURE){
				checkFunctionCall(right_node);
				right_data_type = retID->attribute->attr.functionSignature->returnType;
			}
			else
				printErrorMsg(right_node, NOT_FUNCTION_NAME);
			break;
		default:
			SPAM(("Error: Things go wrong.!!\n"));

	}
	if(left_data_type != right_data_type){
		//need to be done: type conversion
		right_node->dataType = left_data_type;
	}
}


void checkIfStmt(AST_NODE* ifNode)
{
	AST_NODE* condition_node = ifNode->child;
	AST_NODE* block_node = condition_node->rightSibling;
	SymbolTableEntry* retID;
	char* idname;
	switch(condition_node->nodeType){
		case CONST_VALUE_NODE:
			break;
		case IDENTIFIER_NODE:
			idname = getIDNodeName(condition_node);
			retID = retrieveSymbol(idname);
			if(retID == NULL)
				printErrorMsg(condition_node, SYMBOL_UNDECLARED);
			if(retID->attribute->attributeKind == VARIABLE_ATTRIBUTE){
				if(condition_node->semantic_value.identifierSemanticValue.kind == ARRAY_ID &&
						retID->attribute->attr.typeDescriptor->kind == SCALAR_TYPE_DESCRIPTOR){
					printErrorMsg(condition_node, NOT_ARRAY);
					break;
				}
			}
			else if(retID->attribute->attributeKind == FUNCTION_SIGNATURE)
				printErrorMsg(condition_node, IS_FUNCTION_NOT_VARIABLE);
			else if(retID->attribute->attributeKind == TYPE_ATTRIBUTE)
				printErrorMsg(condition_node, IS_TYPE_NOT_VARIABLE);
			break;
		case EXPR_NODE:
			processExprNode(condition_node);
			break;
		case STMT_NODE:
			if(condition_node->semantic_value.stmtSemanticValue.kind != FUNCTION_CALL_STMT){
				SPAM(("Error: This should not exist in for condition\n"));
				break;
			}
			idname = getIDNodeName(condition_node->child);
			retID = retrieveSymbol(idname);
			if(retID == NULL){
				printErrorMsg(condition_node, SYMBOL_UNDECLARED);
				break;
			}
			if(retID->attribute->attributeKind == VARIABLE_ATTRIBUTE)
				printErrorMsg(condition_node, NOT_FUNCTION_NAME);
			else if(retID->attribute->attributeKind == FUNCTION_SIGNATURE)
				checkFunctionCall(condition_node);
			else
				printErrorMsg(condition_node, NOT_FUNCTION_NAME);
			break;
		default:
			SPAM(("Error: This should not exist in if condition\n"));
			break;
	}
	openScope();
	visitChild(block_node);
	closeScope();
}

void checkWriteFunction(AST_NODE* functionCallNode)
{
	AST_NODE* function_name_node = functionCallNode->child;
	AST_NODE* parameter_node = function_name_node->rightSibling;
	if(parameter_node->nodeType == NUL_NODE){
		printErrorMsg(function_name_node, TOO_FEW_ARGUMENTS);
		return;
	}
	if(parameter_node->rightSibling != NULL){
		printErrorMsg(function_name_node, TOO_MANY_ARGUMENTS);	
		return;
	}
	AST_NODE* param_node = parameter_node->child;
	switch(param_node->nodeType){
		case CONST_VALUE_NODE:
			break;
		case EXPR_NODE:
			break;
		case IDENTIFIER_NODE:
			break;
		default:
			printErrorMsg(function_name_node, PARAMETER_TYPE_UNMATCH);	
	}
}

void checkFunctionCall(AST_NODE* functionCallNode)
{
	AST_NODE* function_name_node = functionCallNode->child;
	char* function_name = getIDNodeName(function_name_node);
	SymbolTableEntry* function_symbol = retrieveSymbol(function_name);
	if(strcmp(function_name, "write") == 0){
		checkWriteFunction(functionCallNode);
		return;
	}
	if(function_symbol == NULL){
		printErrorMsg(function_name_node, SYMBOL_UNDECLARED);
		return;
	}
	int parameter_count = function_symbol->attribute->attr.functionSignature->parametersCount;
	Parameter* parameter = function_symbol->attribute->attr.functionSignature->parameterList;
	AST_NODE* parameter_list_node = function_name_node->rightSibling;
	AST_NODE* parameter_node;
	SPAM(("function_call check: parameter_count = %d\n", parameter_count));
	if(parameter_count == 0){
		if(parameter_list_node->nodeType != NUL_NODE){
			printErrorMsg(function_name_node, TOO_MANY_ARGUMENTS);		
		}
		return;
	}
	parameter_node = parameter_list_node->child;
	for(int i = 0; i < parameter_count; i++){
		if(parameter_node == NULL){
			printErrorMsg(function_name_node, TOO_FEW_ARGUMENTS);
			break;
		}
		checkParameterPassing(parameter, parameter_node);

		parameter = parameter->next;
		parameter_node = parameter_node->rightSibling;	
	}
	if(parameter_node != NULL)
		printErrorMsg(function_name_node, TOO_MANY_ARGUMENTS);		

}

void checkParameterPassing(Parameter* formalParameter, AST_NODE* actualParameter)
{
	DATA_TYPE actual_type, formal_type;
	char* idname;
	SymbolTableEntry* ID;
	char* parameter_name;
	SymbolTableEntry* parameter_symbol;
	int check_identifier = 0;//array: 1 //scalar:2 // 0:scalar
	switch(actualParameter->nodeType){
		case IDENTIFIER_NODE:
			parameter_name = getIDNodeName(actualParameter);
			parameter_symbol = retrieveSymbol(parameter_name);
			if(parameter_symbol == NULL)
				printErrorMsg(actualParameter, SYMBOL_UNDECLARED);
			if(parameter_symbol->attribute->attributeKind == VARIABLE_ATTRIBUTE){
				if(actualParameter->semantic_value.identifierSemanticValue.kind == ARRAY_ID &&
						parameter_symbol->attribute->attr.typeDescriptor->kind == SCALAR_TYPE_DESCRIPTOR){
					printErrorMsg(actualParameter, NOT_ARRAY);
					break;
				}
				if(parameter_symbol->attribute->attr.typeDescriptor->kind == ARRAY_TYPE_DESCRIPTOR){
					actual_type = parameter_symbol->attribute->attr.typeDescriptor->properties.arrayProperties.elementType;
					checkVariableDimensions(actualParameter);
					check_identifier = 1;
				}
				else{
					actual_type = parameter_symbol->attribute->attr.typeDescriptor->properties.dataType;
					check_identifier = 2;
				}
			}
			else if(parameter_symbol->attribute->attributeKind == FUNCTION_SIGNATURE)
				printErrorMsg(actualParameter, IS_FUNCTION_NOT_VARIABLE);
			else
				printErrorMsg(actualParameter, IS_TYPE_NOT_VARIABLE);
			break;
		case STMT_NODE:
			if(actualParameter->semantic_value.stmtSemanticValue.kind != FUNCTION_CALL_STMT){
				SPAM(("Error: This should not exist in if condition\n"));
				break;
			}
			idname = getIDNodeName(actualParameter->child);
			ID = retrieveSymbol(idname);
			if(ID == NULL){
				printErrorMsg(actualParameter, SYMBOL_UNDECLARED);
				break;
			}
			if(ID->attribute->attributeKind == VARIABLE_ATTRIBUTE)
				printErrorMsg(actualParameter, NOT_FUNCTION_NAME);
			else if(ID->attribute->attributeKind == FUNCTION_SIGNATURE){
				checkFunctionCall(actualParameter);
				actual_type = ID->attribute->attr.functionSignature->returnType;
			}
			else
				printErrorMsg(actualParameter, NOT_FUNCTION_NAME);
			break;
		case CONST_VALUE_NODE:
			actual_type = actualParameter->semantic_value.const1->const_type;
			break;
		case EXPR_NODE:
			processExprNode(actualParameter);
			actual_type = actualParameter->dataType;
			break;
		default:
			SPAM(("ERROR: There should not be this kind of node in parameter passing list.\n"));
	}
	int dimension_check = 0;//1: check array to array, 2: check array to scalar
	switch(formalParameter->type->kind){
		case SCALAR_TYPE_DESCRIPTOR:
			formal_type = formalParameter->type->properties.dataType;
			dimension_check = 0;
			if(check_identifier == 1)
				dimension_check = 2;
			break;
		case ARRAY_TYPE_DESCRIPTOR:
			formal_type = formalParameter->type->properties.arrayProperties.elementType;
			dimension_check = 1;
			if(check_identifier != 1){
				printErrorMsgSpecial(actualParameter, formalParameter->parameterName, PASS_SCALAR_TO_ARRAY);
				dimension_check = 0;
			}
			break;
	}
	//type check
	if(formal_type != actual_type)
		printErrorMsg(actualParameter, PARAMETER_TYPE_UNMATCH);
	if(check_identifier == 0)
		return;

	ArrayProperties array_properties = parameter_symbol->attribute->attr.typeDescriptor->properties.arrayProperties;
	ArrayProperties formal_array_properties = formalParameter->type->properties.arrayProperties;
	AST_NODE* dimension_node = actualParameter->child;
	int actual_dim = 0, formal_dim = 0;
	if(dimension_check == 2){
		if(checkArrayIsScalar(actualParameter) == 0)
			printErrorMsgSpecial(actualParameter, formalParameter->parameterName, PASS_ARRAY_TO_SCALAR);
	}
	else if(dimension_check == 1){
		while(dimension_node != NULL){
			dimension_node = dimension_node->rightSibling;
			actual_dim += 1;
		}
		if(array_properties.dimension-actual_dim != formal_array_properties.dimension){
			if(array_properties.dimension-actual_dim == 0)
				printErrorMsgSpecial(actualParameter, formalParameter->parameterName, PASS_SCALAR_TO_ARRAY);	
			else
				printErrorMsg(actualParameter, INCOMPATIBLE_ARRAY_DIMENSION);
			return;
		}
		SPAM(("In dimension check.\n"));
		for(; formal_dim < formal_array_properties.dimension, actual_dim < array_properties.dimension; formal_dim++, actual_dim++){
			SPAM(("%d %d\n", formal_array_properties.sizeInEachDimension[formal_dim], array_properties.sizeInEachDimension[actual_dim]));
			if(formal_array_properties.sizeInEachDimension[formal_dim] == 0)
				continue;
			if(formal_array_properties.sizeInEachDimension[formal_dim] > 1000000)
				continue;
			if(array_properties.sizeInEachDimension[actual_dim] != formal_array_properties.sizeInEachDimension[formal_dim]){
				printErrorMsg(actualParameter, INCOMPATIBLE_ARRAY_DIMENSION);
				break;
			}	
		}
		if(formal_dim != formal_array_properties.dimension)
			printErrorMsgSpecial(actualParameter, formalParameter->parameterName, PASS_SCALAR_TO_ARRAY);
		else if(actual_dim != array_properties.dimension)
			printErrorMsg(actualParameter, INCOMPATIBLE_ARRAY_DIMENSION);
	}
}

void processExprRelatedNode(AST_NODE* exprRelatedNode)
{
	switch(exprRelatedNode -> nodeType)
	{
		case EXPR_NODE:
			processExprNode(exprRelatedNode);
			break;
		case CONST_VALUE_NODE:
			processConstValueNode(exprRelatedNode);
			break;
		case IDENTIFIER_NODE:
			processVariableRValue(exprRelatedNode);
			break;
		default:
			fprintf(stderr,"processExprRelatedNode default\n");
			break;
	}

}
/*	store ivalue/fvalue from the value of node */
void getExprOrConstValue(AST_NODE* exprOrConstNode, int* iValue, float* fValue)
{
    if(exprOrConstNode->nodeType == CONST_VALUE_NODE)
    {
        if(exprOrConstNode->dataType == INT_TYPE)
        {
            if(fValue) 
				*fValue = exprOrConstNode->semantic_value.const1->const_u.intval;
            else       
				*iValue = exprOrConstNode->semantic_value.const1->const_u.intval;
            
        }
        else           
			*fValue = exprOrConstNode->semantic_value.const1->const_u.fval;
    }
    else
    {
        if(exprOrConstNode->dataType == INT_TYPE)
        {
            if(fValue)	
				*fValue = exprOrConstNode->semantic_value.exprSemanticValue.constEvalValue.iValue;
            else        
				*iValue = exprOrConstNode->semantic_value.exprSemanticValue.constEvalValue.iValue;
        }
        else            
			*fValue = exprOrConstNode->semantic_value.exprSemanticValue.constEvalValue.fValue;
    }
}


void evaluateExprValue(AST_NODE* exprNode)
{
	if(exprNode->semantic_value.exprSemanticValue.kind == BINARY_OPERATION)
    {
        AST_NODE* left = exprNode->child;
        AST_NODE* right = left->rightSibling;
		/*	left =  right = int */
        if( (left->dataType == INT_TYPE) && (right->dataType == INT_TYPE))
        {
            int leftValue = 0;
            int rightValue = 0;
            
			getExprOrConstValue(left, &leftValue, NULL);
            getExprOrConstValue(right, &rightValue, NULL);
            exprNode->dataType = INT_TYPE;
            switch(exprNode->semantic_value.exprSemanticValue.op.binaryOp)
            {
            case BINARY_OP_ADD:
                exprNode->semantic_value.exprSemanticValue.constEvalValue.iValue = leftValue + rightValue;
                break;
            case BINARY_OP_SUB:
                exprNode->semantic_value.exprSemanticValue.constEvalValue.iValue = leftValue - rightValue;
                break;
            case BINARY_OP_MUL:
                exprNode->semantic_value.exprSemanticValue.constEvalValue.iValue = leftValue * rightValue;
                break;
            case BINARY_OP_DIV:
                exprNode->semantic_value.exprSemanticValue.constEvalValue.iValue = leftValue / rightValue;
                break;
            case BINARY_OP_EQ:
                exprNode->semantic_value.exprSemanticValue.constEvalValue.iValue = leftValue == rightValue;
                break;
            case BINARY_OP_GE:
                exprNode->semantic_value.exprSemanticValue.constEvalValue.iValue = leftValue >= rightValue;
                break;
            case BINARY_OP_LE:
                exprNode->semantic_value.exprSemanticValue.constEvalValue.iValue = leftValue <= rightValue;
                break;
            case BINARY_OP_NE:
                exprNode->semantic_value.exprSemanticValue.constEvalValue.iValue = leftValue != rightValue;
                break;
            case BINARY_OP_GT:
                exprNode->semantic_value.exprSemanticValue.constEvalValue.iValue = leftValue > rightValue;
                break;
            case BINARY_OP_LT:
                exprNode->semantic_value.exprSemanticValue.constEvalValue.iValue = leftValue < rightValue;
                break;
            case BINARY_OP_AND:
                exprNode->semantic_value.exprSemanticValue.constEvalValue.iValue = leftValue && rightValue;
                break;
            case BINARY_OP_OR:
                exprNode->semantic_value.exprSemanticValue.constEvalValue.iValue = leftValue || rightValue;
                break;
            default:
                SPAM(("Unhandled case in void evaluateExprValue(AST_NODE* exprNode)\n"));
                break;
            } 
            return;
        }
        else
        {
            float leftValue = 0;
            float rightValue = 0;
            getExprOrConstValue(left, NULL, &leftValue);
            getExprOrConstValue(right, NULL, &rightValue);
            exprNode->dataType = FLOAT_TYPE;
            switch(exprNode->semantic_value.exprSemanticValue.op.binaryOp)
            {
            case BINARY_OP_ADD:
                exprNode->semantic_value.exprSemanticValue.constEvalValue.fValue = leftValue + rightValue;
                break;
            case BINARY_OP_SUB:
                exprNode->semantic_value.exprSemanticValue.constEvalValue.fValue = leftValue - rightValue;
                break;
            case BINARY_OP_MUL:
                exprNode->semantic_value.exprSemanticValue.constEvalValue.fValue = leftValue * rightValue;
                break;
            case BINARY_OP_DIV:
                exprNode->semantic_value.exprSemanticValue.constEvalValue.fValue = leftValue / rightValue;
                break;
            case BINARY_OP_EQ:
                exprNode->semantic_value.exprSemanticValue.constEvalValue.fValue = leftValue == rightValue;
                break;
            case BINARY_OP_GE:
                exprNode->semantic_value.exprSemanticValue.constEvalValue.fValue = leftValue >= rightValue;
                break;
            case BINARY_OP_LE:
                exprNode->semantic_value.exprSemanticValue.constEvalValue.fValue = leftValue <= rightValue;
                break;
            case BINARY_OP_NE:
                exprNode->semantic_value.exprSemanticValue.constEvalValue.fValue = leftValue != rightValue;
                break;
            case BINARY_OP_GT:
                exprNode->semantic_value.exprSemanticValue.constEvalValue.fValue = leftValue > rightValue;
                break;
            case BINARY_OP_LT:
                exprNode->semantic_value.exprSemanticValue.constEvalValue.fValue = leftValue < rightValue;
                break;
            case BINARY_OP_AND:
                exprNode->semantic_value.exprSemanticValue.constEvalValue.fValue = leftValue && rightValue;
                break;
            case BINARY_OP_OR:
                exprNode->semantic_value.exprSemanticValue.constEvalValue.fValue = leftValue || rightValue;
                break;
            default:
                printf("Unhandled case in void evaluateExprValue(AST_NODE* exprNode)\n");
                break;
            }
        }
    }
    else if (exprNode->semantic_value.exprSemanticValue.kind == UNARY_OPERATION)
    {
        AST_NODE* op = exprNode->child;
        if(op->dataType == INT_TYPE)
        {
            int opValue = 0;
            getExprOrConstValue(op, &opValue, NULL);
            exprNode->dataType = INT_TYPE;
            switch(exprNode->semantic_value.exprSemanticValue.op.unaryOp)
            {
            case UNARY_OP_POSITIVE:
                exprNode->semantic_value.exprSemanticValue.constEvalValue.iValue = opValue;
                break;
            case UNARY_OP_NEGATIVE:
                exprNode->semantic_value.exprSemanticValue.constEvalValue.iValue = -1 * opValue;
                break;
            case UNARY_OP_LOGICAL_NEGATION:
                exprNode->semantic_value.exprSemanticValue.constEvalValue.iValue = !opValue;
                break;
            default:
                SPAM(("evaluateExprValue\n"));
                break;
            }
        }
        else
        {
            float opValue = 0;
            getExprOrConstValue(op, NULL, &opValue);
            exprNode->dataType = FLOAT_TYPE;
            switch(exprNode->semantic_value.exprSemanticValue.op.unaryOp)
            {
            case UNARY_OP_POSITIVE:
                exprNode->semantic_value.exprSemanticValue.constEvalValue.fValue = opValue;
                break;
            case UNARY_OP_NEGATIVE:
                exprNode->semantic_value.exprSemanticValue.constEvalValue.fValue = -1 * opValue;
                break;
            case UNARY_OP_LOGICAL_NEGATION:
                exprNode->semantic_value.exprSemanticValue.constEvalValue.fValue = !opValue;
                break;
            default:
                SPAM(("evaluateExprValue\n"));
                break;
            }
        }
    }
}

void processExprNode(AST_NODE* exprNode)
{
	/* binary or unary */
	EXPR_KIND kind = getExprKind(exprNode);
	if(kind == BINARY_OPERATION)
	{
		AST_NODE* left = exprNode->child;	//	+
		AST_NODE* right = left->rightSibling;	//	3
		processExprRelatedNode(left);
		processExprRelatedNode(right);
		if(left->dataType == CONST_STRING_TYPE || right->dataType == CONST_STRING_TYPE)
		{
			printErrorMsg(left,INCOMPATIBLE_ARRAY_DIMENSION);
			exprNode->dataType = ERROR_TYPE;
		}
		else
		{
			evaluateExprValue(exprNode);
			exprNode->semantic_value.exprSemanticValue.isConstEval = 1 ;
		}
	}
	else if(kind == UNARY_OPERATION)
	{
		AST_NODE* op = exprNode->child;
		processExprRelatedNode(op);
		exprNode->dataType = op->dataType;
		if(op->dataType == CONST_STRING_TYPE)
		{
			printErrorMsg(exprNode,INCOMPATIBLE_ARRAY_DIMENSION);
			exprNode->dataType = ERROR_TYPE;
		}
		else
		{
			evaluateExprValue(exprNode);
			exprNode->semantic_value.exprSemanticValue.isConstEval = 1 ;
		}
	}
}


void processVariableLValue(AST_NODE* idNode)
{
    SymbolTableEntry *sym = retrieveSymbol(getIDNodeName(idNode));
    if(!sym)
    {
        printErrorMsg(idNode, SYMBOL_UNDECLARED);
        idNode->dataType = ERROR_TYPE;
        return;
    }
    setIdSymtabEntry(idNode,sym);
    if(sym->attribute->attributeKind == TYPE_ATTRIBUTE)
    {
        printErrorMsg(idNode, IS_TYPE_NOT_VARIABLE);
        idNode->dataType = ERROR_TYPE;
        return;
    }
    else if(sym->attribute->attributeKind == FUNCTION_SIGNATURE)
    {
        printErrorMsg(idNode, IS_FUNCTION_NOT_VARIABLE);
        idNode->dataType = ERROR_TYPE;
        return;
    }

    TypeDescriptor *type = idNode->semantic_value.identifierSemanticValue.symbolTableEntry->attribute->attr.typeDescriptor;

    if(idNode->semantic_value.identifierSemanticValue.kind == NORMAL_ID)
    {
        if(type->kind == ARRAY_TYPE_DESCRIPTOR)
        {
            printErrorMsg(idNode, INCOMPATIBLE_ARRAY_DIMENSION);
            idNode->dataType = ERROR_TYPE;
        }
        else
        {
            idNode->dataType = type->properties.dataType;
        }
    }
    else if(idNode->semantic_value.identifierSemanticValue.kind == ARRAY_ID)
    {
        int dim = 0;
        AST_NODE *temp ;
		for(temp = idNode->child ; temp !=NULL ; temp = temp->rightSibling)
		{
			dim++;
            processExprRelatedNode(temp);
            if(temp->dataType == ERROR_TYPE)
            {
                idNode->dataType = ERROR_TYPE;
            }
            else if(temp->dataType == FLOAT_TYPE)
            {
                printErrorMsg(idNode, ARRAY_SUBSCRIPT_NOT_INT);
                idNode->dataType = ERROR_TYPE;
            }
            
        }
        if(type->kind == SCALAR_TYPE_DESCRIPTOR)
        {
            printErrorMsg(idNode, NOT_ARRAY);
            idNode->dataType = ERROR_TYPE;
        }
        else
        {
            if(dim == type->properties.arrayProperties.dimension)
            {
                idNode->dataType = type->properties.arrayProperties.elementType;
            }
            else
            {
                printErrorMsg(idNode, INCOMPATIBLE_ARRAY_DIMENSION);
                idNode->dataType = ERROR_TYPE;
            }
        }
    }
}

/*	idNode denotes the first Right Value of the Stmt*/
void processVariableRValue(AST_NODE* idNode)
{
	SPAM(("In processVariableRValue\n"));
	//	Find if the Variable is declared or not.
	SymbolTableEntry *sym = retrieveSymbol(getIDNodeName(idNode));
	setIdSymtabEntry(idNode,sym);
	if(sym == NULL)
	{
		printErrorMsg(idNode,SYMBOL_UNDECLARED);
		idNode->dataType = ERROR_TYPE;
		return;
	}
	// Variable kind attribite
	if(sym->attribute->attributeKind == TYPE_ATTRIBUTE)
	{
		printErrorMsg(idNode,IS_TYPE_NOT_VARIABLE);
		idNode->dataType = ERROR_TYPE;
		return;
	}
	TypeDescriptor* type = idNode->semantic_value.identifierSemanticValue.symbolTableEntry->attribute->attr.typeDescriptor;
	if(getIDKind(idNode) == NORMAL_ID)
	{
		if(type->kind == ARRAY_TYPE_DESCRIPTOR)
		{
			if(type->properties.arrayProperties.elementType == INT_TYPE)
			{
				idNode->dataType = INT_PTR_TYPE;
			}
			else if(type->properties.arrayProperties.elementType == FLOAT_TYPE)
			{
				idNode->dataType = FLOAT_PTR_TYPE;
			}
		}
		else
		{
			idNode ->dataType = type->properties.dataType;
		}
	}
	else if(getIDKind(idNode) == ARRAY_ID)
	{
		if(type->kind == SCALAR_TYPE_DESCRIPTOR)
        {
            printErrorMsg(idNode, NOT_ARRAY);
            idNode->dataType = ERROR_TYPE;
        }
        else
		{
			int dim = 0;
            AST_NODE *temp ;
            for(temp = idNode->child ; temp !=NULL ; temp = temp->rightSibling)
			{
				dim++;
				processExprRelatedNode(temp);
				if(temp->dataType == ERROR_TYPE)
				{
					idNode->dataType = ERROR_TYPE;
				}
				else if(temp->dataType == FLOAT_TYPE)
				{
					printErrorMsg(idNode, ARRAY_SUBSCRIPT_NOT_INT);
                    idNode->dataType = ERROR_TYPE;
                }				
			}
			if(idNode->dataType != ERROR_TYPE)
            {
                if(dim == type->properties.arrayProperties.dimension)
                {
                    idNode->dataType = type->properties.arrayProperties.elementType;
                }
                else if(dim > type->properties.arrayProperties.dimension)
                {
                    printErrorMsg(idNode, INCOMPATIBLE_ARRAY_DIMENSION);
                    idNode->dataType = ERROR_TYPE;
                }
                else if(type->properties.arrayProperties.elementType == INT_TYPE)
                {
                    idNode->dataType = INT_PTR_TYPE;
                }
                else if(type->properties.arrayProperties.elementType == FLOAT_TYPE)
                {
                    idNode->dataType = FLOAT_PTR_TYPE;
                }
            }
		}
	}

}


void processConstValueNode(AST_NODE* constValueNode)
{
	switch(constValueNode->semantic_value.const1->const_type)
	{
		case INTEGERC:
			constValueNode->dataType = INT_TYPE;	
			constValueNode->semantic_value.exprSemanticValue.constEvalValue.iValue =
			constValueNode->semantic_value.const1->const_u.intval;
			break;
		case FLOATC:
			constValueNode->dataType = FLOAT_TYPE;	
			constValueNode->semantic_value.exprSemanticValue.constEvalValue.fValue =
			constValueNode->semantic_value.const1->const_u.fval;
			break;
		case STRINGC:
			constValueNode->dataType = CONST_STRING_TYPE;
			break;
		default:
			SPAM(("In processConstValueNode default\n"));
			break;
	}
}


void checkReturnStmt(AST_NODE* returnNode)
{
	AST_NODE* ret_node = returnNode->child;
	SymbolTableEntry* function = getfunction();
	SymbolTableEntry* retID;
	DATA_TYPE original_ret_type = function->attribute->attr.functionSignature->returnType;
	SPAM(("function name = %s, type = %d\n", function->name, original_ret_type));
	if(original_ret_type == VOID_TYPE){
		if(ret_node->nodeType != NUL_NODE)
			printErrorMsg(ret_node, RETURN_TYPE_UNMATCH);
		return;
	}
	char* idname;
	int scalar;
	if(function == NULL)
		SPAM(("ERROR: There is no return value: not in a function\n"));
	switch(ret_node->nodeType){
		case CONST_VALUE_NODE:
			SPAM(("condt_ret_node type = %d\n", ret_node->semantic_value.const1->const_type));
			if(ret_node->semantic_value.const1->const_type > original_ret_type)
				printErrorMsg(ret_node, RETURN_TYPE_UNMATCH);
			break;
		case IDENTIFIER_NODE:
			idname = getIDNodeName(ret_node);
			retID = retrieveSymbol(idname);
			if(retID == NULL){
				printErrorMsg(ret_node, SYMBOL_UNDECLARED);
				break;
			}
			SPAM(("IT's type is %d\n", retID->attribute->attr.typeDescriptor->kind));
			fflush(stdout);
			if(retID->attribute->attributeKind == VARIABLE_ATTRIBUTE){
				if(ret_node->semantic_value.identifierSemanticValue.kind == ARRAY_ID &&
						retID->attribute->attr.typeDescriptor->kind == SCALAR_TYPE_DESCRIPTOR){
					printErrorMsg(ret_node, NOT_ARRAY);
					break;
				}
				if(retID->attribute->attr.typeDescriptor->kind == ARRAY_TYPE_DESCRIPTOR){
					scalar = checkArrayIsScalar(ret_node);
					if(scalar == 0)
						printErrorMsg(ret_node, RETURN_ARRAY);
					if(retID->attribute->attr.typeDescriptor->properties.arrayProperties.elementType > original_ret_type)
						printErrorMsg(ret_node, RETURN_TYPE_UNMATCH);
				}
				else if(retID->attribute->attr.typeDescriptor->properties.dataType > original_ret_type)
					printErrorMsg(ret_node, RETURN_TYPE_UNMATCH);
			}
			else if(retID->attribute->attributeKind == FUNCTION_SIGNATURE)
				printErrorMsg(ret_node, IS_FUNCTION_NOT_VARIABLE);
			else
				printErrorMsg(ret_node, IS_TYPE_NOT_VARIABLE);
			break;
		case EXPR_NODE:
			processExprNode(ret_node);
			if(ret_node->dataType > original_ret_type)
				printErrorMsg(ret_node, RETURN_TYPE_UNMATCH);
			break;
		case STMT_NODE:
			if(ret_node->semantic_value.stmtSemanticValue.kind != FUNCTION_CALL_STMT){
				SPAM(("Error: This should not exist in return stmt\n"));
				break;
			}
			idname = getIDNodeName(ret_node->child);
			retID = retrieveSymbol(idname);
			if(retID == NULL){
				printErrorMsg(ret_node, SYMBOL_UNDECLARED);
				break;
			}
			if(retID->attribute->attributeKind == VARIABLE_ATTRIBUTE)
				printErrorMsg(ret_node, NOT_FUNCTION_NAME);
			else if(retID->attribute->attributeKind == FUNCTION_SIGNATURE){
				checkFunctionCall(ret_node);
				if(retID->attribute->attr.functionSignature->returnType > original_ret_type)	
					printErrorMsg(ret_node, RETURN_TYPE_UNMATCH);
			}
			else
				printErrorMsg(ret_node, NOT_FUNCTION_NAME);
			break;
		default:
			SPAM(("Error: This should not exist in if condition\n"));
			break;
	}
}


void processBlockNode(AST_NODE* blockNode)
{
	visitChild(blockNode);
}


void processStmtNode(AST_NODE* stmtNode)
{
	switch(stmtNode->semantic_value.stmtSemanticValue.kind )
	{
			/* not sure third parameter */
	    case WHILE_STMT:
			checkWhileStmt(stmtNode);
			break;
	    case FOR_STMT:
			checkForStmt(stmtNode);
			break;
		case ASSIGN_STMT:
			checkAssignmentStmt(stmtNode);
			break;
		case IF_STMT:
			checkIfStmt(stmtNode);
			break;
		case FUNCTION_CALL_STMT:
			checkFunctionCall(stmtNode);
			break;
	    case RETURN_STMT:
			checkReturnStmt(stmtNode);
			break;
		default:
			fprintf(stderr,"In processDeclarationNode error\n");
			break;
	}
}


void processGeneralNode(AST_NODE *node)
{
}

void processDeclDimList(AST_NODE* idNode, TypeDescriptor* typeDescriptor, int ignoreFirstDimSize)
{
	AST_NODE* node;
	int dim_count = 0 ;
	for(node = idNode; node!=NULL ; node = node->rightSibling)
	{		
		if(dim_count == 0 && ignoreFirstDimSize)
		{	
			if(node->nodeType == NUL_NODE)
				typeDescriptor->properties.arrayProperties.sizeInEachDimension[0] == 0 ;
			dim_count++;	
			continue;
		}
		if(dim_count >= MAX_ARRAY_DIM_SIZE)
		{
			printErrorMsg(idNode,EXCESSIVE_ARRAY_DIM_DECLARATION);
			idNode->parent->dataType = ERROR_TYPE;
			break;
		}			
		processExprRelatedNode(node);
		SPAM(("array index value = %d\n",node->semantic_value.exprSemanticValue.constEvalValue.iValue));
		if(node->dataType == FLOAT_TYPE)
		{
			printErrorMsg(node,ARRAY_SIZE_NOT_INT);
			idNode->parent->dataType = ERROR_TYPE;				
		}
		else if( node->semantic_value.exprSemanticValue.constEvalValue.iValue < 0 && node->semantic_value.exprSemanticValue.isConstEval == 1 )
		{
			printErrorMsg(node,ARRAY_SIZE_NEGATIVE);
			idNode->parent->dataType = ERROR_TYPE;
		}
		else
		{
			if(node->semantic_value.exprSemanticValue.isConstEval == 1 || node->nodeType == CONST_VALUE_NODE)
			{
				typeDescriptor->properties.arrayProperties.sizeInEachDimension[dim_count] =  node->semantic_value.exprSemanticValue.constEvalValue.iValue;
			}
			else
			{
				typeDescriptor->properties.arrayProperties.sizeInEachDimension[dim_count] = 0;	
			}
		}
		dim_count++;	
	}
	typeDescriptor->properties.arrayProperties.dimension = dim_count;
}

void processVarDeclList(AST_NODE *node)
{
	visitChild(node);
}

void checkVariableDimensions(AST_NODE* node){
	char* parameter_name = getIDNodeName(node);
	AST_NODE* dimension_node = node->child;
	SymbolTableEntry* parameter_symbol = retrieveSymbol(parameter_name);
	if(parameter_symbol->attribute->attr.typeDescriptor->kind != ARRAY_TYPE_DESCRIPTOR)
		return;
	ArrayProperties array_properties = parameter_symbol->attribute->attr.typeDescriptor->properties.arrayProperties;
	int dimension = 0, ivalue;
	char* idname;
	SymbolTableEntry* ID;
	while(dimension_node != NULL){
		switch(dimension_node->nodeType){
			case CONST_VALUE_NODE:
				if(dimension_node->semantic_value.const1->const_type != INT_TYPE)
					printErrorMsg(node, ARRAY_SUBSCRIPT_NOT_INT);
				break;
			case IDENTIFIER_NODE:
				idname = getIDNodeName(dimension_node);
				ID = retrieveSymbol(idname);
				if(ID == NULL)
					printErrorMsg(dimension_node, SYMBOL_UNDECLARED);
				if(ID->attribute->attributeKind == VARIABLE_ATTRIBUTE){
					if(dimension_node->semantic_value.identifierSemanticValue.kind == ARRAY_ID &&
							ID->attribute->attr.typeDescriptor->kind == SCALAR_TYPE_DESCRIPTOR){
						printErrorMsg(dimension_node, NOT_ARRAY);
						break;
					}
					if(ID->attribute->attr.typeDescriptor->kind == ARRAY_TYPE_DESCRIPTOR){
						checkVariableDimensions(dimension_node);
						if(checkArrayIsScalar(dimension_node) == 0)
							printErrorMsg(dimension_node, ARRAY_SUBSCRIPT_NOT_INT);
					}
					else if(ID->attribute->attr.typeDescriptor->properties.dataType != INT_TYPE)
						printErrorMsg(dimension_node, ARRAY_SUBSCRIPT_NOT_INT);
				}
				else if(ID->attribute->attributeKind == FUNCTION_SIGNATURE)
					printErrorMsg(dimension_node, IS_FUNCTION_NOT_VARIABLE);
				else
					printErrorMsg(dimension_node, IS_TYPE_NOT_VARIABLE);
				break;
			case EXPR_NODE:
				processExprNode(dimension_node);
				if(dimension_node->dataType != INT_TYPE)
					printErrorMsg(dimension_node, ARRAY_SUBSCRIPT_NOT_INT);
				if(dimension_node->semantic_value.exprSemanticValue.isConstEval == 1){
					ivalue = dimension_node->semantic_value.exprSemanticValue.constEvalValue.iValue;
				}
				break;
			case STMT_NODE:
				if(dimension_node->semantic_value.stmtSemanticValue.kind != FUNCTION_CALL_STMT){
					SPAM(("Error: This should not exist in if condition\n"));
					break;
				}
				idname = getIDNodeName(dimension_node->child);
				ID = retrieveSymbol(idname);
				if(ID == NULL){
					printErrorMsg(dimension_node, SYMBOL_UNDECLARED);
					break;
				}
				if(ID->attribute->attributeKind == VARIABLE_ATTRIBUTE)
					printErrorMsg(dimension_node, NOT_FUNCTION_NAME);
				else if(ID->attribute->attributeKind == FUNCTION_SIGNATURE){
					checkFunctionCall(dimension_node);
					if(ID->attribute->attr.functionSignature->returnType != INT_TYPE)	
						printErrorMsg(dimension_node, ARRAY_SUBSCRIPT_NOT_INT);
				}
				else
					printErrorMsg(dimension_node, NOT_FUNCTION_NAME);
				break;
			default:
				printErrorMsg(dimension_node, ARRAY_SUBSCRIPT_NOT_INT);
		}
		dimension += 1;
		dimension_node = dimension_node->rightSibling;	
	}
}

int checkArrayIsScalar(AST_NODE* array_node){
	AST_NODE* dimension_node = array_node->child;
	char* name = getIDNodeName(array_node);
	SymbolTableEntry* ID = retrieveSymbol(name);
	int dimension, dim_count;
	checkVariableDimensions(array_node);
	dimension = ID->attribute->attr.typeDescriptor->properties.arrayProperties.dimension;
	for(dim_count = 0; dim_count < dimension; dim_count++){
		if(dimension_node == NULL)
			break;
		dimension_node = dimension_node->rightSibling;
	}
	if(dim_count != dimension)
		return 0;
	if(dimension_node != NULL){
		printErrorMsg(array_node, INCOMPATIBLE_ARRAY_DIMENSION);
		return -1;
	}
	return 1;
}
