CFLAGS=-static -Wall -std=gnu++11

INCLUDE=-I../../lib/algorithms -I../../lib/data_structure -I../../lib/data_structure/priority_queues -I../../lib -I../../lib/io -I../../lib/tools

LIBRARY=-L../../lib -ldynamic_matching

APPNAME=executer

all:
	make compile_debug
	make link

run:
	./$(APPNAME)

compile_debug:
	g++ -o executer.o -c -g $(CFLAGS) $(INCLUDE) executer.cpp
	g++ -o main.o -c -g $(CFLAGS) $(INCLUDE) main.cpp
	g++ -o main_eps.o -c -g $(CFLAGS) $(INCLUDE) main_eps.cpp

compile:
	g++ -o executer.o -c $(CFLAGS) $(INCLUDE) executer.cpp
	g++ -o main.o -c $(CFLAGS) $(INCLUDE) main.cpp
	g++ -o main_eps.o -c $(CFLAGS) $(INCLUDE) main_eps.cpp

link:
	g++ -o executer main.o executer.o $(LIBRARY)
	g++ -o executer_eps main_eps.o executer.o $(LIBRARY)
	
clean:
	rm -f *.o
	rm -f *.out
	rm -f $(APPNAME)

