/******************************************************************************
 * randomwalkv4_dyn_matching.cpp
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

#include "randomwalkv4_dyn_matching.h"

#ifdef DM_COUNTERS
    #include "counters.h"
#endif
/*
int RNG::nextInt (int lb, int rb) {
    std::uniform_int_distribution<unsigned int> A(lb, rb);
    return A(mt);
}
*/

randomwalkv4_dyn_matching::randomwalkv4_dyn_matching (dyn_graph_access* G, double eps) : dyn_matching(G) {
    M.start_construction(G->number_of_nodes());
    
    this->eps = eps;
    
    for (int i = 0; i < G->number_of_nodes(); ++i) {
        M.new_node();
    }
    
    M.finish_construction();

    #ifdef DM_COUNTERS
        counters::create("rw_v4");
        counters::get("rw_v4").create("match()");
    #endif
//    auto a = static_cast<long unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
//        auto a = std::chrono::system_clock::now();
//    rng.setSeed(a);
}

bool randomwalkv4_dyn_matching::new_edge(NodeID source, NodeID target, double& elapsed) {
    timer t;
    
    bool foo = G->new_edge(source, target);
    bool bar = G->new_edge(target, source);
    
    ASSERT_TRUE(foo == bar);
    
    // starting calculation of matching
    t.restart();
    
    // check whether the vertices are free. if so, add to the matching
    if (freeVertex(source) && freeVertex(target)) {
        match (source, target);
    }
    
    elapsed = t.elapsed();
    
    // finished calculation of matching
    
    return foo;
}

bool randomwalkv4_dyn_matching::remove_edge(NodeID source, NodeID target, double& elapsed) {
    timer t;
    
    bool foo = G->remove_edge(source, target);
    bool bar = G->remove_edge(target, source);
    
    ASSERT_TRUE(foo == bar);
    
    /* starting calculation of matching */
    
    t.restart();
    
    if (isMatched(source, target)) {
        unmatch(source, target);
        
        ASSERT_TRUE(!isMatched(source, target));
        
        solve_conflict(source, 0);
        solve_conflict(target, 0);
    }
    
    elapsed = t.elapsed();
    
    /* finished calculation of matching */
    
    return foo;
}

std::vector<std::pair<NodeID, NodeID> > randomwalkv4_dyn_matching::getM () {
    std::vector<std::pair<NodeID, NodeID> > M_vector;
    for(NodeID n = 0, n_end = M.number_of_nodes(); n < n_end; ++n) {
        for(EdgeID e = 0, e_end = M.getEdgesFromNode(n).size(); e < e_end; ++e) {
            M_vector.push_back({n, M.getEdgeTarget(n,e)});
        }
    }
    
    return M_vector;
}

/*
dyn_graph_access randomwalkv4_dyn_matching::getM () {
    return M;
}
*/
// checks whether a vertex is free
bool randomwalkv4_dyn_matching::freeVertex (NodeID node) {
    return M.getEdgesFromNode(node).size() == 0;
}

// checks whether an edge is matched or not
bool randomwalkv4_dyn_matching::isMatched (NodeID source, NodeID target) {
///*
    bool a = M.isEdge(source, target);
    bool b = M.isEdge(target, source);
    ASSERT_TRUE(a == b);
//*/
    return M.isEdge(source, target) || M.isEdge(target, source);
}

void randomwalkv4_dyn_matching::match (NodeID u, NodeID v) {
    ASSERT_TRUE(M.getNodeDegree(u) == 0);
    ASSERT_TRUE(M.getNodeDegree(v) == 0);
    ASSERT_TRUE(!isMatched(u, v));
    M.new_edge(u, v);
    M.new_edge(v, u);
    ASSERT_TRUE(isMatched(u, v));
    ASSERT_TRUE(M.getNodeDegree(u) == 1);
    ASSERT_TRUE(M.getNodeDegree(v) == 1);
    
    #ifdef DM_COUNTERS
        counters::get("rw_v4").get("match()").inc();
    #endif
}

void randomwalkv4_dyn_matching::unmatch (NodeID u, NodeID v) {
    ASSERT_TRUE(isMatched(u, v));
    ASSERT_TRUE(M.getNodeDegree(u) == 1);
    ASSERT_TRUE(M.getNodeDegree(v) == 1);
    M.remove_edge(u, v);
    M.remove_edge(v, u);
    ASSERT_TRUE(M.getNodeDegree(u) == 0);
    ASSERT_TRUE(M.getNodeDegree(v) == 0);
    ASSERT_TRUE(!isMatched(u, v));
}

NodeID randomwalkv4_dyn_matching::getMatchedEdge (NodeID source) {
    if (freeVertex(source)) return -1;
    
    ASSERT_TRUE(M.getEdgesFromNode(source).size() == 1);
    return M.getEdgesFromNode(source).at(0).target;
}

void randomwalkv4_dyn_matching::solve_conflict (NodeID u, int step) {
    // if the node degree is less then 1.0/eps, the naive settle costs at most O(1/eps) time
    if (G->getNodeDegree(u) <= (1.0/eps) && freeVertex(u)) {
        if (settle(u)) return;
    }
    
    // do this recursion maximal sqrt(m) times or break if u is matched
    if (step >= (1.0/eps) && freeVertex(u)) {
        settle(u);
        return;
    }
    
    if (!freeVertex(u)) return;
    
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
}

bool randomwalkv4_dyn_matching::settle (NodeID u) {
    EdgeID deg_u = G->get_first_invalid_edge(u);
    NodeID v;
    bool mate_found = false;
    
    for (EdgeID i = 0; i < deg_u; ++i) {
        v = G->getEdgeTarget(u, i);
        if (freeVertex(v)) {
            mate_found = true;
            break;
        }
    }
    
    if (mate_found) {
        match(u, v);
    }
    
    return mate_found;
}

void randomwalkv4_dyn_matching::counters_next() {
    #ifdef DM_COUNTERS
        counters::get("rw_v4").get("match()").next();
    #endif
}
