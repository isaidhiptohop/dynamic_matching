CFLAGS=-static -Wall -std=gnu++11

INCLUDE=-I../../lib/algorithms -I../../lib/data_structure -I../../lib/data_structure/priority_queues -I../../lib -I../../lib/io -I../../lib/tools

LIBRARY=-L../../lib -ldynamic_matching

APPNAME=compare

all:
	make compile_debug
	make link

run:
	./$(APPNAME)

compile_debug:
	g++ -o compare_matchings.o -c -g $(CFLAGS) $(INCLUDE) compare_matchings.cpp

compile:
	g++ -o compare_matchings.o -c $(CFLAGS) $(INCLUDE) compare_matchings.cpp

link:
	g++ -o compare compare_matchings.o $(LIBRARY)
	
clean:
	rm -f *.o
	rm -f *.out
	rm -f $(APPNAME)

