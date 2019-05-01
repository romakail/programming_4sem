all: host.o slave.o
	gcc host.o  -o host
	gcc slave.o -o slave

host.o: host.c
	gcc -c host.c -o host.o

slave.o: slave.c
	gcc -c slave.c -o slave.o

clear:
	rm *.o
	rm host
	rm slave
