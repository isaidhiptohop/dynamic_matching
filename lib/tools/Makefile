CC     =g++
CFLAGS =-c -std=gnu++11
INCLUDE=-I. -I../

compile_debug:
	$(CC) -g $(CFLAGS) -o random_functions.o $(INCLUDE) random_functions.cpp
	$(CC) -g $(CFLAGS) -o quality_metrics.o $(INCLUDE) quality_metrics.cpp
	$(CC) -g $(CFLAGS) -o counters.o $(INCLUDE) counters.cpp
compile:
	$(CC) $(CFLAGS) -o random_functions.o $(INCLUDE) random_functions.cpp
	$(CC) $(CFLAGS) -o quality_metrics.o $(INCLUDE) quality_metrics.cpp
	$(CC) $(CFLAGS) -o counters.o $(INCLUDE) counters.cpp
clean:
	rm -f *.o
	rm -f *.gch
library:
	make compile
library_debug:
	make compile_debug

