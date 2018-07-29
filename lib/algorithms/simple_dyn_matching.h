/******************************************************************************
 * simple_dyn_matching.h 
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

#ifndef SIMPLE_DYN_MATCHING_H
#define SIMPLE_DYN_MATCHING_H

#include <bitset>
#include <cassert>
#include <iostream>
#include <vector>
#include <random>
#include <chrono>

#include "dyn_matching.h"
#include "random_functions.h"
#include "timer.h"

/*
struct RNG {
    std::mt19937 mt;
    
    int nextInt (int lb, int rb);
};
*/
class simple_dyn_matching : public dyn_matching {
public:
    simple_dyn_matching (dyn_graph_access* G, double eps);
    
    virtual EdgeID new_edge(NodeID source, NodeID target, double& elapsed);
    
    virtual void remove_edge(NodeID source, NodeID target, double& elapsed);
    
    virtual std::vector<std::pair<NodeID, NodeID> > getM ();
    
    virtual void counters_next();
    
private:
    dyn_graph_access M;
    double eps;
//    random_functions rng;
    
    // checks whether a vertex is free
    bool freeVertex (NodeID node);
    
    // checks whether the edge (source, target) is matched
    bool isMatched (NodeID source, NodeID target);
    
    // returns the mate of source
    NodeID getMatchedEdge (NodeID source);
    
    void match (NodeID u, NodeID v);
    void unmatch (NodeID u, NodeID v);
    
    void solve_conflict (NodeID u, int step);
};

#endif // SIMPLE_DYN_MATCHING_H
