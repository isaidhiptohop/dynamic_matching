/******************************************************************************
 * baswanaguptasen_dyn_matching.h 
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

#ifndef BASWANAGUPTASEN_DYN_MATCHING_H
#define BASWANAGUPTASEN_DYN_MATCHING_H

#include <bitset>
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <chrono>

#include "dyn_matching.h"
#include "random_functions.h"
/*
struct RNG {
    std::mt19937 mt;
    
    int nextInt (int lb, int rb);
};
*/
class baswanaguptasen_dyn_matching : public dyn_matching {
public:
    baswanaguptasen_dyn_matching (dyn_graph_access* G);
    
    virtual EdgeID new_edge(NodeID source, NodeID target);
    
    virtual void remove_edge(NodeID source, NodeID target);
    
    virtual std::vector<std::pair<NodeID, NodeID> > getM ();
    
private:
    // O_u denotes the set of edges owned by u. we save those
    // edges by simply saving the edges endpoint, since every
    // edge owned by u also starts at u.
    std::vector<std::unordered_map<NodeID, NodeID> > O;
    std::vector<int> levels;
    
    // as in the simple algorithm, I save the matching in a
    // dyn_graph_access
    dyn_graph_access M;
    
    random_functions rng;
    
    bool is_free (NodeID u);
    void set_level (NodeID u, int level);
    int level (NodeID u);
    int level (NodeID u, NodeID v);
    NodeID * mate (NodeID u);
    void match (NodeID u, NodeID v);
    void unmatch (NodeID u, NodeID v);
    
    void handle_addition (NodeID u, NodeID v);
    void handling_insertion (NodeID u, NodeID v);
    
    void handle_deletion (NodeID u, NodeID v);
    void handling_deletion (NodeID u);
    
    NodeID * random_settle (NodeID u);
    void naive_settle (NodeID u);
};
/*
int RNG::nextInt (int lb, int rb) {
    std::uniform_int_distribution<unsigned int> A(lb, rb);
    return A(mt);
}


baswanaguptasen_dyn_matching::baswanaguptasen_dyn_matching (dyn_graph_access* G) :
dyn_matching(G) {
    O.resize(G->number_of_nodes());
    levels.resize(G->number_of_nodes());
    
    M.start_construction(G->number_of_nodes());
    
    for (int i = 0; i < G->number_of_nodes(); ++i) {
        std::cout << M.new_node();
    }
    
    M.finish_construction();
    
    auto a = static_cast<long unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    rng.mt.seed(a);
}

EdgeID baswanaguptasen_dyn_matching::new_edge(NodeID source, NodeID target) {
    // first add the node to the data structure G
    EdgeID e = G->new_edge(source, target);
    EdgeID e_bar = G->new_edge(target, source);
    
    handle_addition(source, target);
    
    return e;
}

void baswanaguptasen_dyn_matching::remove_edge(NodeID source, NodeID target) {

}

dyn_graph_access baswanaguptasen_dyn_matching::getM () {
    return M;
}

bool baswanaguptasen_dyn_matching::is_free (NodeID u) {
    return !M.getEdgesFromNode(u).size();
}

void baswanaguptasen_dyn_matching::set_level (NodeID u, int level) {
    ASSERT_TRUE(level >= 0 && level <= 1);
    levels.at(u) = level;
}

int baswanaguptasen_dyn_matching::level (NodeID u) {
    return levels.at(u);
}

int baswanaguptasen_dyn_matching::level (NodeID u, NodeID v) {
    return level(u) > level(v) ? level(u) : level(v);
}

NodeID * baswanaguptasen_dyn_matching::mate (NodeID u) {
    if (is_free(u)) {
        return nullptr;
    } else {
        ASSERT_TRUE(M.getEdgesFromNode(u).size() == 1);
        NodeID * mate_u = new NodeID;
        * mate_u = G->getEdgeTarget(u, G->get_first_edge(u));
        return mate_u;
    }
}

void baswanaguptasen_dyn_matching::match (NodeID u, NodeID v) {
    M.new_edge(u, v);
    M.new_edge(v, u);
}

void baswanaguptasen_dyn_matching::unmatch (NodeID u, NodeID v) {
    M.remove_edge(u, v);
    M.remove_edge(v, u);
}   

void baswanaguptasen_dyn_matching::handle_addition (NodeID u, NodeID v) {
    if (level(u) == 1) {
        ASSERT_TRUE(level(v) == 0);
        O.at(u).insert(v, v);
    } else if (level(v) == 1) {
        O.at(v).insert(u, u);
    } else {
        handling_insertion(u, v);
    }
}

void baswanaguptasen_dyn_matching::handling_insertion (NodeID u, NodeID v) {
    O.at(u).insert(v, v);
    O.at(v).insert(u, u);
    
    if (is_free(u) && is_free(v)) {
        match(u, v);
    }
    
    if (O.at(v).size() > O.at(u).size()) {
        NodeID tmp = u;
        u = v;
        v = tmp;
    }
    
    if (O.at(u).size() >= std::sqrt(G->number_of_nodes())) {
        NodeID * w = mate(u);
        for (auto w_pair : O.at(u)) {
            O.at(w_pair.second).erase(u);
        }
        
        NodeID * x = random_settle(u);
        
        if (x != nullptr) {
            naive_settle(*x);
        }
        
        if (w != nullptr) {
            naive_settle(*w);
        }
        
        delete x;
        delete w;
    }
}

NodeID * baswanaguptasen_dyn_matching::random_settle (NodeID u) {
    NodeID y;
    NodeID * x;
    int tmp = rng.nextInt(0, O.at(u).size());
    for (auto x : O.at(u)) { // not optimal... TODO
        if (tmp-- == 0) {
            y = x.second;
            break;
        }
    }
    
    for (auto w : O.at(y)) {
        O.at(w.second).erase(y);
    }
    
    if (!is_free(y)) { // if y is not free, it is matched
        x = mate(y);
        unmatch(*x, y);
    } else {
        x = nullptr;
    }
    
    match (u, y);
    set_level(u, 1);
    set_level(y, 1);
    
    return x;
}

void baswanaguptasen_dyn_matching::naive_settle (NodeID u) {
    for (auto x : O.at(u)) {
        if (is_free(x.second)) {
            match (u, x.second);
        }
    }
}
*/
#endif // BASWANAGUPTASEN_DYN_MATCHING_H
