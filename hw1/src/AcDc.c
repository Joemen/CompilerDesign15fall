#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "header.h"

int main( int argc, char *argv[] )
{
    FILE *source, *target;	//  source -> .ac file 	target -> .dc file
    Program program;		//	root of the Syntax Tree
    SymbolTable symtab;		//  SymbolTable

    if( argc == 3){
        source = fopen(argv[1], "r");
        target = fopen(argv[2], "w");
        if( !source ){
            printf("can't open the source file\n");
            exit(2);
        }
        else if( !target ){
            printf("can't open the target file\n");
            exit(2);
        }
        else{
            program = parser(source);
            fclose(source);
            symtab = build(program);
            check(&program, &symtab);	//check type float or int
            gencode(program, target,&symtab);
        }
    }
    else{
        printf("Usage: %s source_file target_file\n", argv[0]);
    }


    return 0;
}

/********************************************* 
  Scanning 
 *********************************************/
 //	 support both integer and floating points
Token getNumericToken( FILE *source, char c )
{
    Token token;
    int i = 0;

    while( isdigit(c) ) {
        token.tok[i++] = c;
        c = fgetc(source);
    }

    if( c != '.' ){
        ungetc(c, source);
        token.tok[i] = '\0';
        token.type = IntValue;
        return token;
    }

    token.tok[i++] = '.';

    c = fgetc(source);
    if( !isdigit(c) ){
        ungetc(c, source);
        printf("Expect a digit : %c\n", c);
        exit(1);
    }

    while( isdigit(c) ){
        token.tok[i++] = c;
        c = fgetc(source);
    }

    ungetc(c, source);
    token.tok[i] = '\0';
    token.type = FloatValue;
    return token;
}
Token getAlphabetToken(Token token,FILE *source, char c )
{
	int i = 1;
    while (islower(c = fgetc(source))) {
        token.tok[i++] = c;
    }
    ungetc(c, source);
    token.tok[i] = '\0';
    return token;
}
Token scanner( FILE *source )
{
    char c;
    Token token;

    while( !feof(source) ){
        c = fgetc(source);
		
        while( isspace(c) ) c = fgetc(source);
		if( isdigit(c) )
            return getNumericToken(source, c);
		token.tok[0] = c;
        token.tok[1] = '\0';
        if( islower(c) ){
			char next= fgetc(source);	// see next char
        	if( c == 'f' && isspace(next))
                token.type = FloatDeclaration;
            else if( c == 'i' && isspace(next))
                token.type = IntegerDeclaration;
            else if( c == 'p' && isspace(next)){
                token.type = PrintOp;
				strcat(token.tok, " "); 
			}
            else{
				ungetc(next, source);
				token =  getAlphabetToken(token, source, c);
				token.type = Alphabet;	
			}
			return token;
        }

        switch(c){
            case '=':
                token.type = AssignmentOp;
                return token;
            case '+':
                token.type = PlusOp;
                return token;
            case '-':
                token.type = MinusOp;
                return token;
            case '*':
                token.type = MulOp;
                return token;
            case '/':
                token.type = DivOp;
                return token;
            case EOF:
                token.type = EOFsymbol;
                token.tok[0] = '\0';
                return token;
            default:
                printf("Invalid character : %c\n", c);
                exit(1);
        }
    }

    token.tok[0] = '\0';
    token.type = EOFsymbol;
    return token;
}

/********************************************************
  Parsing
 *********************************************************/
// token  -> type of variable  
// token2 -> name of variable

Declaration parseDeclaration( FILE *source, Token token )
{
    Token token2;
    switch(token.type){
        case FloatDeclaration:
        case IntegerDeclaration:
            token2 = scanner(source);			
            if (strcmp(token2.tok, "f") == 0 ||
                    strcmp(token2.tok, "i") == 0 ||
                    strcmp(token2.tok, "p") == 0) {
                printf("Syntax Error: %s cannot be used as id\n", token2.tok);
                exit(1);
            }
			else if(token2.type == IntValue || token2.type == FloatValue){
				printf("Syntax Error: %s cannot be used as id\n", token2.tok);
                exit(1);
			}
            return makeDeclarationNode( token, token2 );
        default:
            printf("Syntax Error: Expect Declaration %s\n", token.tok);
            exit(1);
    }
}

//	Declarations -> Decl | Decls
Declarations *parseDeclarations( FILE *source )
{
    Token token = scanner(source);
	int i;
    Declaration decl;
    Declarations *decls;
    switch(token.type){
        case FloatDeclaration:
        case IntegerDeclaration:
            decl = parseDeclaration(source, token);
            decls = parseDeclarations(source);
            return makeDeclarationTree( decl, decls );
        case PrintOp:
        case Alphabet:
			for (i = strlen(token.tok) - 1; i >= 0; i--) {
                ungetc(token.tok[i], source);
            }
            return NULL;
        case EOFsymbol:
            return NULL;
        default:
            printf("Syntax Error: Expect declarations %s\n", token.tok);
            exit(1);
    }
}

Expression *parseValue( FILE *source )	// manage token type and value
{
    Token token = scanner(source);
    Expression *value = (Expression *)malloc( sizeof(Expression) );
    value->leftOperand = value->rightOperand = NULL;

    switch(token.type){
        case Alphabet:
            (value->v).type = Identifier;
            strcpy((value->v).val.id , token.tok);
            break;
        case IntValue:
            (value->v).type = IntConst;
            (value->v).val.ivalue = atoi(token.tok);
            break;
        case FloatValue:
            (value->v).type = FloatConst;
            (value->v).val.fvalue = atof(token.tok);
            break;
        default:
            printf("Syntax Error: Expect Identifier or a Number %s\n", token.tok);
            exit(1);
    }

    return value;
}
Expression *parseTerm(FILE *source, Expression *lvalue)
{
    Token token = scanner(source);
    Expression *expr;
	int i ;
    switch (token.type){
        case PlusOp:
        case MinusOp:
        case Alphabet:
        case PrintOp:
            for (i = strlen(token.tok) - 1; i >= 0; i--) {
                ungetc(token.tok[i], source);
            }
            return lvalue;
		case MulOp:
            expr = (Expression *)malloc( sizeof(Expression) );
            (expr->v).type = MulNode;
            (expr->v).val.op = Mul;
            expr->leftOperand = lvalue;
            expr->rightOperand = parseValue(source);
            return parseTerm(source, expr);
        case DivOp:
            expr = (Expression *)malloc( sizeof(Expression) );
            (expr->v).type = DivNode;
            (expr->v).val.op = Div;
            expr->leftOperand = lvalue;
            expr->rightOperand = parseValue(source);
            return parseTerm(source, expr);
        default:
            printf("In function parseTerm\n");
            printf("Syntax Error: Expect a numeric value or an identifier %s\n", token.tok);
            exit(1);
    }

}

Expression *parseExpression( FILE *source, Expression *lvalue )
{
    Token token = scanner(source);
	int i;
    Expression *expr;
	Expression *value;

    switch(token.type){
        case PlusOp:
            expr = (Expression *)malloc( sizeof(Expression) );
            (expr->v).type = PlusNode;
            (expr->v).val.op = Plus;
            expr->leftOperand = lvalue;
			value = parseValue(source);
            expr->rightOperand = parseTerm(source,value);
            return parseExpression(source, expr);
        case MinusOp:
            expr = (Expression *)malloc( sizeof(Expression) );
            (expr->v).type = MinusNode;
            (expr->v).val.op = Minus;
            expr->leftOperand = lvalue;
			value = parseValue(source);
            expr->rightOperand = parseTerm(source,value);
            return parseExpression(source, expr);
        case Alphabet:
        case PrintOp:
            for (i = strlen(token.tok) - 1; i >= 0; i--) {
                ungetc(token.tok[i], source);
            }
            return lvalue;
        case EOFsymbol:
            return lvalue;
        default:
            printf("Syntax Error: Expect a numeric value or an identifier %s\n", token.tok);
            exit(1);
    }
}

Statement parseStatement( FILE *source, Token token )
{
    Token next_token;
    Expression *value, *term,*expr;

    switch(token.type){
        case Alphabet:
            next_token = scanner(source);
            if(next_token.type == AssignmentOp){
                value = parseValue(source);
                term = parseTerm(source, value);
                expr = parseExpression(source, term);
                return makeAssignmentNode(token.tok, value, expr);
            }
            else{
                printf("Syntax Error: Expect an assignment op %s\n", next_token.tok);
                exit(1);
            }
        case PrintOp:
            next_token = scanner(source);
            if(next_token.type == Alphabet)
                return makePrintNode(next_token.tok);
            else{
                printf("Syntax Error: Expect an identifier %s\n", next_token.tok);
                exit(1);
            }
            break;
        default:
            printf("Syntax Error: Expect a statement %s\n", token.tok);
            exit(1);
    }
}

Statements *parseStatements( FILE * source )
{

    Token token = scanner(source);
    Statement stmt;
    Statements *stmts;

    switch(token.type){
        case Alphabet:
        case PrintOp:
            stmt = parseStatement(source, token);
            stmts = parseStatements(source);
            return makeStatementTree(stmt , stmts);
        case EOFsymbol:
            return NULL;
        default:
            printf("Syntax Error: Expect statements %s\n", token.tok);
            exit(1);
    }
}


/*********************************************************************
  Build AST
 **********************************************************************/
Declaration makeDeclarationNode( Token declare_type, Token identifier )
{
    Declaration tree_node;

    switch(declare_type.type){
        case FloatDeclaration:
            tree_node.type = Float;
            break;
        case IntegerDeclaration:
            tree_node.type = Int;
            break;
        default:
            break;
    }
    strcpy(tree_node.name , identifier.tok);

    return tree_node;
}

Declarations *makeDeclarationTree( Declaration decl, Declarations *decls )
{
    Declarations *new_tree = (Declarations *)malloc( sizeof(Declarations) );
    new_tree->first = decl;
    new_tree->rest = decls;

    return new_tree;
}


Statement makeAssignmentNode( char *id, Expression *v, Expression *expr_tail )
{
    Statement stmt;
    AssignmentStatement assign;

    stmt.type = Assignment;
    strcpy(assign.id, id);
    if(expr_tail == NULL)
        assign.expr = v;
    else
        assign.expr = expr_tail;
    stmt.stmt.assign = assign;

    return stmt;
}

Statement makePrintNode( char *id )
{
    Statement stmt;
    stmt.type = Print;
    strcpy(stmt.stmt.variable , id);

    return stmt;
}

Statements *makeStatementTree( Statement stmt, Statements *stmts )
{
    Statements *new_tree = (Statements *)malloc( sizeof(Statements) );
    new_tree->first = stmt;
    new_tree->rest = stmts;

    return new_tree;
}

/* parser */
Program parser( FILE *source )
{
    Program program;

    program.declarations = parseDeclarations(source);
    program.statements = parseStatements(source);

    return program;
}


/********************************************************
  Build symbol table
 *********************************************************/
void InitializeTable( SymbolTable *table )
{
    int i;

    for(i = 0 ; i < 26; i++){
        table->table[i] = Notype;
		table->name[i][0]='\0';
	}
	table->count = 0 ;
}

void add_table( SymbolTable *table, char *s, DataType t )
{
    int i;
    for (i = 0; i < table->count; i++) {
		if(strcmp(s, table->name[i]) == 0) {
            printf("Error : id %s has been declared\n", s);//error
            //exit(1);
        }
    }
    table->count++;
    strcpy(table->name[i], s);
	table->table[i] = t;
}

SymbolTable build( Program program )
{
    SymbolTable table;
    Declarations *decls = program.declarations;
    Declaration current;

    InitializeTable(&table);

    while(decls !=NULL){
        current = decls->first;
        add_table(&table, current.name, current.type);
        decls = decls->rest;
    }

    return table;
}


/********************************************************************
  Type checking
 *********************************************************************/

void convertType( Expression * old, DataType type )
{
    if(old->type == Float && type == Int){
        printf("error : can't convert float to integer\n");
        return;
    }
    if(old->type == Int && type == Float){
        Expression *tmp = (Expression *)malloc( sizeof(Expression) );
        if(old->v.type == Identifier)
            printf("convert to float %s \n",old->v.val.id);
        else
            printf("convert to float %d \n", old->v.val.ivalue);
        tmp->v = old->v;
        tmp->leftOperand = old->leftOperand;
        tmp->rightOperand = old->rightOperand;
        tmp->type = old->type;

        Value v;
        v.type = IntToFloatConvertNode;
        v.val.op = IntToFloatConvert;
        old->v = v;
        old->type = Int;
        old->leftOperand = tmp;
        old->rightOperand = NULL;
    }
}

DataType generalize( Expression *left, Expression *right )
{
    if(left->type == Float || right->type == Float){
        printf("generalize : float\n");
        return Float;
    }
    printf("generalize : int\n");
    return Int;
}

DataType lookup_table( SymbolTable *table, char *s )
{
    
    int i;
    for (i = 0; i < table->count; i++) {
        if(strcmp(s, table->name[i]) == 0) {
            return table->table[i];
        }
    }
    printf("Error : identifier %s is not declared\n", s);//error
    return Notype;
    //exit(1);
}

void checkexpression( Expression * expr, SymbolTable * table )
{
    char *c;
    if(expr->leftOperand == NULL && expr->rightOperand == NULL){
        switch(expr->v.type){
            case Identifier:
                c = expr->v.val.id;
                printf("identifier : %s\n",c);
                expr->type = lookup_table(table, c);
                break;
            case IntConst:
                printf("constant : int\n");
                expr->type = Int;
                break;
            case FloatConst:
                printf("constant : float\n");
                expr->type = Float;
                break;
                //case PlusNode: case MinusNode: case MulNode: case DivNode:
            default:
                break;
        }
    }
    else if (expr->rightOperand == NULL){
        checkexpression(expr->leftOperand, table);
        expr->type = expr->leftOperand->type;
    }
	else{
        Expression *left = expr->leftOperand;
        Expression *right = expr->rightOperand;

        checkexpression(left, table);
        checkexpression(right, table);

        DataType type = generalize(left, right);
        convertType(left, type);//left->type = type;//converto
        convertType(right, type);//right->type = type;//converto
        expr->type = type;
		//	constant folding
		switch (expr->v.type) {
            case PlusNode: case MinusNode: case MulNode: case DivNode:
			    if (type == Int && left->v.type == IntConst && right->v.type == IntConst) {
                    int *a = &left->v.val.ivalue;
					int *b = &right->v.val.ivalue; 
                    int *c = &expr->v.val.ivalue;
					char op;
                    // c = a <op> b
                    switch (expr->v.type) {
                        case PlusNode:
							*c = *a + *b; 
							op = '+'; 
							break;
                        case MinusNode: 
							*c = *a - *b; 
							op = '-'; 
							break;
                        case MulNode:   
							*c = *a * *b; 
							op = '*'; 
							break;
                        case DivNode:   
							*c = *a / *b; 
							op = '/'; 
							break;
                        default: 
							break;
                    }
                    expr->v.type = IntConst;
                    expr->leftOperand = expr->rightOperand = NULL;
                    printf("After constant folding: %d %c %d = %d\n", *a, op, *b, *c);
                }
                else if (type == Float && left->v.type == FloatConst && right->v.type == FloatConst) {
                    float *a = &left->v.val.fvalue;
					float *b = &right->v.val.fvalue; // c = a <op> b
                    float *c = &expr->v.val.fvalue;
                    char op;
                    switch (expr->v.type) {
                        case PlusNode:
							*c = *a + *b; 
							op = '+'; 
							break;
                        case MinusNode: 
							*c = *a - *b; 
							op = '-'; 
							break;
                        case MulNode:   
							*c = *a * *b; 
							op = '*'; 
							break;
                        case DivNode:   
							*c = *a / *b; 
							op = '/'; 
							break;
                        default: 
							break;
                    }
                    expr->v.type = FloatConst;
                    expr->leftOperand = expr->rightOperand = NULL;
                    printf("After constant folding: %f %c %f = %f\n", *a, op, *b, *c);
                }
                break;
            default:
                break;
        }
    }
}
void checkstmt( Statement *stmt, SymbolTable * table )
{
   
	if(stmt->type == Assignment){
		//printf("test1 %d\n",stmt->type);
        AssignmentStatement assign = stmt->stmt.assign;
		//printf("test2 %d\n",stmt->type);
        printf("assignment : %s \n",assign.id);
        checkexpression(assign.expr, table);
        stmt->stmt.assign.type = lookup_table(table, assign.id);
        if (assign.expr->type == Float && stmt->stmt.assign.type == Int) {
            printf("error : can't convert float to integer\n");
        } else {
            convertType(assign.expr, stmt->stmt.assign.type);
        }
    }
    else if (stmt->type == Print){
        printf("print : %s \n",stmt->stmt.variable);
        lookup_table(table, stmt->stmt.variable);
    }
    else printf("error : statement error\n");//error
}

void check( Program *program, SymbolTable * table )
{
    Statements *stmts = program->statements;
    while(stmts != NULL){
        checkstmt(&stmts->first,table);		
        stmts = stmts->rest;
    }
	
}


/***********************************************************************
  Code generation
 ************************************************************************/
void fprint_op( FILE *target, ValueType op )
{
    switch(op){
        case MinusNode:
            fprintf(target,"-\n");
            break;
        case PlusNode:
            fprintf(target,"+\n");
            break;
		case MulNode:
            fprintf(target,"*\n");
            break;
		case DivNode:
            fprintf(target,"/\n");
            break;
        default:
            fprintf(target,"Error in fprintf_op ValueType = %d\n",op);
            break;
    }
}
char find_register(char *s, SymbolTable *table){
    int i;
	for ( i = 0; i < table->count; i++) {
        if (strcmp(s, table->name[i]) == 0) {
            return (char)(i + 'a');
        }
    }
    printf("Error, can not find variable %s", s);
    exit(1);
}
void fprint_expr( FILE *target, Expression *expr, SymbolTable *table)
{

    if(expr->leftOperand == NULL){
        char find_reg ;
		switch( (expr->v).type ){
            case Identifier:
			    find_reg = find_register((expr->v).val.id,table);
                fprintf(target,"l%c\n",find_reg);
                break;
            case IntConst:
				if((expr->v).val.ivalue < 0)
					fprintf(target,"_%d\n",-1 * (expr->v).val.ivalue);
				else
					fprintf(target,"%d\n",(expr->v).val.ivalue);
                break;
            case FloatConst:
                fprintf(target,"%f\n", (expr->v).val.fvalue);
                break;
            default:
                fprintf(target,"Error In fprint_left_expr. (expr->v).type=%d\n",(expr->v).type);
                break;
        }
    }
    else{
        fprint_expr(target, expr->leftOperand,table);
        /*
		if(expr->rightOperand == NULL && expr->v.type == IntToFloatConvertNode) {         // type convert
            fprintf(target,"5k\n");
        }
		*/
		if(expr->rightOperand == NULL){
            fprintf(target,"5k\n");
        }
        else{
            //	fprint_right_expr(expr->rightOperand);
            fprint_expr(target, expr->rightOperand,table);
            fprint_op(target, (expr->v).type);
        }
    }
}

void gencode(Program prog, FILE * target, SymbolTable *table)
{
    Statements *stmts = prog.statements;
    Statement stmt;

    while(stmts != NULL){
        stmt = stmts->first;
        switch(stmt.type){
			char find_reg ;
            case Print:
				find_reg = find_register(stmt.stmt.variable, table);
                fprintf(target,"l%c\n",find_reg);
                fprintf(target,"p\n");
                break;
            case Assignment:
				find_reg = find_register(stmt.stmt.assign.id, table);
                fprint_expr(target, stmt.stmt.assign.expr,table);
                fprintf(target,"s%c\n",find_reg);
                //fprintf(target,"0 k\n");
                
                   if(stmt.stmt.assign.type == Int){
                   fprintf(target,"0 k\n");
                   }
                   else if(stmt.stmt.assign.type == Float){
                   fprintf(target,"5 k\n");
                   }
                
				break;
        }
        stmts=stmts->rest;
    }

}



/***************************************
  For our debug,
  you can omit them.
 ****************************************/
void print_expr(Expression *expr)
{
    if(expr == NULL)
        return;
    else{
        print_expr(expr->leftOperand);
        switch((expr->v).type){
            case Identifier:
                printf("%s ", (expr->v).val.id);
                break;
            case IntConst:
                printf("%d ", (expr->v).val.ivalue);
                break;
            case FloatConst:
                printf("%f ", (expr->v).val.fvalue);
                break;
            case PlusNode:
                printf("+ ");
                break;
            case MinusNode:
                printf("- ");
                break;
            case MulNode:
                printf("* ");
                break;
            case DivNode:
                printf("/ ");
                break;
            case IntToFloatConvertNode:
                printf("(float) ");
                break;
            default:
                printf("error ");
                break;
        }
        print_expr(expr->rightOperand);
    }
}

void test_parser( FILE *source )
{
    Declarations *decls;
    Statements *stmts;
    Declaration decl;
    Statement stmt;
    Program program = parser(source);

    decls = program.declarations;

    while(decls != NULL){
        decl = decls->first;
        if(decl.type == Int)
            printf("i ");
        if(decl.type == Float)
            printf("f ");
        printf("%s ",decl.name);
        decls = decls->rest;
    }

    stmts = program.statements;

    while(stmts != NULL){
        stmt = stmts->first;
        if(stmt.type == Print){
            printf("p %s ", stmt.stmt.variable);
        }

        if(stmt.type == Assignment){
            printf("%s = ", stmt.stmt.assign.id);
            print_expr(stmt.stmt.assign.expr);
        }
        stmts = stmts->rest;
    }

}
