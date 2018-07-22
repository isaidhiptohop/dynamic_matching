#include <vector>

#include "definitions.h"

class counters {
    struct counter {
        
    };
    
    struct simple_counter : public counter {
        size_t value = 0;
        
        size_t inc ();
        size_t dec ();
        size_t get ();
        
        size_t reset ();
    };
    
    struct value_list : public counter {
        std::vector<size_t> values; // default constructor of vector creates empty list
        
        std::vector<size_t> put (size_t value);
        std::vector<size_t> add (size_t value);
        std::vector<size_t> next ();
        std::vector<size_t> inc ();
        std::vector<size_t> dec ();
        std::vector<size_t> get ();
        
        std::vector<size_t> reset ();
    };
    /*
    struct dvalue_list : public counter {
        std::vector<double> values;
        
        std::vector<double> put (double value);
        std::vector<double> dec ();
        std::vector<double> get ();
        
        std::vector<double> reset ();
    };
    
    struct value_matrix : public counter {
        std::vector<std::vector<size_t> > values; // default constructor of vector creates empty list
        
        std::vector<std::vector<size_t> > put (std::vector<size_t> values);
        std::vector<std::vector<size_t> > put (size_t value);
        std::vector<std::vector<size_t> > next ();
        std::vector<std::vector<size_t> > dec ();
        std::vector<std::vector<size_t> > get ();
        
        std::vector<std::vector<size_t> > reset ();
    };
    
    struct dvalue_matrix : public counter {
        std::vector<std::vector<double> > values;
        
        std::vector<std::vector<double> > put (std::vector<double> values);
        std::vector<std::vector<double> > dec ();
        std::vector<std::vector<double> > get ();
        
        std::vector<std::vector<double> > reset ();
    };
    */
    
    static std::vector<std::pair<std::string, counter> > registered_counters;
    
    static counter& new_simple_counter (const std::string& name);
    static counter& new_value_list (const std::string& name);
    /*
    static counter& new_dvalue_list (const std::string& name);
    static counter& new_value_matrix (const std::string& name);
    static counter& new_dvalue_matrix (const std::string& name);
    */
    static counter& get(const std::string& name);
};
