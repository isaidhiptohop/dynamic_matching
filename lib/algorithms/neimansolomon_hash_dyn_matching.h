/******************************************************************************
 * neiman_solomon_dyn_matching.h 
 *
 *
 ******************************************************************************
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 2 of the License, or (at your option)
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

#ifndef NEIMANSOLOMON_HASH_DYN_MATCHING_H
#define NEIMANSOLOMON_HASH_DYN_MATCHING_H

#include <bitset>
#include <cassert>
#include <iostream>
#include <vector>
#include <random>
#include <chrono>

#include "dyn_matching.h"

struct RNG {
    std::mt19937 mt;
    
    int nextInt (int lb, int rb) {
        std::uniform_int_distribution<unsigned int> A(lb, rb);
        return A(mt);
    }
};

class neimansolomon_hash_dyn_matching : public dyn_matching {
public:
    neimansolomon_hash_dyn_matching (dyn_graph_access* G) : dyn_matching(G) {
        
    }
    
    virtual EdgeID new_edge(NodeID source, NodeID target) {
    }
    
    virtual void remove_edge(NodeID source, NodeID target) {
    }
private:
    // the data structure N(v), which is a vector of hash maps which save
    // <id, index>-pairs and a vector of vectors where each inner vector i
    //  saves the neighbours of a node with id=i
    std::vector<std::vector<NodeID> > neighbour_n;
    std::vector<std::unordered_map<NodeID, size_t> > neighbour_n_maps;
    
    // a vector that saves the degree of each node
    std::vector<size_t> deg_n;
};

#endif // NEIMANSOLOMON_HASH_DYN_MATCHING_H
