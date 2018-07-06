/******************************************************************************
 * maxNodeHeap.h 
 *
 * Source of KaHIP -- Karlsruhe High Quality Partitioning.
 *
 ******************************************************************************
 * Copyright (C) 2013-2015 Christian Schulz <christian.schulz@kit.edu>
 *
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

#ifndef MAX_NODE_HEAP_39CK1B8I
#define MAX_NODE_HEAP_39CK1B8I

#include <limits>
#include <vector>
#include <unordered_map>
#include <execinfo.h>

#include "priority_queue_interface.h"
             
typedef int Key;

template < typename Data >
class QElement {
        public:
                QElement( Data data, Key key, int index ) : m_data(data), m_key (key), m_index(index) {}; 
                virtual ~QElement() {};

                Data & get_data() { 
                        return m_data; 
                }

                void set_data(Data & data) { 
                        m_data = data;
                }

                Key get_key() {
                        return m_key;
                }

                void set_key(Key key) {
                        m_key = key;
                }

                int get_index() {
                        return m_index;
                }

                void set_index(int index) {
                        m_index = index;
                }

        private:
                Data m_data;
                Key  m_key;
                int  m_index; // the index of the element in the heap

};

class maxNodeHeap : public priority_queue_interface {
        public:
                              
                struct Data {
                        NodeID node;
                        Data( NodeID node ) : node(node) {};
                };

                typedef QElement<Data> PQElement;
                
                maxNodeHeap() {};
                virtual ~maxNodeHeap() {};

                NodeID size();  
                bool empty();

                bool contains(NodeID node);
                void insert(NodeID id, Gain gain); 

                NodeID deleteMax();
                void deleteNode(NodeID node);
                NodeID maxElement();
                Gain maxValue();

                void decreaseKey(NodeID node, Gain gain);
                void increaseKey(NodeID node, Gain gain);
                void changeKey(NodeID node, Gain gain); 
                Gain getKey(NodeID node); 

        private:
                std::vector< PQElement >               m_elements;      // elements that contain the data
                std::unordered_map<NodeID, int>   m_element_index; // stores index of the node in the m_elements array
                std::vector< std::pair<Key, int> >     m_heap;          // key and index in elements (pointer)

                void siftUp( int pos );
                void siftDown( int pos ); 

};

#endif
