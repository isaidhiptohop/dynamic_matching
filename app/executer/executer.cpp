#include "executer.h"

/* 
 * takes arguments:
 *   -a[lgorithm]: ns, bgs, naive eps, gpa step
 *   -[-]a[t-]o[nce]: uint
 */
std::vector<std::string> ALGORITHM_NAMES = {"bgs", "gpa", "naive", "ns"};

std::vector<std::pair<NodeID, NodeID> > matching_to_pairvec (const Matching & matching) {
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

void check_option_arg (std::string option, int i, int argc, char** argv) {
    if (i+1 >= argc || argv[i+1][0] == '-') {
        throw std::string("ERROR: argument for option " + option + " missing.");
    }
}

void get_arguments(int argc, char ** argv, ex_config& conf) {
    if (argc == 1) {
        throw std::string("ERROR: need some arguments...");
    }
    
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
        } else if (option == "-mr") {
            check_option_arg(option, i, argc, argv);
            conf.multi_run = atoi(argv[i+1]);
            ++i;
/*
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
*/
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
/*    output = output + ALGORITHM_NAMES.at(conf.algorithm) + "_";
    
    if (conf.algorithm == ALGORITHM::gpa) {
        output = output + std::to_string(conf.seed) + "_";
    } else if (conf.algorithm == ALGORITHM::naive) {
        output = output + std::to_string(conf.eps) + "_";
    }
*/    
    output = output + std::to_string(conf.at_once);
    
    std::vector<std::string> buf = split(output, '/');
    output = buf.at(buf.size() - 1);
    
    output = "output/" + output;
    
    std::cout << "printing output to directory: " << output << "/" << std::endl;
    
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

EdgeID get_cumulated_degree(dyn_graph_access& G, std::vector<std::pair<NodeID, NodeID> > edgeset) {
    EdgeID cum_degree = 0;
    
    for (auto e : edgeset) {
        cum_degree += G.getNodeDegree(e.first);
    }
    
    return cum_degree;
}

NodeID count_nodes(std::vector<bool>& nodes) {
    NodeID count = 0;
    
    for (auto n : nodes) {
        if (n) count++;
    }
    
    return count;
}

NodeID count_nodes(std::vector<int>& nodes, int min_degree) {
    NodeID count = 0;
    
    for (auto n : nodes) {
        if (n >= min_degree) count++;
    }
    
    return count;
}
