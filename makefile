build:
	gcc -Wall -O2 -rdynamic -I./include main.c src/jsonLexer.c src/jsonParser.c src/jsonListener.c src/jsonEditor.c src/debug.c

debug_build:
	gcc -fsanitize=address -Wall -g -rdynamic -I./include \
	main.c \
	src/jsonLexer.c \
	src/jsonEditor.c \
	src/obj/set.c \
	src/obj/get.c \
	src/obj/delete.c \
	src/obj/create.c \
	src/arr/set.c \
	src/arr/get.c \
	src/arr/delete.c \
	src/arr/create.c \
	src/arr/append.c \
	src/debug.c

# debug_build:
# 	gcc -fsanitize=address -Wall -g -rdynamic -I./include \
# 	main.c \
# 	src/token.c \
# 	src/jsonLexer.c \
# 	src/jsonParser.c \
# 	src/jsonListener.c \
# 	src/jsonEditor.c \
# 	src/obj/insert.c \
# 	src/debug.c

clean:
	rm a.out

run:
	./a.out testcases/case1.json
	./a.out testcases/case2.json