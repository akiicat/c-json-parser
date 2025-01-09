build:
	gcc -Wall -g -rdynamic main.c token.c jsonLexer.c jsonParser.c jsonListener.c debug.c

run:
	./a.out testcases/case1.json
	./a.out testcases/case2.json