/*
 * Assignment #1
 * Author: Sahil Kumar, Pasumarthi Venkata Sai Dileep, Sharath Reddy
 * Date: 29 Jan 2013
 */

/------------------------------------QUESTION---------------------------------------/

	Extend the hand-written compiler so that it can translate code 
	segments generated by the following grammar:
	
		stmt → id := expr
			| if expr then stmt
			| while expr do stmt
			| begin opt_stmts end 
		opt_stmts → stmt_list |є
		stmt_list  → stmt_list; stmt | stmt

	Also, modify the grammar for handling expressions so that it can handle 
	relational operators (=, <, >) as well as arithmetic operators (+, -, *, /)
  
/-----------------------------------------------------------------------------------/

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

	make
 
/--------------------Execution-----------------------------------------------------/

	./a.out < input1.txt
	./a.out < input2.txt
	./a.out < input3.txt
