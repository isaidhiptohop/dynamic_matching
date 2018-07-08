CC     =g++
CFLAGS =-c -std=gnu++11
INCLUDE=-I../data_structure/ -I../ -I../tools/

compile_debug:
	$(CC) -g $(CFLAGS) -o graph_io.o $(INCLUDE) graph_io.cpp
compile:
	$(CC) $(CFLAGS) -o graph_io.o $(INCLUDE) graph_io.cpp
clean:
	rm -f *.o
	rm -f *.gch
library:
	make compile
library_debug:
	make compile_debug

