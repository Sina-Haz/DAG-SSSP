all: SSSP

SSSP:SSSP.c
	gcc -g -Wall -Werror -fsanitize=address SSSP.c -o SSSP
clean:
	rm SSSP