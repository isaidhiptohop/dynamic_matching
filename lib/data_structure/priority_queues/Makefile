CFLAGS =-c -std=gnu++11
INCLUDE=-I../../tools/ -I../../

compile_debug:
	g++ -g $(CFLAGS) -o maxNodeHeap.o $(INCLUDE) maxNodeHeap.cpp
compile:
	g++ $(CFLAGS) -o maxNodeHeap.o $(INCLUDE) maxNodeHeap.cpp
clean:
	rm -f *.o
	rm -f *.gch
library:
	make compile

