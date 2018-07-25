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
#include "../lib/io/graph_io.h"
#include "../lib/matching/gpa_matching.h"
#include "../lib/tools/timer.h"
#include "../lib/tools/macros_assertions.h"
#include "../lib/tools/random_functions.h"

#include <math.h>

//using namespace std;

struct file_reader {
    std::string directory;
    int iterator;
    int step;
    
    file_reader ();
    bool next_snapshot(basicGraph& realGraph);
};

#include "parse_parameters.h"

std::vector<std::pair<NodeID, NodeID> > matching_to_pairvec (const Matching & matching);
void validate (const std::vector<std::pair<NodeID, NodeID> >& matching);
void print_matching (std::ostream& o, const std::vector<std::pair<NodeID, NodeID> >& matching);

int main(int argn, char **argv) {

        Config config;
        file_reader freader;
        unsigned multi_run = 1;
        
        int ret_code = parse_parameters(argn, argv, config, freader, multi_run); 

        if(ret_code) {
                return 0;
        }
        
        basicGraph realGraph;

        skappa_timer t;
        
        srand(config.seed);
        random_functions::setSeed(config.seed);
        
        
        std::ofstream matchings_file(std::string(freader.directory + "matchings_gpa").c_str());
        std::ofstream data_file(std::string(freader.directory + "data_gpa").c_str());
        
        std::cout << std::string(freader.directory + "matchings_gpa") << " " << matchings_file.is_open() << std::endl;
        std::cout << std::string(freader.directory + "data_gpa") << " " << data_file.is_open() << std::endl;
        
        std::vector<double> time_elapsed;
        std::vector<EdgeID> graph_size;
        
        // dimensions: runs X sequence steps X edge set
        std::vector<std::vector<std::vector<std::pair<NodeID, NodeID> > > > matchings(multi_run);
        
        for (size_t i = 0; i < multi_run; ++i) {
            freader.iterator = freader.step;
            std::cout << "run " << i << "/" << multi_run << std::endl;
            
            int j = 0;
            while (freader.next_snapshot(realGraph)) {
                graph_access G(&realGraph);
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
                    graph_size.push_back(G.number_of_edges());
                } else {
                    time_elapsed.at(j) = t_elapsed / multi_run;
                    graph_size.at(j) = G.number_of_edges();
                    j++;
                }
            }
        }
        
        for (size_t i = 0; i < time_elapsed.size(); ++i) {
            data_file << graph_size.at(i)/2 << " " << matchings.at(0).at(i).size()/2 << " " << time_elapsed.at(i) << std::endl;
        }
        
        return 0;
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
            std::cout << "end of file." << std::endl;
            return false;
        }
    }
    
    graph_io::readGraphWeighted(realGraph, file_name);
    return true;
}
