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

#ifndef NEIMANSOLOMON_DYN_MATCHING_H
#define NEIMANSOLOMON_DYN_MATCHING_H

#include <bitset>
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>
//#include <random>
//#include <chrono>

#include "dyn_matching.h"
#include "avl_tree.h"
#include "maxNodeHeap.h"
#include "timer.h"

class neimansolomon_dyn_matching : public dyn_matching {
public:
    struct Match {
        NodeID id;
        NodeID mate;
        bool has_mate;
        
        Match ();
        
        Match (NodeID id, NodeID mate = 0, bool has_mate = true);
                
        Match (const Match& rop);
                
        bool operator== (Match& rop);

        bool operator< (Match& rop);

        bool operator> (Match& rop);
        
        std::ostream& print (std::ostream& o) const;
    };
    
    
    struct Fv {
        std::vector<bool> free;
        std::vector<NodeID> n_free;
        NodeID total_n;
        
        Fv (NodeID n);
        
        bool insert (NodeID node);
        
        bool remove (NodeID node);
        
        bool has_free ();
                
        // returns false if there is no free, and true + 
        // reference when there is a free neighbour
        bool get_free (NodeID& index);
    };
    
    neimansolomon_dyn_matching (dyn_graph_access* G);
    
    virtual EdgeID new_edge(NodeID source, NodeID target, double& elapsed);
    
    virtual void remove_edge(NodeID source, NodeID target, double& elapsed);
    
    virtual std::vector<std::pair<NodeID, NodeID> > getM ();
    
    virtual void counters_next();
    
private:
    // the matching M
    avl_tree<Match> M;
    
    // N(v) is the set of neighbours of n, which I save in N.
    // the degree deg(v) is simply the size of the avl_tree.
    std::vector<avl_tree<NodeID> > N;
    
    // F(v) is a data structure that holds the free neighbours
    // of a vertex v. We save this data structure for all v in
    // the vector F
    std::vector<Fv> F;
    
    // F_max holds all free vertices index by their degree
     maxNodeHeap F_max;
    
    // mate returns if a node "node" has a mate or not. if it has,
    // it assigns the mates value to the reference "mate".
    bool mate (NodeID node, NodeID& mate);
    
    // mate with a single parameter simply does not return the mate
    bool mate (NodeID node);
    
    // returns wether the node is free or not by checking, if it has a mate
    bool is_free (NodeID node);
    
    // checks wether the node has a free neighbour by performing F.at(node).has_free().
    // I wrap the Fv-structs function so that I can stick close to the paper syntax.
    bool has_free (NodeID node);
    
    // returns the free node of neighbour, assuming that has_free has been performed earlier
    NodeID get_free (NodeID node);
    
    // adds an edge to the matching M
    void match (NodeID u, NodeID v);
    
    // returns the degree of a node
    NodeID deg (NodeID u);
    
    // returns the value sqrt(2*n + 2*m), where n is the number of nodes and m the number of edges
    NodeID threshold ();
    
    // function to eliminate augmenting paths of length 3
    void aug_path (NodeID u);
    
    // find a surrogate node for u.
    NodeID surrogate (NodeID u);
    
    // function to wrap the matchings update
    void handle_addition (NodeID u, NodeID v);
    void handle_deletion (NodeID u, NodeID v);
};

std::ostream& operator<<(std::ostream& o, const neimansolomon_dyn_matching::Match& rop);

#endif // NEIMANSOLOMON_DYN_MATCHING_H
