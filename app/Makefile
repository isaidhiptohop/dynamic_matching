CFLAGS=-g -Wall -std=gnu++11

INCLUDE=-I../lib/algorithms -I../lib/data_structure -I../lib/data_structure/priority_queues -I../lib -I../lib/io -I../lib/tools

LIBRARY=-L../lib -ldynamic_matching

APPNAME=sequence


all:
#	make classes
	make compile_test

run:
	./$(APPNAME)

compile_test:
	make compile
	make link

compile:
	g++ -o executer.o -c $(CFLAGS) $(INCLUDE) executer.cpp
	g++ -o sequence.o -c $(CFLAGS) $(INCLUDE) sequence.cpp
	g++ -o main_executer.o -c $(CFLAGS) $(INCLUDE) main_executer.cpp
	g++ -o main.o -c $(CFLAGS) $(INCLUDE) main.cpp

link:
	g++ -o executer main_executer.o executer.o $(LIBRARY)
	g++ -o $(APPNAME) main.o sequence.o $(LIBRARY)
	
clean:
	rm -f *.o
	rm -f *.out
