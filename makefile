build:
	gcc -Wall -O2 -rdynamic -I./include main.c src/token.c src/jsonLexer.c src/jsonParser.c src/jsonListener.c src/jsonEditor.c src/debug.c

debug_build:
	gcc -fsanitize=address -Wall -g -rdynamic -I./include main.c src/token.c src/jsonLexer.c src/jsonParser.c src/jsonListener.c src/jsonEditor.c src/debug.c

clean:
	rm a.out

run:
	./a.out testcases/case1.json
	./a.out testcases/case2.json