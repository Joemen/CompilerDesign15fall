%option noyywrap 
%{
#include <stdio.h>
#include "header.h" 
int linenumber;
symtab * lookup();
symtab * ptr;
void     insertID();
void 	 printSymTab();
void	 comment();
/* You need to define for all tokens in C--, here are some examples */
#define RESERVE		9
#define INT			10
#define FLOAT 		11 
#define OP_ASSIGN 	12 
#define OP_OR 		13 
#define MK_LPAREN 	14 
#define MK_RPAREN 	15
#define STRING		16
#define	ID			17
#define	OP_AND		18
#define	OP_ADD		19
#define	OP_MINUS	20
#define	OP_DIV		21
#define	OP_MUL		22
#define	OP_SMALLER	23
#define	OP_BIGGER	24
#define	OP_SMALL_EQ	25
#define	OP_BIG_EQ	26
#define	OP_NOT		27
#define	OP_NOT_EQ	28
#define	OP_EQ		29
#define DL_LPAREN 	30
#define DL_RPAREN 	31
#define DL_LBRACE 	32
#define DL_RBRACE 	33
#define DL_COMMA  	34
#define DL_SEMICOL 	35
#define DL_DOT  	36
#define NEWLINE		50


#define ERROR 		100 

%}

letter   [A-Za-z]
digit    [0-9]
ID	 {letter}({letter}|{digit}|"_")*
WS	 [ \t]+
Int -?{digit}+
Float -?{digit}*\.{digit}+([eE][-+]?{digit}+)?|{Int}
String \"(\\.|[^\\"])*\"

/* You need to define the following RE's 
Float_constant
String_constant
comment
*/

OP_assign		"="
OP_add			"+"
OP_minus		"-"
OP_div			"/"
OP_mul			"*"
OP_or			"||"
OP_and 			"&&"
OP_not			"!"
OP_smaller		"<"
OP_bigger		">"
OP_small_eq		"<="
OP_big_eq		">="
OP_not_eq		"!="
OP_eq			"=="

/* Other operators appear here */ 
newline		"\n"

DL_lparen 	"("
DL_rparen 	")"
DL_lbrace 	"{"
DL_rbrace 	"}"
DL_comma  	","
DL_semicol 	";"
DL_dot  	"."

/* Other separators appear here */

ERR .

%%
"/*"		{ comment();}
{WS}		{}
"int"		{ return RESERVE;}
"float"		{ return RESERVE;}
"return"	{ return RESERVE;}
"char"		{ return RESERVE;}
"typedef"	{ return RESERVE;}
"if"		{ return RESERVE;}
"else"		{ return RESERVE;}
"for"		{ return RESERVE;}
"void"		{ return RESERVE;}
"while"		{ return RESERVE;}
{Int}		{ return INT;}
{Float}		{ return FLOAT;}
{String}	{ return STRING;}
{ID}		{
			ptr = lookup(yytext);
			if (ptr == NULL)
			     insertID(yytext);	
			else 
			     ptr->counter++;
			return ID;
}
{OP_assign} { return OP_ASSIGN;}
{OP_add}	{ return OP_ADD;}
{OP_minus}	{ return OP_MINUS;}
{OP_div}	{ return OP_DIV;}
{OP_mul}	{ return OP_MUL;}
{OP_or}		{ return OP_OR;}
{OP_and} 	{ return OP_AND;}
{OP_not}	{ return OP_NOT;}
{OP_smaller} {return OP_SMALLER;}
{OP_bigger}	{ return OP_BIGGER;}
{OP_small_eq} {return OP_SMALL_EQ;}
{OP_big_eq} { return OP_BIG_EQ;}
{OP_not_eq} { return OP_NOT_EQ;}
{OP_eq}		{ return OP_EQ;}
{DL_lparen} { return DL_LPAREN;}
{DL_rparen} { return DL_RPAREN;}
{DL_lbrace} { return DL_LBRACE;}
{DL_rbrace} { return DL_RBRACE;}
{DL_comma}  { return DL_COMMA;}
{DL_semicol} {return DL_SEMICOL;}
{DL_dot}	{return DL_DOT;}
{newline}	{
	linenumber += 1;
//	return NEWLINE;
}
ERR {	
	printf("line %d: ERR\n", linenumber);
}

%%
void comment(){
	char c, c1;
	printf("/*");
loop:
	while((c = input()) != '*' && c != 0){
		putchar(c);
	}
	if((c1 = input()) != '/' && c != 0){
		putchar(c);
		unput(c1);
		goto loop;
	}

	if(c != 0)
		printf("*/\n");
}

int main(int argc, char **argv)
{
	argc--;
	++argv;
	if (argc > 0)
		yyin = fopen(argv[0], "r");
	else
		yyin = stdin;
	while(yylex() != 0){}
	printf("\n");
//   printSymTab();
   print_Frequency();
}

