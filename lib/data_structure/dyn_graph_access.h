/******************************************************************************
 * dyn_graph_access.h 
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

#ifndef dyn_graph_access_EFRXO4X2
#define dyn_graph_access_EFRXO4X2

#include <bitset>
#include <cassert>
#include <iostream>
#include <vector>
#include <unordered_map>

#include "definitions.h"
#include "graph_access.h"


class dyn_graph_access {
public:
    dyn_graph_access();    
    ~dyn_graph_access();

    // construction of the graph
    void start_construction(NodeID nodes, EdgeID edges = 0);
    NodeID new_node();
    void finish_construction();
    
    void convert_from_graph_access(graph_access& H);
    void convert_to_graph_access(graph_access& H);
    
    bool new_edge(NodeID source, NodeID target);
    bool remove_edge(NodeID source, NodeID target);
    
    bool isEdge(NodeID source, NodeID target);
    
    // access 
    EdgeID number_of_edges();
    NodeID number_of_nodes();
    
    std::vector<Edge> getEdgesFromNode (NodeID node);
    
    EdgeID get_first_edge (NodeID node);
    EdgeID get_first_invalid_edge (NodeID node);
    
    NodeWeight getNodeWeight(NodeID node);
    void setNodeWeight(NodeID node, NodeWeight weight);

    EdgeWeight getNodeDegree(NodeID node);
    EdgeWeight getWeightedNodeDegree(NodeID node);
    EdgeWeight getMaxDegree();
    NodeID getMaxDegreeNode();

    EdgeWeight getEdgeWeight(NodeID source, EdgeID edge);
    void setEdgeWeight(NodeID source, EdgeID edge, EdgeWeight weight);

    NodeID getEdgeTarget(NodeID source, EdgeID edge);

private:
    // %%%%%%%%%%%%%%%%%%% DATA %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // split properties for coarsening and uncoarsening
    std::vector<Node> m_nodes;
    std::vector<std::vector<Edge> > m_edges;
    std::vector<std::unordered_map<NodeID, size_t> > m_edges_maps;
    
    EdgeID edge_count;
    NodeID node_count;
    
    NodeID max_degree_node;
            
    // construction properties
    bool m_building_graph;
    /*
    NodeID node; //current node that is constructed
    EdgeID e;    //current edge that is constructed
    */
};


//makros - dyn graph access
#define forall_edges_d(G,n,e) { \
    for(NodeID n = 0, n_end = G.number_of_nodes(); n < n_end; ++n) \
        for(EdgeID e = 0, e_end = G.getEdgesFromNode(n).size(); e < e_end; ++e) {
#define forall_nodes_d(G,n) { \
    for(NodeID n = 0, end = G.number_of_nodes(); n < end; ++n) {
#define forall_out_edges_d(G,e,n) { \
    for(EdgeID e = G.get_first_edge(n), end = G.get_first_invalid_edge(n); e < end; ++e) {
#define forall_out_edges_starting_at_d(G,e,n,e_bar) { \
    for(EdgeID e = e_bar, end = G.get_first_invalid_edge(n); e < end; ++e) {
/*
#define forall_blocks(G,p) { for (PartitionID p = 0, end = G.get_partition_count(); p < end; p++) {
*/
#define endfor }}

#endif /* end of include guard: dyn_graph_access_EFRXO4X2 */
