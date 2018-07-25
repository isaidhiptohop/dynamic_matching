CC     =g++
CFLAGS =-g -c -std=gnu++11
INCLUDE=-I../data_structure -I../tools -I../

compile:
	$(CC) $(CFLAGS) -o path.o $(INCLUDE) path.cpp
	$(CC) $(CFLAGS) -o path_set.o $(INCLUDE) path_set.cpp
	$(CC) $(CFLAGS) -o gpa_matching.o $(INCLUDE) gpa_matching.cpp
clean:
	rm -f *.o
	rm -f *.gch
library:
	make compile
