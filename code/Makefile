all: reggraph.c getreg.c lexical.l main.c m.h syntax.y semantic.c ReadWrite.c intercodelist.c intercodeproduce.c assembly.c
	flex lexical.l
	bison -d syntax.y
	gcc reggraph.c getreg.c assembly.c errors.c semantic.c symbollist.c ReadWrite.c intercodelist.c intercodeproduce.c main.c syntax.tab.c -lfl -ly -o ../parser

clean:
	rm lex.yy.c ../parser syntax.tab.c syntax.tab.h syntax.output

output:
	bison -d -v syntax.y

debug: reggraph.c getreg.c assembly.c lexical.l main.c m.h syntax.y semantic.c ReadWrite.c intercodelist.c intercodeproduce.c
	flex lexical.l
	bison -d syntax.y
	gcc reggraph.c getreg.c assembly.c errors.c semantic.c symbollist.c ReadWrite.c intercodelist.c intercodeproduce.c main.c syntax.tab.c -lfl -ly -o ../parser -g

play:
	./../parser ../TEST/test1
	
	
