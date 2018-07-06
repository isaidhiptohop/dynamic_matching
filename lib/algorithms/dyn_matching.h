/******************************************************************************
 * dyn_matching.h 
 *
 *
 ******************************************************************************
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#ifndef DYN_MATCHING_H
#define DYN_MATCHING_H

#include <bitset>
#include <cassert>
#include <iostream>
#include <vector>
#include <unordered_map>

#include "dyn_graph_access.h"

class dyn_matching {
public:
    dyn_matching (dyn_graph_access* G);
    
    ~dyn_matching ();
    
    virtual EdgeID new_edge(NodeID source, NodeID target) = 0;
    
    virtual void remove_edge(NodeID source, NodeID target) = 0;
    
    dyn_graph_access getG ();
    
    virtual std::vector<std::pair<NodeID, NodeID> > getM () = 0;
    
protected:
    dyn_graph_access* G;
};

#endif // DYN_MATCHING_H
