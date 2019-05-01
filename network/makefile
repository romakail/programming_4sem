all: host.o slave.o network.o
	gcc host.o  network.o -o host
	gcc slave.o network.o -o slave

host.o: host.c
	gcc -c host.c -o host.o

slave.o: slave.c
	gcc -c slave.c -o slave.o

network.o: network.c network.h
	gcc -c network.c -o network.o

clear:
	rm *.o
	rm host
	rm slave
