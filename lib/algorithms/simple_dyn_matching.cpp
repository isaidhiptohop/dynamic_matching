/******************************************************************************
 * simple_dyn_matching.cpp
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

#include "simple_dyn_matching.h"

#ifdef DM_COUNTERS
    #include "counters.h"
#endif
/*
int RNG::nextInt (int lb, int rb) {
    std::uniform_int_distribution<unsigned int> A(lb, rb);
    return A(mt);
}
*/

simple_dyn_matching::simple_dyn_matching (dyn_graph_access* G, double eps) : dyn_matching(G) {
    M.start_construction(G->number_of_nodes());
    
    for (int i = 0; i < G->number_of_nodes(); ++i) {
        M.new_node();
    }
    
    M.finish_construction();

    this->eps = eps;
    
    #ifdef DM_COUNTERS
        counters::new_counter(std::string("matches_per_step_simple" + std::to_string(eps)));
        counters::new_counter(std::string("solve_conflict" + std::to_string(eps)));
    #endif
//    auto a = static_cast<long unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
//        auto a = std::chrono::system_clock::now();
//    rng.setSeed(a);
}

EdgeID simple_dyn_matching::new_edge(NodeID source, NodeID target) {
    EdgeID e = G->new_edge(source, target);
    EdgeID e_bar = G->new_edge(target, source);
    
    // starting calculation of matching
    
    // check whether the vertices are free. if so, add to the matching
    if (freeVertex(source) && freeVertex(target)) {
        match (source, target);
    }
    
    // finished calculation of matching
    
    return e;
}

void simple_dyn_matching::remove_edge(NodeID source, NodeID target) {
    G->remove_edge(source, target);
    G->remove_edge(target, source);
    
    /* starting calculation of matching */
    
    if (isMatched(source, target)) {
        unmatch(source, target);
        
        ASSERT_TRUE(!isMatched(source, target));
        
        solve_conflict(source, 0);
        solve_conflict(target, 0);
    }
    
    /* finished calculation of matching */
}

std::vector<std::pair<NodeID, NodeID> > simple_dyn_matching::getM () {
    std::vector<std::pair<NodeID, NodeID> > M_vector;
    for(NodeID n = 0, n_end = M.number_of_nodes(); n < n_end; ++n) {
        for(EdgeID e = 0, e_end = M.getEdgesFromNode(n).size(); e < e_end; ++e) {
            M_vector.push_back({n, M.getEdgeTarget(n,e)});
        }
    }
    
    return M_vector;
}

/*
dyn_graph_access simple_dyn_matching::getM () {
    return M;
}
*/
// checks whether a vertex is free
bool simple_dyn_matching::freeVertex (NodeID node) {
    return M.getEdgesFromNode(node).size() == 0;
}

// checks whether an edge is matched or not
bool simple_dyn_matching::isMatched (NodeID source, NodeID target) {
///*
    bool a = M.isEdge(source, target);
    bool b = M.isEdge(target, source);
    ASSERT_TRUE(a == b);
//*/
    return M.isEdge(source, target) || M.isEdge(target, source);
}

void simple_dyn_matching::match (NodeID u, NodeID v) {
    ASSERT_TRUE(M.getNodeDegree(u) == 0);
    ASSERT_TRUE(M.getNodeDegree(v) == 0);
    ASSERT_TRUE(!isMatched(u, v));
    M.new_edge(u, v);
    M.new_edge(v, u);
    ASSERT_TRUE(isMatched(u, v));
    ASSERT_TRUE(M.getNodeDegree(u) == 1);
    ASSERT_TRUE(M.getNodeDegree(v) == 1);
    
    #ifdef DM_COUNTERS
        counters::get(std::string("matches_per_step_simple" + std::to_string(eps))).inc();
    #endif
}

void simple_dyn_matching::unmatch (NodeID u, NodeID v) {
    ASSERT_TRUE(isMatched(u, v));
    ASSERT_TRUE(M.getNodeDegree(u) == 1);
    ASSERT_TRUE(M.getNodeDegree(v) == 1);
    M.remove_edge(u, v);
    M.remove_edge(v, u);
    ASSERT_TRUE(M.getNodeDegree(u) == 0);
    ASSERT_TRUE(M.getNodeDegree(v) == 0);
    ASSERT_TRUE(!isMatched(u, v));
}

NodeID simple_dyn_matching::getMatchedEdge (NodeID source) {
    if (freeVertex(source)) return -1;
    
    // not more than one edge per node allowed for a matching
/*
    if (M.getEdgesFromNode(source).size() != 1) {
        std::cout << "going to fail... " << source << " has " << M.getEdgesFromNode(source).size() << " matched edges: " << std::endl;
        for (auto e : M.getEdgesFromNode(source)) {
            std::cout << e.target << " ";
        }
        std::cout << std::endl;
    }
*/ 
    ASSERT_TRUE(M.getEdgesFromNode(source).size() == 1);
    return M.getEdgesFromNode(source).at(0).target;
}

void simple_dyn_matching::solve_conflict (NodeID u, int step) {
    // do this recursion maximal 1/eps times
    if (step >= (1.0/eps) || !freeVertex(u)) return;
    
    // all edges between index 0 and max_index-1 really exist
    EdgeID max_index = G->get_first_invalid_edge(u);
    
    // if the max_index is -1, then u holds 0 edges. in this case
    // we don't choose a random edge, but simply quit searching
    // for matchable edges.
    if (max_index <= 0) {
        return;
    }
    
    EdgeID new_matching_edge = random_functions::nextInt(0, max_index-1);
    NodeID v = G->getEdgeTarget(u, new_matching_edge);
    
    // if the target node v is not free, remove the matched edge (v,w)
    // add the edge (u,v) and call the functions recursively on w
    if (!freeVertex(v)) {
        NodeID w = getMatchedEdge(v);
        
        // remove the matched edge (v,w)
        unmatch(v,w);
        
        // add the edge (u,v) to the matching
        match(u,v);
        
        solve_conflict(w, ++step);
    } else {
        // no conflict, no trouble. simply add the edge to the matching.
        match(u,v);
    }
    
    #ifdef DM_COUNTERS
        counters::get(std::string("solve_conflict" + std::to_string(eps))).inc();
    #endif
}

void simple_dyn_matching::counters_next() {
    #ifdef DM_COUNTERS
        counters::get(std::string("matches_per_step_simple" + std::to_string(eps))).next();
        counters::get(std::string("solve_conflict" + std::to_string(eps))).next();
    #endif
}
