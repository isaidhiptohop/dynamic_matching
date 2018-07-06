#include "dyn_matching.h"

dyn_matching::dyn_matching (dyn_graph_access* G) {
    this->G = G;
}

dyn_matching::~dyn_matching () {
    delete G;
}
/*
EdgeID dyn_matching::new_edge(NodeID source, NodeID target) {
    return 0;
}

void dyn_matching::remove_edge(NodeID source, NodeID target) {
    return;
}
*/
dyn_graph_access dyn_matching::getG () {
    return *G;
}

