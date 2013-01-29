/*
 * Author: Sahil Kumar, Pasumarthi Venkata Sai Dileep, Sharath Reddy
 * Date: 29 Jan 2013
 */

#include <stdio.h>
#include <stdlib.h>
#include "lex.h"


extern char *newname( void);
extern void freename( char *name );
char *rel( void );
char *factor( void );
char *term( void );
char *expression( void );
char *expression2( void );
void opt_stmt();
void stmt_list();
void print_space(int v);

static int beg=0;

/*
 * stmt --> id ":=" expression
	 			  | "if" expression "then" stmt
  				| "while" expression "do" stmt
  				| "begin" opt_stmts "end"
 */
  
stmt()
{
	char *tempvar;
	if(match(IF))
        {
						  advance();
						  tempvar = expression();
						  freename( tempvar );
						  if(match(THEN))
						  {
						  		print_space(count);
							  	printf("if %s then\n",tempvar);
							  	print_space(count);
							  	printf("{\n");
							  	count++;
									advance();
									stmt();
									count--;
									print_space(count);
									printf("}\n");
									if(beg==0)
									{
										if(!match(EOI))
										{
											 
										   printf("\nError:- wrong syntax for if-then -- Exiting the code. \n");
											exit(0);
										}
									}
							}
							else
							{
								printf("\nError:- 'then' missing -- Exiting the code.\n");
								exit(0);
							}
			}
			else if(match(ASSIG))
			{
					printf(" := {\n");
					count++;
					advance() ;
					tempvar=expression();
					print_space(count);
					printf("\b\b\b}  %s \n",tempvar);
					freename( tempvar );
					count--;
			}
			else if(match(WHILE))
			{
					advance();
					tempvar = expression();
					freename( tempvar );
					if(match(DO))
					{
						print_space(count);
						printf("while %s do\n",tempvar);
						print_space(count);
						printf("{\n");
						count++;
						advance();
						stmt();
						count--;
						print_space(count);
						printf("}\n");
					
						if(beg==0)
						{
							if(!match(EOI))
							{
								 printf("Error:- wrong syntax for while-do -- Exiting the code. \n");
								exit(0);
							}
						}
					}
					else
					{
						printf("Error:- 'do' missing -- Exiting the code. \n");
						exit(0);
					}
			}
			else if(match(BEGIN))
			{
							print_space(count);
							printf("begin\n");
							print_space(count);
							printf("{\n");
							count++;
							beg++;
							advance();
						  opt_stmt();
						  if(match(END))
						  {
								count--;
								print_space(count);
								printf("}\n");
								print_space(count);
								printf("end\n");
								advance();	
								beg--;
								if(beg==0)
								{
									if(!match(EOI))
									{
										
										 printf("Error:- wrong syntax for begin-end -- Exiting the code.\n");
										 exit(0);
									}
								}
							}
							else
							{
								printf("Error:- 'end' -- Exiting the code.\n");
								exit(0);
							}
			}
			else
			{
				printf("Error:- syntax error \n");
				exit(0);
			}
}

/*
 * stmt_list --> stmt; opt_stmts
 */
void stmt_list()
{
	 stmt();
	 if(match(SEMI))
	 {
		 advance();
		 if(!match(END))
		 {
		     opt_stmt();
	     }
	     else
	     {
			 printf("\n  Error:- ommitting extra semicolon  \n");
		 }
	}
}

/* 
 * opt_stmts --> stmt_list | e
 */
void opt_stmt()
{
	if(!match(END))
	 stmt_list();
}

/* 
 * expression -->  "=" rel | rel
 */
char *expression()
{
	char  *tempvar, *tempvar2;

    tempvar = rel();
    while( 1 )
    {
		if(match(EQUAL))
		{
			advance();
			tempvar2 = rel();
			print_space(count);
			printf("%s = ( %s == %s )\n",tempvar, tempvar, tempvar2 );
			freename( tempvar2 );
		}
		else break;
    }

    return tempvar;
}

/* 
 * rel --> rel < expression2 
           | rel > expression2 
           | expression2
 */
char  *rel()
{
	  char  *tempvar, *tempvar2 ;
    tempvar = expression2();
    while(1 )
    {
			if( match(LESSER))
			{
				advance();
				tempvar2 = expression2();
				print_space(count);
				printf("%s = ( %s < %s )\n",tempvar, tempvar, tempvar2 );
				freename( tempvar2 );
			}
			else if(match(GREATER))
			{
				advance();
				tempvar2 = expression2();
				print_space(count);
				printf("%s = ( %s > %s )\n",tempvar, tempvar, tempvar2 );
				freename( tempvar2 );
			}
			else break;
    }
    return tempvar;
}

/*
 * expression2 -->  + term 
 									| - term 
 									| term
 */
char    *expression2()
{
   char  *tempvar, *tempvar2;
    tempvar = term();
    while( 1 )
    {
			if(match( PLUS ))
			{
		      advance();
		      tempvar2 = term();
		      print_space(count);
		      printf("%s += %s\n", tempvar, tempvar2 );
		      freename( tempvar2 );
			}
		  else if(match( MINUS ))
		  {
					advance();
		      tempvar2 = term();
		      print_space(count);
		      printf("%s -= %s\n", tempvar, tempvar2 );
		      freename( tempvar2 );
			}
			else break;
    }

    return tempvar;
}

/*
 * term --> * factor
 						| / factor 
 						| factor
 */
char    *term()
{
    char  *tempvar, *tempvar2 ;
    tempvar = factor();
    while(1 )
    {
			if( match( TIMES ))
			{
				advance();
				tempvar2 = factor();
				print_space(count);
				printf("%s *= %s\n", tempvar, tempvar2 );
				freename( tempvar2 );
			}
			else if(match(DIV))
			{
				advance();
				tempvar2 = factor();
				print_space(count);
				printf("%s /= %s\n", tempvar, tempvar2 );
				freename( tempvar2 );
			}
			else break;
    }

    return tempvar;
}

/*
 * factor --> number 
 							| (expression) 
 */
char    *factor()
{
    char *tempvar;

    if( match(NUM_OR_ID) )
    {
			/* Print the assignment instruction. The %0.*s conversion is a form of
			 * %X.Ys, where X is the field width and Y is the maximum number of
			 * characters that will be printed (even if the string is longer). I'm
			 * using the %0.*s to print the string because it's not \0 terminated.
			 * The field has a default width of 0, but it will grow the size needed
			 * to print the string. The ".*" tells printf() to take the maximum-
			 * number-of-characters count from the next argument (yyleng).
			 */
	 			
	 			print_space(count);
        printf("%s = %0.*s\n", tempvar = newname(), yyleng, yytext );
        advance();
    }
    else if( match(LP) )
    {
        advance();
        tempvar = expression();
        if( match(RP) )
            advance();
        else
        {
            fprintf(stderr, "%d: Mismatched parenthesis\n", yylineno );
            exit(1);
		}
    }
    else
    {
		fprintf( stderr, "%d: Number or identifier expected\n", yylineno );
		exit(1);
	}
    return tempvar;
}

/*
 * This function is used for indentation purpose.
 */
void print_space(int v)
{
	int i=0;
	for(i=0; i<v; i++)
		printf("\t");
}
