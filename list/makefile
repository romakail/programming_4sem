all: prog

#==========Usual assembling stuff==========
prog: main.o list.o
	g++ -o prog main.o list.o

#CPPFLAGS := -g -MD --coverage

main.o: main.cpp
	g++ main.cpp -c -o main.o

list.o: list.cpp list.h
	g++ list.cpp -c -o list.o


#==========Cleaning stuff==================
clean:
	rm -rf *.o *.d *.gcno prog *.gcov *.gcda

clear:
	rm -rf *.o *.d *.gcno prog *.gcov *.gcda

#===========Testing stuff==================
test: mainTest.o listTest.o fakeCalloc.o
	g++ -o prog mainTest.o listTest.o fakeCalloc.o -lgcov
	./prog
	gcov listTest.gcda
	gedit list.cpp.gcov

mainTest.o: test.cpp
	g++ test.cpp -c -o mainTest.o

listTest.o: list.cpp
	g++ list.cpp -c -o listTest.o -ftest-coverage -fprofile-arcs -g -DTEST

fakeCalloc.o: fakeCalloc.cpp
	g++ fakeCalloc.cpp -c -o fakeCalloc.o

#==========================================

-include *.d
