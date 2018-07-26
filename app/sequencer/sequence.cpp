#include "sequence.h"

std::vector<std::string> MODE_NAMES = {"addition-only", "random-step", "sliding-window", "meyerhenke", "native"};

sequence::sequence (size_t n, size_t k, MODE mode, size_t window, long seed, std::string ifile) {
    this->n = n;
    this->k = k;
    this->mode = mode;
    
    this->window = window;
    
    if (seed < 0) {
        seed = static_cast<long unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
        random_functions rng;
        rng.setSeed(seed);
        this->seed = rng.nextInt(0, UINT_MAX);
    } else {
        this->seed = seed;
    }
    
    helper = 0;
    
    this->ifile = ifile;
    
    add_count = 0;
    del_count = 0;
    
    built = false;
    
    ofile.open("sequences/" + get_name());
    
    it = 0;
}

std::vector<std::string> sequence::split (const std::string& input, const char& mark) {
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

std::pair<NodeID, NodeID> sequence::load_from_file() {
    std::ifstream input(ifile);
    std::string line;
    
    int lineCount = 0;
    std::vector<std::pair<std::pair<int, int>, std::pair<NodeID, NodeID> > > from_file;
    
//    input.open();
    long n_buf = -1;
    long k_buf = -1;
    
    NodeID max_u = 0;
    NodeID max_v = 0;
    
    size_t i = 0;
//    int skipped = 0;
    
    if (input.is_open()) {
        while (std::getline(input, line)) {
            if (line.size() != 0 && line.at(0) == '%' && lineCount == 1) {
                // first valid line has to be the n
                if (n_buf < 0 && k_buf < 0) {
                    std::vector<std::string> substrings = split(line, ' ');
                    
                    k_buf = atoi(substrings.at(1).c_str());
                    n_buf = atoi(substrings.at(2).c_str());
                    
                    if (substrings.size() > 3) {
                        long check = atoi(substrings.at(3).c_str());
                    
                        if (n_buf != check) {
                            n_buf += check;
                        }
                    }
                }
            } else if (line.size() != 0 && line.at(0) != '%') {
//                if (helper++ % 10) { i++; continue; }
                if (i >= k) break;
                if (!(i % 10000)) std::cout << i << "/" << k_buf << " read..." << std::endl;
                std::vector<std::string> substrings = split(line, ' ');
                
                // n counts from 0 to n, the koblenz format starts counting from 1, therefore we substract 1
                NodeID u = atoi(substrings.at(0).c_str()) - 1;
                NodeID v = atoi(substrings.at(1).c_str()) - 1;
                
                if (u == v) continue;
                
                if (u > max_u && u < n) max_u = u;
                if (v > max_v && v < n) max_v = v;
                
                if (substrings.size() >= 4) {
                    int addition = (atoi(substrings.at(2).c_str()) < 0 ? 0 : 1);
                    int timestamp = atoi(substrings.at(3).c_str());
                    
                    if (u < n && v < n && u != v) {
                        from_file.push_back({{timestamp, addition}, {u, v}});
                        i++;
                    }
                } else {
                    if (u < n && v < n && u != v) {
                        from_file.push_back({{i, 1}, {u, v}});
                        i++;
                    }
                }
            }
            
            lineCount++;
        }
        
        input.close();
    } else {
        throw new std::string("could not find file " + ifile);
    }
    
    std::cout << "nodes: " << max_u << " " << max_v << std::endl;
    
    std::pair<unsigned, unsigned> result({max_u, max_v});
    
    // now we sort edges_buf by timestamp using std::sort
    
    std::sort(
        from_file.begin(), 
        from_file.end(), 
        [](const std::pair<std::pair<int, int>, std::pair<NodeID, NodeID> >& a, const std::pair<std::pair<int, int>, std::pair<NodeID, NodeID> >& b) {
            return a.first.first < b.first.first;
        }
    );
    
    for (size_t i = 0; i < from_file.size(); ++i) {
        buf.push_back({from_file.at(i).first.second, from_file.at(i).second});
    }
    
    std::cout << "read " << buf.size() << " edges." << std::endl;
    
    // adjust sequence size to size of sequence in file
    if (buf.size() < k) {
        k = buf.size();
    }
    
    std::cout << "k after file read is " << k << std::endl;
    
    return result;
}

// only_addition, random, sliding_window, meyerhenke
void sequence::create () {
    if (built) return;
    
    std::pair<unsigned, unsigned> max_n;
    
    if (ifile == "") {
        std::cout << "creating " << MODE_NAMES[mode] << " sequence from seed... " << std::endl;
    } else {
        max_n = load_from_file();
    }
    
    ofile << "# " << n << std::endl;
    
    if (mode == MODE::only_addition) {
        create_only_addition_seq();
    } else if (mode == MODE::random_step) {
        create_random_seq();
    } else if (mode == MODE::sliding_window) {
        create_sliding_window_seq();
    } else if (mode == MODE::meyerhenke) {
        create_meyerhenke_seq();
    } else if (mode == MODE::native) {
        create_native_seq();
    } else {
        return;
    }
    
    built = true;
}

void sequence::finish () {
    ofile.close();
}

void sequence::print_last () {
    std::pair<int, std::pair<NodeID, NodeID> > edge = edge_sequence.at(edge_sequence.size() - 1);
    ofile << edge.first << " " << edge.second.first << " " << edge.second.second << std::endl;
}

std::pair<int, std::pair<NodeID, NodeID> > sequence::create_edge (const random_functions& rng) {
    if (ifile == "") {
        int u = rng.nextInt(0, n-1);
        int v;

        do { // avoid that u == v
            v = rng.nextInt(0, n-1);
        } while (v == u);
        
        return std::pair<int, std::pair<NodeID, NodeID> >({1, {u, v}});
    } else {
        if (it < buf.size()) {
            std::pair<int, std::pair<NodeID, NodeID> > edge = buf.at(it++);
            return edge;
        } else {
            throw std::string("no more edge insertions in file: " + std::to_string(it));
        }
    }
}

void sequence::create_native_seq() {
    /* creates a sequence of length k, containing only random additions
     */
    random_functions rng;
    rng.setSeed(seed);
    
    for (size_t i = 0; i < k; ++i) {
        std::pair<int, std::pair<NodeID, NodeID> > edge = create_edge(rng);
        edge_sequence.push_back(edge);
        add_count++;
        print_last();
    }
}

void sequence::create_only_addition_seq() {
    /* creates a sequence of length k, containing only random additions
     */
    random_functions rng;
    rng.setSeed(seed);
    
    for (size_t i = 0; i < k; ++i) {
        std::pair<NodeID, NodeID> edge = create_edge(rng).second;
        edge_sequence.push_back({1, edge});
        add_count++;
        print_last();
    }
}

void sequence::create_random_seq() {
    random_functions rng;
    rng.setSeed(seed);
    
    std::vector<std::pair<NodeID, NodeID> > additions;
    
    for (size_t i = 0; i < k; ++i) {
        bool addition;
        
        if (additions.size() >= 1) { // only allow possibility of deletion, when there are added edges
            addition = rng.nextInt(0,1);
        } else {
            addition = true;
        }
        
        if (addition) { // if it's an addition, I simply create a random edge with u,v \in [0,n-1]
            std::pair<NodeID, NodeID> edge = create_edge(rng).second;
            edge_sequence.push_back({1, edge});
            additions.push_back(edge);
            add_count++;
        } else { // if it's an deletion, I take a random edge from the vector of additions and add it as deletion.
            int index = rng.nextInt(0, additions.size()-1);
            edge_sequence.push_back({0, additions.at(index)});
            additions.at(index) = additions.back();
            additions.pop_back();
            del_count++;
        }
        print_last();
    }
}

void sequence::create_sliding_window_seq() {
    random_functions rng;
    rng.setSeed(seed);
    
    std::vector<std::pair<NodeID, NodeID> > additions;
    
    size_t i = 0;
    size_t j = 0;
    
    for ( ; i < window && i < k; i++) {
        std::pair<NodeID, NodeID> edge = create_edge(rng).second;
        edge_sequence.push_back({1, edge});
        additions.push_back(edge);
        add_count++;
        print_last();
    }
    
    for ( ; i < k - window; i += 2, j++) {
        std::pair<NodeID, NodeID> edge = create_edge(rng).second;
        edge_sequence.push_back({1, edge});
        additions.push_back(edge);
        add_count++;
        print_last();
        
        edge = additions.at(j);
        edge_sequence.push_back({0, edge});
        del_count++;
        print_last();
    }
    
    for ( ; j < additions.size(); j++) {
        std::pair<NodeID, NodeID> edge = additions.at(j);
        edge_sequence.push_back({0, edge});
        del_count++;
        print_last();
    }
}

void sequence::create_meyerhenke_seq() {
    random_functions rng;
    rng.setSeed(seed);
    
    std::vector<std::pair<NodeID, NodeID> > additions;
    
    size_t i = 0;
    
    for (; i < window;) {
        std::pair<NodeID, NodeID> edge = create_edge(rng).second;
        edge_sequence.push_back({1, edge});
        additions.push_back(edge);
        i++;
        add_count++;
        print_last();
    }
    
    for (; i < k - window;) {
        int index = rng.nextInt(0, additions.size()-1);
        std::pair<NodeID, NodeID> edge = additions.at(index);
        
        edge_sequence.push_back({0, edge});
        i++;
        del_count++;
        print_last();
        
        edge_sequence.push_back({1, edge});
        i++;
        add_count++;
        print_last();
    }
    
    for (int j = 0; i < k;) {
        std::pair<NodeID, NodeID> edge = additions.at(j++);
        edge_sequence.push_back({0, edge});
        i++;
        del_count++;
        print_last();
    }
}

std::string sequence::get_name() {
    std::string name = "";
    
    if (ifile == "") {
        name = name + "fromseed_";
    } else {
        std::vector<std::string> foo = split(ifile, '/');
        name = name + foo.at(foo.size() - 1) + "_";
    }
    
    name = name + std::to_string(seed) + "_";
    name = name + std::to_string(n) + "_";
    name = name + std::to_string(k) + "_";
    name = name + MODE_NAMES.at(mode);
    
    if (mode == MODE::sliding_window || mode == MODE::meyerhenke) {
        name = name + "_" + std::to_string(window);
    }
    
    name = name + ".sequence";
    
    return name;
}
