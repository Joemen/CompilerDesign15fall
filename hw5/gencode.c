#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "header.h"
#include "symbolTable.h"
#include "gencode.h"

#if 0
  #define SPAM(a) printf a
#else
  #define SPAM(a) (void)0
#endif

FILE* fp;
AST_NODE* curNode;

static int i = 0 ;
static int ifcount = 0;
static int elsecount = 0;
static int whilecount = 0;
static int funccount = 0 ;
static int constcount = 0 ;
static int forcount = 0 ;


void initialRegisterTable();
void genGlobalVarDecl(AST_NODE* node);
void genFuncDecl(AST_NODE* node);

void genUniOp(UNARY_OPERATOR op, Register dst, Register lhs, RegType retType);
void genBinOp(BINARY_OPERATOR op, Register dst, Register lhs, Register rhs, RegType retType);

SymbolTableEntry* getSymbolTableEntry(AST_NODE* node);
TypeDescriptor* getTypeAttr(SymbolTableEntry* entry);
DATA_TYPE getDataType(SymbolTableEntry* symEntry);
char *getIDName(AST_NODE* idNode);
IDENTIFIER_KIND getIDKind(AST_NODE* idNode);
DECL_KIND getDecKind(AST_NODE* node);
STMT_KIND getStmtKind(AST_NODE* node);
EXPR_KIND getExprKind(AST_NODE* node);
int isConstEval(AST_NODE* exprNode); 
int getExprConstValue(AST_NODE* exprNode); 
BINARY_OPERATOR getExprBinOp(AST_NODE* exprNode); 
UNARY_OPERATOR getExprUniOp(AST_NODE* exprNode); 

int countArrSize(SymbolTableEntry *entry);
void genBlock(AST_NODE* blockNode);
void genStmt(AST_NODE* stmtNode);
void genFuncCallStmt(AST_NODE* funcCallStmtNode);
void genIfStmt(AST_NODE* stmtNode);
void genAssign(AST_NODE* stmtNode);
void genWhileStmt(AST_NODE* stmtNode);
void genForStmt(AST_NODE* stmtNode);
int calcBlockOffset(AST_NODE* blockNode);
ArrayProperties getArrProperties(SymbolTableEntry* entry);

void updateNewReg(Register reg);
Register checkRegister(AST_NODE* node);
Register switchRegister(RegType type);
Register getRegister(AST_NODE* node);
void freeRegister(Register reg);

RegisterTableEntry* registerTable;
Register pre = 0, ppre = 0, switch_xw = w9, switch_s = s9;
int frameSize;

Register genExpr(AST_NODE* node);
Register genIntExpr(AST_NODE* node);
Register genFloatExpr(AST_NODE* node);

void genUniOp(UNARY_OPERATOR op, Register dst, Register rhs, RegType retType)
{
	printf("op = %d\n",op);
	switch (op) {
		case UNARY_OP_POSITIVE:
			
			break;
		case UNARY_OP_NEGATIVE: 
			break;
		case UNARY_OP_LOGICAL_NEGATION:
			fprintf(fp,"movn %s, %s, #uimm16{, LSL}\n", regName[dst], regName[rhs]);
			SPAM(("		movn %s, %s, #uimm16{, LSL}\n", regName[dst], regName[rhs]));
			break;
		default: 
			break;
    }
}


void genBinOp(BINARY_OPERATOR op, Register dst, Register lhs, Register rhs, RegType retType)
{
	switch (op) {

		case BINARY_OP_ADD:	
			if(retType == W){
				fprintf(fp,"add %s, %s, %s\n", regName[dst], regName[lhs], regName[rhs]);  
				SPAM(("		add %s, %s, %s\n", regName[dst], regName[lhs], regName[rhs]));
			}
			else{
				fprintf(fp,"fadd %s, %s, %s\n", regName[dst], regName[lhs], regName[rhs]);  
				SPAM(("		fadd %s, %s, %s\n", regName[dst], regName[lhs], regName[rhs]));
			}
			break;
		case BINARY_OP_SUB: 
			if(retType == W){
				fprintf(fp,"sub %s, %s, %s\n", regName[dst], regName[lhs], regName[rhs]);  
				SPAM(("		sub %s, %s, %s\n", regName[dst], regName[lhs], regName[rhs]));
			}
			else{
				fprintf(fp,"fsub %s, %s, %s\n", regName[dst], regName[lhs], regName[rhs]);  
				SPAM(("		fsub %s, %s, %s\n", regName[dst], regName[lhs], regName[rhs]));
			}
			break;
		case BINARY_OP_MUL: 
			if(retType == W){
				fprintf(fp,"mul %s, %s, %s\n", regName[dst], regName[lhs], regName[rhs]);  
				SPAM(("		mul %s, %s, %s\n", regName[dst], regName[lhs], regName[rhs]));
			}
			else{
				fprintf(fp,"fmul %s, %s, %s\n", regName[dst], regName[lhs], regName[rhs]);  
				SPAM(("		fmul %s, %s, %s\n", regName[dst], regName[lhs], regName[rhs]));
			}
			break;
		case BINARY_OP_DIV: 
			if(retType == W){
				fprintf(fp,"sdiv %s, %s, %s\n", regName[dst], regName[lhs], regName[rhs]);  
				SPAM(("		sdiv %s, %s, %s\n", regName[dst], regName[lhs], regName[rhs]));
			}
			else{
				fprintf(fp,"fdiv %s, %s, %s\n", regName[dst], regName[lhs], regName[rhs]);  
				SPAM(("		fdiv %s, %s, %s\n", regName[dst], regName[lhs], regName[rhs]));
			}
			break;
		case BINARY_OP_AND:
			fprintf(fp,"and %s, %s, %s\n", regName[dst], regName[lhs], regName[rhs]);  
			SPAM(("		and %s, %s, %s\n", regName[dst], regName[lhs], regName[rhs]));  
			break;
		case BINARY_OP_OR:
			fprintf(fp,"orr %s, %s, %s\n", regName[dst], regName[lhs], regName[rhs]);  
			SPAM(("		orr %s, %s, %s\n", regName[dst], regName[lhs], regName[rhs]));  
			break;

		case BINARY_OP_EQ: case BINARY_OP_GE: case BINARY_OP_LE:
		case BINARY_OP_NE: case BINARY_OP_GT: case BINARY_OP_LT:
			if(registerTable[lhs].regType == S || registerTable[rhs].regType == S){
				fprintf(fp,"fcmp %s, %s\n", regName[lhs], regName[rhs]);
				SPAM(("		fcmp %s, %s\n", regName[lhs], regName[rhs]));
			}
			else{
				fprintf(fp,"cmp %s, %s\n", regName[lhs], regName[rhs]);
				SPAM(("		cmp %s, %s\n", regName[lhs], regName[rhs]));
			}
			switch (op) {
				case BINARY_OP_EQ:
					fprintf(fp,"cset %s, eq\n", regName[dst]);
					SPAM(("		cset %s, eq\n", regName[dst]));
					break;
				case BINARY_OP_GE:
					fprintf(fp,"cset %s, ge\n", regName[dst]);
					SPAM(("		cset %s, ge\n", regName[dst]));
					break; 
				case BINARY_OP_LE:
					fprintf(fp,"cset %s, le\n", regName[dst]);
					SPAM(("		cset %s, le\n", regName[dst]));
					break;
				case BINARY_OP_NE: 
					fprintf(fp,"cset %s, ne\n", regName[dst]);
					SPAM(("		cset %s, ne\n", regName[dst]));
					break;
				case BINARY_OP_GT: 
					fprintf(fp,"cset %s, gt\n", regName[dst]);
					SPAM(("		cset %s, gt\n", regName[dst]));
					break;
				case BINARY_OP_LT:
					fprintf(fp,"cset %s, lt\n", regName[dst]);
					SPAM(("		cset %s, lt\n", regName[dst]));
					break;
				default:
					break;		
			}
			break;
		default: 
			break;
    }
	
	
}
Register genExpr(AST_NODE* node)
{
	Register retReg, reg_left, reg_right;
	SPAM(("node->nodeType = %d\n",node->nodeType));
	switch (node->nodeType)
	{
		case EXPR_NODE:
			if(isConstEval(node)){
				SPAM(("EXPR is CONSTANT %d\n",node->nodeType));
				fflush(stdout);
				return getRegister(node);
			}
			else if (getExprKind(node)== BINARY_OPERATION){
				reg_left = genExpr(node->child);
				reg_right = genExpr(node->child->rightSibling);
				checkRegister(node->child);
				retReg = getRegister(node);
				genBinOp(getExprBinOp(node), retReg, reg_left, reg_right, registerTable[retReg].regType);
				freeRegister(reg_left);
				freeRegister(reg_right);
				return retReg;
			}
			else{	//UNARY_OPERATION
				reg_right = genExpr(node->child);
				retReg = getRegister(node);
				genUniOp(getExprUniOp(node), retReg, reg_right, registerTable[retReg].regType);
				freeRegister(reg_right);
				return retReg;
			}
		case CONST_VALUE_NODE:
			retReg = getRegister(node);
			return retReg;
		case IDENTIFIER_NODE:
			retReg = getRegister(node);
			return retReg;
		case STMT_NODE:  // FUNCTION_CALL_STMT
			retReg = getRegister(node);
			return retReg;
		default:
			break;
	}
	printf("ERROR NOT IMPLEMENT\n");
	return retReg;
//	return (node->dataType == FLOAT_TYPE) ? genFloatExpr(node) : genIntExpr(node);
}
int getExprConstValue(AST_NODE* exprNode)
{
	return exprNode->semantic_value.exprSemanticValue.constEvalValue.iValue;
}
int isConstEval(AST_NODE* exprNode)
{
	return exprNode->semantic_value.exprSemanticValue.isConstEval;
}

void initialRegisterTable(){
	registerTable = (RegisterTableEntry*)malloc(94*sizeof(RegisterTableEntry));
	Register i;
	for(i = 0; i < 94; i++){
    	registerTable[i].node = NULL;
		if(i < 31 && i >= 0)
		    registerTable[i].regType = W;
		else if(i < 62 && i >= 31)
		    registerTable[i].regType = X;
		else
		    registerTable[i].regType = S;
	    registerTable[i].state = FREE;
	}

}

SymbolTableEntry* getSymbolTableEntry(AST_NODE* node)
{
	return node->semantic_value.identifierSemanticValue.symbolTableEntry;
}

DATA_TYPE getDataType(SymbolTableEntry* symEntry)
{
	return symEntry->attribute->attr.typeDescriptor->properties.dataType;
}

ArrayProperties getArrProperties(SymbolTableEntry* entry)
{
	return entry->attribute->attr.typeDescriptor->properties.arrayProperties;
}

char *getIDName(AST_NODE* idNode)
{
	return idNode->semantic_value.identifierSemanticValue.identifierName;
}

TypeDescriptor* getTypeAttr(SymbolTableEntry* entry)
{
	return entry->attribute->attr.typeDescriptor ;
}

IDENTIFIER_KIND getIDKind(AST_NODE* idNode)
{
	return idNode->semantic_value.identifierSemanticValue.kind;
}

DECL_KIND getDecKind(AST_NODE* node)
{
	return node->semantic_value.declSemanticValue.kind;
}

STMT_KIND getStmtKind(AST_NODE* node)
{
	return node->semantic_value.stmtSemanticValue.kind;
}

EXPR_KIND getExprKind(AST_NODE* node)
{
	return node->semantic_value.exprSemanticValue.kind;
}


BINARY_OPERATOR getExprBinOp(AST_NODE* exprNode)
{
	return exprNode->semantic_value.exprSemanticValue.op.binaryOp;
}

UNARY_OPERATOR getExprUniOp(AST_NODE* exprNode)
{
	return exprNode->semantic_value.exprSemanticValue.op.unaryOp;
}

int countArrSize(SymbolTableEntry* entry)
{
	int dim;
	int count = 1;
	for (dim = 0; dim < entry->attribute->attr.typeDescriptor->properties.arrayProperties.dimension; dim++) {
		count *= entry->attribute->attr.typeDescriptor->properties.arrayProperties.sizeInEachDimension[dim];
	}
	return count;
}

int calcBlockOffset(AST_NODE* blockNode)
{
	AST_NODE *node = blockNode->child;
    int baseSize = 0, innerSize = 0;
    if (node != NULL && node->nodeType == VARIABLE_DECL_LIST_NODE) {
        AST_NODE *declNode;
        for (declNode = node->child; declNode != NULL; declNode = declNode->rightSibling) {
            AST_NODE* idNode;
            switch (getDecKind(declNode)) {
            case VARIABLE_DECL:
                for (idNode = declNode->child->rightSibling; idNode != NULL; idNode = idNode->rightSibling) {
                    int product = 1, i;
                    switch (getIDKind(idNode)) {
                    case NORMAL_ID:
                        baseSize += 4;
                        break;
                    case ARRAY_ID:
                        baseSize += 4 * countArrSize(getSymbolTableEntry(idNode));
                        break;
                    case WITH_INIT_ID:
                        baseSize += 4;
                        break;
                    default:
                        break;
                    }
                }
                break;
            default:
                break;
            }
        }
        node = node->rightSibling;
    }
    if (node != NULL) {
        for (node = node->child ; node != NULL; node = node->rightSibling) {
            int tmp = 0, tmp2 = 0;
            if (node->nodeType == STMT_NODE) {
                switch (getStmtKind(node)) {
                case WHILE_STMT:
                    if (node->child->rightSibling->nodeType == BLOCK_NODE) {
                        tmp = calcBlockOffset(node->child->rightSibling);
                    }
                    break;
                case FOR_STMT:
                    if (node->child->rightSibling->rightSibling->rightSibling->nodeType == BLOCK_NODE) {
                        tmp = calcBlockOffset(node->child->rightSibling->rightSibling->rightSibling);
                    }
                    break;
                case IF_STMT:
                    if (node->child->rightSibling->nodeType == BLOCK_NODE) {
                        tmp = calcBlockOffset(node->child->rightSibling);
                    }
                    if (node->child->rightSibling->rightSibling->nodeType == BLOCK_NODE) {
                        tmp2 = calcBlockOffset(node->child->rightSibling->rightSibling);
                    }                    
                    if (tmp < tmp2) 
						tmp = tmp2;
                    break;
                default:
                    break;
                }
            } else if (node->nodeType == BLOCK_NODE) {
                tmp = calcBlockOffset(node);
            } else if (node->nodeType == NUL_NODE) {
                // do nothing
            }
            if (innerSize < tmp)
				innerSize = tmp;
        }
    }
    return baseSize + innerSize;
	
	
}

void genFuncCallStmt(AST_NODE* funcCallStmtNode)
{
	
	AST_NODE *funcIdNode, *paramListNode, *exprNode;
    funcIdNode = funcCallStmtNode->child;
    paramListNode = funcIdNode->rightSibling;
    // special cases for read/write
    if (strcmp(getIDName(funcCallStmtNode->child), "write") == 0)
    {
        Register reg = genExpr(paramListNode->child) ;
        
		//printf("Get reg = %s\n",regName[reg]);
        if (paramListNode->child->dataType == CONST_STRING_TYPE)
        {
            fprintf(fp,"mov x0, %s\n", regName[reg]);	SPAM(("		mov x0, %s\n", regName[reg]));	
            fprintf(fp,"bl _write_str\n");				SPAM(("		bl _write_str\n"));	
        }
        else if (paramListNode->child->dataType == FLOAT_TYPE)
        {
            fprintf(fp,"fmov s0, %s\n", regName[reg]);	SPAM(("		fmov s0, %s\n", regName[reg]));
            fprintf(fp,"bl _write_float\n");			SPAM(("		bl _write_float\n"));	
        }
        else if (paramListNode->child->dataType == INT_TYPE)
        {
            fprintf(fp,"mov w0, %s\n", regName[reg]);	SPAM(("		mov w0, %s\n", regName[reg]));
            fprintf(fp,"bl _write_int\n");				SPAM(("		bl _write_int\n"));	
        }
        freeRegister(reg);
    }
    else if (strcmp(getIDName(funcCallStmtNode->child), "read") == 0)
    {
		fprintf(fp,"bl _read_int\n");				SPAM(("		bl _read_int\n"));	
        
	}
	else
	{		
		
		/*	TODO	*/
		
		
		
		fprintf(fp,"bl _start_%s\n", getIDName(funcCallStmtNode->child));	        
		SPAM(("		bl _start_%s\n", getIDName(funcCallStmtNode->child)));
    }
}

void genIfStmt(AST_NODE* stmtNode)
{
	Register result;
    int idIf = ifcount++;
    int idElse = elsecount++;
    AST_NODE* elseNode = stmtNode->child->rightSibling->rightSibling;
    AST_NODE* exprNode = stmtNode->child;
    if(exprNode->nodeType == STMT_NODE && getStmtKind(exprNode) == ASSIGN_STMT)		// if(n=1)
    {
		genAssign(exprNode);
		result = (exprNode->dataType == FLOAT_TYPE) ? s16 : x9;
	}
	else                                 //	> or < or != ...
	{
		result = genExpr(exprNode);
	}
	 
	fprintf(fp,"cmp %s, #0\n", regName[result]);	SPAM(("		cmp %s, #0\n", regName[result]));
	fprintf(fp,"beq _if_end_%d\n", idIf);		SPAM(("		beq _if_end_%d\n", idIf));
	
	if (exprNode->rightSibling->nodeType == STMT_NODE)			// if without {}
	{
        genStmt(exprNode->rightSibling);
    } 
    else if (exprNode->rightSibling->nodeType == BLOCK_NODE)	// if with {}
    {
        genBlock(exprNode->rightSibling);
    }
    fprintf(fp,"b _else_end_%d\n", idElse);		SPAM(("		b _else_end_%d\n", idElse));
    fprintf(fp,"_if_end_%d:\n", idIf);			SPAM(("		_if_end_%d:\n", idIf));
    if (elseNode->nodeType != NUL_NODE) {
        if (elseNode->nodeType == STMT_NODE) {
            genStmt(elseNode);
        } else if (elseNode->nodeType == BLOCK_NODE) {
            genBlock(elseNode);
        }
    }
    fprintf(fp,"_else_end_%d:\n", idElse);
}

void genAssign(AST_NODE* stmtNode)
{
	AST_NODE* left_node = stmtNode->child;
	AST_NODE* right_node = left_node->rightSibling;
	Register lreg, rreg;

	rreg = genExpr(right_node);
	lreg = getRegister(left_node);
	
	if(left_node->dataType == INT_TYPE){
		fprintf(fp,"mov %s, %s\n", regName[lreg], regName[rreg]);
		SPAM(("		mov %s, %s\n", regName[lreg], regName[rreg]));
	}
	else{
		fprintf(fp,"fmov %s, %s\n", regName[lreg], regName[rreg]);
		SPAM(("		fmov %s, %s\n", regName[lreg], regName[rreg]));
	}
	freeRegister(rreg);
	freeRegister(lreg);

	// TODO read() && fread()
}

void genWhileStmt(AST_NODE* stmtNode)
{
	int idWhile = whilecount++;
	Register result ;
	fprintf(fp,"_while_start_%d:\n", idWhile);
	SPAM(("		_while_start_%d:\n", idWhile));
	if (stmtNode->child->nodeType == STMT_NODE && getStmtKind(stmtNode->child) == ASSIGN_STMT)
	{
		genAssign(stmtNode->child);
		result = (stmtNode->child->dataType == FLOAT_TYPE) ? s16 : x9;
	}
	else
	{
		result = genExpr(stmtNode->child);
	}
    fprintf(fp,"cmp %s, #0\n", regName[result]);
    SPAM(("		cmp %s, #0\n", regName[result]));
    fprintf(fp,"beq _while_end_%d\n", idWhile);
    SPAM(("		beq _while_end_%d\n", idWhile));
	if (stmtNode->child->rightSibling->nodeType == STMT_NODE) 	// if without {}
	{
		genStmt(stmtNode->child->rightSibling);
	}
	else if (stmtNode->child->rightSibling->nodeType == BLOCK_NODE)	// if with {}
	{
		genBlock(stmtNode->child->rightSibling);
	}
	else if (stmtNode->child->rightSibling->nodeType == NUL_NODE) 
	{
                // do nothing
	}
	fprintf(fp,"b _while_start_%d\n", idWhile);		SPAM(("		b _while_start_%d\n", idWhile));
	fprintf(fp,"_while_end_%d:\n", idWhile);		SPAM(("		_while_end_%d:\n", idWhile));
}

void genForStmt(AST_NODE* stmtNode)
{
	int idFor = forcount++;
	Register result;
    AST_NODE *initNode, *conditionNode, *incrementNode, *loopNode;
	initNode = stmtNode->child;
	conditionNode = initNode->rightSibling;
	incrementNode = conditionNode->rightSibling;
	loopNode = incrementNode->rightSibling;
	fprintf(fp,"_for_start_%d:\n", idFor);
	SPAM(("		_for_start_%d:\n", idFor));

	if (initNode->nodeType == NONEMPTY_ASSIGN_EXPR_LIST_NODE)
	{
		AST_NODE *assignNode;
		for (assignNode = initNode->child ;  assignNode != NULL; assignNode = assignNode->rightSibling)
		{
				genAssign(assignNode);
		}
	}
	fprintf(fp,"_for_loop_%d:\n", idFor);
	SPAM(("		_for_loop_%d:\n", idFor));
	
	if (conditionNode->nodeType == NONEMPTY_RELOP_EXPR_LIST_NODE)
	{
		AST_NODE* exprNode ;
		for (exprNode = conditionNode->child; exprNode != NULL; exprNode = exprNode->rightSibling)
		{
			result = genExpr(exprNode);
		}
		fprintf(fp,"cmp %s, #0\n", regName[result]);
		SPAM(("		cmp %s, #0\n", regName[result]));
				
		fprintf(fp,"beq _for_end_%d\n", idFor);
		SPAM(("		beq _for_end_%d\n", idFor));
	
	}
	if (loopNode->nodeType == STMT_NODE)
	{
		genStmt(loopNode);
	}
	else if (loopNode->nodeType == BLOCK_NODE)
	{
		genBlock(loopNode);
	}
	else if (loopNode->nodeType == NUL_NODE)
	{
			// do nothing
	}
	if (incrementNode->nodeType == NONEMPTY_ASSIGN_EXPR_LIST_NODE) 
	{
		AST_NODE *assignNode ;
		for (assignNode = incrementNode->child; assignNode != NULL; assignNode = assignNode->rightSibling) 
		{
			if (assignNode->nodeType == STMT_NODE) 
			{
				genAssign(assignNode);
			} 
			else if (assignNode->nodeType == EXPR_NODE) 
			{
				genExpr(assignNode);
			}
		}
	}
	fprintf(fp,"b _for_loop_%d\n", idFor);
	SPAM(("		b _for_loop_%d\n", idFor));
	fprintf(fp,"_for_end_%d:\n", idFor);
	SPAM(("		_for_end_%d:\n", idFor));
}
	
void genStmt(AST_NODE* stmtNode)
{
    switch (getStmtKind(stmtNode)) {
    case WHILE_STMT:
		genWhileStmt(stmtNode);
        break;
    case FOR_STMT:
		genForStmt(stmtNode);
        break;
    case ASSIGN_STMT:
			SPAM(("assign_stmt is safe\n"));
			fflush(stdout);
		genAssign(stmtNode);
        break;
    case IF_STMT:
		genIfStmt(stmtNode);
        break;
    case FUNCTION_CALL_STMT:
		genFuncCallStmt(stmtNode);
        break;
    case RETURN_STMT:
		if (stmtNode->child->nodeType != NUL_NODE) {
            Register result = genExpr(stmtNode->child);
            if (stmtNode->child->dataType == FLOAT_TYPE) {
                if (result != s0) 
					fprintf(fp,"fmov s0, %s\n", regName[result]);	// not sure
					SPAM(("		fmov s0, %s\n", regName[result]));		
            }
            else {
                if (result != w0) 
					fprintf(fp,"mov w0, %s\n", regName[result]);
					SPAM(("		mov w0, %s\n", regName[result]));
            }
        }
        fprintf(fp,"b _end_%s\n", getIDName(curNode));
        SPAM(("		b _end_%s\n", getIDName(curNode)));
        
        break;
    default:
        break;
    }
}

void genBlock(AST_NODE *blockNode)
{
	AST_NODE *node = blockNode->child;
    if (node != NULL && node->nodeType == VARIABLE_DECL_LIST_NODE) {
        AST_NODE *declNode;        
        for (declNode = node->child; declNode != NULL; declNode = declNode->rightSibling) {
            if (getDecKind(declNode) == VARIABLE_DECL) {
                AST_NODE *typeNode, *idNode;
                for (idNode =declNode->child->rightSibling; idNode != NULL; idNode = idNode->rightSibling) {
                    if (getIDKind(idNode) == WITH_INIT_ID) {
                        //	TODO initial ID
                        Register result = genExpr(idNode->child);
						fprintf(fp,"str %s, [x29, #%d]\n",regName[result], getSymbolTableEntry(idNode)->offset);
						SPAM(("		str %s, [x29, #%d]\n",regName[result], getSymbolTableEntry(idNode)->offset));
                       
                    }
                }
            }
        }
        node = node->rightSibling;
    }
    
	if (node != NULL)
	{
		AST_NODE *i_node;
		
		for (i_node= node->child; i_node != NULL; i_node = i_node->rightSibling) 
		{
			if (i_node->nodeType == STMT_NODE)
			{
				genStmt(i_node);
			}
			else if (i_node->nodeType == BLOCK_NODE)
			{
				genBlock(i_node);
			} 			
		}
    }
	
}

void genGlobalVarDecl(AST_NODE *node)
{
	fprintf(fp,".data\n"); 	SPAM(("		.data\n"));
	AST_NODE* declNode ;
	for( declNode = node->child ; declNode  != NULL ; declNode =declNode ->rightSibling )
	{
		if (getDecKind(declNode) == VARIABLE_DECL)
		{
			AST_NODE* typeNode = declNode->child; //	int or float or void
            AST_NODE* varNode = typeNode->rightSibling;	// variable
            
            SymbolTableEntry* varSym = getSymbolTableEntry(varNode);
            DATA_TYPE type = getDataType(varSym);
            
            for(varNode = typeNode->rightSibling ;varNode!=NULL ; varNode = varNode->rightSibling )
            {
				char name[1024];
				memset(name,0,sizeof(name));
				char* typeName;
				
				sprintf(name, "_g_%s", getIDName(varNode));
				
				switch(getIDKind(varNode)){
						case NORMAL_ID: 
							if(type == INT_TYPE)
							{
								int ivalue = 0 ;
								typeName = ".word";
								if(varNode->child != NULL )
								{
									ivalue = varNode->child->semantic_value.const1->const_u.intval;
								}
								fprintf(fp,"%s: %s %d\n", name, typeName, ivalue);
								SPAM(("		%s: %s %d\n", name, typeName, ivalue));
							}
							else if(type == FLOAT_TYPE)
							{
								float fvalue = 0.0;
								typeName = ".float";
								if(varNode->child != NULL )
								{
									fvalue = varNode->child->semantic_value.const1->const_u.fval;
								}
								fprintf(fp,"%s: %s %.1f\n", name, typeName, fvalue);
								SPAM(("		%s: %s %.1f\n", name, typeName, fvalue));
							}							
							break;
						case ARRAY_ID:
							{
								int size ; 
								SymbolTableEntry* arraySym ;
								arraySym = getSymbolTableEntry(varNode);
								size = countArrSize(arraySym) * 4;
								fprintf(fp,"%s: .space %d\n", name, size);		//	skip size*4 bytes reference by course ppt ARM64-CodeGen-I.ptt p.20
								SPAM(("		%s: .space %d\n", name, size));		//	not sure reference by course ppt ARM64-CodeGen-I.ptt p.21
							}
							break;
						case WITH_INIT_ID: 
							{
								AST_NODE *constNode = varNode->child;
								//SPAM(("Type = %d\n",varNode->child->semantic_value.const1->const_type));
								if(varNode->child->semantic_value.const1->const_type == INTEGERC)
								{
									int value = constNode->semantic_value.const1->const_u.intval;
									fprintf(fp,"%s: .word %d\n", name, value);
									SPAM(("		%s: .word %d\n", name, value));
								}
								else if (varNode->child->semantic_value.const1->const_type == FLOATC)
								{
									float value = constNode->semantic_value.const1->const_u.fval;
									fprintf(fp,"%s: .float %.1f\n", name, value);
									SPAM(("		%s: .float %.1f\n", name, value));
								} 
							}
							break;
						default:
							break;
				}
			}
		}
	}	
}

void genFuncDecl(AST_NODE *node)
{
	char name[300];
	int frameSize = 92 ;
	int len;
	AST_NODE *funtionTypeNode, *funtionNameNode, *paraListNode, *blockNode;
    funtionTypeNode = node->child;				//	int 
    funtionNameNode = funtionTypeNode->rightSibling;	//	MAIN
    paraListNode = funtionNameNode->rightSibling;	//	parameterListNode
    blockNode = paraListNode->rightSibling;	//	block node
    curNode = funtionNameNode;
		
	memset(name,0,sizeof(name));
	
	strcpy(name,"frameSize_");
	strcat(name,getIDName(funtionNameNode));
	
	len = strlen(name);
	name[len] = '\0';
	
	frameSize = 92 + calcBlockOffset(blockNode);
	SPAM(("frameSize = %d\n",frameSize));
	fprintf(fp,".text\n");										SPAM(("		.text\n"));
    fprintf(fp,"_start_%s:\n", getIDName(funtionNameNode));		SPAM(("		_start_%s:\n", getIDName(funtionNameNode)));
	fprintf(fp,"str x30, [sp, #0]\n");							SPAM(("		str x30, [sp, #0]\n"));
	fprintf(fp,"str x29, [sp, #-8]\n");							SPAM(("		str x29, [sp, #-8]\n"));
	fprintf(fp,"add x29, sp, #-8\n");							SPAM(("		add x29, sp, #-8\n"));
	fprintf(fp,"add sp, sp, #-16\n");							SPAM(("		add sp, sp, #-16\n"));
	
	
	fprintf(fp,"ldr x30, =_%s\n",name);										SPAM(("		ldr x30, =_%s\n",name));
	fprintf(fp,"ldr x30, [x30, #0]\n");										SPAM(("		ldr x30, [x30, #0]\n"));
	fprintf(fp,"sub sp, sp, w30\n");										SPAM(("		sub sp, sp, w30\n"));
	
	fprintf(fp,"str x9, [sp, #8]\n");										SPAM(("		str x9, [sp, #8]\n"));
	fprintf(fp,"str x10, [sp, #16]\n");										SPAM(("		str x10, [sp, #16]\n"));
	fprintf(fp,"str x11, [sp, #24]\n");										SPAM(("		str x11, [sp, #24]\n"));
	fprintf(fp,"str x12, [sp, #32]\n");										SPAM(("		str x12, [sp, #32]\n"));
	fprintf(fp,"str x13, [sp, #40]\n");										SPAM(("		str x13, [sp, #40]\n"));
	fprintf(fp,"str x14, [sp, #48]\n");										SPAM(("		str x14, [sp, #48]\n"));
	fprintf(fp,"str x15, [sp, #56]\n");										SPAM(("		str x15, [sp, #56]\n"));
	fprintf(fp,"str s16, [sp, #64]\n");										SPAM(("		str s16, [sp, #64]\n"));
	fprintf(fp,"str s17, [sp, #68]\n");										SPAM(("		str s17, [sp, #68]\n"));
	fprintf(fp,"str s18, [sp, #72]\n");										SPAM(("		str s18, [sp, #72]\n"));
	fprintf(fp,"str s19, [sp, #76]\n");										SPAM(("		str s19, [sp, #76]\n"));
	fprintf(fp,"str s20, [sp, #80]\n");										SPAM(("		str s20, [sp, #80]\n"));
	fprintf(fp,"str s21, [sp, #84]\n");										SPAM(("		str s21, [sp, #84]\n"));
	fprintf(fp,"str s22, [sp, #88]\n");										SPAM(("		str s22, [sp, #88]\n"));
	fprintf(fp,"str s23, [sp, #92]\n");										SPAM(("		str s23, [sp, #92]\n"));
	
	
	genBlock(blockNode);
	
	fprintf(fp,"_end_%s:\n", getIDName(funtionNameNode));		SPAM(("		_end_%s:\n", getIDName(funtionNameNode)));	
	fprintf(fp,"ldr x9, [sp, #8]\n");							SPAM(("		ldr x9, [sp, #8]\n"));
	fprintf(fp,"ldr x10, [sp, #16]\n");							SPAM(("		ldr x10, [sp, #16]\n"));	
	fprintf(fp,"ldr x11, [sp, #24]\n");							SPAM(("		ldr x11, [sp, #24]\n"));	
	fprintf(fp,"ldr x12, [sp, #32]\n");							SPAM(("		ldr x12, [sp, #32]\n"));	
	fprintf(fp,"ldr x13, [sp, #40]\n");							SPAM(("		ldr x13, [sp, #40]\n"));	
	fprintf(fp,"ldr x14, [sp, #48]\n");							SPAM(("		ldr x14, [sp, #48]\n"));	
	fprintf(fp,"ldr x15, [sp, #56]\n");							SPAM(("		ldr x15, [sp, #56]\n"));
	fprintf(fp,"ldr s16, [sp, #64]\n");							SPAM(("		ldr s16, [sp, #64]\n"));	
	fprintf(fp,"ldr s17, [sp, #68]\n");							SPAM(("		ldr s17, [sp, #68]\n"));	
	fprintf(fp,"ldr s18, [sp, #72]\n");							SPAM(("		ldr s18, [sp, #72]\n"));		
	fprintf(fp,"ldr s19, [sp, #76]\n");							SPAM(("		ldr s19, [sp, #76]\n"));	
	fprintf(fp,"ldr s20, [sp, #80]\n");							SPAM(("		ldr s20, [sp, #80]\n"));	
	fprintf(fp,"ldr s21, [sp, #84]\n");							SPAM(("		ldr s21, [sp, #84]\n"));
	fprintf(fp,"ldr s22, [sp, #88]\n");							SPAM(("		ldr s22, [sp, #88]\n"));	
	fprintf(fp,"ldr s23, [sp, #92]\n");							SPAM(("		ldr s23, [sp, #92]\n"));	
	fprintf(fp,"ldr x30, [x29, #8]\n");							SPAM(("		ldr x30, [x29, #8]\n"));
	
	fprintf(fp,"mov sp, x29\n");								SPAM(("		mov sp, x29\n"));
	fprintf(fp,"add sp, sp, #8\n");								SPAM(("		add sp, sp, #8\n"));
	fprintf(fp,"ldr x29, [x29,#0]\n");							SPAM(("		ldr x29, [x29,#0]\n"));
	fprintf(fp,"RET x30\n");									SPAM(("		RET x30\n"));
	fprintf(fp,".data\n");										SPAM(("		.data\n"));
	fprintf(fp,"_%s: .word %d\n",name,frameSize);				SPAM(("		_%s: .word %d\n",name,frameSize));
	
	

}

void updateNewReg(Register reg){
	ppre = pre;
	pre = reg;
}
Register switchRegister(RegType type){
	Register startReg, nowReg;
	int i, find_free = 0, j;
	if(type == X || type == W){
		startReg = w9;
		//check for state == FREE
		for(i = 0; i < 20; i++){
			SPAM(("In switch function: reg %s: state %d, reg %s: state %d\n", regName[startReg+i], registerTable[startReg+i].state, regName[startReg+i+x0], registerTable[startReg+i+x0].state));
			if(registerTable[startReg+i].state != FREE || registerTable[startReg+i+x0].state != FREE)
				continue;
			if(type == W)
				return startReg+i;
			else
				return startReg+i+x0;
		}
		//check for state == DIRTY
		for(i = 0; i < 20; i++){
			if(registerTable[startReg+i].state != DIRTY && registerTable[startReg+i+x0].state != DIRTY)
				continue;
			if(registerTable[startReg+i].state == DIRTY)
				j = 0;
			else
				j = x0;
			freeRegister(startReg+i+j);
			return startReg+i+j;
		}
		//No? switch clean
		while(switch_xw == pre || switch_xw == ppre || switch_xw+x0 == pre || switch_xw+x0 == ppre){
			switch_xw += 1;
			if(switch_xw == w29)
				switch_xw = w9;
		}
		frameSize += 4;
		Offset -= 4;
		registerTable[switch_xw].node->inRegTable = 0;
		registerTable[switch_xw].node->offset = Offset;
		fprintf(fp,"str %s, [fp, #%d]\n", regName[switch_xw], Offset);
		SPAM(("		str %s, [fp, #%d]\n", regName[switch_xw], Offset));	
		registerTable[switch_xw].state = FREE;
		registerTable[switch_xw].node = NULL;
	}
	if(type == S){
		startReg = s9;
		//check for state == FREE
		for(i = 0; i < 20; i++){
			if(registerTable[startReg+i].state != FREE)
				continue;
			return startReg+i;
		}
		//check for state == DIRTY
		for(i = 0; i < 20; i++){
			if(registerTable[startReg+i].state != DIRTY)
				continue;
			freeRegister(startReg+i);
			return startReg+i;
		}
		while(switch_s == pre || switch_s == ppre){
			switch_s += 1;
			if(switch_s == s29)
				switch_s = s9;
		}
		frameSize += 4;
		Offset -= 4;
		registerTable[switch_s].node->inRegTable = 0;
		registerTable[switch_s].node->offset = Offset;
		fprintf(fp,"str %s, [fp, #%d]\n", regName[switch_s], Offset);
		SPAM(("		str %s, [fp, #%d]\n", regName[switch_s], Offset));	
		registerTable[switch_s].state = FREE;
		registerTable[switch_s].node = NULL;
	}
}
Register getRegister(AST_NODE* node){
	AST_NODE* dimNode;
	Register retReg, tmpReg, tmpReg2, tmpReg3;
	char buf[256],name[256];
	int i, c_int, array_offset, dimension;
	int* dimList;
	float c_float;
	SymbolTableEntry* entry;
	switch(node->nodeType){
		case IDENTIFIER_NODE:			
			entry = node->semantic_value.identifierSemanticValue.symbolTableEntry;
			if(entry->nestingLevel != 0){	//local
				switch(entry->attribute->attr.typeDescriptor->properties.dataType){
					case INT_TYPE:
						retReg = switchRegister(W);
						break;
					case FLOAT_TYPE:
						retReg = switchRegister(S);
						break;
					default:
						retReg = switchRegister(W);
						break;
				}
				updateNewReg(retReg);
				registerTable[retReg].state = CLEAN;
				if(getIDKind(node) == ARRAY_ID){
					//calculate offset
					dimNode = node->child;
					array_offset = 0;
					entry = node->semantic_value.identifierSemanticValue.symbolTableEntry;
					dimension = 0;
					dimList = entry->attribute->attr.typeDescriptor->properties.arrayProperties.sizeInEachDimension;
					tmpReg = switchRegister(W);
					fprintf(fp,"ldr %s, #0\n", regName[tmpReg]);
					SPAM(("		ldr %s, #0\n", regName[tmpReg]));
					registerTable[tmpReg].state = CLEAN;
					while(dimNode != NULL){
						tmpReg2 = getRegister(dimNode);
						registerTable[tmpReg2].state = CLEAN;
						tmpReg3 = switchRegister(W);
						fprintf(fp,".data\n");
						fprintf(fp,"_CONSTANT_%d: .word %d\n", constcount, dimList[dimension]);
						fprintf(fp,".align 3\n");												
						SPAM(("		.data\n"));
						SPAM(("		_CONSTANT_%d: .word %d\n", constcount, dimList[dimension]));
						SPAM(("		.align 3\n"));
						fprintf(fp,".text\n");
						SPAM(("		.text\n"));	
						fprintf(fp,"ldr %s, _CONSTANT_%d\n", regName[tmpReg3], constcount);
						SPAM(("		ldr %s, _CONSTANT_%d\n", regName[tmpReg3], constcount));
						constcount++;
						fprintf(fp,"mul %s, %s, %s\n", regName[tmpReg], regName[tmpReg], regName[tmpReg3]);
						SPAM(("		mul %s, %s, %s\n", regName[tmpReg], regName[tmpReg], regName[tmpReg3]));
						
						fprintf(fp,"add %s, %s, %s\n", regName[tmpReg], regName[tmpReg], regName[tmpReg2]);
						SPAM(("		add %s, %s, %s\n", regName[tmpReg], regName[tmpReg], regName[tmpReg2]));
						registerTable[tmpReg2].state = FREE;

						dimNode = dimNode -> rightSibling;
						dimension++;
					}

					fprintf(fp,"LSL %s, %s, #2\n", regName[tmpReg], regName[tmpReg]);
					SPAM(("		LSL %s, %s, #2\n", regName[tmpReg], regName[tmpReg]));
					fprintf(fp,"add %s, %s, #%d\n", regName[tmpReg], regName[tmpReg], entry->offset);
					SPAM(("		add %s, %s, #%d\n", regName[tmpReg], regName[tmpReg], entry->offset));
					tmpReg2 = switchRegister(X);
					fprintf(fp,"STR %s, [%s, #0]\n", regName[tmpReg], regName[tmpReg2]);
					SPAM(("		STR %s, [%s, #0]\n", regName[tmpReg], regName[tmpReg2]));
					fprintf(fp,"ldr %s, [x29, %s]\n", regName[retReg], regName[tmpReg2]);
					SPAM(("		ldr %s, [x29, %s]\n", regName[retReg], regName[tmpReg2]));
					registerTable[tmpReg].state = FREE;
				}
				else{
					fprintf(fp,"ldr %s, [x29, #%d]\n", regName[retReg], entry->offset);
					SPAM(("		ldr %s, [x29, #%d]\n", regName[retReg], entry->offset));
				}
			}
			else{	//global
				memset(name,0,sizeof(name));
				sprintf(name, "=_g_%s", entry->name);
				tmpReg3 = switchRegister(X);
				fprintf(fp,"ldr %s, %s\n", regName[tmpReg3], name);
				SPAM(("		ldr %s, %s\n", regName[tmpReg3], name));
				registerTable[tmpReg3].state = CLEAN;
				switch(entry->attribute->attr.typeDescriptor->properties.dataType){
					case INT_TYPE:
						retReg = switchRegister(W);
						break;
					case FLOAT_TYPE:
						retReg = switchRegister(S);
						break;			
					default:
						retReg = switchRegister(W);
						break;
				}
				updateNewReg(retReg);
				registerTable[retReg].state = CLEAN;
				if(getIDKind(node) == ARRAY_ID){
					//calculate offset
					dimNode = node->child;
					array_offset = 0;
					entry = node->semantic_value.identifierSemanticValue.symbolTableEntry;
					dimension = 0;
					dimList = entry->attribute->attr.typeDescriptor->properties.arrayProperties.sizeInEachDimension;
					tmpReg = switchRegister(W);
					fprintf(fp,"ldr %s, #0\n", regName[tmpReg]);
					SPAM(("		ldr %s, #0\n", regName[tmpReg]));
					registerTable[tmpReg].state = CLEAN;
					while(dimNode != NULL){
						tmpReg2 = getRegister(dimNode);
						registerTable[tmpReg2].state = CLEAN;
						tmpReg3 = switchRegister(W);
						fprintf(fp,".data\n");
						fprintf(fp,"_CONSTANT_%d: .word %d\n", constcount, dimList[dimension]);
						fprintf(fp,".align 3\n");												
						SPAM(("		.data\n"));
						SPAM(("		_CONSTANT_%d: .word %d\n", constcount, dimList[dimension]));
						SPAM(("		.align 3\n"));
						fprintf(fp,".text\n");
						SPAM(("		.text\n"));	
						fprintf(fp,"ldr %s, _CONSTANT_%d\n", regName[tmpReg3], constcount);
						SPAM(("		ldr %s, _CONSTANT_%d\n", regName[tmpReg3], constcount));
						constcount++;
						fprintf(fp,"mul %s, %s, %s\n", regName[tmpReg], regName[tmpReg], regName[tmpReg3]);
						SPAM(("		mul %s, %s, %s\n", regName[tmpReg], regName[tmpReg], regName[tmpReg3]));
						
						fprintf(fp,"add %s, %s, %s\n", regName[tmpReg], regName[tmpReg], regName[tmpReg2]);
						SPAM(("		add %s, %s, %s\n", regName[tmpReg], regName[tmpReg], regName[tmpReg2]));
						registerTable[tmpReg2].state = FREE;

						dimNode = dimNode -> rightSibling;
						dimension++;
					}
					fprintf(fp,"LSL %s, %s, #2\n", regName[tmpReg], regName[tmpReg]);
					SPAM(("		LSL %s, %s, #2\n", regName[tmpReg], regName[tmpReg]));
					fprintf(fp,"ldr %s, [%s, %s]\n", regName[retReg], regName[tmpReg3], regName[tmpReg]);
					SPAM(("		ldr %s, [%s, %s]\n", regName[retReg], regName[tmpReg3], regName[tmpReg]));
					registerTable[tmpReg].state = FREE;
					registerTable[tmpReg3].state = FREE;
				}
				else{
					fprintf(fp,"ldr %s, [%s, #0]\n", regName[retReg], regName[tmpReg3]);
					SPAM(("		ldr %s, [%s, #0]\n", regName[retReg], regName[tmpReg3]));
					registerTable[tmpReg3].state = FREE;
				}
			}
			registerTable[retReg].node = node;
			node->inRegTable = 1;
			node->reg = retReg;
			return retReg;
	    case EXPR_NODE:
			switch(node->dataType){
				case INT_TYPE:
					retReg = switchRegister(W);
					if(node->semantic_value.exprSemanticValue.isConstEval == 1){
						c_int = node->semantic_value.exprSemanticValue.constEvalValue.iValue;
						fprintf(fp,".data\n");
						fprintf(fp,"_CONSTANT_%d: .word %d\n", constcount, c_int);
						fprintf(fp,".align 3\n");												
						SPAM(("		.data\n"));
						SPAM(("		_CONSTANT_%d: .word %d\n", constcount, c_int));
						SPAM(("		.align 3\n"));
						fprintf(fp,".text\n");
						SPAM(("		.text\n"));	
						fprintf(fp,"ldr %s, _CONSTANT_%d\n", regName[retReg], constcount);
						SPAM(("		ldr %s, _CONSTANT_%d\n", regName[retReg], constcount));
						constcount++;
					}
					break;
				case FLOAT_TYPE:
					if (getExprKind(node) == BINARY_OPERATION){
						switch(getExprBinOp(node)){
							case BINARY_OP_EQ: case BINARY_OP_GE: case BINARY_OP_LE:
							case BINARY_OP_NE: case BINARY_OP_GT: case BINARY_OP_LT:
								retReg = switchRegister(W);								
								break;
							default:
								retReg = switchRegister(S);
						}
					}
					else
						retReg = switchRegister(S);
					if(node->semantic_value.exprSemanticValue.isConstEval == 1){
						c_float = node->semantic_value.exprSemanticValue.constEvalValue.fValue;
						fprintf(fp,".data\n");
						if(registerTable[retReg].regType == S){
							fprintf(fp,"_CONSTANT_%d: .float %f\n", constcount, c_float);
							fprintf(fp,".align 3\n");												
							SPAM(("		.data\n"));
							SPAM(("		_CONSTANT_%d: .float %f\n", constcount, c_float));
							SPAM(("		.align 3\n"));
							fprintf(fp,".text\n");
							SPAM(("		.text\n"));	
						}
						else{
							fprintf(fp,"_CONSTANT_%d: .word %d\n", constcount, (int)c_float);
							fprintf(fp,".align 3\n");												
							SPAM(("		.data\n"));
							SPAM(("		_CONSTANT_%d: .float %d\n", constcount, (int)c_float));
							SPAM(("		.align 3\n"));
							fprintf(fp,".text\n");
							SPAM(("		.text\n"));	
						}
						fprintf(fp,"ldr %s, _CONSTANT_%d\n", regName[retReg], constcount);
						SPAM(("		ldr %s, _CONSTANT_%d\n", regName[retReg], constcount));
						constcount++;
					}
					break;
				default:
					retReg = switchRegister(W);
			}
			updateNewReg(retReg);
			registerTable[retReg].state = CLEAN;
			registerTable[retReg].node = node;
			node->inRegTable = 1;
			node->reg = retReg;
			return retReg;
		case CONST_VALUE_NODE: //ex:1, 2, "constant string"
			switch(node->semantic_value.const1->const_type){
				case INTEGERC:
					c_int = node->semantic_value.const1->const_u.intval;
					fprintf(fp,".data\n");
					fprintf(fp,"_CONSTANT_%d: .word %d\n", constcount, c_int);
					fprintf(fp,".align 3\n");												
					SPAM(("		.data\n"));
					SPAM(("		_CONSTANT_%d: .word %d\n", constcount, c_int));
					SPAM(("		.align 3\n"));
					retReg = switchRegister(W);
					updateNewReg(retReg);
					registerTable[retReg].state = CLEAN;
					registerTable[retReg].node = node;
					fprintf(fp,".text\n");
					SPAM(("		.text\n"));	
					fprintf(fp,"ldr %s, _CONSTANT_%d\n", regName[retReg], constcount);
					SPAM(("		ldr %s, _CONSTANT_%d\n", regName[retReg], constcount));
					constcount++;
					node->inRegTable = 1;
					node->reg = retReg;
					return retReg;
				case FLOATC:
					c_float = node->semantic_value.const1->const_u.fval;
					fprintf(fp,".data\n");
					fprintf(fp,"_CONSTANT_%d: .float %f\n", constcount, c_float);
					fprintf(fp,".align 3\n");												
					SPAM(("		.data\n"));
					SPAM(("		_CONSTANT_%d: .float %f\n", constcount, c_float));
					SPAM(("		.align 3\n"));
					retReg = switchRegister(S);
					updateNewReg(retReg);
					registerTable[retReg].state = CLEAN;
					registerTable[retReg].node = node;
					fprintf(fp,".text\n");
					SPAM(("		.text\n"));	
					fprintf(fp,"ldr %s, _CONSTANT_%d\n", regName[retReg], constcount);
					SPAM(("		ldr %s, _CONSTANT_%d\n", regName[retReg], constcount));
					constcount++;
					node->inRegTable = 1;
					node->reg = retReg;
					return retReg;
				case STRINGC:
					memset(name,0,sizeof(name));
					strncpy(buf, node->semantic_value.const1->const_u.sc, 256);
					for (i = 1; buf[i] != '"'; i++);
					buf[i] = '\0';
					fprintf(fp,".data\n");
					fprintf(fp,"_CONSTANT_%d: .ascii \"%s\\000\"\n", constcount, buf + 1);
					fprintf(fp,".align 3\n");												
					sprintf(name, "_CONSTANT_%d", constcount);
					SPAM(("		.data\n"));
					SPAM(("		_CONSTANT_%d: .ascii \"%s\\000\"\n", constcount, buf + 1));
					SPAM(("		.align 3\n"));
					retReg = switchRegister(X);
					updateNewReg(retReg);
					registerTable[retReg].state = CLEAN;
					registerTable[retReg].node = node;
					fprintf(fp,".text\n");
					SPAM(("		.text\n"));	
					fprintf(fp,"ldr %s, =_CONSTANT_%d\n", regName[retReg], constcount);
					SPAM(("		ldr %s, =_CONSTANT_%d\n", regName[retReg], constcount));
					constcount++;
					node->inRegTable = 1;
					node->reg = retReg;
					return retReg;
			}
	    case STMT_NODE:
			genFuncCallStmt(node);
			if(node->dataType == INT_TYPE){
				retReg = switchRegister(W);
				updateNewReg(retReg);
				registerTable[retReg].state = CLEAN;
				registerTable[retReg].node = node;
				node->inRegTable = 1;
				node->reg = retReg;
				fprintf(fp,"mov %s, w0\n", regName[retReg]);
				SPAM(("		mov %s, w0\n", regName[retReg]));
			}
			else if(node->dataType == FLOAT_TYPE){
				retReg = switchRegister(S);
				updateNewReg(retReg);
				registerTable[retReg].state = CLEAN;
				registerTable[retReg].node = node;
				node->inRegTable = 1;
				node->reg = retReg;
				fprintf(fp,"fmov %s, s0\n", regName[retReg]);
				SPAM(("		fmov %s, s0\n", regName[retReg]));
			}
			else{
				SPAM(("under what condition you need to get this thing? PLZ tell me.\n"));
			}
			return retReg;
		case PROGRAM_NODE:
	    case DECLARATION_NODE:
		case PARAM_LIST_NODE:
		case NUL_NODE:
		case BLOCK_NODE:
		case VARIABLE_DECL_LIST_NODE:
		case STMT_LIST_NODE:
		case NONEMPTY_ASSIGN_EXPR_LIST_NODE:
		case NONEMPTY_RELOP_EXPR_LIST_NODE:
			SPAM(("In getRegister: wrong type %d\n", node->nodeType));
			break;
	}
}
Register checkRegister(AST_NODE* node){
	Register retReg;
	SymbolTableEntry* entry;
	if(node->inRegTable == 0){
		switch(node->nodeType){
			case IDENTIFIER_NODE:
				entry = node->semantic_value.identifierSemanticValue.symbolTableEntry;
				switch(entry->attribute->attr.typeDescriptor->properties.dataType){
					case INT_TYPE:
						retReg = switchRegister(W);
						break;
					case FLOAT_TYPE:
						retReg = switchRegister(S);
						break;			
				}
				updateNewReg(retReg);
				fprintf(fp,"ldr %s, [x29, #%d]\n", regName[retReg], node->offset);
				SPAM(("		ldr %s, [x29, #%d]\n", regName[retReg], node->offset));	
				node->inRegTable = 1;
				node->reg = retReg;
				return retReg;
			case EXPR_NODE:
				switch(node->dataType){
					case INT_TYPE:
						retReg = switchRegister(W);
						break;
					case FLOAT_TYPE:
						if (getExprKind(node) == BINARY_OPERATION){
							switch(getExprBinOp(node)){
								case BINARY_OP_EQ: case BINARY_OP_GE: case BINARY_OP_LE:
								case BINARY_OP_NE: case BINARY_OP_GT: case BINARY_OP_LT:
									retReg = switchRegister(W);
									break;
								default:
									retReg = switchRegister(S);
							
							}
							break;
						}
						retReg = switchRegister(S);
						break;
				}
				updateNewReg(retReg);
				fprintf(fp,"ldr %s, [x29, #%d]\n", regName[retReg], node->offset);
				SPAM(("		ldr %s, [x29, #%d]\n", regName[retReg], node->offset));	
				node->inRegTable = 1;
				node->reg = retReg;
				return retReg;
			case CONST_VALUE_NODE:
				switch(node->semantic_value.const1->const_type){
					case INTEGERC:
						retReg = switchRegister(W);
						fprintf(fp,"ldr %s, [x29, #%d]\n", regName[retReg], node->offset);
						SPAM(("		ldr %s, [x29, #%d]\n", regName[retReg], node->offset));	
						updateNewReg(retReg);
						node->inRegTable = 1;
						node->reg = retReg;
						return retReg;
					case FLOATC:
						retReg = switchRegister(S);
						fprintf(fp,"ldr %s, [x29, #%d]\n", regName[retReg], node->offset);
						SPAM(("		ldr %s, [x29, #%d]\n", regName[retReg], node->offset));	
						updateNewReg(retReg);
						node->inRegTable = 1;
						node->reg = retReg;
						return retReg;
					case STRINGC:
						retReg = switchRegister(X);
						fprintf(fp,"ldr %s, [x29, #%d]\n", regName[retReg], node->offset);
						SPAM(("		ldr %s, [x29, #%d]\n", regName[retReg], node->offset));	
						updateNewReg(retReg);
						node->inRegTable = 1;
						node->reg = retReg;
						return retReg;
				}
		}
	}
	else
		return node->reg;
}
void freeRegister(Register reg){
	//Dirty
	int offset;
	char name[256];
	Register tmpReg;
	SymbolTableEntry* entry;
//	printf("freeing reg %s; state:%d\n", regName[reg], registerTable[reg].state);
	switch(registerTable[reg].node->nodeType){
		case IDENTIFIER_NODE:
			entry = registerTable[reg].node->semantic_value.identifierSemanticValue.symbolTableEntry;
			if(entry->nestingLevel != 0){
				offset = registerTable[reg].node->semantic_value.identifierSemanticValue.symbolTableEntry->offset;
				fprintf(fp,"str %s, [x29, #%d]\n", regName[reg], offset);
				SPAM(("		str %s, [x29, #%d]\n", regName[reg], offset));
			}
			else{	//global
				memset(name,0,sizeof(name));
				sprintf(name, "=_g_%s", entry->name);
				tmpReg = switchRegister(X);
				fprintf(fp,"ldr %s, %s\n", regName[tmpReg], name);
				SPAM(("		ldr %s, %s\n", regName[tmpReg], name));
				fprintf(fp,"str %s, [%s, #0]\n", regName[reg], regName[tmpReg]);
				SPAM(("		str %s, [%s, #0]\n", regName[reg], regName[tmpReg]));
			}
			break;
	    case EXPR_NODE:
			break;
		case CONST_VALUE_NODE:
			break;
	}
	registerTable[reg].state = FREE;
	registerTable[reg].node = NULL;
}


/* visit child of prog node */
void codegen(AST_NODE* prog)
{
	initialRegisterTable();
	fp = fopen("output.s", "w");
	AST_NODE* node = prog->child;

	while(node != NULL){
		if(getAstType(node) == VARIABLE_DECL_LIST_NODE)
			genGlobalVarDecl(node);
		if(getAstType(node) == DECLARATION_NODE)
			genFuncDecl(node);
		node = node -> rightSibling;
	}
	
	fprintf(fp,"_start_read:\n");		SPAM(("		_start_read:\n"));
    fprintf(fp,"b _read_int\n");		SPAM(("		b _read_int\n"));
    fprintf(fp,"_start_fread:\n");		SPAM(("		_start_fread:\n"));
    fprintf(fp,"b _read_float\n");		SPAM(("		b _read_float\n"));
	
	fclose(fp);	
}
