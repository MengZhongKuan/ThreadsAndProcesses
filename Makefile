output: a1-elf.o
	clang -Wall -Wpedantic -Wextra -Werror a1-procs.c -o a1-procs
	clang -Wall -Wpedantic -Wextra -Werror -lpthread a1-threads.c -o a1-threads

clean:
	rm $(wildcard *.o) a1-procs a1-threads
