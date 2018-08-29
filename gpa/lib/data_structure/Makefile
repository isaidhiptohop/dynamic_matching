CC     =g++
CFLAGS =-c -std=gnu++11
INCLUDE=-I../tools/ -I../

compile:
#	$(CC) $(CFLAGS) -o graph_access.o $(INCLUDE) graph_access.cpp
	$(CC) $(CFLAGS) -o graph_access.h.gch $(INCLUDE) graph_access.h
	$(CC) $(CFLAGS) -o static_graph.h.gch $(INCLUDE) static_graph.h
	$(CC) $(CFLAGS) -o dyn_graph_access.o $(INCLUDE) dyn_graph_access.cpp
compile-debug:
#	$(CC) -g $(CFLAGS) -o graph_access.o $(INCLUDE) graph_access.cpp
	$(CC) -g $(CFLAGS) -o graph_access.h.gch $(INCLUDE) graph_access.h
	$(CC) -g $(CFLAGS) -o static_graph.h.gch $(INCLUDE) static_graph.h
	$(CC) -g $(CFLAGS) -o dyn_graph_access.o $(INCLUDE) dyn_graph_access.cpp
clean:
	rm -f *.o
	rm -f *.gch
library:
	make compile
library-debug:
	make compile-debug

