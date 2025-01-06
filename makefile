build:
	gcc -Wall -g -rdynamic main.c jsonLexer.c jsonParser.c debug.c

run:
	./a.out testcases/case1.json