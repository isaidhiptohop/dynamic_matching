CC     =g++
CFLAGS =-c -std=gnu++11
INCLUDE=-I. -I../

compile_debug:
	$(CC) -g $(CFLAGS) -o random_functions.o $(INCLUDE) random_functions.cpp
compile:
	$(CC) $(CFLAGS) -o random_functions.o $(INCLUDE) random_functions.cpp
clean:
	rm -f *.o
	rm -f *.gch
library:
	make compile

