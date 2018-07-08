CC     =g++
CFLAGS =-c -std=gnu++11
INCLUDE=-I../tools/ -I../

compile_debug:
	$(CC) -g $(CFLAGS) -o avl_tree.h.gch $(INCLUDE) avl_tree.h
	$(CC) -g $(CFLAGS) -o graph_access.o $(INCLUDE) graph_access.cpp
	$(CC) -g $(CFLAGS) -o dyn_graph_access.o $(INCLUDE) dyn_graph_access.cpp
compile:
	$(CC) $(CFLAGS) -o avl_tree.h.gch $(INCLUDE) avl_tree.h
	$(CC) $(CFLAGS) -o graph_access.o $(INCLUDE) graph_access.cpp
	$(CC) $(CFLAGS) -o dyn_graph_access.o $(INCLUDE) dyn_graph_access.cpp
clean:
	rm -f *.o
	rm -f *.gch
library:
	make compile
	cd priority_queues && make library
library_debug:
	make compile_debug

