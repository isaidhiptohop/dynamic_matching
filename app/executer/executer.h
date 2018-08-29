#ifndef EXECUTER_H
#define EXECUTER_H

#include "dyn_matching.h"
#include "naive_dyn_matching.h"
#include "simple_dyn_matching.h"
#include "randomwalkv2_dyn_matching.h"
#include "randomwalkv3_dyn_matching.h"
#include "baswanaguptasen_dyn_matching.h"
#include "neimansolomon_dyn_matching.h"

#include "tools/timer.h"
#include "tools/quality_metrics.h"
#include "io/graph_io.h"

#include "counters.h"

//#include "../GPAMatching/app/gpa_match.h"

/*
#include "config.h"
#include "data_structure/graph_access.h"
#include "io/graph_io.h"
#include "matching/gpa_matching.h"
#include "tools/macros_assertions.h"
#include "tools/random_functions.h"
#include "../GPAMatching/lib/tools/random_functions.h"
*/
#include <string>
#include <iostream>
#include <fstream>
#include <climits>
#include <algorithm>
 
enum ALGORITHM {bgs, gpa, naive, ns, rw_v1, rw_v2, rw_v3};
extern std::vector<std::string> ALGORITHM_NAMES;

struct ex_config {
    std::string file;
    long seed; // gpa seed
    size_t at_once;
    int multi_run;
    
    ex_config() {
        seed = 0;
        at_once = 1;
        multi_run = 1;
    }
};

// convert matching from std::vector<NodeID> to std::vector<std::pair<NodeID, NodeID> >
std::vector<std::pair<NodeID, NodeID> > matching_to_pairvec (const Matching & matching);

// print matching
void print_matching (std::ostream& o, const std::vector<std::pair<NodeID, NodeID> >& matching);

// get command line arguments
void check_option_arg (std::string option, int i, int argc, char** argv);
void get_arguments(int argc, char ** argv, ex_config& conf);

// file io
std::vector<std::string> split (const std::string& input, const char& mark);
size_t read_sequence (std::string file, std::vector<std::pair<int, std::pair<NodeID, NodeID> > >& edge_sequence);
std::string get_output_filename (ex_config conf);

// helper function to easily create graph
dyn_graph_access* create_graph (size_t n);

// auxiliary functions
EdgeID get_cumulated_degree(dyn_graph_access& G, std::vector<std::pair<NodeID, NodeID> > edgeset);
NodeID count_nodes(std::vector<bool>& nodes);
NodeID count_nodes(std::vector<int>& nodes, int min_degree = 0);

#endif // EXECUTER_H
