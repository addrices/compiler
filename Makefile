parser:main.c lexical.l syntax.y
	flex lexical.l
	bison -d syntax.y
	gcc main.c syntax.tab.c -lfl -ly -o parser

clean:
	rm lex.yy.c scanner
	
scanner:main.c lexical.l
	flex lexical.l
	gcc main.c lex.yy.c -lfl -o scanner
	