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

#include "neimansolomon_dyn_matching.h"

#ifdef DM_COUNTERS
    #include "counters.h"
#endif

//========================// Match struct methods //========================//

neimansolomon_dyn_matching::Match::Match () {
    
}

neimansolomon_dyn_matching::Match::Match (NodeID id, NodeID mate, bool has_mate) {
    this->id = id;
    this->mate = mate;
    this->has_mate = has_mate;
}

neimansolomon_dyn_matching::Match::Match (const Match& rop) {
    this->id = rop.id;
    this->mate = rop.mate;
    this->has_mate = rop.has_mate;
}

bool neimansolomon_dyn_matching::Match::operator== (Match& rop) {
    return id == rop.id;
}

bool neimansolomon_dyn_matching::Match::operator< (Match& rop) {
    return id < rop.id;
}

bool neimansolomon_dyn_matching::Match::operator> (Match& rop) {
    return id > rop.id;
}

std::ostream& neimansolomon_dyn_matching::Match::print (std::ostream& o) const {
    if (has_mate) {
        o << "(" << id << "," << mate << ")";
    } else {
        o << "(" << id << ",NO MATE)";
    }
    return o;
}


//========================// Fv struct methods //========================//

neimansolomon_dyn_matching::Fv::Fv (NodeID n) {
    // in the beginning no neighbour is free, since there is no neighbour, when there are no edges
    free.resize(n, false);
    
    // hence every segment holds 0 free neighbours.
    if (std::sqrt(n) == (int)std::sqrt(n)) {
        n_free.resize((int)std::sqrt(n), 0);
    } else { // if sqrt(n) != int(sqrt(n)), we create int(sqrt(n))+1 intervals
        n_free.resize((int)std::sqrt(n) + 1, 0);
    }
    
    total_n = 0;
}

bool neimansolomon_dyn_matching::Fv::insert (NodeID node) {
    if (free.at(node)) {
        return false;
    }
    
    free.at(node) = true;
    
    size_t n_free_index = (1.0*node) / sqrt(free.size());
    n_free.at(n_free_index) = n_free.at(n_free_index) + 1;
    
    total_n++;
    return true;
}

bool neimansolomon_dyn_matching::Fv::remove (NodeID node) {
    if (!free.at(node)) {
        return false;
    }
    
    free.at(node) = false;
    
    size_t n_free_index = (1.0*node) / sqrt(free.size());
    n_free.at(n_free_index) = n_free.at(n_free_index) - 1;
    
    total_n--;
    return true;
}

bool neimansolomon_dyn_matching::Fv::has_free () {
    return total_n > 0;
}

bool neimansolomon_dyn_matching::Fv::get_free (NodeID& index) { // returns false if there is no free, and true + reference when there is a free neighbour
//    std::cout << "free neighbours: " << total_n << std::endl;
    #ifdef DM_COUNTERS
        counters::get("ns").get("get_free()").inc();
    #endif
    
    if (!has_free()) return false;
    
    index = 0;
    
    while (!n_free.at(index)) { // iterate through n_free until we have a 
                                // segment, where there are free nodes
        index++;
    }
    
    index = (n_free.size() - 1) * index;
    
    while (!free.at(index)) {
        index++;
    }
    
    return true;
}


//========================// Neimansolomon matching methods //========================//

neimansolomon_dyn_matching::neimansolomon_dyn_matching (dyn_graph_access* G) : dyn_matching(G) {
    // the underlying data structure dyn_graph_access for the graph itself is initialized
    // by the call to dyn_matching(G). now we initialise our data structures.
    // - M is the matching and empty in the beginning,
    // - N is a n-size vector of empty avl_trees, where n denotes the number of nodes in G
    // - F is a n-size vector of n-size Fv-data structures, where n again denotes the number of nodes in G
    
    M = avl_tree<Match>();
    N.resize(G->number_of_nodes(), avl_tree<NodeID>());
    F.resize(G->number_of_nodes(), Fv(G->number_of_nodes()));
    
    #ifdef DM_COUNTERS
        counters::create("ns");
        counters::get("ns").create("match()");
        counters::get("ns").create("get_free()");
        
        counters::get("ns").create_d("rt_in");
        counters::get("ns").create_d("rt_out");
        counters::get("ns").create_d("rt_get_free()");
        
        counters::get("ns").create_d("rt_in_G");
        counters::get("ns").create_d("rt_out_G");
        /*
        counters::new_counter("neighbours_of_u");
        counters::new_counter("u_s");
        counters::new_counter("easy_insert_ns");
        counters::new_counter("complex_insert_aug_path");
        counters::new_counter("complex_insert_is_free");
        counters::new_counter("remove_matched_edge_ns");
        */
    #endif
}

EdgeID neimansolomon_dyn_matching::new_edge(NodeID source, NodeID target, double& elapsed) {
    // first add the node to the data structure G;
    timer t;
    
    EdgeID e = G->new_edge(source, target);
    EdgeID e_bar = G->new_edge(target, source);
    
    #ifdef DM_COUNTERS
        elapsed = t.elapsed();
        counters::get("ns").get_d("rt_in_G").add(elapsed);
    #endif
    
    t.restart();
    handle_addition(source, target);
    elapsed = t.elapsed();
    
    #ifdef DM_COUNTERS
        counters::get("ns").get_d("rt_in").add(elapsed);
    #endif
    
    return e;
}

void neimansolomon_dyn_matching::handle_addition (NodeID u, NodeID v) {
//    std::cout << "inserting: " << u << "," << v << std::endl;
//    std::cout << "F_max size BEFORE INSERTION: " << F_max.size() << std::endl;
    N.at(u).insert(v);
    N.at(v).insert(u);
    
    if (F_max.contains(u)) {
        F_max.changeKey(u, deg(u));
    } else {
        F_max.insert(u, deg(u));
    }
    
    if (F_max.contains(v)) {
        F_max.changeKey(v, deg(v));
    } else {
        F_max.insert(v, deg(v));
    }
    
    if (is_free(u) && is_free(v)) { // if both are free...
        match(u, v);                 // match them!
        
        #ifdef DM_COUNTERS
        #endif
    } else if (is_free(u) || is_free(v)) {
        // if u is free, then the code works out fine,
        // otherwise we simply swap u and v, in order
        // to stay as near as possible to the papers notation
        if (is_free(v)) {
            NodeID tmp = u;
            u = v;
            v = tmp;
        }
        
        NodeID v_;
        ASSERT_TRUE(mate(v, v_)); // save the mate of v in v_
                                 // and assert, that v has a mate
                                 // notice, that since we change 
                                 // u and v so that u is the free
                                 // node, v always has to be matched
        F.at(v_).remove(u);
        
        if (has_free(v_)) {
            M.remove(v);
            M.remove(v_);

            // switch from u - v---v_ - v_free (<- augh-path /w len 3)
            // to          u---v - v_---v_free
            match(u, v);
            match(v_, get_free(v_));
            #ifdef DM_COUNTERS
            #endif
        } else {
            #ifdef DM_COUNTERS
            #endif
            // tell all neighbours of u, that u is free
            for (NodeID w : N.at(u).allElements()) {
                F.at(w).insert(u);
            }
        }
    } else {
        // do nothing
    }
}
    
void neimansolomon_dyn_matching::remove_edge(NodeID source, NodeID target, double& elapsed) {
    timer t;
    
    G->remove_edge(source, target);
    G->remove_edge(target, source);
    
    #ifdef DM_COUNTERS
        elapsed = t.elapsed();
        counters::get("ns").get_d("rt_out_G").add(elapsed);
    #endif
    
    t.restart();
    handle_deletion(source, target);
    elapsed = t.elapsed();
    
    #ifdef DM_COUNTERS
        counters::get("ns").get_d("rt_out").add(elapsed);
    #endif
}

std::vector<std::pair<NodeID, NodeID> > neimansolomon_dyn_matching::getM () {
    std::vector<std::pair<NodeID, NodeID> > M_vector;
    for (auto e : M.allElements()) {
        M_vector.push_back({e.id, e.mate});
    }
    
    return M_vector;
}
/*
avl_tree<neimansolomon_dyn_matching::Match> neimansolomon_dyn_matching::getM () {
    return M;
}
*/
// mate returns if a node "node" has a mate or not. if it has,
// it assigns the mates value to the reference "mate".
bool neimansolomon_dyn_matching::mate (NodeID node, NodeID& mate) {
    Match tmp (node); // since M holds Matches, I create a dummy match with the
                         // ID I'm looking for. not nice, but should work.
    if (!M.isIn(tmp)) { // if it's not in the matching, it's because it's free
        return false;
    } else { // if it's in, then it has a mate, which we safe in the mate-reference
        if (!M.get(tmp)->has_mate) {
            return false;
        } else {
            mate = M.get(tmp)->mate; // wasTODO: I should change the avl_tree, 
                                     //       this is not nice. it shouldn't return a node,
                                     //       but the saved object. DONE!
//            std::cout << "mate of " << node << " is " << mate << std::endl; 
            return true;
        }
    }
}

// mate with a single parameter simply does not return the mate
bool neimansolomon_dyn_matching::mate (NodeID node) { 
    NodeID tmp (0); 
    return mate(node, tmp);
}

bool neimansolomon_dyn_matching::is_free (NodeID node) {
    // a node without mate is a free node.
    return !mate(node);
}

bool neimansolomon_dyn_matching::has_free (NodeID node) {
    return F.at(node).has_free();
}

NodeID neimansolomon_dyn_matching::get_free (NodeID node) {
    NodeID free_node;
    // since the described algorithm always first checks has_free
    // before calling get_free, I'm going to do it likewise.
    // therefore I assume and assert, that get_free has found 
    // a free node
    #ifdef DM_COUNTERS
        timer t;
    #endif
    
    ASSERT_TRUE(F.at(node).get_free(free_node));
    
    #ifdef DM_COUNTERS
        double elapsed = t.elapsed();
        counters::get("ns").get_d("rt_get_free()").add(elapsed);
    #endif
    
    return free_node;
}

void neimansolomon_dyn_matching::match (NodeID u, NodeID v) {
    M.insert(Match(u, v, false));
    M.insert(Match(v, u, false));
    
    //M.print();
    
    // remove u and v from F_max, since when matched, they're not free anymore
    if (F_max.contains(u)) {
        F_max.deleteNode(u);
    }
    if (F_max.contains(v)) {
        F_max.deleteNode(v);
    }
    
    for (NodeID w : {u, v}) {
        if (!mate(w)) {
            for (NodeID x : N.at(w).allElements()) {
                F.at(x).remove(w);
            }
        }
    }
    
    M.get(Match(u, v))->has_mate = true;
    M.get(Match(v, u))->has_mate = true;
    
    #ifdef DM_COUNTERS
        counters::get("ns").get("match()").inc();
    #endif
    
    //)M.print();
}

NodeID neimansolomon_dyn_matching::deg (NodeID u) {
    return N.at(u).size();
}

NodeID neimansolomon_dyn_matching::threshold () {
    return std::sqrt(2 * G->number_of_edges());
}

void neimansolomon_dyn_matching::aug_path (NodeID u) {
    ASSERT_TRUE(!has_free(u));
    ASSERT_TRUE(deg(u) <= std::sqrt(2*G->number_of_nodes() + 2*G->number_of_edges()));
    
    NodeID w, w_, x;
    bool aug_path_found = false;
    
    for (NodeID dummy : N.at(u).allElements()) {
        w = dummy;
        // since we assume that u has no free neighbours,
        // all neighbours have to be matched and therefore
        // have a mate
        ASSERT_TRUE(mate(w, w_));
        if (has_free(w_)) {
            x = get_free(w_);
            aug_path_found = true;
            break;
        }
    }
    
    if (aug_path_found) {
        // here I swapped the statements from the original paper due to technical limitations
        M.remove (w);
        M.remove (w_);
        
        match(u, w);
        match(w_, x);
    } else {
        #ifdef DM_COUNTERS 
        #endif
        
        for (NodeID dummy : N.at(u).allElements()) {
            F.at(dummy).insert(u);
        }
        
        if (F_max.contains(u)) {
            F_max.changeKey(u, deg(u));
        } else {
            F_max.insert(u, deg(u));
        }
        ASSERT_TRUE(!mate(u));
    }
    
    return;
}

NodeID neimansolomon_dyn_matching::surrogate (NodeID u) {
    ASSERT_TRUE(!has_free(u));
    ASSERT_TRUE(deg(u) > threshold());
    
    NodeID w, w_;
    
    for (NodeID dummy : N.at(u).allElements()) {
        w = dummy;
        // since we assume that u has no free neighbours,
        // all neighbours have to be matched and therefore
        // have a mate
        ASSERT_TRUE(mate(w, w_));
        if (deg(w_) <= threshold()) break;
    }
    
    ASSERT_TRUE(w == M.get(w_)->mate && w_ == M.get(w)->mate);
    M.remove(w);
    M.remove(w_);
    
    match(u, w);
    
    return w_;
}

void neimansolomon_dyn_matching::handle_deletion (NodeID u, NodeID v) {
    //std::cout << "removing: " << u << "," << v << std::endl;
    //std::cout << "N(u):" << std::endl; 
    //N.at(u).print();
    //std::cout << "N(v):" << std::endl; 
    //N.at(v).print();
    
    N.at(u).remove(v);
    N.at(v).remove(u);
    
    //N.at(u).print();
    //N.at(v).print();
    
    if (F_max.contains(u)) {
        F_max.changeKey(u, deg(u));
    }
    if (F_max.contains(v)) {
        F_max.changeKey(v, deg(v));
    }
    
    // the edge (u,v) is not in the matching if:
    //   1. u or v or both are not in the matching
    //   2. u and v are in the matching, but not each others mates.
    if ((!M.isIn(u)) || // 1.
        (!M.isIn(v)) || // 1.
        (M.isIn(u) && M.isIn(v) && u != M.get(v)->mate) // 2.
    ) {
//        ASSERT_TRUE(v != M.get(u)->mate); // this assertion holds only for case 2., not for 1.
        
        if (is_free(u)) {
            F.at(v).remove(u);
        }
        
        if (is_free(v)) {
            F.at(u).remove(v);
        }
    } else { // (u,v) is in the matching
        ASSERT_TRUE((u == M.get(v)->mate) && (v == M.get(u)->mate));
        
        M.remove(u);
        M.remove(v);
        
        bool surrogated = false;
        for (NodeID z : {u, v}) {
            do { // auxiliary while-loop in order to jump back 
                 // to 3(b)i. on surrogation
                surrogated = false;
                
                if (has_free(z)) {
                    match (z, get_free(z));
                } else {
                    if (deg(z) > threshold()) {
                        z = surrogate(z);
                        surrogated = true;
                    } else {
                        aug_path(z);
                    }
                }
            } while (surrogated);
        }
        
        #ifdef DM_COUNTERS
        #endif
    }
}

void neimansolomon_dyn_matching::counters_next() {
    #ifdef DM_COUNTERS
        counters::get("ns").get("match()").next();
        counters::get("ns").get("get_free()").next();
        
        counters::get("ns").get_d("rt_in").next();
        counters::get("ns").get_d("rt_out").next();
        counters::get("ns").get_d("rt_get_free()").next();
        
        counters::get("ns").get_d("rt_in_G").next();
        counters::get("ns").get_d("rt_out_G").next();
    #endif
}

std::ostream& operator<<(std::ostream& o, const neimansolomon_dyn_matching::Match& rop) {
    return rop.print(o);
}

