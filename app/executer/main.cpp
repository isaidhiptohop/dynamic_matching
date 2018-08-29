#include <sys/stat.h>

#include "executer.h"

// creates new instance of the passed algorithm.
dyn_matching * init_algorithm (ALGORITHM algorithm, dyn_graph_access * G, double eps);

std::string simplify (long secs);

int main (int argc, char ** argv) {
    try {
        // get command line arguments and save them to ex_config struct 
        ex_config conf;
        
        try {
            get_arguments(argc, argv, conf);
        } catch (std::string e) {
            std::cout << e << std::endl;
            return 1;
        }
        
        
        // initialize edge sequence 
        std::vector<std::pair<int, std::pair<NodeID, NodeID> > > edge_sequence;
        size_t n = read_sequence(conf.file, edge_sequence);
        
        if (edge_sequence.size() == 0) {
            throw std::string("ERROR: read empty edge sequence from file " + conf.file);
        }
        
        
        // initialize list of algorithms 
        std::vector<ALGORITHM> algorithms({ALGORITHM::bgs, 
                                           ALGORITHM::naive,
                                           ALGORITHM::ns,
                                           ALGORITHM::rw_v1, 
                                           ALGORITHM::rw_v1, 
                                           ALGORITHM::rw_v1,
                                           ALGORITHM::rw_v2, 
                                           ALGORITHM::rw_v2, 
                                           ALGORITHM::rw_v2,
                                           ALGORITHM::rw_v3
        });
        std::vector<double>    eps       ({0,
                                           0,
                                           0,
                                           0.5,
                                           0.1,
                                           0.01,
                                           0.5,
                                           0.1,
                                           0.01,
                                           0
        });
        
        ASSERT_TRUE(algorithms.size() == eps.size());
        
        srand(conf.seed);
        random_functions::setSeed(conf.seed);
        
        
        // initialize output directory and files 
        std::string output_filename = get_output_filename(conf);
        
        mkdir(std::string(output_filename).c_str(), 0775);

        std::ofstream output_file;
        output_file.open(output_filename + "/data");
        
        output_file << "# ";
        
        for (size_t i = 0; i < algorithms.size(); ++i) {
            output_file << ALGORITHM_NAMES.at(algorithms.at(i)) << (eps.at(i)? (" " + std::to_string(eps.at(i))) : "") << ", ";
        }
        
        std::ofstream matching_file;
        matching_file.open(output_filename + "/matchings");
        
        std::ofstream counters_file;
        counters_file.open(output_filename + "/counters");
        
        std::ofstream counternames_file;
        counternames_file.open(output_filename + "/counter_names");
        
//        mkdir(std::string(output_filename + "/snapshots").c_str(), 0775);
        
        
        // calculate size (lines) of the resulting data to reserve space.
        size_t result_size;
        if (edge_sequence.size() % conf.at_once == 0) {
            result_size = edge_sequence.size()/conf.at_once;
        } else {
            result_size = edge_sequence.size()/conf.at_once + 1;
        }
        
        std::cout << "result size: " << result_size << std::endl;
        
        // intialize result vectors
        
        // combined_data is 3D: algorithms X evaluated sequence steps X measurements
        // algorithms is determined by algorithms.size(),
        // the evaluated sequence steps corresponds to result_size,
        // the measurements are 4: insertions, deletions, cardinality of G, cardinality of M
        std::vector<std::vector<std::vector<int> > > combined_data(algorithms.size(), std::vector<std::vector<int> >(result_size, std::vector<int>(4, 0)));
        
        // combined_runtime is 2D: algorithms X evaluated sequence steps
        // the dimensions are the same as for combined_data, only there is only one measurement, the elapsed time
        std::vector<std::vector<double> > combined_runtime(algorithms.size(), std::vector<double>(result_size, 0));
        
        // matchings is 3D vector, which contains at the position (i, j, k) the kth edge of the jth sequence step of the ith algorithm:
        //
        // matchings = [
        //                 [                           // algorithm
        //                     [(u,v), (w,x), ...],    // matching of sequence step which edges as entries
        //                     [(a,b), (...), ...],
        //                     ...
        //                 ],
        //                 [                           // next algorithm
        //                     [...],                  // again matching for every sequence step.
        //                     ...
        //                 ],
        //                 ...
        //             ]
        // 
        // matchings is therefore of dimension: algorithms X evaluated sequence steps X matching size (X 2, since the elements then again are pairs)
        std::vector<std::vector<std::vector<std::pair<NodeID, NodeID> > > > matchings(algorithms.size(), std::vector<std::vector<std::pair<NodeID, NodeID> > >(result_size));
        
        // previous matchings holds the matchings of the previous run, in order to compare results of different runs
        // for deterministic algorithms (neiman-solomon) there should be no changes in the results (similarity = 1), 
        // for randomized algorithms it is highly unlikely that the results do not differ (similarity < 1).
        std::vector<std::vector<std::vector<std::pair<NodeID, NodeID> > > > previous_matchings;
        
        std::vector<std::vector<double> > cross_run_similarities(algorithms.size(), std::vector<double>(result_size, 0));
        std::vector<std::vector<int> > cross_run_sim_counters(algorithms.size(), std::vector<int>(result_size, 0));
        
        std::vector<bool> nodes(n, false);
        std::vector<int> node_degrees(n, 0);
        
        
        //================// START CALCULATIONS //================//
        
        timer t;
        
        for (int k = 0; k < conf.multi_run; ++k) { // run the whole thing several times
//            std::cout << "run " << k+1 << "/" << conf.multi_run << std::endl;
            // hold all matchings of one run in vector.
            
            for (size_t l = 0; l < algorithms.size(); ++l) { // run different algorithms
//                std::cout << "running algorithm " << ALGORITHM_NAMES.at(algorithms.at(l)) << std::endl;
                dyn_graph_access * G = create_graph(n);
                dyn_matching * algorithm = init_algorithm(algorithms.at(l), G, eps.at(l));
                
                // initalize timer and counters
                double time_elapsed = 0;
                int insertions = 0;
                int deletions = 0;
                int j = 0; // counter for entries in data vectors. from range 0 to result_size
                
                node_degrees.resize(0);
                node_degrees.resize(n, 0);
                
                for (size_t i = 0; i < edge_sequence.size(); ++i) { // iterate through sequence
                    std::pair<NodeID, NodeID> edge = edge_sequence.at(i).second;
                    
                    double tmp;
                    bool success;
                    
                    // for every NodeID save if it exists or not.
                    nodes.at(edge.first) = true;
                    nodes.at(edge.second) = true;
                    
                    // determine whether to do an insertion or a deletion
                    if (edge_sequence.at(i).first) {
                        success = algorithm->new_edge(edge.first, edge.second, tmp);
                        
                        if (success) {
                            node_degrees.at(edge.first) = node_degrees.at(edge.first) + 1;
                            node_degrees.at(edge.second) = node_degrees.at(edge.second) + 1;
                        }
                        
                        insertions++;
                    } else {
                        success = algorithm->remove_edge(edge.first, edge.second, tmp);
                        
                        if (success) {
                            if (node_degrees.at(edge.first) > 0) {
                                node_degrees.at(edge.first) = node_degrees.at(edge.first) - 1;
                            } else {
                                node_degrees.at(edge.first) = 0;
                            }
                            
                            if (node_degrees.at(edge.second) > 0) {
                                node_degrees.at(edge.second) = node_degrees.at(edge.second) - 1;
                            } else {
                                node_degrees.at(edge.second) = 0;
                            }
                        }
                        
                        deletions++;
                    }
                    
                    time_elapsed += tmp;
                    
                    // every at_once steps we collect data
                    if ((i+1) % conf.at_once == 0 || i == edge_sequence.size() - 1) {
                        std::vector<std::pair<NodeID, NodeID> > matching = algorithm->getM();
                        
                        algorithm->counters_next();
                        
                        // validate the matching to exclude errors in the algorithm
                        quality_metrics::matching_validation(matching);
                        
                        
                        // do snapshots, but only in the first of multiple runs.
                        /*
                        if (k == 0) {
                            graph_access G_static;
                            G->convert_to_graph_access(G_static);
                            graph_io::writeGraph(G_static, std::string(output_filename + "/snapshots/snapshot_" + std::to_string(i+1) + ".graph"));                        
                        }
                        */
                        
                        // save data in arrays, l = algorithm, j = entry
                        matchings.at(l).at(j) = matching;
                        
                        combined_data.at(l).at(j).at(0) = get_cumulated_degree(*G, matching);
                        combined_data.at(l).at(j).at(1) = count_nodes(node_degrees, 1); // count number of nodes with degree > 1
                        
                        // size gets divided by two since M holds every edge twice as (u,v) and (v,u)
                        combined_data.at(l).at(j).at(2) = algorithm->getG().number_of_edges()/2;
                        combined_data.at(l).at(j).at(3) = matching.size()/2;
                        combined_runtime.at(l).at(j) = combined_runtime.at(l).at(j) + time_elapsed/conf.multi_run;
                        
                        // iterator for data vector gets incremented only when data is written to the data vector
                        // j = 0:1:result_size; // according to octave syntax
                        j++;
                        
                        // reset counters
                        time_elapsed = 0;
                        insertions = 0;
                        deletions = 0;
                        
                        // monitor some progress
                        std::ofstream progress;
                        progress.open(output_filename + "/progress", std::ios::trunc);
                        double pre_status = ((k * edge_sequence.size() * algorithms.size()) + (l * edge_sequence.size()) + (i) * 1.0) / (conf.multi_run * algorithms.size() * edge_sequence.size());
                        progress << ((int) (pre_status * 10000))/100.0 << "% done, estimated remaining time: " << simplify(1.0 / pre_status * t.elapsed())  << std::endl;
                        progress.close();
                    }
                }
                
                delete algorithm;
                //delete G; // uncommented since the destructor of the algorithm also frees the passed graph pointer.
            } // going to next algorithm
            
            if (previous_matchings.size() > 0) {
                ASSERT_TRUE(matchings.size() == previous_matchings.size()); // both vectors should contains algorithms.size() subvectors
                ASSERT_TRUE(matchings.size() == algorithms.size());
                
                for (size_t i = 0; i < matchings.size(); ++i) { // iterate through the algorithms
                    ASSERT_TRUE(matchings.at(i).size() == previous_matchings.at(i).size()); // the subvectors contain the matchings for
                    ASSERT_TRUE(matchings.at(i).size() == result_size);                     // the corresponding ith sequence step. since
                                                                                            // every run processes the same sequence, the
                                                                                            // size of those subvectors should also match.
                    for (size_t j = 0; j < matchings.at(i).size(); ++j) {
                        if (matchings.at(i).at(j).size() != 0 || previous_matchings.at(i).at(j).size() != 0) {
                            /*
                            std::cout << std::endl;
                            print_matching(std::cout, matchings.at(i).at(j));
                            print_matching(std::cout, previous_matchings.at(i).at(j));
                            */
                            int union_size = 1;
                            int intersect_size = quality_metrics::edgeset_intersect(matchings.at(i).at(j), previous_matchings.at(i).at(j), union_size).size();
                            
                            // same as with cross algorithm similarity, for union size = 0, similarity is 0
                            cross_run_similarities.at(i).at(j) = cross_run_similarities.at(i).at(j) + (union_size ? ((intersect_size * 1.0) / union_size) : 0);
                            cross_run_sim_counters.at(i).at(j)++;
                        }
                    }
                }
            }
            
            previous_matchings = matchings;
            matchings.resize(algorithms.size());
        } // end of multiple runs with the same algorithm
        
        
        //================// PROCESS INFORMATION AND PRINT TO FILE //================//
        
        // get counters
        counters::print_names(counternames_file);
        
//        counters::divide_by(conf.multi_run);
//        counters::divide_by_d(conf.multi_run);
        
        counters::print(counters_file);
        
        size_t node_count = count_nodes(nodes);
        
        std::ofstream n_file;
        n_file.open(output_filename + "/n");
        
        n_file << node_count;
        
        output_file << "nodes: " << node_count << std::endl;
        
        output_file << "# " << algorithms.size() << "x "
                    << "cum_degree_M nodes_deg>1 G M time cr-similarity";
        
        output_file << std::endl;
        /*
        std::vector<double> total_cross_run_similarities(algorithms.size(), 0);
        std::vector<int> total_cross_run_sim_counters(algorithms.size(), 0);
        
        for (size_t i = 0; i < matchings.size(); ++i) {
            for (size_t j = 0; j < result_size; ++j) {
                total_cross_run_similarities.at(i) = total_cross_run_similarities.at(i) + cross_run_similarities.at(i).at(j);
                total_cross_run_sim_counters.at(i) = total_cross_run_sim_counters.at(i) + cross_run_sim_counters.at(i).at(j);
            }
            
            std::cout << "cross run similarity for " << ALGORITHM_NAMES.at(algorithms.at(i))
                      << (eps.at(i)? (" " + std::to_string(eps.at(i))) : "") << ": "
                      << total_cross_run_similarities.at(i) / total_cross_run_sim_counters.at(i)
                      << std::endl;
        }
        
        std::cout << "cross algorithm similarity: " << similarity / sim_counter << std::endl;
        */
        
        ASSERT_TRUE(combined_data.size() == combined_runtime.size());
        
        // write data to file
        for (size_t i = 0; i < result_size; ++i) { // iterate through sequence steps
            for (size_t j = 0; j < algorithms.size(); ++j) { // iterate through different runs with different algorithms
                output_file << combined_data.at(j).at(i).at(0) << " " 
                            << combined_data.at(j).at(i).at(1) << " " 
                            << combined_data.at(j).at(i).at(2) << " " 
                            << combined_data.at(j).at(i).at(3) << " " 
                            << combined_runtime.at(j).at(i) << " "
                            
                            // check that we don't divdide by zero
                            << (cross_run_sim_counters.at(j).at(i) ? cross_run_similarities.at(j).at(i) / cross_run_sim_counters.at(j).at(i) : 0) << " ";
                            
                print_matching(matching_file, matchings.at(j).at(i));
            }
            
            output_file << std::endl;
            matching_file << std::endl;
        }
    } catch (std::string e) {
        std::cout << e << std::endl;
    }
    
    return 0;
}


// auxiliary functions 

dyn_matching * init_algorithm (ALGORITHM algorithm, dyn_graph_access * G, double eps) {
    if (algorithm == ALGORITHM::bgs) {
        return new baswanaguptasen_dyn_matching(G);
        
    } else if (algorithm == ALGORITHM::naive) {
        return new naive_dyn_matching(G);
        
    } else if (algorithm == ALGORITHM::ns) {
        return new neimansolomon_dyn_matching(G);
        
    } else if (algorithm == ALGORITHM::rw_v1) {
        return new simple_dyn_matching(G, eps);
        
    } else if (algorithm == ALGORITHM::rw_v2) {
        return new randomwalkv2_dyn_matching(G, eps);
        
    } else if (algorithm == ALGORITHM::rw_v3) {
        return new randomwalkv3_dyn_matching(G);
        
    } else {
        return nullptr;
    }
}

std::string simplify (long secs) {
    int mins = (secs / 60) % 60;
    secs -= mins * 60;
    int hours = (secs / (60 * 60)) % 24;
    secs -= hours * 60 * 60;
    int days = (secs / (60 * 60 * 24));
    
    return std::string(std::to_string(days) + "d " + std::to_string(hours) + "h " + std::to_string(mins) + "m");
}
