/* Parser template 2015 
===== Definition Section ===== */

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "header.h"
int linenumber = 1;
AST_NODE *prog;

extern int g_anyErrorOccur;

static inline AST_NODE* makeSibling(AST_NODE *a, AST_NODE *b)
{ 
    while (a->rightSibling) {
        a = a->rightSibling;
    }
    if (b == NULL) {
        return a;
    }
    b = b->leftmostSibling;
    a->rightSibling = b;
    
    b->leftmostSibling = a->leftmostSibling;
    b->parent = a->parent;
    while (b->rightSibling) {
        b = b->rightSibling;
        b->leftmostSibling = a->leftmostSibling;
        b->parent = a->parent;
    }
    return b;
}

static inline AST_NODE* makeChild(AST_NODE *parent, AST_NODE *child)
{
    if (child == NULL) {
        return parent;
    }
    if (parent->child) {
        makeSibling(parent->child, child);
    } else {
        child = child->leftmostSibling;
        parent->child = child;
        while (child) {
            child->parent = parent;
            child = child->rightSibling;
        }
    }
    return parent;
}

static AST_NODE* makeFamily(AST_NODE *parent, int childrenCount, ...)
{
    va_list childrenList;
    va_start(childrenList, childrenCount);
    AST_NODE* child = va_arg(childrenList, AST_NODE*);
    makeChild(parent, child);
    AST_NODE* tmp = child;
    int index = 1;
    for (index = 1; index < childrenCount; ++index) {
        child = va_arg(childrenList, AST_NODE*);
        tmp = makeSibling(tmp, child);
    }
    va_end(childrenList);
    return parent;
}

static inline AST_NODE* makeIDNode(char *lexeme, IDENTIFIER_KIND idKind)
{
    AST_NODE* identifier = Allocate(IDENTIFIER_NODE);
    identifier->semantic_value.identifierSemanticValue.identifierName = lexeme;
    identifier->semantic_value.identifierSemanticValue.kind = idKind;
    identifier->semantic_value.identifierSemanticValue.symbolTableEntry = NULL;
    return identifier;                        
}

static inline AST_NODE* makeStmtNode(STMT_KIND stmtKind)
{
    AST_NODE* stmtNode = Allocate(STMT_NODE);
    stmtNode->semantic_value.stmtSemanticValue.kind = stmtKind;
    return stmtNode;                        
}

static inline AST_NODE* makeDeclNode(DECL_KIND declKind)
{
    AST_NODE* declNode = Allocate(DECLARATION_NODE);
    declNode->semantic_value.declSemanticValue.kind = declKind;
    return declNode;                        
}

static inline AST_NODE* makeExprNode(EXPR_KIND exprKind, int operationEnumValue)
{
    AST_NODE* exprNode = Allocate(EXPR_NODE);
    exprNode->semantic_value.exprSemanticValue.isConstEval = 0;
    exprNode->semantic_value.exprSemanticValue.kind = exprKind;
    if (exprKind == BINARY_OPERATION) {
        exprNode->semantic_value.exprSemanticValue.op.binaryOp = operationEnumValue;
    } else if (exprKind == UNARY_OPERATION) {
        exprNode->semantic_value.exprSemanticValue.op.unaryOp = operationEnumValue;
    } else {
        printf("Error in static inline AST_NODE* makeExprNode(EXPR_KIND exprKind, int operationEnumValue)\n");
    }
    return exprNode;                        
}

%}



%union{
	char *lexeme;
	CON_Type  *const1;
	AST_NODE  *node;
};

%token <lexeme>ID
%token <const1>CONST
%token VOID    
%token INT     
%token FLOAT   
%token IF      
%token ELSE    
%token WHILE   
%token FOR 
%token TYPEDEF 
%token OP_ASSIGN  
%token OP_OR   
%token OP_AND  
%token OP_NOT  
%token OP_EQ   
%token OP_NE   
%token OP_GT   
%token OP_LT   
%token OP_GE   
%token OP_LE   
%token OP_PLUS 
%token OP_MINUS        
%token OP_TIMES        
%token OP_DIVIDE       
%token MK_LB 
%token MK_RB 
%token MK_LPAREN       
%token MK_RPAREN       
%token MK_LBRACE       
%token MK_RBRACE       
%token MK_COMMA        
%token MK_SEMICOLON    
%token MK_DOT  
%token ERROR
%token RETURN

%type <node> program global_decl_list global_decl function_decl block stmt_list decl_list decl var_decl type init_id_list init_id  stmt relop_expr relop_term relop_factor expr term factor var_ref
%type <node> param_list param dim_fn expr_null id_list dim_decl cexpr mcexpr cfactor assign_expr_list test assign_expr rel_op relop_expr_list nonempty_relop_expr_list
%type <node> add_op mul_op dim_list type_decl nonempty_assign_expr_list


%start program

%%

/* ==== Grammar Section ==== */

/* Productions */               /* Semantic actions */
program		: global_decl_list 
		 			{ 
						printf("program: global_decl_list\n");
						$$=Allocate(PROGRAM_NODE);  
						makeChild($$,$1); 
						prog=$$;
					}
				| 
					{ 
						printf("Null program\n");
						$$=Allocate(PROGRAM_NODE); 
						prog=$$;
					}
				;

global_decl_list: global_decl_list global_decl 
                    {
						printf("global_decl_list: global_decl, recursive\n");
                        $$ = makeSibling($1, $2);
                    }	
                | global_decl
                    {
						printf("global_decl_list: global_decl\n");
                        $$ = $1;
                    }
                ; 

global_decl	: decl_list function_decl
                	{
						printf("global_decl: decl_list function_decl\n");
                    	$$ = makeSibling(makeChild(Allocate(VARIABLE_DECL_LIST_NODE), $1), $2);
               		}
            	| function_decl 
                	{
						printf("global_decl: function_decl\n");
                    	$$ = $1;
                	}
            ;

function_decl	: type ID MK_LPAREN param_list MK_RPAREN MK_LBRACE block MK_RBRACE     
                    {
//						printf("function_decl: type ID=%s param_list\n", $2);
                        $$ = makeDeclNode(FUNCTION_DECL);
                        AST_NODE* parameterList = Allocate(PARAM_LIST_NODE);
                        makeChild(parameterList, $4);
                        makeFamily($$, 4, $1, makeIDNode($2, NORMAL_ID), parameterList, $7);
                    }
                | VOID ID MK_LPAREN param_list MK_RPAREN MK_LBRACE block MK_RBRACE      
                    {
//						printf("function_decl: void ID=%s param_list\n", $2);
                        $$ = makeDeclNode(FUNCTION_DECL);
						AST_NODE* parameterList = Allocate(PARAM_LIST_NODE);
						makeChild(parameterList,$4);
						AST_NODE* type_n = makeIDNode("void", NORMAL_ID);
						makeFamily($$, 4, type_n, makeIDNode($2, NORMAL_ID), parameterList, $7);
						
						/*done*/
                    }
                | type ID MK_LPAREN MK_RPAREN MK_LBRACE block MK_RBRACE 
                    {
//						printf("function_decl: type ID=%s\n", $2);
                        $$ = makeDeclNode(FUNCTION_DECL);
                        AST_NODE* emptyParameterList = Allocate(PARAM_LIST_NODE);
                        makeFamily($$, 4, $1, makeIDNode($2, NORMAL_ID), emptyParameterList, $6);
                    }
                | VOID ID MK_LPAREN  MK_RPAREN MK_LBRACE block MK_RBRACE 
                    {
//						printf("function_decl: void ID=%s\n", $2);
						$$ = makeDeclNode(FUNCTION_DECL);
						AST_NODE* emptyParameterList = Allocate(PARAM_LIST_NODE);
						AST_NODE* type_n =  makeIDNode("void",NORMAL_ID);
						makeFamily($$, 4, type_n, makeIDNode($2, NORMAL_ID), emptyParameterList, $6);
						/*done*/
                    } 
                ;

param_list	: param_list MK_COMMA  param 
                {
//					printf("param_list: param_list param\n");
                    $$ = makeSibling($1, $3);
                }
            | param	
                {
//					printf("param_list: param\n");
                    $$ = $1 ;
					/*done*/
                }
            ;

param		: type ID 
                {
//					printf("param: type ID=%s\n", $2);
                    $$ = makeDeclNode(FUNCTION_PARAMETER_DECL);
                    makeFamily($$, 2, $1, makeIDNode($2, NORMAL_ID));
                }
            | type ID dim_fn 
                {
//					printf("param: type ID=%s dim_fn\n", $2);
                    $$ = makeDeclNode(FUNCTION_PARAMETER_DECL);
					AST_NODE* type_n = makeIDNode($2, NORMAL_ID);
					makeChild(type_n, $3);	//b[3][4] => b->3-->4
					makeFamily($$, 2, $1, type_n);
					/*done*/
                }
            ;
dim_fn		: MK_LB expr_null MK_RB	/* int i[] */ 
                {
//					printf("dim_fn: expr_null\n");
                    $$ = $2;
                }
            | dim_fn MK_LB expr MK_RB	/* int i[...][3] */
                {
//					printf("dim_fn: dim_fn expr\n");
                    $$ = makeSibling($1, $3);
                }
		;

expr_null	:expr 
                {		/* something like "3", "3*4", "1*3+5", but how about "i*j + k"? */
//					printf("expr_null: expr\n");
                 	$$ = $1;
					   /* done */
                }
            |
                {
//					printf("expr_null: NULL\n");
                    $$ = Allocate(NUL_NODE); 
                }
            ;

block           : decl_list stmt_list 
                    {
//						printf("block: decl_list stmt_list\n");
//						fflush(stdout);
                        $$ = Allocate(BLOCK_NODE);
						makeFamily($$, 2, makeChild(Allocate(VARIABLE_DECL_LIST_NODE), $1), makeChild(Allocate(STMT_LIST_NODE), $2));
						/*done*/
                    }
                | stmt_list  
                    {
//						printf("block: stmt_list\n");
//						fflush(stdout);
                        $$ = Allocate(BLOCK_NODE);
                        makeChild($$, makeChild(Allocate(STMT_LIST_NODE), $1));
                    }
                | decl_list 
                    {
//						printf("block: decl_list\n");
//						fflush(stdout);
                        $$ = Allocate(BLOCK_NODE);
                        makeChild($$, makeChild(Allocate(VARIABLE_DECL_LIST_NODE), $1));
                    }
                |
					{
//						printf("block: NULL\n");
//						fflush(stdout);
                        $$ = Allocate(BLOCK_NODE);
                        /*done*/
                    }
                ;
 
decl_list	: decl_list decl /* become sibiling*/
                {
//					printf("decl_list: decl_list decl\n");
//					fflush(stdout);
                    $$ = makeSibling($1, $2);
                    /*done*/
                }
            | decl	/* need a child*/ 
                {
//					printf("decl_list: decl\n");
//					fflush(stdout);
					$$ = $1;
                    /*done*/
                }
            ;

decl		: type_decl 
                {
                    $$ = $1;
                }
            | var_decl 
                {
//					printf("decl: var_decl\n");
//					fflush(stdout);
                    $$ = $1;
                }
            ;

type_decl 	: TYPEDEF type id_list MK_SEMICOLON 	/*need to check? */ 
                {
					$$ = makeDeclNode(TYPE_DECL);
					makeFamily($$, 2, $2, $3);
                    /*done*/
                }
            | TYPEDEF VOID id_list MK_SEMICOLON 
                {
					$$ = makeDeclNode(TYPE_DECL);
					AST_NODE* type_n = makeIDNode("void", NORMAL_ID);
					makeFamily($$, 2, type_n, $3);
                    /*done*/
                }
            ;

var_decl	: type init_id_list MK_SEMICOLON 
                {
//					printf("var_decl: type id_list\n");
//					fflush(stdout);
                	$$ = makeDeclNode(VARIABLE_DECL);
					makeFamily($$, 2, $1, $2);
                    /*done*/
                }
            | ID id_list MK_SEMICOLON		/*INT f[3][123]*/
                {
//					printf("var_decl: ID=%s id_list\n", $1);
                	$$ = makeDeclNode(VARIABLE_DECL);
					makeFamily($$, 2, makeIDNode($1, NORMAL_ID), $2);
                    /*done*/
                }
            ;

type		: INT 
                {
					fflush(stdout);
					$$ = makeIDNode("int", NORMAL_ID);  
                }
            | FLOAT 
                {
                    $$ = makeIDNode("float", NORMAL_ID);
                }
            ;

id_list		: ID 
                {
					$$ = makeIDNode($1, NORMAL_ID);
                }
            | id_list MK_COMMA ID 
                {
					$$ = makeSibling($1, makeIDNode($3, NORMAL_ID));
                    /*done*/
                }
            | id_list MK_COMMA ID dim_decl
                {
//					printf("id_list: id_list ID=%s dim_Decl\n", $3);
//					fflush(stdout);
					$$ = makeIDNode($3, ARRAY_ID);
					makeChild($$, $4);
					$$ = makeSibling($1, $$);
                    /*done*/
                }
            | ID dim_decl	/* a[3][4] */
                {
//					printf("id_list: ID=%s dim_decl\n", $1);
//					fflush(stdout);
					$$ = makeIDNode($1, ARRAY_ID);
					makeChild($$, $2);
                    /*done*/
                }
		;
dim_decl	: MK_LB cexpr MK_RB
                {
					$$ = $2;	
                    /*done*/
                } 
            | dim_decl MK_LB cexpr MK_RB
				{
                    $$ = makeSibling($1, $3);
					/*done*/
				}
            /*TODO: Try if you can define a recursive production rule
            | .......
            */
            ;
cexpr		: cexpr OP_PLUS mcexpr 
                {
                    $$ = makeExprNode(BINARY_OPERATION, BINARY_OP_ADD);
                    makeFamily($$, 2, $1, $3);
                } /* This is for array declarations */ 
            | cexpr OP_MINUS mcexpr
                {
                    $$ = makeExprNode(BINARY_OPERATION, BINARY_OP_SUB);
                    makeFamily($$, 2, $1, $3);
                    /*done*/
                } 
            | mcexpr 
                {
					$$ = $1;
                    /*done*/
                }
            ;  
mcexpr		: mcexpr OP_TIMES cfactor 
                {
                    $$ = makeExprNode(BINARY_OPERATION, BINARY_OP_MUL);
                    makeFamily($$, 2, $1, $3);
                    /*done*/
                }
            | mcexpr OP_DIVIDE cfactor 
                {
                    $$ = makeExprNode(BINARY_OPERATION, BINARY_OP_DIV);
                    makeFamily($$, 2, $1, $3);
                    /*done*/
                }
            | cfactor 
                {
					$$ = $1;
                    /*done*/
                }
            ;
        
cfactor:	CONST 
                {
                    $$ = Allocate(CONST_VALUE_NODE);
                    $$->semantic_value.const1=$1;
                    /*done*/
                }
            | MK_LPAREN cexpr MK_RPAREN 
                {
					$$ = $2;
                    /*done*/
                }
            ;

init_id_list	: init_id 
                    {
						$$ = $1;
                        /*done*/
                    }
                | init_id_list MK_COMMA init_id 
                    {
						$$ = makeSibling($1, $3);
                        /*done*/
                    }
                ;

init_id		: ID 
                {
                    $$ = makeIDNode($1, NORMAL_ID);
                }
            | ID dim_decl
                {
					$$ = makeIDNode($1, ARRAY_ID);
					makeChild($$, $2);
                    /*done*/
                }
            | ID OP_ASSIGN relop_expr 
                {
					$$ = makeIDNode($1, WITH_INIT_ID);
					makeChild($$, $3);
                    /*done*/
                }
            ;

stmt_list	: stmt_list stmt 
                {
//					printf(":stmt_list	->	stmt_list stmt	*** makeSibling\n");
					$$ = makeSibling($1, $2);	
                    /*done*/
                }
            | stmt
                {
//					printf("stmt_list	->	stmt\n\n\n\n\n\n");
					$$ = $1;
                    /*DONE*/
                }
            ;



stmt		: MK_LBRACE block MK_RBRACE 
                {
	                $$ = $2;
					/*DONE*/
                }
            /*TODO: | While Statement */
			| WHILE MK_LPAREN relop_expr MK_RPAREN stmt
            	{
					$$ = makeStmtNode(WHILE_STMT);
					makeFamily($$, 2, $3, $5);
					/*DONE*/

				}
			| FOR MK_LPAREN assign_expr_list MK_SEMICOLON relop_expr_list MK_SEMICOLON assign_expr_list MK_RPAREN stmt
                {
//					printf("stmt	->	FOR MK_LPAREN assign_expr_list MK_SEMICOLON relop_expr_list MK_SEMICOLON assign_expr_list MK_RPAREN stmt	***	makeStmt For Node\n");
					
                	$$ = makeStmtNode(FOR_STMT);
					makeFamily($$, 4 , $3, $5, $7, $9);
					/*DONE*/
                }
            | var_ref OP_ASSIGN relop_expr MK_SEMICOLON
                {
                	$$ = makeStmtNode(ASSIGN_STMT);
                	makeFamily($$, 2, $1, $3);
					/*DONE*/
                }
		    /*DONE: | If Statement */
			| IF MK_LPAREN relop_expr MK_RPAREN stmt
				{
					$$ = makeStmtNode(IF_STMT);
					makeFamily($$, 2, $3, $5);
				}
           	/*DONE: | If then else */
			| IF MK_LPAREN relop_expr MK_RPAREN stmt ELSE stmt
				{
//					printf("stmt	->	IF MK_LPAREN relop_expr MK_RPAREN stmt ELSE stmt\n");
					$$ = makeStmtNode(IF_STMT);
					makeFamily($$, 3, $3, $5, $7);					
				}
			| relop_expr MK_SEMICOLON
				{
//					printf("stmt	->	relop_expr MK_SEMICOLON\n");
					$$ = $1;
        		    /*DONE: | function call */

				}
            | MK_SEMICOLON 
                {
//					printf("stmt	->	MK_SEMICOLON	***	Allocate NUL_NODE\n");
					$$ = Allocate(NUL_NODE);	
                    /*DONE*/
                }
            | RETURN MK_SEMICOLON  
                {
                    $$ = makeStmtNode(RETURN_STMT);
					/*DONE*/
                }
            | RETURN relop_expr MK_SEMICOLON
                {
                	$$ = makeStmtNode(RETURN_STMT);
					makeFamily($$, 1, $2);
					/*DONE*/
                }
            ;

assign_expr_list : nonempty_assign_expr_list 
                     {
//						printf("assign_expr_list	->	 nonempty_assign_expr_list\n");                    
						$$ = $1;
						/*DONE*/
                     }
                 |  
                     {
//						printf("\n\n\n\n\n\n\nassign_expr_list	->	NULL	*** allocate NUL_NODE\n");
                         $$ = Allocate(NUL_NODE); 
                     }
                 ;

nonempty_assign_expr_list        : nonempty_assign_expr_list MK_COMMA assign_expr 
                                    {
//										printf("nonempty_assign_expr_list	->	nonempty_assign_expr_list MK_COMMA assign_expr\n");
	                                    $$ = makeChild($1, $3);
										/*TODO*/
                                    }
                                 | assign_expr
                                    {
//										printf("nonempty_assign_expr_list	->	 assign_expr	*** Allocate Node makechild\n");
	                                    $$ = Allocate(NONEMPTY_ASSIGN_EXPR_LIST_NODE);
										makeChild($$, $1);
										/*DONE*/
                                    }
                                 ;

test		: assign_expr
                {
                    $$ = $1;
                }
            ;

assign_expr     : ID OP_ASSIGN relop_expr 
                    {
//						printf("assign_expr	->	ID OP_ASSIGN relop_expr %s		*** makeIDNode makeStmtNode\n",$1);
                		AST_NODE* id = makeIDNode($1, NORMAL_ID);
						$$ = makeStmtNode(ASSIGN_STMT);
						makeFamily($$,2,id,$3);
						/*DONE*/

					}
                | relop_expr
                    {
//						printf("relop_expr");
                        /*TODO*/
                    }
		;

relop_expr	: relop_term 
                {
//					printf("relop_expr	->	relop_term\n");
					$$ = $1;
                }
            | relop_expr OP_OR relop_term
                {
//					printf("relop_expr	->	relop_expr	OP_OR	relop_term\n");
                    $$ = makeExprNode(BINARY_OPERATION, BINARY_OP_OR);
                    makeFamily($$, 2, $1, $3);
                }
            ;

relop_term	: relop_factor 
                {
//					printf("relop_term	->	relop_factor\n");
					$$ = $1;
                    /*DONE*/
                }
            | relop_term OP_AND relop_factor
                {
//					printf("relop_term	->	relop_term OP_AND relop_factor\n");
                    $$ = makeExprNode(BINARY_OPERATION, BINARY_OP_OR);
					makeFamily($$, 2, $1, $3);
					/*TODO*/
                }
            ;

relop_factor	: expr
                    {
//						printf("relop_factor	->	expr\n");
                    	$$ = $1;
						/*DONE*/
                    }
                | expr rel_op expr 
                    {
  //          			printf("relop_factor	->	expr rel_op expr\n"); 
						$$ = makeFamily($2, 2, $1, $3); 
						/*DONE*/
                    }
                ;

rel_op		: OP_EQ
                {
                    $$ = makeExprNode(BINARY_OPERATION, BINARY_OP_EQ);
                    /*DONE*/
                }
            | OP_GE 
                {
                    $$ = makeExprNode(BINARY_OPERATION, BINARY_OP_GE);
                    /*DONE*/
                }
            | OP_LE 
                {
	//				printf("rel_op	->	OP_LE	***	makeExprNode\n");
                    $$ = makeExprNode(BINARY_OPERATION, BINARY_OP_LE);
                    /*DONE*/
                }
            | OP_NE 
                {
                    $$ = makeExprNode(BINARY_OPERATION, BINARY_OP_NE);
                    /*DONE*/
                }
            | OP_GT 
                {
                    $$ = makeExprNode(BINARY_OPERATION, BINARY_OP_GT);
                    /*DONE*/
                }
            | OP_LT 
                {
	//				printf("\n\n\n\nrel_op	->	OP_LT	*** makeExprNode\n");
                    $$ = makeExprNode(BINARY_OPERATION, BINARY_OP_LT);						
                    /*DONE*/
                }
            ;


relop_expr_list	: nonempty_relop_expr_list 
                    {
	//					printf("relop_expr_list	->	nonempty_relop_expr_list\n");
						$$ = $1;
						/*DONE*/
                    }
                | 
                    {
	//					printf("\n\n\nrelop_expr_list	->	nonempty_relop_expr_list *** allocate nul_node\n"); 						
                        $$ = Allocate(NUL_NODE);
                    }
                ;

nonempty_relop_expr_list	: nonempty_relop_expr_list MK_COMMA relop_expr
                                {
	//								printf("nonempty_relop_expr_list	->	nonempty_relop_expr_list MK_COMMA relop_expr\n");
							
                                    $$ = makeChild($1, $3);
									/*TODO*/
                                }
                            | relop_expr 
                                {
	//								printf("nonempty_relop_expr_list	->	relop_expr	Make NONEMPTY_NODE Makechild\n");
                                    $$ = Allocate(NONEMPTY_RELOP_EXPR_LIST_NODE);
									makeChild($$, $1);
									/*DONE*/
                                }
                            ;

expr		: expr add_op term 
                {
	//				printf("expr	->	expr add_op term\n");
					$$ = makeFamily($2, 2, $1, $3);
                    /*TODO*/
                }
            | term 
                {
	//				printf("expr	->	term\n");
					$$ = $1;
                    /*DONE*/
                }
            ;

add_op		: OP_PLUS
                {
                    $$ = makeExprNode(BINARY_OPERATION, BINARY_OP_ADD);
                }
            | OP_MINUS 
                {
                    $$ = makeExprNode(BINARY_OPERATION, BINARY_OP_SUB);
                }
            ;

term		: term mul_op factor
                {
	//				printf("term	->	term mul_op factor\n");
                	$$ = makeFamily($2, 2, $1, $3);
					/*TODO*/
                }
            | factor
                {
	//				printf("term	->	factor\n");
					$$ = $1;
                    /*DONE*/
                }
            ;

mul_op		: OP_TIMES
                {
                    $$ = makeExprNode(BINARY_OPERATION, BINARY_OP_MUL);
                    /*TODO*/
               }
            | OP_DIVIDE 
                {
                    $$ = makeExprNode(BINARY_OPERATION, BINARY_OP_DIV);
                    /*TODO*/
                }
            ;


factor		: MK_LPAREN relop_expr MK_RPAREN
                {
                    /*TODO*/
					$$ = $2;
                }
            /*TODO: | -(<relop_expr>) e.g. -(4) */
            | OP_MINUS MK_LPAREN relop_expr MK_RPAREN
                {   
                    
					$$ = makeExprNode(UNARY_OPERATION,UNARY_OP_NEGATIVE);
               		makeChild($$,$3); 
      //           	printf("factor	->	OP_MINUS MK_LRAREN\n");   
                }
            | OP_NOT MK_LPAREN relop_expr MK_RPAREN
                {   
					$$ = makeExprNode(UNARY_OPERATION,UNARY_OP_LOGICAL_NEGATION);
					makeChild($$,$3);
                    /*TODO*/
                }
            | CONST 
                {
                    $$ = Allocate(CONST_VALUE_NODE);
                    $$->semantic_value.const1=$1;
                }
            /*TODO: | -<constant> e.g. -4 */
            | OP_MINUS CONST
                {
        //         	printf("factor	->	OP_MINUS CONST\n");
					$$ = makeExprNode(UNARY_OPERATION,UNARY_OP_NEGATIVE);
					AST_NODE* newnode = Allocate(CONST_VALUE_NODE);
					newnode->semantic_value.const1 = $2;
					makeChild($$,newnode);

					/*TODO*/
                }
            | OP_NOT CONST
                {
		//			printf("factor	->	OP_NOT CONST");
					$$ = makeExprNode(UNARY_OPERATION,UNARY_OP_LOGICAL_NEGATION);
					AST_NODE* newnode = Allocate(CONST_VALUE_NODE);
					newnode->semantic_value.const1 = $2;
					makeChild($$,newnode);

                    /*TODO*/
                }
            | ID MK_LPAREN relop_expr_list MK_RPAREN 
                {
					$$ = makeStmtNode(FUNCTION_CALL_STMT);
					AST_NODE* newnode = makeIDNode($1,NORMAL_ID);
					makeFamily($$, 2, newnode,$3);
					
                    /*TODO*/
                }
            /*TODO: | -<function call> e.g. -f(4) */
            | OP_MINUS ID MK_LPAREN relop_expr_list MK_RPAREN
                {
					$$ = makeExprNode(UNARY_OPERATION,UNARY_OP_NEGATIVE);
					AST_NODE* stnode =	makeStmtNode(FUNCTION_CALL_STMT);
					AST_NODE* newnode = makeIDNode($2,NORMAL_ID);
					makeChild($$, stnode);
					makeFamily(stnode, 2, newnode,$4);

                    /*TODO*/
                }
			| OP_NOT ID MK_LPAREN relop_expr_list MK_RPAREN
                {
				
					$$ = makeExprNode(UNARY_OPERATION,UNARY_OP_LOGICAL_NEGATION);
					AST_NODE* stnode =	makeStmtNode(FUNCTION_CALL_STMT);
					AST_NODE* newnode = makeIDNode($2,NORMAL_ID);
					makeChild($$, stnode);
					makeFamily(stnode, 2, newnode,$4);


					/*TODO*/
                }
            | var_ref 
                {
					$$ = $1;
                    /*TODO*/
                }
            /*TODO: | -<var_ref> e.g. -var */
            | OP_MINUS var_ref 
                {
		//			printf("factor	->	OP_MINUS var_ref\n");
                    $$ = makeExprNode(UNARY_OPERATION,UNARY_OP_NEGATIVE);
					makeChild($$,$2);		
					/*TODO*/
                }
			| OP_NOT var_ref 
                {
					$$ = makeExprNode(UNARY_OPERATION,UNARY_OP_LOGICAL_NEGATION);
					makeChild($$,$2);
                    /*TODO*/
                }
            ;
var_ref		: ID 
                {
		//			printf("var_ref	->	ID\n");
                    $$ = makeIDNode($1,NORMAL_ID); 
					/*DONE*/					
                }
            | ID dim_list 
                {
                    $$ = makeIDNode($1, ARRAY_ID);
					makeChild($$, $2);
					/*TODO*/
                }
            ;


dim_list	: dim_list MK_LB expr MK_RB 
                {
                    $$ = makeSibling($1, $3);
					/*TODO*/
                }
            | MK_LB expr MK_RB
                {
                    $$ = $2;
					/*TODO*/
                }
		;


%%

#include "lex.yy.c"
main (argc, argv)
int argc;
char *argv[];
  {
     yyin = fopen(argv[1],"r");
     yyparse();
	 printf("%s\n", "Parsing completed. No errors found.");
	 printGV(prog, NULL);
  } /* main */


int yyerror (mesg)
char *mesg;
  {
  printf("%s\t%d\t%s\t%s\n", "Error found in Line ", linenumber, "next token: ", yytext );
  exit(1);
  }
