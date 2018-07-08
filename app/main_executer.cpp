#include "executer.h"

// creates new instance of the passed algorithm.
dyn_matching * init_algorithm (ALGORITHM algorithm, dyn_graph_access * G, double eps);

int main (int argc, char ** argv) {
    try {
        /* get command line arguments and save them to ex_config struct */
        ex_config conf;
        
        try {
            get_arguments(argc, argv, conf);
        } catch (std::string e) {
            std::cout << e << std::endl;
            return 1;
        }
        
        
        /* initialize edge sequence */
        std::vector<std::pair<int, std::pair<NodeID, NodeID> > > edge_sequence;
        size_t n = read_sequence(conf.file, edge_sequence);
        
        if (edge_sequence.size() == 0) {
            throw std::string("ERROR: read empty edge sequence from file " + conf.file);
        }
        
        
        /* initialize list of algorithms */
        std::vector<ALGORITHM> algorithms({ALGORITHM::bgs, ALGORITHM::naive, ALGORITHM::naive, ALGORITHM::naive, ALGORITHM::ns});
        std::vector<double>    eps       ({0,              0.5,              0.1,              0.01,             0});
        
        srand(conf.seed);
        random_functions::setSeed(conf.seed);
        
        
        /* initialize output file */
        std::string output_filename = get_output_filename(conf);
        
        std::ofstream output_file;
        output_file.open(output_filename);
        
        output_file << "# ";
        
        for (size_t i = 0; i < algorithms.size(); ++i) {
            output_file << ALGORITHM_NAMES.at(algorithms.at(i)) << (eps.at(i)? (" " + std::to_string(eps.at(i))) : "") << ", ";
        }
        
        output_file << std::endl;
        output_file << "# ";
        
        for (size_t i = 0; i < algorithms.size(); ++i) {
            output_file << "insertion deletions G M time ";
        }
        
        output_file << std::endl;
        
        std::ofstream matching_file;
        matching_file.open(output_filename + "_matchings");
        
        /*
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
                    
                    graph_io::writeGraph(G, std::string("snapshots/snapshot_" + std::to_string(i) + ".graph"));
                    

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
                    
//                    pairvec_matching = gpa_match::match(G, time_elapsed, conf.seed);
                    
                    std::cout << "matching: ";
                    
//                    print_matching(std::cout, pairvec_matching);
                    
                    // divide matching size by two for undirected graphs
//                    output_file << insertions << " " << deletions << " " << G.number_of_edges()/2 << " "<< pairvec_matching.size()/2 << " " << time_elapsed << std::endl;
                    
                    insertions = 0;
                    deletions = 0;
                    
                    std::cout << std::endl;
*//*
                }
            }
            
            delete dyn_G;
        } else {
        */
        /* calculate size (lines) of the resulting data to reserve space. */
        size_t result_size;
        if (edge_sequence.size() % conf.at_once == 0) {
            result_size = edge_sequence.size()/conf.at_once;
        } else {
            result_size = edge_sequence.size()/conf.at_once + 1;
        }
        
        
        /* intialize result vectors */
        std::vector<std::vector<std::vector<int> > > combined_data(algorithms.size(), std::vector<std::vector<int> >(result_size, std::vector<int>(4, 0)));
        std::vector<std::vector<double> > combined_runtime(algorithms.size(), std::vector<double>(result_size, 0));
        
        /*
         * matchings is 3D vector, which contains at the position (i, j, k) the kth edge of the jth sequence step of the ith algorithm:
         * 
         * matchings =  [
         *                  [                           // algorithm
         *                      [(u,v), (w,x), ...],    // matching of sequence step which edges as entries
         *                      [(a,b), (...), ...],
         *                      ...
         *                  ],
         *                  [                           // next algorithm
         *                      [...],                  // again matching for every sequence step.
         *                      ...
         *                  ],
         *                  ...
         *              ]
         */
        std::vector<std::vector<std::vector<std::pair<NodeID, NodeID> > > > matchings(algorithms.size(), std::vector<std::vector<std::pair<NodeID, NodeID> > >(result_size));
        // previous matchings holds the matchings of the previous run, in order to compare results of different runs
        // for deterministic algorithms (neiman-solomon) there should be no changes in the results (similarity = 1), 
        // for randomized algorithms it is highly unlikely that the results do not differ (similarity < 1).
        std::vector<std::vector<std::vector<std::pair<NodeID, NodeID> > > > previous_matchings;
        
        std::vector<double> cross_run_similarities(matchings.size(), 0);
        std::vector<int> cross_run_sim_counters(matchings.size(), 0);
        
        double similarity = 0;
        int sim_counter = 0;
        
        
        /* start calculations */
        for (int k = 0; k < conf.multi_run; ++k) { // run the whole thing several times
            // hold all matchings of one run in vector.
            size_t m = 0;
            
            for (size_t l = 0; l < algorithms.size(); ++l) { // run different algorithms
                dyn_graph_access * G = create_graph(n);
                dyn_matching * algorithm = init_algorithm(algorithms.at(l), G, eps.at(l));
                
                // initalize timer and counters
                timer t;
                double time_elapsed = 0;
                int insertions = 0;
                int deletions = 0;
                int j = 0;
                
                for (size_t i = 0; i < edge_sequence.size(); ++i) { // iterate through sequence
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
                    
                    if ((i+1) % conf.at_once == 0 || i == edge_sequence.size() - 1) {
                        std::vector<std::pair<NodeID, NodeID> > matching = algorithm->getM();
                        
                        quality_metrics::matching_validation(matching);
//                        print_matching(std::cout, matching);
                        
                        matchings.at(l).at(j) = matching;
                        
                        combined_data.at(l).at(j).at(0) = insertions;
                        combined_data.at(l).at(j).at(1) = deletions;
                        
                        // size gets divided by two since M holds every edge twice as (u,v) and (v,u)
                        combined_data.at(l).at(j).at(2) = algorithm->getG().number_of_edges()/2;
                        combined_data.at(l).at(j).at(3) = matching.size()/2;
                        combined_runtime.at(l).at(j) = time_elapsed/conf.multi_run;
                        
                        // iterator for data vector gets incremented only when data is written to the data vector
                        // j = 0:1:result_size; // according to octave syntax
                        j++;
                        
                        // reset counters
                        time_elapsed = 0;
                        insertions = 0;
                        deletions = 0;
                    }
                }
                
                /* 
                 * matchings is 3D vector, which contains at the position (i, j, k) the kth edge of the jth sequence step of the ith algorithm.
                 * see above for details.
                 * 
                 * therefore the size of every subvector in matchings should have the same size, namely the size of the sequence.
                 * however the size of the calculated matchings inside the subvector can differ.
                 * 
                 * in the following if-block we iterate through the sequence steps and compare the matching resutls of different algorithms
                 * for every time-step.
                 * 
                 * for randomized algorithms the result of the same algorithm should also differ from run to run.
                 */
                if (l >= 1 && m < algorithms.size()) {
                    size_t upper_limit = matchings.at(m).size() > matchings.at(m+1).size() ? matchings.at(m).size() : matchings.at(m+1).size();
                    if (m+1 < algorithms.size()) {
                        for (size_t i = 0; i < upper_limit; ++i) {
                            similarity = similarity + quality_metrics::matching_similarity(matchings.at(m).at(i), matchings.at(m+1).at(i));
                            sim_counter++;
                        }
                    } else {
                        for (size_t i = 0; i < upper_limit; ++i) {
                            similarity = similarity + quality_metrics::matching_similarity(matchings.at(m).at(i), matchings.at(0).at(i));
                            sim_counter++;
                        }
                    }
                    
                    m++;
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
                            cross_run_similarities.at(i) = cross_run_similarities.at(i) + quality_metrics::matching_similarity(matchings.at(i).at(j), previous_matchings.at(i).at(j));
                            cross_run_sim_counters.at(i)++;
                        }
                    }
                }
            }
            
            previous_matchings = matchings;
            matchings.resize(algorithms.size());
        } // end of multiple runs with the same algorithm
        
        for (size_t i = 0; i < matchings.size(); ++i) {
            std::cout << "cross run similarity for " << ALGORITHM_NAMES.at(algorithms.at(i))
                      << (eps.at(i)? (" " + std::to_string(eps.at(i))) : "") << ": "
                      << cross_run_similarities.at(i) / cross_run_sim_counters.at(i)
                      << std::endl;
        }
        
        std::cout << "cross algorithm similarity: " << similarity / sim_counter << std::endl;
        
        ASSERT_TRUE(combined_data.size() == combined_runtime.size());
        
        for (size_t i = 0; i < result_size; ++i) { // iterate through sequence steps
            for (size_t j = 0; j < algorithms.size(); ++j) { // iterate through different runs with different algorithms
                output_file << combined_data.at(j).at(i).at(0) << " " 
                            << combined_data.at(j).at(i).at(1) << " " 
                            << combined_data.at(j).at(i).at(2) << " " 
                            << combined_data.at(j).at(i).at(3) << " " 
                            << combined_runtime.at(j).at(i) << " ";
                            
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


/* auxiliary functions */

dyn_matching * init_algorithm (ALGORITHM algorithm, dyn_graph_access * G, double eps) {
    if (algorithm == ALGORITHM::bgs) {
        return new baswanaguptasen_dyn_matching(G);
    } else if (algorithm == ALGORITHM::naive) {
        return new simple_dyn_matching(G, eps);
    } else if (algorithm == ALGORITHM::ns) {
        return new neimansolomon_dyn_matching(G);
    } else {
        return nullptr;
    }
}

