all:
	rm -f lex.yy.c
	rm -f lexer
	flex ll1_parser.l
	g++ -o lexer lex.yy.c cfg.cpp cfg.h -ll
	
example1:all
	(./lexer example/grammar1 < example/input1) > out
	
example2:all
	(./lexer example/grammar2 < example/input2) > out
	
example3:all
	(./lexer example/grammar3 < example/input3) > out

error1:all
	(./lexer example/grammar1 < example/error1) > out
	
error2:all
	(./lexer example/grammar2 < example/error2) > out
	
error3:all
	(./lexer example/grammar3 < example/error3) > out
	
clean:	
	rm -f lex.yy.c
	rm -f lexer
