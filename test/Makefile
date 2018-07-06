CFLAGS=-g -Wall -std=gnu++11

INCLUDE=-I../lib/algorithms \
        -I../lib/data_structure \
        -I../lib/matching \
        -I../lib/tools \
        -I../lib

LIBRARY=-L../lib -ldynamic_matching

all:
#	make classes
	make compile_test

run:
	./$(APPNAME)

compile_test:
	make compile
	make link

compile:
	g++ -o test.o -c $(CFLAGS) $(INCLUDE) test.cpp

link:
	g++ -o a.out test.o $(LIBRARY)
	
clean:
	rm -f *.o
	rm -f *.out
