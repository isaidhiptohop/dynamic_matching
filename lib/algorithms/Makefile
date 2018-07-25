CC     =g++
CFLAGS =-c -std=gnu++11
INCLUDE=-I../data_structure -I../data_structure/priority_queues -I../tools -I../

compile:
	$(CC) $(CFLAGS) -o dyn_matching.o $(INCLUDE) dyn_matching.cpp
	$(CC) $(CFLAGS) -o simple_dyn_matching.o $(INCLUDE) simple_dyn_matching.cpp
	$(CC) $(CFLAGS) -o neimansolomon_dyn_matching.o $(INCLUDE) neimansolomon_dyn_matching.cpp
	$(CC) $(CFLAGS) -o baswanaguptasen_dyn_matching.o $(INCLUDE) baswanaguptasen_dyn_matching.cpp
	
compile_debug:
	$(CC) -g $(CFLAGS) -o dyn_matching.o $(INCLUDE) dyn_matching.cpp
	$(CC) -g $(CFLAGS) -o simple_dyn_matching.o $(INCLUDE) simple_dyn_matching.cpp
	$(CC) -g $(CFLAGS) -o neimansolomon_dyn_matching.o $(INCLUDE) neimansolomon_dyn_matching.cpp
	$(CC) -g $(CFLAGS) -o baswanaguptasen_dyn_matching.o $(INCLUDE) baswanaguptasen_dyn_matching.cpp
	
compile_counters:
	$(CC) -g -DDM_COUNTERS $(CFLAGS) -o dyn_matching.o $(INCLUDE) dyn_matching.cpp
	$(CC) -g -DDM_COUNTERS $(CFLAGS) -o simple_dyn_matching.o $(INCLUDE) simple_dyn_matching.cpp
	$(CC) -g -DDM_COUNTERS $(CFLAGS) -o neimansolomon_dyn_matching.o $(INCLUDE) neimansolomon_dyn_matching.cpp
	$(CC) -g -DDM_COUNTERS $(CFLAGS) -o baswanaguptasen_dyn_matching.o $(INCLUDE) baswanaguptasen_dyn_matching.cpp
	
clean:
	rm -f *.o
	rm -f *.gch
	
library:
	make compile
	
library_debug:
	make compile_debug
	
library_counters:
	make compile_counters
    
