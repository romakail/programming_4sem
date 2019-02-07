all: prog

prog: main.o print.o
	gcc -o prog main.o print.o

CFLAGS := -g -MD

clean:
	rm -rf *.o *.d prog
clear:
	rm -rf *.o *.d prog

-include *.d
