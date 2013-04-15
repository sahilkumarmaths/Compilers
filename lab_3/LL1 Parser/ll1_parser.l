%{
#include<stdio.h>
#include<stdlib.h>
#include "cfg.h"

    #define ID 32 //identfier
    #define TLPAREN	9	// (
    #define TRPAREN	10	// )

    #define TDOUBLE 11
    #define TINTEGER 12
    #define TEQUAL 13	// = 
    #define TCEQ 14		//==
    #define TCNE 15		//!=
    #define TCLT 16		//<
    #define TCGT 17		//>
    #define TCGE 18		//>=
    #define TCLE 19		//<=
    #define TPLUS 20	// +
    #define TMINUS 21	//-
    #define TMUL 22		//*
    #define TDIV 23		// /
    #define SEMI 24		// ;
    
    //keywords
    #define IF 25
    #define ELSE 26
    #define WHILE 27	
    #define DO 28
    #define BEG 29	//{
    #define END 30	//}
    
    
    #define INT 33
    #define FLOAT 34
    #define FOR 35
    #define COMMA 36 //,
    
    #define CALL 37 
    #define COLON 38 //:
    #define BREAK 39 
    #define CONTINUE 40
    #define DEF 41
    
    #define STR 42
    #define CHAR 43		//char
    #define CHARACTER 44	//'_'
    #define STRING 45
    
    #define DOLLAR	99	//$   

    
%}
%%
[ \t\n]                 ;
"if"					return IF;
"else"					return ELSE;
"while"					return WHILE;
"do"					return DO;
"for"					return FOR;
"int"					return INT;
"float"					return FLOAT;
"char"					return CHAR;
"call"					return CALL;
"def"					return DEF;
"break"					return BREAK;
"continue"				return CONTINUE;
"string"				return STRING;
"{"						return BEG;
"}"						return END;
["][a-zA-Z0-9 ]*["]		return STR;
['][_][']				return CHARACTER;
[a-zA-Z_][a-zA-Z0-9_]*  return ID;
[0-9]+\.[0-9]*          return TDOUBLE;
[0-9]+                  return TINTEGER;
"("                     return TLPAREN;
")"                     return TRPAREN;
"="                     return TEQUAL;
"=="                    return TCEQ;
"!="                    return TCNE;
"<"                     return TCLT;
"<="                    return TCLE;
">"                     return TCGT;
">="                    return TCGE;
"+"                     return TPLUS;
"-"                     return TMINUS;
"*"                     return TMUL;
"/"                     return TDIV;
";"						return SEMI;
","						return COMMA;
":"						return COLON;

[$] {return DOLLAR;}
%%
int main(int argc, char *argv[] )
{
	
	cfg obj(argv[1]);
    int id=0;
	do{
        id=yylex();
        switch(id)
        {
			case 9:
				printf("lex:(\n");
				obj.parseinput("(");
				break;
			case 10:
				printf("lex:)\n");
				obj.parseinput(")");
				break;
			case 11:
				printf("lex:TDOUBLE\n");
				obj.parseinput("TDOUBLE");
				break;
			case 12:
				printf("lex:INTEGER\n");
				obj.parseinput("TINTEGER");
				break;
			case 13:
				printf("lex:=\n");
				obj.parseinput("=");
				break;
			case 14:
				printf("lex:==\n");
				obj.parseinput("==");
				break;
			case 15:
				printf("lex:!=\n");
				obj.parseinput("!=");
				break;
			case 16:
				printf("lex:<\n");
				obj.parseinput("<");
				break;
			case 17:
				printf("lex:>\n");
				obj.parseinput(">");
				break;
			case 18:
				printf("lex:>=\n");
				obj.parseinput(">=");
				break;
			case 19:
				printf("lex:<=\n");
				obj.parseinput("<=");
				break;	
			case 20:
				printf("lex:+\n\n");
				obj.parseinput("+");
				break;
			case 21:
				printf("lex:-\n");
				obj.parseinput("-");
				break;
			case 22:
				printf("lex:*\n\n");
				obj.parseinput("*");
				break;
			case 23:
				printf("lex:/\n\n");
				obj.parseinput("/");
				break;
			case 24:
				printf("lex:;\n\n");
				obj.parseinput(";");
				break;
			case 25:
				printf("lex:if\n\n");
				obj.parseinput("if");
				break;
			case 26:
				printf("lex:else\n\n");
				obj.parseinput("else");
				break;
			case 27:
				printf("lex:while\n");
				obj.parseinput("while");
				break;
			case 28:
				printf("lex:do\n");
				obj.parseinput("do");
				break;
			case 29:
				printf("lex:{\n");
				obj.parseinput("{");
				break;
			case 30:
				printf("lex:}\n");
				obj.parseinput("}");
				break;		
			case 32:
				printf("lex:ID\n\n");
				obj.parseinput("ID");
				break;
			case 33:
				printf("lex:int\n");
				obj.parseinput("int");
				break;
			case 34:
				printf("lex:float\n");
				obj.parseinput("float");
				break;	
			case 35:
				printf("lex:for\n");
				obj.parseinput("for");
				break;		
			case 36:
				printf("lex:,\n");
				obj.parseinput(",");
				break;
			case 37:
				printf("lex:call\n");
				obj.parseinput("call");
				break;	
			case 38:
				printf("lex::\n");
				obj.parseinput(":");
				break;
			case 39:
				printf("lex:break\n");
				obj.parseinput("break");
				break;	
			case 40:
				printf("lex:continue\n");
				obj.parseinput("continue");
				break;	
			case 41:
				printf("lex:def\n");
				obj.parseinput("def");
				break;
			case 42:
				printf("lex:STR\n");
				obj.parseinput("str");
				break;
			case 43:
				printf("lex:char\n");
				obj.parseinput("char");
				break;
			case 44:
				printf("lex:character\n");
				obj.parseinput("character");
				break;
			case 45:
				printf("lex:string\n");
				obj.parseinput("string");
				break;
			case 99:
				printf("lex:$\n\n");
				obj.parseinput("$");
				exit(0);
			
        }       
    }while(id != 0);

}
