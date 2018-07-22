#include "counters.h"

size_t counters::simple_counter::inc () { return ++value; }
size_t counters::simple_counter::dec () { return --value; }
size_t counters::simple_counter::get () { return value; }
size_t counters::simple_counter::reset () { value = 0; return value; }

std::vector<size_t> counters::value_list::put (size_t value) { values.push_back(value); return values; }
std::vector<size_t> counters::value_list::add (size_t value) { values.back() = values.back() + value; return values; }
std::vector<size_t> counters::value_list::next () { values.push_back(0); return values; }
std::vector<size_t> counters::value_list::inc () { values.back() = values.back() + 1; return values; }
std::vector<size_t> counters::value_list::dec () { values.back() = values.back() - 1; return values; }
std::vector<size_t> counters::value_list::get () { return values; }
std::vector<size_t> counters::value_list::reset () { values.resize(0); return values; }
/*
std::vector<double> counters::dvalue_list::put (double value) { values.push_back(value); return values; }
std::vector<double> counters::dvalue_list::dec () { values.pop_back(); return values; }
std::vector<double> counters::dvalue_list::get () { return values; }
std::vector<double> counters::dvalue_list::reset () { values.resize(0); return values; }

std::vector<std::vector<size_t> > counters::value_matrix::put (std::vector<size_t> value_lst) { values.push_back(value_lst); return values; }
std::vector<std::vector<size_t> > counters::value_matrix::put (size_t value) { values.back().push_back(value); return values; }
std::vector<std::vector<size_t> > counters::value_matrix::next () { values.push_back(std::vector<size_t>()); return values; }
std::vector<std::vector<size_t> > counters::value_matrix::dec () { values.pop_back(); return values; }
std::vector<std::vector<size_t> > counters::value_matrix::get () { return values; }
std::vector<std::vector<size_t> > counters::value_matrix::reset () { values.resize(0); return values; }

std::vector<std::vector<double> > counters::dvalue_matrix::put (std::vector<double> value_lst) { values.push_back(value_lst); return values; }
std::vector<std::vector<double> > counters::dvalue_matrix::dec () { values.pop_back(); return values; }
std::vector<std::vector<double> > counters::dvalue_matrix::get () { return values; }
std::vector<std::vector<double> > counters::dvalue_matrix::reset () { values.resize(0); return values; }
*/

counters::counter& counters::new_simple_counter (const std::string& name) {
    registered_counters.push_back({name, simple_counter()});
    return registered_counters.back().second;
}

counters::counter& counters::new_value_list (const std::string& name) {
    registered_counters.push_back({name, value_list()});
    return registered_counters.back().second;
}
/*
counters::counter& counters::new_dvalue_list (const std::string& name) {
    registered_counters.push_back({name, dvalue_list()});
    return registered_counters.back().second;
}

counters::counter& counters::new_value_matrix (const std::string& name) {
    registered_counters.push_back({name, value_matrix()});
    return registered_counters.back().second;
}

counters::counter& counters::new_dvalue_matrix (const std::string& name) {
    registered_counters.push_back({name, dvalue_matrix()});
    return registered_counters.back().second;
}
*/
counters::counter& counters::get(const std::string& name) {
    for (int i = 0; i < registered_counters.size(); ++i) {
        if (registered_counters.at(i).first == name) {
            return registered_counters.at(i).second;
        }
    }
}
