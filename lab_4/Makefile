all: gen

clean:
	-rm -rf *.o *~ *.dSYM *.gch *.tab.* lex.yy.c gen

.cpp.o:
	g++ -c -g -Wall $^

AST.o: AST.cpp AST.h Label.h Address.h
Address.o: Address.cpp Address.h
Label.o: Label.cpp Label.h

archive: gen.ypp gen.l AST.cpp AST.h Label.h Label.cpp Address.h Address.cpp Makefile
	tar czvf gen.tar.gz $^

gen: gen.y gen.l AST.o AST.h Label.h Address.h Label.o Address.o
	bison -d gen.y
	flex gen.l
	g++ gen.tab.c lex.yy.c AST.o Label.o Address.o -lfl -o $@
