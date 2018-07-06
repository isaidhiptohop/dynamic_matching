#include "graph_access.h"
#include "gpa_matching.h"
#include "config.h"
#include "random_functions.h"

#include <iostream>
#include <vector>

int main () {
    graph_access G;
    
    G.start_construction(8, 8);
    
    G.new_node();
    G.new_node();
    G.new_node();
    G.new_node();
    
    G.new_edge(0, 1);
    G.new_edge(1, 2);
    G.new_edge(2, 3);
    G.new_edge(3, 4);
    G.new_edge(4, 5);
    G.new_edge(5, 6);
    G.new_edge(6, 7);
    G.new_edge(7, 0);
    
    G.finish_construction();
    
    gpa_matching match;
    Matching matching;
    Config config;
    
    config.seed = 0;
    
    srand(config.seed);
    random_functions::setSeed(config.seed);

    match.match(config, G, matching);
    
    for (size_t i = 0; i < matching.size(); ++i) {
        if (i != matching.at(i)) {
            std::cout << i << " " << matching.at(i) << "; ";
        }
    }
    
    std::cout << std::endl;
}
