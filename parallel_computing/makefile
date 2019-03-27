all: main.o
	gcc -o prog main.o -pthread

main.o: main.c
	gcc -c main.c -o main.o

clear:
	rm *.o prog

tests:
	time ./prog 1
	time ./prog 2
	time ./prog 3
	time ./prog 4
	time ./prog 6
	time ./prog 8
	time ./prog 16
	time ./prog 64
	time ./prog 128
