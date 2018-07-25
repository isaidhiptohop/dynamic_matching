#ifndef COUNTERS_H
#define COUNTERS_H

#include <vector>

#include "definitions.h"
#include "macros_assertions.h"

class counters {
public:
    struct counter {
        bool restarted;
        
        std::vector<size_t> records;
        size_t iterator;
        
        counter ();
        
        void inc ();
        void dec ();
        void next ();
        void next (size_t value);
        void add (size_t value);
        void put (size_t value);
        size_t get ();
        std::vector<size_t>& all ();
        
        void reset ();
        void restart ();
    };
    
    static std::vector<std::pair<std::string, counter> > registered_counters;
    
    static std::vector<std::pair<std::string, counter> > get_all();
    static counter& new_counter (const std::string& name);
    static bool exists (const std::string& name);
    static counter& get(const std::string& name);
    static void divide_by (size_t divisor);
    static void print (std::ostream& o);
};

#endif // COUNTERS_H
