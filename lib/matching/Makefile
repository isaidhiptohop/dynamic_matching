CC     =g++
CFLAGS =-c -std=gnu++11
INCLUDE=-I../data_structure -I../tools -I../

compile:
	$(CC) $(CFLAGS) -o path.o $(INCLUDE) path.cpp
	$(CC) $(CFLAGS) -o path_set.o $(INCLUDE) path_set.cpp
	$(CC) $(CFLAGS) -o gpa_matching.o $(INCLUDE) gpa_matching.cpp
compile_debug:
	$(CC) -g $(CFLAGS) -o path.o $(INCLUDE) path.cpp
	$(CC) -g $(CFLAGS) -o path_set.o $(INCLUDE) path_set.cpp
	$(CC) -g $(CFLAGS) -o gpa_matching.o $(INCLUDE) gpa_matching.cpp
clean:
	rm -f *.o
	rm -f *.gch
library:
	make compile
library_debug:
	make compile_debug

