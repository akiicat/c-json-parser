build:
	gcc -Wall -O2 -rdynamic -I./include main.c src/obj_hash_linear_probing.c src/arr_dynamic_array.c src/json.c

debug_build:
	gcc \
	  	-fsanitize=address   \
		-fno-omit-frame-pointer  \
        -fsanitize=address  \
        -fsanitize=undefined  \
        -fsanitize=float-cast-overflow  \
        -fsanitize-address-use-after-scope  \
        -fno-sanitize-recover  \
	-Wall -g -rdynamic -I./include \
	main.c \
	src/obj_hash_linear_probing.c \
	src/arr_dynamic_array.c \
	src/json.c

clean:
	rm a.out

run:
	./a.out testcases/case1.json
	./a.out testcases/case2.json