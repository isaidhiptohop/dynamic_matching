#include <iostream>
#include <fstream>
#include <vector>

#include "tools/quality_metrics.h"

struct fileio {
    int index = 0;
    int algorithm = 0; // 
    int num_of_algorithms = 5;
    
    std::string filename;
    std::ifstream input;
    
    fileio (std::string filename);
    
    void reset ();
    
    bool next_matching (std::vector<std::pair<NodeID, NodeID> >& matching);
    bool next_gpa_matching (std::vector<std::pair<NodeID, NodeID> >& matching);
};

std::vector<std::string> split (const std::string& input, const char& mark);
bool parse_to_matching (std::vector<std::pair<NodeID, NodeID> >& matching, std::string line);


int main (int argc, char ** argv) {
    std::string prefix;
    
    if (argc >= 2) {
        prefix = argv[1];
    } else {
        std::cout << "error. no directory path given." << std::endl;
        return 1;
    }
    
    
    // initialize fileio access
    fileio matchings(std::string(prefix + "matchings"));
    fileio gpa_matchings_io(std::string(prefix + "matchings_gpa"));
    
    
    // buffer matching
    std::vector<std::pair<NodeID, NodeID> > matching;
    
    
    // get all gpa_matchings into vector
    std::vector<std::vector<std::pair<NodeID, NodeID> > > gpa_matchings;
    
    while (gpa_matchings_io.next_gpa_matching(matching)) {
        gpa_matchings.push_back(matching);
        std::cout << gpa_matchings_io.input.eof() << " " << matching.size() << std::endl;
    }
    
    std::cout << gpa_matchings.size() << " matchings" << std::endl;
    
    
    std::vector<std::vector<int> > results (gpa_matchings.size());
    
    
    for (int i = 0; i < matchings.num_of_algorithms; ++i) {
        matchings.algorithm = i;
        size_t j = 0;
        
        while (matchings.next_matching(matching) && j < gpa_matchings.size()) {
            int union_size = 1;
            int intersect_size = quality_metrics::edgeset_intersect(gpa_matchings.at(j), matching, union_size).size();
            
            results.at(j).push_back(gpa_matchings.at(j).size()/2);
            results.at(j).push_back(matching.size()/2);
            results.at(j).push_back(intersect_size/2);
            /*
            std::cout << j << " " 
                      << gpa_matchings.at(j).size()/2 << " "
                      << matching.size()/2 << " "
                      << intersect_size/2 << " "
                      << std::endl;
            */
            j++;
        }
        
        std::cout << j << " matchings" << std::endl;
        
        matchings.reset();
    }
    
    std::ofstream output(std::string(prefix + "intersects"));
    
    for (size_t i = 0; i < results.size(); ++i) {
        for (size_t j = 0; j < results.at(i).size(); ++j) {
            output << results.at(i).at(j) << " ";
        }
        
        output << std::endl;
    }
    
    return 0;
}


fileio::fileio (std::string filename) {
    this->filename = filename;
    input = std::ifstream(filename);
}

void fileio::reset () {
    input.clear();
    input.seekg(0, std::ios::beg);
    index = 0;
}

bool fileio::next_matching (std::vector<std::pair<NodeID, NodeID> >& matching) {
    matching.resize(0);
    std::string line;
    
    if (input.is_open()) {
        // from each block of num_of_algorithms lines (=matchings) we only want one.
        // num_of_algorithms +1 for the blank line.
        while (index % (num_of_algorithms+1) != algorithm) {
            getline(input, line);
            index++;
            
            if (input.eof()) return false;
        } // as soon as the condition doesn't match, we stop at the right position.
        
        if (input.eof()) return false;
        getline(input, line);
        index++;
    }
    
    return parse_to_matching(matching, line);
}

bool fileio::next_gpa_matching (std::vector<std::pair<NodeID, NodeID> >& matching) {
    matching.resize(0);
    std::string line;
    
    if (input.is_open()) {
        getline(input, line);
    }
    
    if (input.eof()) return false;
    
    return parse_to_matching(matching, line);
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

bool parse_to_matching (std::vector<std::pair<NodeID, NodeID> >& matching, std::string line) {
    std::vector<std::string> substrings = split(line, ';');
    
    for (size_t i = 0; i < substrings.size(); ++i) {
        std::vector<std::string> edge = split(substrings.at(i), ' ');
        size_t j = (edge.at(0) == "" ? 1 : 0);
        
        if (edge.size() > j+1) {
            matching.push_back({
                std::stoi(edge.at(j)),
                std::stoi(edge.at(j+1))
            });
        }
    }
    
    return true;
}
