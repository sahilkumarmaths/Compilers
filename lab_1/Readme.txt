/*
 * Author: Sahil Kumar, Pasumarthi Venkata Sai Dileep, Sharath Reddy
 * Date: 29 Jan 2013
 */

This is a parser that accepts strings according to the following grammar.

/--------------------GRAMMAR-------------------------------------------------------/

	stmt ---------------> id ":=" expression
 			  									 | "if" expression "then" stmt
													 | "while" expression "do" stmt
													 | "begin" opt_stmts "end"
	opt_stmts------------> stmt_list 
													 | e
	stmt_list------------> stmt; opt_stmts
	expression-----------> expression "=" rel 
													 | rel
	rel      ------------> rel < expression2 
													 | rel > expression2 
													 | expression2
	expression2 ---------> expression2 + term 
													 | expression2 - term 
													 | term
	term ----------------> term * factor 
													 | term / factor 
													 | factor
	factor --------------> number 
													 | (expression) 
													 
									
/--------------------Compilation-----------------------------------------------------/

 gcc main.c lex.c lex.h name.c code_generator.c 
 
/--------------------Execution-----------------------------------------------------/

./a.out < input1.txt
