/******************************************************************************
 * gpa_matching.cpp
 *
 * Source of GPA Matching Code 
 *
 ******************************************************************************
 * Copyright (C) 2013 Christian Schulz <christian.schulz@kit.edu>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <string.h>
#include <unordered_map>

#include <argtable2.h>
#include <regex.h>

#include "../lib/config.h"
#include "../lib/data_structure/graph_access.h"
#include "../lib/data_structure/dyn_graph_access.h"
#include "../lib/io/graph_io.h"
#include "../lib/matching/gpa_matching.h"
#include "../lib/tools/timer.h"
#include "../lib/tools/macros_assertions.h"
#include "../lib/tools/random_functions.h"

#include <math.h>

#include "parse_parameters.h"

std::vector<std::string> split (const std::string& input, const char& mark);
size_t read_sequence (std::string file, std::vector<std::pair<int, std::pair<NodeID, NodeID> > >& edge_sequence);
basicGraph& init (size_t n, basicGraph& graph);
basicGraph& build_from_sequence (std::vector<std::pair<int, std::pair<NodeID, NodeID> > >& edge_sequence, size_t iterator, size_t step, basicGraph& graph);
    
std::vector<std::pair<NodeID, NodeID> > matching_to_pairvec (const Matching & matching);
void validate (const std::vector<std::pair<NodeID, NodeID> >& matching);
void print_matching (std::ostream& o, const std::vector<std::pair<NodeID, NodeID> >& matching);

int main(int argn, char **argv) {
    try {
        Config config;
        std::string fileinput = "";
        unsigned step = 1;
        unsigned multi_run = 1;
        
        int ret_code = parse_parameters(argn, argv, config, fileinput, step, multi_run); 
        
        if(ret_code) {
                return 0;
        }
        
        basicGraph realGraph;
        
        skappa_timer t;
        
        srand(config.seed);
        random_functions::setSeed(config.seed);
        
        // fileinput should be of format "sequences/name.sequence", output directory should be "output/name_step/"
        std::string directory = std::string("output/" + split(split(fileinput, '/').at(1), '.').at(0) + "_" + std::to_string(step) + "/");
        
        std::ofstream matchings_file(std::string(directory + "matchings_gpa").c_str());
        std::ofstream data_file(std::string(directory + "data_gpa").c_str());
        
        std::vector<double> time_elapsed;
        std::vector<EdgeID> graph_size;
        
        // input sequence
        std::vector<std::pair<int, std::pair<NodeID, NodeID> > > edge_sequence;
        size_t n = read_sequence(fileinput, edge_sequence);
        
        // dimensions: runs X sequence steps X edge set
        std::vector<std::vector<std::vector<std::pair<NodeID, NodeID> > > > matchings(multi_run);
        
        for (size_t i = 0; i < multi_run; ++i) {
            dyn_graph_access dynG;
            
            // initialize dyn_graph_access dynG
            dynG.start_construction(n, 0);
            
            for (size_t j = 0; j < n; ++j) {
                dynG.new_node();
            }
            
            dynG.finish_construction();
            
            // results iterator
            int j = 0;
            for (size_t k = 0; k < edge_sequence.size(); k += step) {
                for (size_t l = k; l < k + step && l < edge_sequence.size(); ++l) {
                    // apply sequence steps
                    if (edge_sequence.at(l).first) {
                        dynG.new_edge(edge_sequence.at(l).second.first, edge_sequence.at(l).second.second);
                        dynG.new_edge(edge_sequence.at(l).second.second, edge_sequence.at(l).second.first);
                    } else {
                        dynG.remove_edge(edge_sequence.at(l).second.first, edge_sequence.at(l).second.second);
                        dynG.remove_edge(edge_sequence.at(l).second.second, edge_sequence.at(l).second.first);
                    }
                }
                
                // create static instance from dynamic graph
                basicGraph * realGraph = dynG.convert_to_basic_graph();
//                printGraph(*realGraph);
                
                graph_access G (realGraph);
                
                double t_elapsed;
                
                t.restart();
                
                gpa_matching match;
                Matching M;
                match.match(config, G, M);
                
                t_elapsed = t.elapsed();
                
                std::vector<std::pair<NodeID, NodeID>> matching = matching_to_pairvec(M);
                
                validate(matching);
                
                matchings.at(i).push_back(matching);
                
                if (i == 0) {
                    print_matching(matchings_file, matching);
                    time_elapsed.push_back(t_elapsed / multi_run);
                    graph_size.push_back(G.number_of_edges()/2);
                } else {
                    time_elapsed.at(j) = t_elapsed / multi_run;
                    graph_size.at(j) = G.number_of_edges()/2;
                    j++;
                }
                
                delete realGraph;
            }
        }
        
        data_file << "# G M time" << std::endl;
        
        for (size_t i = 0; i < time_elapsed.size(); ++i) {
            data_file << graph_size.at(i) << " " << matchings.at(0).at(i).size()/2 << " " << time_elapsed.at(i) << std::endl;
        }
        
        return 0;
    } catch (std::bad_alloc e) {
        std::cout << e.what() << std::endl;
    }
}

std::vector<std::pair<NodeID, NodeID> > matching_to_pairvec (const Matching & matching) {
    std::vector<std::pair<NodeID, NodeID> > pairvec;
    
    for (size_t i = 0; i < matching.size(); ++i) {
        if (matching.at(i) != i) {
            pairvec.push_back({i, matching.at(i)});
        }
    }
    
    return pairvec;
}

void validate (const std::vector<std::pair<NodeID, NodeID> >& matching) {
    // do this once the left side...
    std::unordered_map<NodeID, NodeID> nodes_u;
    
    // go through matched edges
    for (auto edge : matching) {
        // assert that key u of edge (u,v) is not found in the map
        ASSERT_TRUE(nodes_u.find(edge.first) == nodes_u.end());
        // add the edge to the map. if another edge should be incident
        // to the same node u, than the above assertion will fail.
        nodes_u[edge.first] = edge.second;
    }
    
    // ...and once the right side
    std::unordered_map<NodeID, NodeID> nodes_v;
    
    // same as above, but checking with v as key of (u,v)
    for (auto edge : matching) {
        ASSERT_TRUE(nodes_v.find(edge.second) == nodes_v.end());
        nodes_v[edge.second] = edge.first;
    }
}

void print_matching (std::ostream& o, const std::vector<std::pair<NodeID, NodeID> >& matching) {
    for (size_t i = 0; i < matching.size(); ++i) {
        o << matching.at(i).first << " " << matching.at(i).second << "; ";
    }
    o << std::endl;
}

std::vector<std::string> split (const std::string& input, const char& mark) {
    std::vector<std::string> substrings;
    std::string buf = "";
    
    for (size_t i = 0; i < input.size(); ++i) {
        if (input.at(i) != mark) {
            buf = buf + input.at(i);
        } else {
            substrings.push_back(buf);
            buf = "";
        }
    }
    
    if (buf != "") {
        substrings.push_back(buf);
    }
    
    return substrings;
}

size_t read_sequence (std::string file, std::vector<std::pair<int, std::pair<NodeID, NodeID> > >& edge_sequence) {
    std::string line;
    std::ifstream input(file);
    edge_sequence.resize(0);
    size_t n = 0;
    int i = 0;
    
    if (input.is_open()) {
        std::getline(input, line);
        std::vector<std::string> substr = split(line, ' ');
        
        std::string hash = substr.at(0);
        if (hash != "#") throw std::string("META DATA SEEMS TO BE MISSING");
        
//        n = std::stoul(substr.at(1).c_str());
        
        while (std::getline(input, line)) {
            i++;
            std::vector<std::string> substr = split(line, ' ');
            
            int addition = atoi(substr.at(0).c_str());
            NodeID u = atoi(substr.at(1).c_str());
            NodeID v = atoi(substr.at(2).c_str());
            
            if (u > n) n = u;
            if (v > n) n = v;
            
            edge_sequence.push_back({addition, {u, v}});
        }
    }
    
    input.close();
    
    return n+1;
}
/*
file_reader::file_reader() {
    directory = "";
    iterator = 0;
    step = 1;
}

bool file_reader::next_snapshot(basicGraph& realGraph) {
    std::string file_name = directory + "snapshots/snapshot_" + std::to_string(iterator) + ".graph";
    iterator += step;
    
    {
        ifstream f(file_name.c_str());
        if(!f.good()) {
            return false;
        }
    }
    
    graph_io::readGraphWeighted(realGraph, file_name);
    return true;
}
*/
