build:
	gcc -fsanitize=address -Wall -g -rdynamic main.c token.c jsonLexer.c jsonParser.c jsonListener.c debug.c

clean:
	rm a.out

run:
	./a.out testcases/case1.json
	./a.out testcases/case2.json