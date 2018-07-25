CC     =g++
CFLAGS =-g -c -std=gnu++11
INCLUDE=-I../tools/ -I../

compile:
	$(CC) $(CFLAGS) -o graph_access.o $(INCLUDE) graph_access.cpp
clean:
	rm -f *.o
	rm -f *.gch
library:
	make compile

