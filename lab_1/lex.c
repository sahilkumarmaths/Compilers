/*
 * Author: Sahil Kumar, Pasumarthi Venkata Sai Dileep, Sharath Reddy
 * Date: 29 Jan 2013
 */

#include "lex.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include<stdlib.h>
#include <ctype.h>

char* yytext = ""; 	/* Lexeme (not '\0' terminated) */
int yyleng   = 0;   /* Lexeme length.           */
int yylineno = 0;   /* Input line number        */
int count = 0;

/*
 * Lexeme Analyser 
 */
int lex()
{
	
   static char input_buffer[1024];
   char        *current;
   current = yytext + yyleng; /* Skip current lexeme */

   while(1)
   {     /* Get the next one         */
      while(!*current )
      {
         /* Get new lines, skipping any leading
          * white space on the line,
          * until a nonblank line is found.
          */
         current = input_buffer;
         if(!gets(input_buffer)){
            *current = '\0' ;\
            return EOI;
         }
         ++yylineno;
         while(isspace(*current))
            ++current;
      }
       /* Get the next token */
      for(; *current; ++current){
         yytext = current;
         yyleng = 1;
         switch( *current )
         {
           case ';':
            return SEMI;
           case '+':
            return PLUS;
           case '-':
            return MINUS;
           case '*':
            return TIMES;
           case '/':
            return DIV;
           case '(':
            return LP;
           case ')':
            return RP;
           case'<':
						return LESSER;
					 case'>':
						return GREATER;
					 case'=':
						return EQUAL;
				   case '\n':
				   case '\t':
				   case ' ' :
				   	break;
				   default:
            if(!isalnum(*current))
               fprintf(stderr, "Agvonse la8emevn eisagwgn <%c>\n", *current);
            else if(isdigit(*current))
            {
            	 while(isdigit(*current))
               {
                 ++current;
			  			 }
               yyleng = current - yytext;
               char *keyword;
               return NUM_OR_ID;
            }
            else
            {
							 while(isalnum(*current))
               {
                 ++current;
						   }
			     	   yyleng = current - yytext; 
							 if(yytext[0]=='i' && yytext[1]=='f' && yyleng == 2)
							 {
								return IF;
							 }
							 else if(yytext[0]=='t' && yytext[1]=='h' && yytext[2]=='e' && yytext[3]=='n' && yyleng == 4) 
							 {
								 return THEN;
							 }else if(yytext[0]=='w' && yytext[1]=='h' && yytext[2]=='i' && yytext[3]=='l' && yytext[4]=='e' && yyleng == 5)
							 {
							 		return WHILE;
							 }else if(yytext[0]=='d' && yytext[1]=='o' && yyleng == 2)
							 {
							 		return DO;
							 }else if(yytext[0]=='b' && yytext[1]=='e' && yytext[2]=='g' && yytext[3]=='i' && yytext[4]=='n' && yyleng==5 )
							 {
							 		return BEGIN;
							 }else if(yytext[0]=='e' && yytext[1]=='n' && yytext[2]=='d' && yyleng==3)
							 {
							 		return END;
							 }
               char *keyword = current;
               while(isspace(*current))
            			++current;
            	 keyword=current;
			  			 if(keyword[0]!=':')
									return NUM_OR_ID;
			   			 if(keyword[1]=='=')
							 {
								 int i=0;
								 current++;
									print_space(count);
								 for(i=0;i<yyleng;i++)
								 {
								 		printf("%c",yytext[i]);
								 }
								 yytext=current;
								 return ASSIG;
							 }
							 else
							 {
								 printf("Error:- '=' expected. -- Exiting the code.\n");
								 exit(0);
							 }
						}
            break;
         }//Switch ends
      }//For loop ends
   }//While loop ends
}

static int Lookahead = -1; /* Lookahead token  */

int match(int token){
   /* Return true if "token" matches the
      current lookahead symbol.                */
      
   if(Lookahead == -1)
      Lookahead = lex();

   return token == Lookahead;
}

int get_lookahead()
{
	return Lookahead;	
}

void advance(void){
/* Advance the lookahead to the next
   input symbol.                               */

    Lookahead = lex();
}
