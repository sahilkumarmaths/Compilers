%{
/*****************************************************************
Author:
	Gunamgari Sharath Reddy        
	P V S Dileep               	   
	Sahil Kumar Goyal 
Date:
	5/3/2013             
*****************************************************************/

    #include "symtable.h"		// Data structure for symbol table
    int line=1;
    int num;
    // KEYWORDS
    #define IF    	1	// if
    #define THEN  	2	// then
    #define ELSE  	3	// else
    #define FOR   	4	// for
    #define WHILE 	5	// while

    // SEPARATORS
    #define COMMA 	6	// ,
    #define SEMICOLON 	7	// ;
    #define DOT    	8	// .

    // PUNCTUATIONS
    #define OBRAC	9	// (
    #define CBRAC	10	// )
    #define OFBRAC	11	// {
    #define CFBRAC 	12	// }
    #define OSBRAC	13	// [
    #define CSBRAC	14	// ]

    // RELOP
    #define LT	15	// <
    #define GT	16	// >
    #define LE	17	// <=
    #define GE	18	// >=
    #define NE	19	// !=
    #define	EQ	20	// ==

    // OP
    #define UPLUS	21	// unary +
    #define UMINUS	22	// unary -
    #define PLUS	23	// +
    #define MINUS	24	// -
    #define	MULT	25	// *
    #define DIV	26	// /
    #define ASSIGN	27	// =

    #define INT 28 // int
    #define CHAR 29 //char
    #define COLON   30 // :
    #define STRING 31 // strings
    #define IDENTIFIER 32 //identfier
    #define NUM 33
    #define ERROR 34 // error
%}
%%
[\n]    {line++;}
"if"    {printf("KEYWORD  IF\n");return IF;}
"else"  {printf("KEYWORD  ELSE\n");return ELSE;}
"int"   {printf("KEYWORD  INT\n");return INT;}
"char"  {printf("KEYWORD  CHAR\n");return CHAR;}
"for"   {printf("KEYWORD  FOR\n");return FOR;}
"while" {printf("KEYWORD  WHILE\n");return WHILE;}
[+] {printf("OPERATOR PLUS\n");return PLUS;}
[-] {printf("OPERATOR MINUS\n");return MINUS;}
[*] {printf("OPERATOR MULT\n");return MULT;}
[/] {printf("OPERATOR DIV\n");return DIV;}
[+][+]  {printf("OPERATOR UPLUS\n");return UPLUS;}
[-][-]  {printf("OPERATOR UMINUS\n");return UMINUS;}
[{] {printf("PUNC OFBRAC\n");return OFBRAC;}
[}] {printf("PUNC CFBRAC\n");return CFBRAC;}
[[] {printf("PUNC OSBRAC\n");return OSBRAC;}
[\]]    {printf("PUNC CSBRAC\n");return CSBRAC;}
[(] {printf("PUNC OBRAC\n");return OBRAC;}
[)] {printf("PUNC CBRAC\n");return CBRAC;}
[;] {printf("SEP SEMICOLON\n");return SEMICOLON;}
[:] {printf("SEP COLON\n");return COLON;}
[,] {printf("SEP COMMA\n");return COMMA;}
[=] {printf("RELOP ASSIGN\n");return ASSIGN;}
[!][=]  {printf("RELOP NE\n");return NE;}
[>] {printf("RELOP GT\n");return GT;}
[<] {printf("RELOP LT\n");return LT;}
[>][=]  {printf("RELOP GE\n");return GE;}
[<][=]  {printf("RELOP LE\n");return LE;}
["][a-zA-Z0-9 ]*["] {printf("STRING %s\n",yytext);return STRING;}
[a-zA-Z][a-zA-Z0-9]*    {install_id();return IDENTIFIER;}
[0-9]+[ ]  {num = atoi(yytext);printf("NUM  %d\n",num);return NUM;}
[0-9]+[.][0-9]+[ ] {num = atof(yytext);printf("NUM  %d\n",num);return NUM;}
[ \t]	{}
.  {printf("line no%s*%d:error\n",yytext,line);return ERROR;}

%%

int main(){
    int id=0;
	do{
        id=yylex();
    }while(id != 0);
    printf("SYMBOL TABLE \n");
    showTable();
}
