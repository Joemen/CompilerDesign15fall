#ifndef HEADER_H_INCLUDED
#define HEADER_H_INCLUDED
#define LEN 256
/******************************************************************************************************************************************
    All enumeration literals
       TokenType : Specify the type of the token scanner returns
	   DataType  : The data type of the declared variable
	   StmtType  : Indicate one statement in AcDc program is print or assignment statement.
	   ValueType : The node types of the expression tree that represents the expression on the right hand side of the assignment statement.
	               Identifier, IntConst, FloatConst must be the leaf nodes ex: a, b, c , 1.5 , 3.
				   PlusNode, MinusNode, MulNode, DivNode are the operations in AcDc. They must be the internal nodes.
                   Note that IntToFloatConvertNode to represent the type coercion may appear after finishing type checking. 			  
	   Operation : Specify all arithematic expression, including +, - , *, / and type coercion.
	Prog  -> Dcls Stmts $
	Dcls  -> Dcl	Dcls | lamda
	Dcl   -> floatdcl id | intdcl id
    Stmts -> Stmt Stmts | lamda
    Stmt  -> id assign Val Expr | print id
	Expr  -> ( Plus | Minus ) Val Term Expr |
								  Term Expr
	Term  -> ( Mul  | Div   ) Val Term | lamda	
	Val   -> id | inum | fnum
	
*******************************************************************************************************************************************/

typedef enum TokenType { FloatDeclaration, IntegerDeclaration, PrintOp, AssignmentOp, PlusOp, MinusOp,
             MulOp, DivOp, Alphabet, IntValue, FloatValue, EOFsymbol } TokenType;
typedef enum DataType { Int, Float, Notype }DataType;
typedef enum StmtType { Print, Assignment } StmtType;
typedef enum ValueType { Identifier, IntConst, FloatConst, PlusNode, MinusNode, MulNode, DivNode, IntToFloatConvertNode } ValueType;
typedef enum Operation { Plus, Minus, Mul, Div, Assign, IntToFloatConvert } Operation;


/**************************************************************************************** 
   All structures to facilitate the processes of 
   scanning, parsing, AST, type-checking, building the symbol table, and code generation.
*****************************************************************************************/


/* For scanner */
typedef struct Token{
    TokenType type;
    char tok[LEN+1];
}Token;

/*** The following are nodes of the AST. ***/

/* For decl production or say one declaration statement */
typedef struct Declaration{
    DataType type;
    char name[LEN+1];
}Declaration;

/* 
    For decls production or say all declarations. (
	You can view it as the subtree for decls in AST,
	or just view it as the linked list that stores 
	all declarations. ) 
*/
typedef struct Declarations{
    Declaration first;
    struct Declarations *rest;
}Declarations;

/* For the nodes of the expression on the right hand side of one assignment statement */
typedef struct Value{
    ValueType type;
    union{
        char id[LEN+1];            /* if the node represent the access of the identifier */
        Operation op;              /* store +, -, *, /, =, type_convert */
        int ivalue;                /* for integer constant in the expression */
        float fvalue;              /* for float constant */
    }val;
}Value;


/* 
   The data structure of the expression tree.
   Recall how to deal with expression by tree 
   in data structure course.   
*/
typedef struct Expression{	// expression -> ( plus | minus ) value expression
    Value v;						
    struct Expression *leftOperand;	
    struct Expression *rightOperand;	
    DataType type;					//	Int or Float
}Expression;


/* For one assignment statement */
typedef struct AssignmentStatement{
    char id[LEN+1];		//	left side of the equation
    Expression *expr;	
    DataType type;      /* For type checking to store the type of all expression on the right. */// float or int
}AssignmentStatement;


/* For stmt production or say one statement*/
typedef struct Statement{		// print or assign
    StmtType type;
    union{
        char variable[LEN+1];              /* print statement */
        AssignmentStatement assign;
    }stmt;
}Statement;

/* For stmts production or say all productions */
typedef struct Statements{
    struct Statement first;
    struct Statements *rest;
}Statements;

/* For the root of the AST. */
typedef struct Program{
    Declarations *declarations;
    Statements *statements;
}Program;

/* For building the symbol table */
typedef struct SymbolTable{
    DataType table[26];
	char name[26][LEN + 1];
    int count;
} SymbolTable;


Token getNumericToken( FILE *source, char c );
Token scanner( FILE *source );
Declaration makeDeclarationNode( Token declare_type, Token identifier );
Declarations *makeDeclarationTree( Declaration decl, Declarations *decls );
Declaration parseDeclaration( FILE *source, Token token );
Declarations *parseDeclarations( FILE *source );
Expression *parseValue( FILE *source );
Expression *parseExpression( FILE *source, Expression *lvalue );
Statement makeAssignmentNode( char *id, Expression *v, Expression *expr_tail );
Statement makePrintNode( char *id );
Statements *makeStatementTree( Statement stmt, Statements *stmts );
Statement parseStatement( FILE *source, Token token );
Statements *parseStatements( FILE * source );
Program parser( FILE *source );
void InitializeTable( SymbolTable *table );
void add_table( SymbolTable *table, char *s, DataType t );
SymbolTable build( Program program );
void convertType( Expression * old, DataType type );
DataType generalize( Expression *left, Expression *right );
DataType lookup_table( SymbolTable *table, char *s );
void checkexpression( Expression * expr, SymbolTable * table );
void checkstmt( Statement *stmt, SymbolTable * table );
void check( Program *program, SymbolTable * table);
void fprint_op( FILE *target, ValueType op );
void fprint_expr( FILE *target, Expression *expr, SymbolTable *table);
void gencode( Program prog, FILE * target, SymbolTable *table );
Expression *parsePosValue(Token token);
Expression *parseTerm(FILE *source, Expression *lvalue);
void print_expr( Expression *expr );
void test_parser( FILE *source );
Expression *parseTerm(FILE *source, Expression *lvalue);
#endif // HEADER_H_INCLUDED
