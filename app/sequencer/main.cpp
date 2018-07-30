#include "sequence.h"

/* 
 * passable options:
 *  -n: uint
 *  -k: uint
 *  -m[ode]: a[ddition-only], r[andom], s[liding-window], m[eyerhenke]
 *  -w[indow]: uint ... used on sliding-window and meyerhenke. determines the amount of additions before doing alternating additions/deletion
 *  -s[eed]: long
 *  -f[ile]: string ... input file
 */

struct seq_config {
    size_t n;
    size_t k;
    MODE mode;
    size_t window;
    size_t poolsize;
    long seed;
    std::string ifile;
    
    seq_config () {
        n = 0;
        k = 0;
        mode = MODE::only_addition;
        window = 0;
        poolsize = 0;
        seed = -1;
        ifile = "";
    }
};

void check_option_arg (std::string option, int i, int argc, char** argv) {
    if (i+1 >= argc || argv[i+1][0] == '-') {
        throw std::string("ERROR: argument for option " + option + " missing.");
    }
}

void get_arguments(int argc, char ** argv, seq_config& conf) {
    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] != '-') {
            throw std::string("ERROR: " + std::string(argv[i]) + " not an option.");
        }
        
        std::string option = argv[i];
        
        if (option == "-n") {
            check_option_arg(option, i, argc, argv);
            conf.n = atoi(argv[i+1]);
            i++;
        } else if (option == "-k") {
            check_option_arg(option, i, argc, argv);
            conf.k = atoi(argv[i+1]);
            i++;
        } else if (option == "-m") {
            check_option_arg(option, i, argc, argv);
            std::string mode_str = argv[i+1];
            
            std::cout << "read mode: " << mode_str << std::endl;
            
            if (mode_str == MODE_NAMES[0]) {
                conf.mode = MODE::only_addition;
            } else if (mode_str == MODE_NAMES[1]) {
                conf.mode = MODE::random_step;
            } else if (mode_str == MODE_NAMES[2]) {
                conf.mode = MODE::sliding_window;
            } else if (mode_str == MODE_NAMES[3]) {
                conf.mode = MODE::meyerhenke;
            } else if (mode_str == MODE_NAMES[4]) {
                conf.mode = MODE::pooled_meyerhenke;
            } else if (mode_str == MODE_NAMES[5]) {
                conf.mode = MODE::native;
            }
            
            i++;
        } else if (option == "-w") {
            check_option_arg(option, i, argc, argv);
            conf.window = atoi(argv[i+1]);
            i++;
        } else if (option == "-p") {
            check_option_arg(option, i, argc, argv);
            conf.poolsize = atoi(argv[i+1]);
            i++;
        } else if (option == "-s") {
            check_option_arg(option, i, argc, argv);
            conf.seed = atol(argv[i+1]);
            i++;
        } else if (option == "-f") {
            check_option_arg(option, i, argc, argv);
            conf.ifile = argv[i+1];
            i++;
        } else {
            
        }
    }
}

int main (int argc, char ** argv) {
    try {
        seq_config conf;
        get_arguments(argc, argv, conf);
        
        sequence seq (conf.n, conf.k, conf.mode, conf.window, conf.poolsize, conf.seed, conf.ifile);
        
        seq.create();
        seq.finish();
    } catch (std::string e) {
        std::cout << e << std::endl;
    }
    return 0;
}
