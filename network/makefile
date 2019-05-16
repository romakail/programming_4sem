all: host.o slave.o network.o integral.o
	gcc host.o  network.o -o host
	gcc slave.o network.o integral.o -o slave -pthread

host.o: host.c
	gcc -c host.c -o host.o

slave.o: slave.c
	gcc -c slave.c -o slave.o

network.o: network.c network.h
	gcc -c network.c -o network.o

integral.o: integral.c integral.h
	gcc -c integral.c -o integral.o

clear:
	rm *.o
	rm host
	rm slave
