all: lexical.l main.c m.h syntax.y
	flex lexical.l
	bison -d syntax.y
	gcc main.c syntax.tab.c -lfl -ly -o parser -g

clean:
	rm lex.yy.c parser syntax.tab.c syntax.tab.h
	
	