all: lexical.l main.c m.h syntax.y semantic.c ReadWrite.c
	flex lexical.l
	bison -d syntax.y
	gcc errors.c semantic.c symbollist.c ReadWrite.c main.c syntax.tab.c -lfl -ly -o ../parser

clean:
	rm lex.yy.c ../parser syntax.tab.c syntax.tab.h syntax.output

output:
	bison -d -v syntax.y

debug:lexical.l main.c m.h syntax.y semantic.c ReadWrite.c
	flex lexical.l
	bison -d syntax.y
	gcc errors.c semantic.c symbollist.c ReadWrite.c main.c syntax.tab.c -lfl -ly -o ../parser -g

play:
	./../parser ../TEST/test1
	
	
