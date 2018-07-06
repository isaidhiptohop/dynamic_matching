CFLAGS=-g -Wall -std=gnu++11

INCLUDE=-I../lib/algorithms -I../lib/data_structure -I../lib/data_structure/priority_queues -I../lib -I../lib/tools  -I../GPAMatching/lib/data_structure -I../GPAMatching/lib

LIBRARY=-L../lib -ldynamic_matching -L../GPAMatching/lib -lgpa_matching

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
	g++ -o main.o -c $(CFLAGS) $(INCLUDE) main.cpp

link:
	g++ -o executer ../GPAMatching/app/gpa_match.o executer.o $(LIBRARY)
	g++ -o $(APPNAME) main.o sequence.o $(LIBRARY)
	
clean:
	rm -f *.o
	rm -f *.out
