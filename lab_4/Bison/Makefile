all:clean
	bison -d -o parser.cpp parser.y
	flex -o tokens.cpp tokens.l
	g++ -o parser parser.cpp tokens.cpp main.cpp

clean:
	rm -f parser.hpp
	rm -f tokens.cpp
	rm -f parser.cpp
	rm -f parser
	
example1:all
	./parser < example/input1
	
error1:all
	./parser < example/error1

example2:all
	./parser < example/input2
	
error2:all
	./parser < example/error2
