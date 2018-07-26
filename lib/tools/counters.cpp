#include "counters.h"

std::vector<std::pair<std::string, counters::counter_set> > counters::counter_sets;

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

std::vector<std::pair<std::string, counters::counter> > counters::counter_set::all () {
    return registered_counters;
}

counters::counter& counters::counter_set::create (const std::string& name) {
    if (exists(name)) {
        get(name).restart();
        return get(name);
    }
    
    registered_counters.push_back({name, counter()});
    return registered_counters.back().second;
}

bool counters::counter_set::exists (const std::string& name) {
    for (int i = 0; i < registered_counters.size(); ++i) {
        if (registered_counters.at(i).first == name) {
            return true;
        }
    }
    
    return false;
}

counters::counter& counters::counter_set::get (const std::string& name) {
    for (int i = 0; i < registered_counters.size(); ++i) {
        if (registered_counters.at(i).first == name) {
            return registered_counters.at(i).second;
        }
    }
    
    throw std::string("counter \"" + name + "\" not found.");
}

void counters::counter_set::divide_by (size_t divisor) {
    for (size_t i = 0; i < registered_counters.size(); ++i) {
        for (size_t j = 0; j < registered_counters.at(i).second.records.size(); ++j) {
            registered_counters.at(i).second.records.at(j) = (1.0 * registered_counters.at(i).second.records.at(j)) / divisor;
        }
    }
}

void counters::counter_set::print (std::ostream& o) {
    if (all().size() > 0) {
        size_t max_size = all().at(0).second.all().size();
        size_t min_size = all().at(0).second.all().size();
        
        for (auto c : all() ) {
            o << "# ";
            o << c.first << " ";
            o << std::endl;
            
            if (c.second.all().size() > max_size) max_size = c.second.all().size();
            if (c.second.all().size() < min_size) min_size = c.second.all().size();
        }
        
        if (max_size != min_size) std::cout << "there might be something wrong with the counters: " << min_size << " vs " << max_size << std::endl;
            
        for (size_t i = 0; i < max_size; ++i) {
            for (auto c : all() ) {
                o << c.second.all().at(i) << " ";
            }
            
            o << std::endl;
        }
    }
}


std::vector<std::pair<std::string, counters::counter_set> > counters::all () {
    return counter_sets;
}

counters::counter_set& counters::create (const std::string& name) {
    if (exists(name)) {
//            get(name).restart();
        return get(name);
    }
    
    counter_sets.push_back({name, counter_set()});
    return counter_sets.back().second;
}

bool counters::exists (const std::string& name) {
    for (int i = 0; i < counter_sets.size(); ++i) {
        if (counter_sets.at(i).first == name) {
            return true;
        }
    }
    
    return false;
}

counters::counter_set& counters::get (const std::string& name) {
    for (int i = 0; i < counter_sets.size(); ++i) {
        if (counter_sets.at(i).first == name) {
            return counter_sets.at(i).second;
        }
    }
    
    throw std::string("counter set \"" + name + "\" not found.");
}
