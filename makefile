clean:
	rm -rf a.out lex.yy.c y.tab.h y.tab.c out.txt

compile:
	flex expr.l
	yacc -d expr.y
	g++ defs.cpp symtutil.cpp lex.yy.c y.tab.c -o a.out -lfl

run: compile
	./a.out < input.txt > out.txt
