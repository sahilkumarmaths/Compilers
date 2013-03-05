#define EOI		0	/* End of input			*/
#define SEMI		1	/* ; 				*/
#define PLUS 		2	/* + 				*/
#define TIMES		3	/* * 				*/
#define LP		4	/* (				*/
#define RP		5	/* )				*/
#define NUM_OR_ID	6	/* Decimal Number or Identifier */
#define OR		7	/* Decimal Number or Identifier */
#define AND		8
#define STAR		9
#define CHAR		10
#define DASH	11
extern char *yytext;		/* in lex.c			*/
extern int yyleng;
extern int yylineno;
