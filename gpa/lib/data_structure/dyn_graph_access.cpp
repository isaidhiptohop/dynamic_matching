/******************************************************************************
 * dyn_graph_access.cpp
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

#include "dyn_graph_access.h"

dyn_graph_access::dyn_graph_access() : m_building_graph(false) {
    edge_count = 0;
    node_count = 0;
    max_degree_node = 0;
}

dyn_graph_access::~dyn_graph_access() {};

// construction of the graph
void dyn_graph_access::start_construction(NodeID nodes, EdgeID edges) {
    m_building_graph = true;
    m_nodes.resize(nodes);
    m_edges.resize(nodes);
    m_edges_maps.resize(nodes);
}

NodeID dyn_graph_access::new_node() {
    ASSERT_TRUE(m_building_graph);
    return node_count++;
}

void dyn_graph_access::finish_construction() {
    m_building_graph = false;
    m_nodes.resize(node_count);
    m_edges.resize(node_count);
    m_edges_maps.resize(node_count);
}
/*
void dyn_graph_access::convert_from_graph_access(graph_access& H) {
    start_construction(H.number_of_nodes());
    
    forall_nodes(H,n)
        new_node();
    endfor
    
    finish_construction();
    
    forall_nodes(H,n)
        forall_out_edges(H,e,n)
            new_edge(n,H.getEdgeTarget(e));
        endfor
    endfor
    
    ASSERT_TRUE(number_of_nodes() == H.number_of_nodes());
    ASSERT_TRUE(number_of_edges() == H.number_of_edges());
}
*/
basicGraph * dyn_graph_access::convert_to_basic_graph() {
    basicGraph * realGraph = new basicGraph();
    realGraph->start_construction(number_of_nodes(), number_of_edges());
    
    for (int n = 0; n < number_of_nodes(); ++n) {
        realGraph->new_node();
    }
    
    for (int n = 0; n < number_of_nodes(); ++n) {
        for (int e = 0; e < getEdgesFromNode(n).size(); ++e) {
            realGraph->new_edge(n, getEdgeTarget(n, e));
        }
    }
    
    realGraph->finish_construction();
    
//    delete H.graphref;
//    H.graphref = realGraph;
    
    ASSERT_TRUE(number_of_nodes() == realGraph->number_of_nodes());
    ASSERT_TRUE(number_of_edges() == realGraph->number_of_edges());
    
    return realGraph;
}

bool dyn_graph_access::new_edge(NodeID source, NodeID target) {
    ASSERT_TRUE(&(m_edges_maps[source]) != 0);
    ASSERT_TRUE(target < node_count);

    // look up if edge exists. if not, continue
    if (m_edges_maps[source].find(target) == m_edges_maps[source].end()) {
        // create new edge with target node to add to the end of
        // the corresponding edges-vector
        basicEdge e_buf;
        e_buf.targetNode = target;
        m_edges[source].push_back(e_buf);
        
        // add the index of the new edge and the target node as 
        // <key,value>-pair to the unordered map
        m_edges_maps[source][target] = m_edges[source].size() - 1;

    
        // adjust the max degree node
        if (getNodeDegree(source) > getMaxDegree()) {
            max_degree_node = source;
        }
        
        edge_count++;
        return true;
    } else {
        return false;
    }
}

bool dyn_graph_access::remove_edge(NodeID source, NodeID target) {
    // look up edge before removing
    if (m_edges_maps[source].find(target) != m_edges_maps[source].end()) {
        size_t i = m_edges_maps[source][target];
        // copy the last element and paste it to the position of the edge which is
        // supposed to be deleted. then truncate the corresponding vector
        basicEdge e_buf = m_edges[source].back();
        m_edges[source][i] = e_buf;
        m_edges[source].pop_back();
        
        // adjust the entries in the unordered map since the index of the last 
        // edge has now changed to the index of the edge which has been deleted.
        m_edges_maps[source][e_buf.targetNode] = i;
        m_edges_maps[source].erase(target);
        
        // if the max_degree_node 
        if (source == max_degree_node) {
            EdgeWeight cur_degree = getNodeDegree(source);
            for (NodeID n = 0; n < node_count; ++n) {
                if (getNodeDegree(n) > cur_degree) {
                    max_degree_node = n;
                    break;
                }   
            }
        }
        
        edge_count--;
        return true;
    } else {
        return false;
    }
}

bool dyn_graph_access::isEdge(NodeID source, NodeID target) {
    return (m_edges_maps[source].find(target) != m_edges_maps[source].end());
}


EdgeID dyn_graph_access::number_of_edges() {
    return edge_count;
}

NodeID dyn_graph_access::number_of_nodes() {
    return node_count;
}

std::vector<basicEdge> dyn_graph_access::getEdgesFromNode (NodeID node) {
    ASSERT_TRUE(m_edges.size() > 0);
    return m_edges[node];
}


EdgeID dyn_graph_access::get_first_edge (NodeID node) {
    return 0;
}

EdgeID dyn_graph_access::get_first_invalid_edge (NodeID node) {
    ASSERT_TRUE(m_edges.size() > 0);
    return m_edges[node].size();
}

NodeWeight dyn_graph_access::getNodeWeight(NodeID node) {
    ASSERT_TRUE(m_edges.size() > 0);
    return m_nodes[node].weight;
}

void dyn_graph_access::setNodeWeight(NodeID node, NodeWeight weight) {
    m_nodes[node].weight = weight;
}

EdgeWeight dyn_graph_access::getNodeDegree(NodeID node) {
    return m_edges[node].size();
}

EdgeWeight dyn_graph_access::getWeightedNodeDegree(NodeID node) {
    EdgeWeight degree = 0;
    
    for (unsigned e = 0; e < m_edges[node].size(); ++e) {
        degree += getEdgeWeight(node, e);
    }
    
    return degree;
}

EdgeWeight dyn_graph_access::getMaxDegree() {
    return getNodeDegree(max_degree_node);
}

NodeID dyn_graph_access::getMaxDegreeNode() {
    return max_degree_node;
}

EdgeWeight dyn_graph_access::getEdgeWeight(NodeID source, EdgeID edge) {
    return m_edges[source][edge].weight;
}

void dyn_graph_access::setEdgeWeight(NodeID source, EdgeID edge, EdgeWeight weight) {
    m_edges[source][edge].weight = weight;
}

NodeID dyn_graph_access::getEdgeTarget(NodeID source, EdgeID edge) {
    ASSERT_TRUE(m_edges.size() > 0);
//    std::cout << m_edges[source].size() <<  " > " << edge << std::endl;
    ASSERT_TRUE(m_edges[source].size() > edge);
    
    return m_edges[source][edge].targetNode;
}

