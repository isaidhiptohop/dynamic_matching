#include <sys/stat.h>

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
        
        
        /* initialize output directory and files */
        std::string output_filename = get_output_filename(conf);
        
        mkdir(std::string(output_filename).c_str(), 0775);

        std::ofstream output_file;
        output_file.open(output_filename + "/data");
        
        output_file << "# ";
        
        for (size_t i = 0; i < algorithms.size(); ++i) {
            output_file << ALGORITHM_NAMES.at(algorithms.at(i)) << (eps.at(i)? (" " + std::to_string(eps.at(i))) : "") << ", ";
        }
        
        output_file << std::endl;
        output_file << "# " << algorithms.size() << "x "
                    << "insertion deletions G M time cr-similarity intersect-ca";
        
        output_file << std::endl;
        
        std::ofstream matching_file;
        matching_file.open(output_filename + "/matchings");
        
        std::ofstream counters_file;
        counters_file.open(output_filename + "/counters");
        
        mkdir(std::string(output_filename + "/snapshots").c_str(), 0775);
        
        
        /* calculate size (lines) of the resulting data to reserve space. */
        size_t result_size;
        if (edge_sequence.size() % conf.at_once == 0) {
            result_size = edge_sequence.size()/conf.at_once;
        } else {
            result_size = edge_sequence.size()/conf.at_once + 1;
        }
        
        std::cout << "result size: " << result_size << std::endl;
        
        /* intialize result vectors */
        
        /*  combined_data is 3D: algorithms X evaluated sequence steps X measurements
         *  algorithms is determined by algorithms.size(),
         *  the evaluated sequence steps corresponds to result_size,
         *  the measurements are 4: insertions, deletions, cardinality of G, cardinality of M
         *  
         *  combined_runtime is 2D: algorithms X evaluated sequence steps
         *  the dimensions are the same as for combined_data, only there is only one measurement, the elapsed time
         */
        std::vector<std::vector<std::vector<int> > > combined_data(algorithms.size(), std::vector<std::vector<int> >(result_size, std::vector<int>(4, 0)));
        std::vector<std::vector<double> > combined_runtime(algorithms.size(), std::vector<double>(result_size, 0));
        
        /*  matchings is 3D vector, which contains at the position (i, j, k) the kth edge of the jth sequence step of the ith algorithm:
         * 
         *  matchings = [
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
         *  
         *  matchings is therefore of dimension algorithms X evaluated sequence steps X matching size (X 2, since the elements then again are pairs)
         * 
         *  previous matchings holds the matchings of the previous run, in order to compare results of different runs
         *  for deterministic algorithms (neiman-solomon) there should be no changes in the results (similarity = 1), 
         *  for randomized algorithms it is highly unlikely that the results do not differ (similarity < 1).
         */
        std::vector<std::vector<std::vector<std::pair<NodeID, NodeID> > > > matchings(algorithms.size(), std::vector<std::vector<std::pair<NodeID, NodeID> > >(result_size));
        std::vector<std::vector<std::vector<std::pair<NodeID, NodeID> > > > previous_matchings;
        
        std::vector<std::vector<int> > cross_algorithm_matching_intersect(algorithms.size(), std::vector<int>(result_size, 0));
        
        std::vector<std::vector<double> > cross_run_similarities(algorithms.size(), std::vector<double>(result_size, 0));
        std::vector<std::vector<int> > cross_run_sim_counters(algorithms.size(), std::vector<int>(result_size, 0));
        
        double similarity = 0;
        int sim_counter = 0;
        
        
        /* start calculations */
        for (int k = 0; k < conf.multi_run; ++k) { // run the whole thing several times
//            std::cout << "run " << k+1 << "/" << conf.multi_run << std::endl;
            // hold all matchings of one run in vector.
            /*
            for (auto c : counters::get_all() ) {
                c.second.restart();
            }
            */
            for (size_t l = 0; l < algorithms.size(); ++l) { // run different algorithms
//                std::cout << "running algorithm " << ALGORITHM_NAMES.at(algorithms.at(l)) << std::endl;
                dyn_graph_access * G = create_graph(n);
                dyn_matching * algorithm = init_algorithm(algorithms.at(l), G, eps.at(l));
                
                // initalize timer and counters
                double time_elapsed = 0;
                int insertions = 0;
                int deletions = 0;
                int j = 0; // counter for entries in data vectors. from range 0 to result_size
                
                for (size_t i = 0; i < edge_sequence.size(); ++i) { // iterate through sequence
                    timer t;
                    std::pair<NodeID, NodeID> edge = edge_sequence.at(i).second;
                    
                    /* determine whether to do an insertion or a deletion */
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
                    
                    
                    /*  every at_once steps we acquire data */
                    if ((i+1) % conf.at_once == 0 || i == edge_sequence.size() - 1) {
//                        std::cout << "processed " << i+1 << " of " << edge_sequence.size() << " sequence steps" << std::endl;
                        std::vector<std::pair<NodeID, NodeID> > matching = algorithm->getM();
                        
                        algorithm->counters_next();
                        
                        // validate the matching to exclude errors in the algorithm
                        quality_metrics::matching_validation(matching);
//                        print_matching(std::cout, matching);
                        
                        
                        // do snapshots
                        graph_access G_static;
                        G->convert_to_graph_access(G_static);
                        graph_io::writeGraph(G_static, std::string(output_filename + "/snapshots/snapshot_" + std::to_string(i+1) + ".graph"));                        
                        
                        // save data in arrays, l = algorithm, j = entry
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
        /*
        counters::divide_by(conf.multi_run);
        counters::print(counters_file);
        */
        // = = = = // COLLECT INFORMATION // = = = = //
        
            /* 
             * matchings is 3D vector, which contains at the position (i, j, k) the kth edge of the jth sequence step of the ith algorithm.
             * see above for details.
             * 
             * therefore the size of every subvector in matchings should have the same size, namely the size of the sequence.
             * however the size of the calculated matchings inside the subvector can differ.
             * 
             * in the following if-block we iterate through the sequence steps and compare the matching resutls of different algorithms
             * for every time-step.
             */
            
        for (size_t m = 0; m < algorithms.size(); ++m) {
            // TODO: what does upper_limit mean? why do i need it? i suspect it should be the other way around
            
            if (m+1 < algorithms.size()) {
                size_t upper_limit = matchings.at(m).size() > matchings.at(m+1).size() ? matchings.at(m).size() : matchings.at(m+1).size();
                for (size_t i = 0; i < upper_limit; ++i) {
                    int union_size = 1;
                    int intersect_size = quality_metrics::edgeset_intersect(matchings.at(m).at(i), matchings.at(m+1).at(i), union_size).size();
                    
                    // save intersect size, divide it by two since every edge is hold twice in it
                    cross_algorithm_matching_intersect.at(m).at(i) = intersect_size/2;
                    /*
                    std::cout << "comparing " << ALGORITHM_NAMES.at(algorithms.at(m)) 
                              << " with "     << ALGORITHM_NAMES.at(algorithms.at(m+1)) 
                              << ": " << intersect_size/2 << std::endl;
                    */
                    similarity = similarity + (union_size ? ((intersect_size * 1.0) / union_size) : 0); // if union is zero, similarity is zero
                    sim_counter++;
                }
            } else { // last algorithm gets compared with first one.
                size_t upper_limit = matchings.at(m).size() > matchings.at(0).size() ? matchings.at(m).size() : matchings.at(0).size();
                for (size_t i = 0; i < upper_limit; ++i) {
                    int union_size = 1;
                    int intersect_size = quality_metrics::edgeset_intersect(matchings.at(m).at(i), matchings.at(0).at(i), union_size).size();
                    
                    // save intersect size, divide it by two since every edge is hold twice in it
                    cross_algorithm_matching_intersect.at(m).at(i) = intersect_size/2;
                    /*
                    std::cout << "comparing " << ALGORITHM_NAMES.at(algorithms.at(m)) 
                              << " with "     << ALGORITHM_NAMES.at(algorithms.at(0)) 
                              << ": " << intersect_size/2 << std::endl;
                    */
                    similarity = similarity + (union_size ? ((intersect_size * 1.0) / union_size) : 0);
                    sim_counter++;
                }
            }
        }
        
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
        
        ASSERT_TRUE(combined_data.size() == combined_runtime.size());
        
        for (size_t i = 0; i < result_size; ++i) { // iterate through sequence steps
            for (size_t j = 0; j < algorithms.size(); ++j) { // iterate through different runs with different algorithms
                output_file << combined_data.at(j).at(i).at(0) << " " 
                            << combined_data.at(j).at(i).at(1) << " " 
                            << combined_data.at(j).at(i).at(2) << " " 
                            << combined_data.at(j).at(i).at(3) << " " 
                            << combined_runtime.at(j).at(i) << " "
                            
                            // check that we don't divdide by zero
                            << (cross_run_sim_counters.at(j).at(i) ? cross_run_similarities.at(j).at(i) / cross_run_sim_counters.at(j).at(i) : 0) << " "
                            << cross_algorithm_matching_intersect.at(j).at(i) << " ";
                            
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

