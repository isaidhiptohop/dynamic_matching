CFLAGS=-static -Wall -std=gnu++11

INCLUDE=-I../lib/algorithms -I../lib/data_structure -I../lib/data_structure/priority_queues -I../lib -I../lib/io -I../lib/tools

LIBRARY=-L../lib -ldynamic_matching

all:
	make compile_debug
	make link

run:
	./$(APPNAME)

compile_debug:
	g++ -o sequence.o -c -g $(CFLAGS) $(INCLUDE) sequence.cpp
	g++ -o main.o -c -g $(CFLAGS) $(INCLUDE) main.cpp
	g++ -o executer.o -c -g $(CFLAGS) $(INCLUDE) executer.cpp
	g++ -o main_executer.o -c -g $(CFLAGS) $(INCLUDE) main_executer.cpp
	g++ -o compare_matchings.o -c -g $(CFLAGS) $(INCLUDE) compare_matchings.cpp

compile:
	g++ -o sequence.o -c $(CFLAGS) $(INCLUDE) sequence.cpp
	g++ -o main.o -c $(CFLAGS) $(INCLUDE) main.cpp
	g++ -o executer.o -c $(CFLAGS) $(INCLUDE) executer.cpp
	g++ -o main_executer.o -c $(CFLAGS) $(INCLUDE) main_executer.cpp
	g++ -o compare_matchings.o -c $(CLFAGS) $(INCLUDE) compare_matchings.cpp

link:
	g++ -o executer main_executer.o executer.o $(LIBRARY)
	g++ -o sequencer main.o sequence.o $(LIBRARY)
	g++ -o compare compare_matchings.o $(LIBRARY)
	
clean:
	rm -f *.o
	rm -f *.out
	rm -f compare
	rm -f executer
	rm -f sequencer
	rm -f gpa_matching
