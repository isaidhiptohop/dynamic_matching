library:
	rm -f *.a
	cd algorithms && make library
	cd data_structure && make library
	cd matching && make library
	cd io && make library
	cd tools && make library
	ar -cvq libdynamic_matching.a \
		./algorithms/simple_dyn_matching.o \
		./algorithms/baswanaguptasen_dyn_matching.o \
		./algorithms/neimansolomon_dyn_matching.o \
		./algorithms/dyn_matching.o \
		./matching/path.o \
		./matching/path_set.o \
		./matching/gpa_matching.o \
		./io/graph_io.o \
		./data_structure/priority_queues/maxNodeHeap.o \
		./data_structure/graph_access.o \
		./data_structure/dyn_graph_access.o \
		./data_structure/avl_tree.h.gch  \
		./tools/random_functions.o \
		./tools/quality_metrics.o \
		./tools/counters.o
	
library_counters:
	rm -f *.a
	cd algorithms && make library_counters
	cd data_structure && make library
	cd matching && make library
	cd io && make library
	cd tools && make library
	ar -cvq libdynamic_matching.a \
		./algorithms/simple_dyn_matching.o \
		./algorithms/baswanaguptasen_dyn_matching.o \
		./algorithms/neimansolomon_dyn_matching.o \
		./algorithms/dyn_matching.o \
		./matching/path.o \
		./matching/path_set.o \
		./matching/gpa_matching.o \
		./io/graph_io.o \
		./data_structure/priority_queues/maxNodeHeap.o \
		./data_structure/graph_access.o \
		./data_structure/dyn_graph_access.o \
		./data_structure/avl_tree.h.gch  \
		./tools/random_functions.o \
		./tools/quality_metrics.o \
		./tools/counters.o
	
library_counters_debug:
	rm -f *.a
	cd algorithms && make library_counters_debug
	cd data_structure && make library_debug
	cd matching && make library_debug
	cd io && make library_debug
	cd tools && make library_debug
	ar -cvq libdynamic_matching.a \
		./algorithms/simple_dyn_matching.o \
		./algorithms/baswanaguptasen_dyn_matching.o \
		./algorithms/neimansolomon_dyn_matching.o \
		./algorithms/dyn_matching.o \
		./matching/path.o \
		./matching/path_set.o \
		./matching/gpa_matching.o \
		./io/graph_io.o \
		./data_structure/priority_queues/maxNodeHeap.o \
		./data_structure/graph_access.o \
		./data_structure/dyn_graph_access.o \
		./data_structure/avl_tree.h.gch  \
		./tools/random_functions.o \
		./tools/quality_metrics.o \
		./tools/counters.o
	
library_debug:
	rm -f *.a
	cd algorithms && make library_debug
	cd data_structure && make library_debug
	cd matching && make library_debug
	cd io && make library_debug
	cd tools && make library_debug
	ar -cvq libdynamic_matching.a \
		./algorithms/simple_dyn_matching.o \
		./algorithms/baswanaguptasen_dyn_matching.o \
		./algorithms/neimansolomon_dyn_matching.o \
		./algorithms/dyn_matching.o \
		./matching/path.o \
		./matching/path_set.o \
		./matching/gpa_matching.o \
		./io/graph_io.o \
		./data_structure/priority_queues/maxNodeHeap.o \
		./data_structure/graph_access.o \
		./data_structure/dyn_graph_access.o \
		./data_structure/avl_tree.h.gch  \
		./tools/random_functions.o \
		./tools/quality_metrics.o \
		./tools/counters.o
	
clean:
	cd algorithms && make clean
	cd data_structure && make clean
	cd data_structure/priority_queues && make clean
	rm -f *.o
	rm -f *.gch
