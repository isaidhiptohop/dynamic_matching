library:
	rm -f *.a
	cd matching && make library
	cd data_structure && make library
#	cd io && make library
	cd tools && make library
	ar -cvq libgpa_matching.a ./matching/path.o ./matching/path_set.o ./matching/gpa_matching.o ./data_structure/graph_access.o ./tools/random_functions.o
	
clean:
	cd algorithms && make clean
	cd data_structure && make clean
	cd tools && make clean
	rm -f *.o
	rm -f *.gch
