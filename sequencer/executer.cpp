#include "dyn_matching.h"
#include "simple_dyn_matching.h"
#include "baswanaguptasen_dyn_matching.h"
#include "neimansolomon_dyn_matching.h"

#include "tools/timer.h"

#include "../GPAMatching/app/gpa_match.h"

/*
#include "config.h"
#include "data_structure/graph_access.h"
#include "io/graph_io.h"
#include "matching/gpa_matching.h"
#include "tools/macros_assertions.h"
#include "tools/random_functions.h"
#include "../GPAMatching/lib/tools/random_functions.h"
*/
#include <string>
#include <iostream>
#include <fstream>
#include <climits>
#include <algorithm>

/* 
 * takes arguments:
 *   -a[lgorithm]: ns, bgs, naive eps, gpa step
 *   -[-]a[t-]o[nce]: uint
 */
 
enum ALGORITHM {bgs, gpa, naive, ns};
std::vector<std::string> ALGORITHM_NAMES = {"bgs", "gpa", "naive", "ns"};

struct ex_config {
    std::string file;
    ALGORITHM algorithm;
    double eps; // eps for naive
    long seed; // gpa seed
    size_t at_once;
    int multi_run;
};

std::vector<std::pair<NodeID, NodeID> > to_pairvec (const Matching & matching) {
    std::vector<std::pair<NodeID, NodeID> > pairvec;
    
    for (size_t i = 0; i < matching.size(); ++i) {
        if (matching.at(i) != i) {
            pairvec.push_back({i, matching.at(i)});
        }
    }
    
    return pairvec;
}

void print_matching (std::ostream& o, const std::vector<std::pair<NodeID, NodeID> >& matching) {
    for (size_t i = 0; i < matching.size(); ++i) {
        o << matching.at(i).first << " " << matching.at(i).second << "; ";
    }
    o << std::endl;
}

void validate (const std::vector<std::pair<NodeID, NodeID> >& matching) {
    // do this once the left side...
    std::unordered_map<NodeID, NodeID> nodes_u;
    
    // go through matched edges
    for (auto edge : matching) {
        // assert that key u of edge (u,v) is not found in the map
        ASSERT_TRUE(nodes_u.find(edge.first) == nodes_u.end());
        // add the edge to the map. if another edge should be incident
        // to the same node u, than the above assertion will fail.
        nodes_u[edge.first] = edge.second;
    }
    
    // ...and once the right side
    std::unordered_map<NodeID, NodeID> nodes_v;
    
    // same as above, but checking with v as key of (u,v)
    for (auto edge : matching) {
        ASSERT_TRUE(nodes_v.find(edge.second) == nodes_v.end());
        nodes_v[edge.second] = edge.first;
    }
}

void check_option_arg (std::string option, int i, int argc, char** argv) {
    if (i+1 >= argc || argv[i+1][0] == '-') {
        throw std::string("ERROR: argument for option " + option + " missing.");
    }
}

void get_arguments(int argc, char ** argv, ex_config& conf) {
    if (argc == 1) {
        throw std::string("ERROR: need some arguments...");
    }
    
    conf.at_once = 1;
    conf.multi_run = 1;
    
    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] != '-') {
            throw std::string("ERROR: " + std::string(argv[i]) + " not an option.");
        }
        
        std::string option = argv[i];
        
        if (option == "-ao" || option == "--at-once") {
            check_option_arg(option, i, argc, argv);
            conf.at_once = atoi(argv[i+1]);
            i++;
        } else if (option == "-f") {
            check_option_arg(option, i, argc, argv);
            conf.file = argv[i+1];
            ++i;
        } else if (option == "-a") {
            check_option_arg(option, i, argc, argv);
            std::string algorithm = argv[i+1];
            ++i;
            
            if (algorithm == "bgs") {
                conf.algorithm = ALGORITHM::bgs;
            } else if (algorithm == "gpa") {
                conf.algorithm = ALGORITHM::gpa;
                if (i+1 >= argc || argv[i+1][0] == '-') {
                    conf.seed = 1;
                } else {
                    conf.seed = atoi(argv[i+1]);
                }
            } else if (algorithm == "naive") {
                conf.algorithm = ALGORITHM::naive;
                check_option_arg(option, i, argc, argv);
                conf.eps = atof(argv[i+1]);
                ++i;
            } else if (algorithm == "ns") {
                conf.algorithm = ALGORITHM::ns;
            } else {
                throw std::string ("ERROR: unkown algorithm: " + algorithm);
            }
        } else {
            
        }
    }
}

std::vector<std::string> split (const std::string& input, const char& mark) {
    std::vector<std::string> substrings;
    std::string buf = "";
    
    for (size_t i = 0; i < input.size(); ++i) {
        if (input.at(i) != mark) {
            buf = buf + input.at(i);
        } else {
            substrings.push_back(buf);
            buf = "";
        }
    }
    
    if (buf != "") {
        substrings.push_back(buf);
    }
    
    return substrings;
}

size_t read_sequence (std::string file, std::vector<std::pair<int, std::pair<NodeID, NodeID> > >& edge_sequence) {
    std::string line;
    std::ifstream input(file);
    edge_sequence.resize(0);
    size_t n = 0;
    int i = 0;
    
    if (input.is_open()) {
        std::getline(input, line);
        std::vector<std::string> substr = split(line, ' ');
        
        std::string hash = substr.at(0);
        if (hash != "#") throw std::string("META DATA SEEMS TO BE MISSING");
        
        n = std::stoul(substr.at(1).c_str());
        
        while (std::getline(input, line)) {
            i++;
            std::vector<std::string> substr = split(line, ' ');
            
            int addition = atoi(substr.at(0).c_str());
            NodeID u = atoi(substr.at(1).c_str());
            NodeID v = atoi(substr.at(2).c_str());
            
            if (u < n && v < n) {
                edge_sequence.push_back({addition, {u, v}});
            }
        }
    }
    
    input.close();
    
    return n;
}

std::string get_output_filename (ex_config conf) {
    std::string output = split(conf.file, '.').at(0) + "_";
    output = output + ALGORITHM_NAMES.at(conf.algorithm) + "_";
    
    if (conf.algorithm == ALGORITHM::gpa) {
        output = output + std::to_string(conf.seed) + "_";
    } else if (conf.algorithm == ALGORITHM::naive) {
        output = output + std::to_string(conf.eps) + "_";
    }
    
    output = output + std::to_string(conf.at_once);
    
    std::vector<std::string> buf = split(output, '/');
    output = buf.at(buf.size() - 1);
    
    output = "output/" + output;
    
    std::cout << "printing output to file: " << output << std::endl;
    
    return output;
}

dyn_graph_access* create_graph (size_t n) {
    dyn_graph_access * G = new dyn_graph_access();
    
    G->start_construction(n);
    
    for (size_t i = 0; i < n; ++i) {
        G->new_node();
    }
    
    G->finish_construction();
    
    return G;
}

int main (int argc, char ** argv) {
    try {
        std::vector<std::pair<int, std::pair<NodeID, NodeID> > > edge_sequence;
        
        ex_config conf;
        
        try {
            get_arguments(argc, argv, conf);
        } catch (std::string e) {
            std::cout << e << std::endl;
            return 1;
        }
        
        size_t n = read_sequence(conf.file, edge_sequence);
        std::string output_filename = get_output_filename(conf);
        
        if (edge_sequence.size() == 0) {
            throw std::string("ERROR: read empty edge sequence from file " + conf.file);
        }
        
        std::ofstream output_file;
        output_file.open(output_filename);
        output_file << "# insertion deletions G M time" << std::endl;
        
        if (conf.algorithm == ALGORITHM::gpa) {
            dyn_graph_access * dyn_G = create_graph(n);
            
            timer t;
            double time_elapsed = 0;
            int insertions = 0;
            int deletions = 0;
            
            for (size_t i = 0; i < edge_sequence.size(); ++i) {
                std::pair<NodeID, NodeID> edge = edge_sequence.at(i).second;
                
                // do addition or deletion
                if (edge_sequence.at(i).first) {
                    dyn_G->new_edge(edge.first, edge.second);
//                    dyn_G->new_edge(edge.second, edge.first);
                    
                    insertions++;
                } else {
                    dyn_G->remove_edge(edge.first, edge.second);
//                    dyn_G->new_edge(edge.second, edge.first);
                    
                    deletions++;
                }
                
                
                // for gpa, calculate matching only all "at_once" steps.
                if ((i+1) % conf.at_once == 0 || i == edge_sequence.size() - 1) {
                    graph_access G;
                    dyn_G->convert_to_graph_access(G);
                    
                    std::cout << "step: " << i << std::endl;
                    
                    std::cout <<  "graph has " <<  G.number_of_nodes() <<  " nodes and " <<  G.number_of_edges() <<  " edges"  << std::endl;
                    std::cout << "graph:    ";
                    forall_nodes(G,n) {
                        forall_out_edges(G,e,n) {
                            std::cout << n << " " << G.getEdgeTarget(e) << "; ";
                        } endfor
                    } endfor
                    
                    std::cout << std::endl;
                    
                    std::vector<std::pair<NodeID, NodeID> > pairvec_matching;
                    
                    pairvec_matching = gpa_match::match(G, time_elapsed, conf.seed);
                    
                    std::cout << "matching: ";
                    
                    print_matching(std::cout, pairvec_matching);
                    
                    // divide matching size by two for undirected graphs
                    output_file << insertions << " " << deletions << " " << G.number_of_edges()/2 << " "<< pairvec_matching.size()/2 << " " << time_elapsed << std::endl;
                    
                    insertions = 0;
                    deletions = 0;
                    
                    std::cout << std::endl;
                }
            }
            
            delete dyn_G;
        } else {
            int result_size;
            if (edge_sequence.size() % conf.at_once == 0) {
                result_size = edge_sequence.size()/conf.at_once;
            } else {
                result_size = edge_sequence.size()/conf.at_once + 1;
            }
            
            std::vector<std::vector<int> > combined_data(result_size);
            std::vector<double> combined_runtime(result_size, 0);
            
            for (int k = 0; k < conf.multi_run; ++k) {
                dyn_graph_access * G = create_graph(n);
                dyn_matching * algorithm;
                
                if (conf.algorithm == ALGORITHM::bgs) {
                    algorithm = new baswanaguptasen_dyn_matching(G);
                } else if (conf.algorithm == ALGORITHM::naive) {
                    algorithm = new simple_dyn_matching(G, conf.eps);
                } else if (conf.algorithm == ALGORITHM::ns) {
                    algorithm = new neimansolomon_dyn_matching(G);
                }
                
                timer t;
                double time_elapsed = 0;
                int insertions = 0;
                int deletions = 0;
                int j = 0;
                
                for (size_t i = 0; i < edge_sequence.size(); ++i) {
                    std::pair<NodeID, NodeID> edge = edge_sequence.at(i).second;
                    
                    if (edge_sequence.at(i).first) {
                        t.restart();
                        algorithm->new_edge(edge.first, edge.second);
                        time_elapsed += t.elapsed();
                        
                        insertions++;
                    } else {
                        t.restart();
                        algorithm->remove_edge(edge.first, edge.second);
                        time_elapsed += t.elapsed();
                        
                        deletions++;
                    }
                    
                    // size gets divided by two since M holds every edge twice as (u,v) and (v,u)
                    if ((i+1) % conf.at_once == 0 || i == edge_sequence.size() - 1) {
                        std::vector<std::pair<NodeID, NodeID> > matching = algorithm->getM();
                        
                        validate(matching);
                        print_matching(std::cout, matching);
                        
                        combined_data.at(j).push_back(insertions);
                        combined_data.at(j).push_back(deletions);
                        combined_data.at(j).push_back(algorithm->getG().number_of_edges()/2);
                        combined_data.at(j).push_back(matching.size()/2);
                        combined_runtime.at(j) = time_elapsed/conf.multi_run;
                        
                        j++;
                        
                        time_elapsed = 0;
                        insertions = 0;
                        deletions = 0;
                    }
                }
                
                for (size_t i = 0; i < combined_data.size(); ++i) {
                    output_file << combined_data.at(i).at(0) << " " 
                                << combined_data.at(i).at(1) << " " 
                                << combined_data.at(i).at(2) << " " 
                                << combined_data.at(i).at(3) << " " 
                                << combined_runtime.at(i) << std::endl;
                }
                
                delete G;
            }
        }
    } catch (std::string e) {
        std::cout << e << std::endl;
    }
    
    return 0;
}
