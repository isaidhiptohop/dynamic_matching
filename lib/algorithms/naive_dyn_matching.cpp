/******************************************************************************
 * naive_dyn_matching.cpp
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

#include "naive_dyn_matching.h"

#ifdef DM_COUNTERS
    #include "counters.h"
#endif
/*
int RNG::nextInt (int lb, int rb) {
    std::uniform_int_distribution<unsigned int> A(lb, rb);
    return A(mt);
}
*/

naive_dyn_matching::naive_dyn_matching (dyn_graph_access* G) : dyn_matching(G) {
    M.start_construction(G->number_of_nodes());
    
    for (int i = 0; i < G->number_of_nodes(); ++i) {
        M.new_node();
    }
    
    M.finish_construction();

    #ifdef DM_COUNTERS
        counters::create("naive");
        counters::get("naive").create("match()");
    #endif
//    auto a = static_cast<long unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
//        auto a = std::chrono::system_clock::now();
//    rng.setSeed(a);
}

bool naive_dyn_matching::new_edge(NodeID source, NodeID target, double& elapsed) {
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

bool naive_dyn_matching::remove_edge(NodeID source, NodeID target, double& elapsed) {
    timer t;
    
    bool foo = G->remove_edge(source, target);
    bool bar = G->remove_edge(target, source);
    
    ASSERT_TRUE(foo == bar);
    
    /* starting calculation of matching */
    
    t.restart();
    
    if (isMatched(source, target)) {
        unmatch(source, target);
        
        ASSERT_TRUE(!isMatched(source, target));
        
        settle(source);
        settle(target);
    }
    
    elapsed = t.elapsed();
    
    /* finished calculation of matching */
    
    return foo;
}

std::vector<std::pair<NodeID, NodeID> > naive_dyn_matching::getM () {
    std::vector<std::pair<NodeID, NodeID> > M_vector;
    for(NodeID n = 0, n_end = M.number_of_nodes(); n < n_end; ++n) {
        for(EdgeID e = 0, e_end = M.getEdgesFromNode(n).size(); e < e_end; ++e) {
            M_vector.push_back({n, M.getEdgeTarget(n,e)});
        }
    }
    
    return M_vector;
}

/*
dyn_graph_access naive_dyn_matching::getM () {
    return M;
}
*/
// checks whether a vertex is free
bool naive_dyn_matching::freeVertex (NodeID node) {
    return M.getEdgesFromNode(node).size() == 0;
}

// checks whether an edge is matched or not
bool naive_dyn_matching::isMatched (NodeID source, NodeID target) {
///*
    bool a = M.isEdge(source, target);
    bool b = M.isEdge(target, source);
    ASSERT_TRUE(a == b);
//*/
    return M.isEdge(source, target) || M.isEdge(target, source);
}

void naive_dyn_matching::match (NodeID u, NodeID v) {
    ASSERT_TRUE(M.getNodeDegree(u) == 0);
    ASSERT_TRUE(M.getNodeDegree(v) == 0);
    ASSERT_TRUE(!isMatched(u, v));
    M.new_edge(u, v);
    M.new_edge(v, u);
    ASSERT_TRUE(isMatched(u, v));
    ASSERT_TRUE(M.getNodeDegree(u) == 1);
    ASSERT_TRUE(M.getNodeDegree(v) == 1);
    
    #ifdef DM_COUNTERS
        counters::get("naive").get("match()").inc();
    #endif
}

void naive_dyn_matching::unmatch (NodeID u, NodeID v) {
    ASSERT_TRUE(isMatched(u, v));
    ASSERT_TRUE(M.getNodeDegree(u) == 1);
    ASSERT_TRUE(M.getNodeDegree(v) == 1);
    M.remove_edge(u, v);
    M.remove_edge(v, u);
    ASSERT_TRUE(M.getNodeDegree(u) == 0);
    ASSERT_TRUE(M.getNodeDegree(v) == 0);
    ASSERT_TRUE(!isMatched(u, v));
}

NodeID naive_dyn_matching::getMatchedEdge (NodeID source) {
    if (freeVertex(source)) return -1;
    
    ASSERT_TRUE(M.getEdgesFromNode(source).size() == 1);
    return M.getEdgesFromNode(source).at(0).target;
}

void naive_dyn_matching::settle (NodeID u) {
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
}

void naive_dyn_matching::counters_next() {
    #ifdef DM_COUNTERS
        counters::get("naive").get("match()").next();
    #endif
}
