CFLAGS=-static -Wall -std=gnu++11

INCLUDE=-I../../lib/algorithms -I../../lib/data_structure -I../../lib/data_structure/priority_queues -I../../lib -I../../lib/io -I../../lib/tools

LIBRARY=-L../../lib -ldynamic_matching

APPNAME=sequencer

all:
	make compile_debug
	make link

run:
	./$(APPNAME)

compile_debug:
	g++ -o sequence.o -c -g $(CFLAGS) $(INCLUDE) sequence.cpp
	g++ -o main.o -c -g $(CFLAGS) $(INCLUDE) main.cpp

compile:
	g++ -o sequence.o -c $(CFLAGS) $(INCLUDE) sequence.cpp
	g++ -o main.o -c $(CFLAGS) $(INCLUDE) main.cpp

link:
	g++ -o $(APPNAME) main.o sequence.o $(LIBRARY)
	
clean:
	rm -f *.o
	rm -f *.out
	rm -f $(APPNAME) 
