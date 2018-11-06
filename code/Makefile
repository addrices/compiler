all: lexical.l main.c m.h syntax.y semantic.c
	flex lexical.l
	bison -d syntax.y
	gcc main.c syntax.tab.c -lfl -ly -o ../parser

clean:
	rm lex.yy.c ../parser syntax.tab.c syntax.tab.h syntax.output

output:
	bison -d -v syntax.y
	
	
