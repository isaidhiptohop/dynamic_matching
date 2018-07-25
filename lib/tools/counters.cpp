#include "counters.h"

std::vector<std::pair<std::string, counters::counter> > counters::registered_counters;

counters::counter::counter () { 
    records = std::vector<size_t>();
    iterator = 0;
    restarted = false;
    next();
    ASSERT_TRUE(records.size() != 0); 
}

void counters::counter::inc () { 
    if (restarted) {
        records.at(iterator) = records.at(iterator) + 1;
    } else {
        records.back() = records.back() + 1; 
    }
}

void counters::counter::dec () { 
    if (restarted) {
        records.at(iterator) = records.at(iterator) - 1;
    } else {
        records.back() = records.back() - 1; 
    }
}

void counters::counter::next () { 
    if (restarted) {
        iterator++;
    } else {
        records.push_back(0); 
    }
}

void counters::counter::next (size_t value) { 
    if (restarted) {
        records.at(iterator) = value;
    } else {
        records.push_back(value); 
    }
}

void counters::counter::add (size_t value) { 
    ASSERT_TRUE(records.size() != 0);
    if (restarted) {
        records.at(iterator) = records.at(iterator) + value;
    } else {
        records.back() = records.back() + value;
    }
}

void counters::counter::put (size_t value) { 
    if (restarted) {
        records.at(iterator) = value;
    } else {
        records.back() = value; 
    }
}

size_t counters::counter::get () { 
    if (restarted) {
        return records.at(iterator);
    } else {
        return records.back(); 
    }
}

std::vector<size_t>& counters::counter::all () { 
    return records; 
}

void counters::counter::reset () { 
    records.resize(0); 
    next();
}

void counters::counter::restart () { 
    restarted = true;
    iterator = 0;
}

std::vector<std::pair<std::string, counters::counter> > counters::get_all () {
    return registered_counters;
}

counters::counter& counters::new_counter (const std::string& name) {
    if (exists(name)) {
        get(name).restart();
        return get(name);
    }
    
    registered_counters.push_back({name, counter()});
    return registered_counters.back().second;
}

bool counters::exists (const std::string& name) {
    for (int i = 0; i < registered_counters.size(); ++i) {
        if (registered_counters.at(i).first == name) {
            return true;
        }
    }
    
    return false;
}

counters::counter& counters::get (const std::string& name) {
    for (int i = 0; i < registered_counters.size(); ++i) {
        if (registered_counters.at(i).first == name) {
            return registered_counters.at(i).second;
        }
    }
    
    ASSERT_TRUE(false);
}

void counters::divide_by (size_t divisor) {
    for (size_t i = 0; i < registered_counters.size(); ++i) {
        for (size_t j = 0; j < registered_counters.at(i).second.records.size(); ++j) {
            registered_counters.at(i).second.records.at(j) = (1.0 * registered_counters.at(i).second.records.at(j)) / divisor;
        }
    }
}

void counters::print (std::ostream& o) {
    size_t max_size = get_all().at(0).second.all().size();
    size_t min_size = get_all().at(0).second.all().size();
    
    for (auto c : get_all() ) {
        o << "# ";
        o << c.first << " ";
        o << std::endl;
        
        if (c.second.all().size() > max_size) max_size = c.second.all().size();
        if (c.second.all().size() < min_size) min_size = c.second.all().size();
    }
    
    if (max_size != min_size) std::cout << "there might be something wrong with the counters: " << min_size << " vs " << max_size << std::endl;
        
    for (size_t i = 0; i < max_size; ++i) {
        for (auto c : get_all() ) {
            o << c.second.all().at(i) << " ";
        }
        
        o << std::endl;
    }
}
