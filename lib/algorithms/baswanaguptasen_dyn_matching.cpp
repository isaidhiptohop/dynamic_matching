#include "baswanaguptasen_dyn_matching.h"
/*
int RNG::nextInt (int lb, int rb) {
    std::uniform_int_distribution<unsigned int> A(lb, rb);
    return A(mt);
}
*/

baswanaguptasen_dyn_matching::baswanaguptasen_dyn_matching (dyn_graph_access* G) :
dyn_matching(G) {
    O.resize(G->number_of_nodes());
    levels.resize(G->number_of_nodes());
    
    M.start_construction(G->number_of_nodes());
    
    for (int i = 0; i < G->number_of_nodes(); ++i) {
        M.new_node();
    }
    
    M.finish_construction();
    
//    auto a = static_cast<long unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
//    rng.setSeed(a);
}

baswanaguptasen_dyn_matching::~baswanaguptasen_dyn_matching() {
//    delete G;
}

EdgeID baswanaguptasen_dyn_matching::new_edge(NodeID source, NodeID target) {
    // first add the node to the data structure G
    EdgeID e = G->new_edge(source, target);
    EdgeID e_bar = G->new_edge(target, source);
    
    handle_addition(source, target);
    
    return e;
}

void baswanaguptasen_dyn_matching::remove_edge(NodeID source, NodeID target) {
    G->remove_edge(source, target);
    G->remove_edge(target, source);
    
    handle_deletion(source, target);
}

std::vector<std::pair<NodeID, NodeID> > baswanaguptasen_dyn_matching::getM () {
    std::vector<std::pair<NodeID, NodeID> > M_vector;
    for(NodeID n = 0, n_end = M.number_of_nodes(); n < n_end; ++n) {
        for(EdgeID e = 0, e_end = M.getEdgesFromNode(n).size(); e < e_end; ++e) {
            M_vector.push_back({n, M.getEdgeTarget(n,e)});
        }
    }
    
    return M_vector;
}
/*
dyn_graph_access baswanaguptasen_dyn_matching::getM () {
    return M;
}
*/
bool baswanaguptasen_dyn_matching::is_free (NodeID u) {
    return M.getEdgesFromNode(u).size() == 0;
}

void baswanaguptasen_dyn_matching::set_level (NodeID u, int level) {
    ASSERT_TRUE(level >= 0 && level <= 1);
    levels.at(u) = level;
}

int baswanaguptasen_dyn_matching::level (NodeID u) {
    return levels.at(u);
}

// returns level of an edge, which is defined as max-level of both endnodes
int baswanaguptasen_dyn_matching::level (NodeID u, NodeID v) {
    return level(u) > level(v) ? level(u) : level(v);
}

NodeID * baswanaguptasen_dyn_matching::mate (NodeID u) {
    if (is_free(u)) {
        return nullptr;
    } else {
        if (M.getEdgesFromNode(u).size() != 1) {
            std::cout << "M.getEdgesFromNode(" << u << ").size(): " << M.getEdgesFromNode(u).size() << std::endl;
        }
        ASSERT_TRUE(M.getEdgesFromNode(u).size() == 1);
        NodeID * mate_u = new NodeID;
        * mate_u = M.getEdgeTarget(u, M.get_first_edge(u));
        return mate_u;
    }
}

void baswanaguptasen_dyn_matching::match (NodeID u, NodeID v) {
//    ASSERT_TRUE(mate(u) == nullptr);
//    ASSERT_TRUE(mate(v) == nullptr);
    ASSERT_TRUE(M.getEdgesFromNode(u).size() == 0);
    ASSERT_TRUE(M.getEdgesFromNode(v).size() == 0);
    M.new_edge(u, v);
    M.new_edge(v, u);
}

void baswanaguptasen_dyn_matching::unmatch (NodeID u, NodeID v) {
    ASSERT_TRUE(M.getEdgesFromNode(u).size() == 1);
    ASSERT_TRUE(M.getEdgesFromNode(v).size() == 1);
    M.remove_edge(u, v);
    M.remove_edge(v, u);
}   

void baswanaguptasen_dyn_matching::handle_addition (NodeID u, NodeID v) {
    if (level(u) == 1) {
//        ASSERT_TRUE(level(v) == 0);
        O.at(u).insert({v, v});
    } else if (level(v) == 1) {
        O.at(v).insert({u, u});
    } else { // both at level 0
        handling_insertion(u, v);
    }
}

void baswanaguptasen_dyn_matching::handling_insertion (NodeID u, NodeID v) {
    // handling_insertion gets called when both nodes are on level 0,
    // therefore both nodes own the edge. since I only have to save the
    // target node (source node is defined by the index in the hash map 
    // array), but the unordered_map uses pairs, I add a pair, where 
    // value=key
    O.at(u).insert({v, v});
    O.at(v).insert({u, u});
    
    // if both nodes are free, simply match them
    if (is_free(u) && is_free(v)) {
        match(u, v);
    }
    
    // swap the nodes, so that u owns more edges than v
    if (O.at(v).size() > O.at(u).size()) {
        NodeID tmp = u;
        u = v;
        v = tmp;
    }
    
    if (O.at(u).size() >= std::sqrt(G->number_of_nodes())) {
        // (u,w) becomes unmatched, if w is the mate of u.
        NodeID * w = mate(u);
        
        if (w != nullptr) {
            unmatch(u, *w);
        }
        
        // since u is at level 0, but violates invariant 2,
        // it will rise to level 1. therefore we assign u as
        // the only owner of all adjacent edges.
        for (auto w_pair : O.at(u)) {
            O.at(w_pair.second).erase(u);
        }
        
        NodeID * x = random_settle(u);
        
        if (x != nullptr) {
            naive_settle(*x);
        }
        
        if (w != nullptr && is_free(*w)) {
            naive_settle(*w);
        }
        
        delete x;
        delete w;
    }
}

void baswanaguptasen_dyn_matching::handle_deletion (NodeID u, NodeID v) {
    // whatever happens afterwards, the edge (u,v) does not longer
    // exist in the graph, therefore all nodes have to forget the
    // respective endpoint as their neighbour and update their set
    // of owned edges. otherwise the algorithm will match the nodes
    // directly again.
    O.at(u).erase(v);
    O.at(v).erase(u);
    
    // if (u,v) is not in M, then there's nothing to do
    if (!M.isEdge(u,v)) {
        ASSERT_TRUE(!M.isEdge(v,u));
//        std::cout << "the edge (" << u << "," << v << ") was not in M" << std::endl;
        return;
    }
    
    // if we didn't return, we remove (u,v) from M
    unmatch(u,v);
    
    // if (u,v) was in M and...
    if (level(u,v) == 0) { // ... (u,v) was at level 0
        // maybe here is missing some update on O.at({u,v}).
        // the paper doesn't say anything about it, but this
        // seems suspicious to me, since neither u nor v can
        // own the edge {u,v} anymore.
        // after deleting an edge, it still appears since the
        // edge is still present in the O_u/O_v set. therefore
        // i try deleting it from O
        // 
        // RESULT: it turned out, that in any case, whether 
        // the edge is part of the matching or whatever level
        // every node has to forget about the neighbour to
        // which it was connected by this edge. therefore I
        // combined this deletion at the very begin of the
        // function HANDLE_DELETION
        naive_settle(u);
        naive_settle(v);
    }
    
    if (level(u,v) == 1) {
        handling_deletion(u);
        handling_deletion(v);
    }
}

void baswanaguptasen_dyn_matching::handling_deletion (NodeID u) {
    std::vector<NodeID> buf;
    for (auto w : O.at(u)) {
        buf.push_back(w.second);
    }
    
    // all edges with endpoint w at level 1 take the ownership of (u,w)
    for (auto w : buf) {
        if (level(w) == 1) {
            O.at(w).insert({u, u});
            
            // TODO: removing content from the data structure over which
            // I iterate causes invalid reads.
            O.at(u).erase(w);
        }
    }
    
    if (O.at(u).size() >= std::sqrt(G->number_of_nodes())) {
        // if O_u is still greater or equal than sqrt(n), it stays at level 1 
        // and therefore has to be matched (invariant 1)
        NodeID * x = random_settle(u);
        if (x != nullptr) {
            naive_settle(*x);
        }
    } else {
        // otherwise, if O_u is now less than sqrt(n), it drops to level 0
        set_level(u, 0);
        for (auto w : O.at(u)) {
            // edges where both endpoints are level 0 are owned by both endpoints
            if (level(w.second) == 0) {
                O.at(w.second).insert({u, u});
            }
        }
        
        naive_settle(u);
        
        // the set of owned edges of the previously update neighbours increased
        // by 1, therefore they might violate invariant 2.
        for (auto w : O.at(u)) {
            if (O.at(u).size() >= std::sqrt(G->number_of_nodes())) {
                NodeID * mate_w = mate(w.second);
                if (mate_w != nullptr) {
                    unmatch(w.second, *mate_w);
                }
                
                NodeID * x = random_settle(w.second);
                
                if (x != nullptr) {
                    naive_settle(*x);
                }
                
                if (mate_w != nullptr) {
                    naive_settle(*mate_w);
                }
                
                delete x;
            }
        }
    }
}

NodeID * baswanaguptasen_dyn_matching::random_settle (NodeID u) {
    NodeID y;
    int tmp = random_functions::nextInt(0, O.at(u).size() - 1);
    for (auto x : O.at(u)) { 
        if (tmp-- == 0) {
            y = x.second;
            break;
        }
    }
    ASSERT_TRUE(tmp == -1);
    
    for (auto w : O.at(y)) {
        O.at(w.second).erase(y);
    }
    
    NodeID * x;
    if (!is_free(y)) { // if y is not free, it is matched
        x = mate(y);
        unmatch(*x, y);
    } else {
        x = nullptr;
    }
    
    match (u, y);
    //TODO
    set_level(u, 1);
    set_level(y, 1);
    
    return x;
}

void baswanaguptasen_dyn_matching::naive_settle (NodeID u) {
    ASSERT_TRUE(is_free(u));
    for (auto x : O.at(u)) {
        if (is_free(x.second)) {
            match (u, x.second);
            break;
        }
    }
}

