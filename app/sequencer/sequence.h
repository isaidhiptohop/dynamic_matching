/*
 * the sequencer creates sequences of edge insertions and deletions.
 * this can be done randomly, using some specific method or by reading
 * dynamic graphs from file.
 * 
 * initially, it will support creating
 * - sequences with n nodes, k steps and only random additions 
 * - sequences with n nodes, k steps and random additions and deletions
 * - sequences with n nodes, k steps and an average size of m edges (sliding window), constructed using random additions and deletions
 * - sequences with n nodes, k steps and an average size of m edges, where additions/deletions follow the idea of bergamini and meyerhenke.
 * - sequences from koblenz-files, where n and k are read from the file (only additions)
 * - sequences from koblenz-files, where n and k are read from the file and which maintains an average size of m edges (sliding window)
 * - sequences from koblenz-files, where n and k are read from the file and which maintains an average size of m edges (bergamini, meyerhenke)
 * - sequences from koblenz-files with n nodes, k steps, where n and k are given (only additions)
 * - sequences from koblenz-files with n nodes, k steps, where n and k are given and which maintains an average size of m edges (sliding window)
 * - sequences from koblenz-files with n nodes, k steps, where n and k are give and which maintains an average size of m edges (bergamini, meyerhenke)
 * 
 */

#ifndef SEQUENCE_H
#define SEQUENCE_H

#include "dyn_graph_access.h"
#include "random_functions.h"
#include "timer.h"

#include <string>
#include <iostream>
#include <fstream>
#include <climits>
#include <algorithm>
#include <chrono>


enum MODE {only_addition, random_step, sliding_window, meyerhenke, pooled_meyerhenke, native};
extern std::vector<std::string> MODE_NAMES;

class sequence {
public:
    sequence (size_t n, size_t k, MODE mode, size_t window);
    sequence (size_t n, size_t k, MODE mode, size_t window, size_t poolsize, long seed, std::string ifile = "");
    sequence (size_t n, size_t k, MODE mode, size_t window, std::string ifile);
    
    std::vector<std::string> split (const std::string& input, const char& mark);
    std::pair<NodeID, NodeID> load_from_file ();
    std::pair<int, std::pair<NodeID, NodeID> > create_edge (const random_functions& rng);
    
    void create();
    void finish();
    
    void create_only_addition_seq();
    void create_random_seq();
    void create_sliding_window_seq();
    void create_meyerhenke_seq();
    void create_pooled_meyerhenke_seq();
    void create_native_seq();
    
    std::string get_name();
    void print_last();
    
private:
    NodeID count_nodes();
    
    size_t n;
    size_t k;
    MODE mode;
    
    size_t window;
    size_t poolsize;
    long seed;
    
    std::string ifile;
    std::ofstream ofile;
    
    size_t add_count;
    size_t del_count;
    
    int helper;
    
    bool built;
    
    std::vector<bool> nodes;
    
    std::vector<std::pair<int, std::pair<NodeID, NodeID> > > buf;
    size_t it;
    
    std::vector<std::pair<int, std::pair<NodeID, NodeID> > > edge_sequence;
};

#endif // SEQUENCE_H
