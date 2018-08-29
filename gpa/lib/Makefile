library:
	rm -f *.a
	cd matching && make library
	cd data_structure && make library
#	cd io && make library
	cd tools && make library
	ar -cvq libgpa_matching.a ./matching/path.o ./matching/path_set.o ./matching/gpa_matching.o ./data_structure/graph_access.h.gch ./data_structure/static_graph.h.gch ./data_structure/dyn_graph_access.o ./tools/random_functions.o

library-debug:
	rm -f *.a
	cd matching && make library-debug
	cd data_structure && make library-debug
#	cd io && make library
	cd tools && make library-debug
	ar -cvq libgpa_matching.a ./matching/path.o ./matching/path_set.o ./matching/gpa_matching.o ./data_structure/graph_access.h.gch ./data_structure/static_graph.h.gch ./data_structure/dyn_graph_access.o ./tools/random_functions.o
	
clean:
	cd algorithms && make clean
	cd data_structure && make clean
	cd tools && make clean
	rm -f *.o
	rm -f *.gch
