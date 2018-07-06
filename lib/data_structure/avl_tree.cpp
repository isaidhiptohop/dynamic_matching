/******************************************************************************
 * avl_tree.cpp 
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

#include "avl_tree.h"

template <class T>
avl_tree<T>::avl_tree () {
    root = nullptr;
    count = 0;
}

template <class T>
bool avl_tree<T>::insert (T id) {
    size_t tmp = count;
    root = insert(id, root);
    return (tmp+1 == count);
}

template <class T>
bool avl_tree<T>::remove (T id) {
    size_t tmp = count;
    root = remove(id, root);
    return (tmp-1 == count);
}

template <class T>
T * avl_tree<T>::get (T id) {
    return &(get(id, root)->id);
}    

template <class T>
T * avl_tree<T>::getMin () {
    return &(getMin (root)->id);
}

template <class T>
T * avl_tree<T>::getMax () {
    return &(getMax (root)->id);
}

template <class T>
bool avl_tree<T>::isIn (T id) {
    T * result = get(id);
    return result;
}

template <class T>
size_t avl_tree<T>::size() {
    return count;
}

template <class T>
void avl_tree<T>::print () { // left=-1, both=0, right=1
    print (root);
}
    
template <class T>
std::vector<T> avl_tree<T>::allElements () {
    std::vector<T> content;
    content = fill(root, content);
    return content;
}

template <class T>
avl_tree<T>::Node::Node (T id) {
    this->id = id;
    left = nullptr;
    right = nullptr;
    height = 1;
}

template <class T>
size_t avl_tree<T>::height (avl_tree<T>::Node * node) {
    if (!node) {
        return 0;
    }
    
    return node->height;
}

template <class T>
size_t avl_tree<T>::max (size_t a, size_t b) {
    return (a > b) ? a : b;
}

template <class T>
typename avl_tree<T>::Node * avl_tree<T>::leftRotate (Node * x) {
    Node * y = x->right;
    Node * a = y->left;
    
    x->right = a;
    y->left = x;
    
    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;
    
    return y;
}

template <class T>
typename avl_tree<T>::Node * avl_tree<T>::rightRotate (Node * x) {
    Node * y = x->left;
    Node * a = y->right;
    
    x->left = a;
    y->right = x;
    
    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;
    
    return y;
}

template <class T>
typename avl_tree<T>::Node * avl_tree<T>::insert (T id, Node * node) {
    if (!node) {
        Node * new_node = new Node (id);
        count++;
        return new_node;
    }
    
    if (id < node->id) { // left case
        node->left = insert(id, node->left);
    } else if (id > node->id) { // right case
        node->right = insert(id, node->right);
    } else { // same id, not allowed
        return node;
    }
    
    node->height = max(height(node->left), height(node->right)) + 1;
    
    int balance = height(node->left) - height(node->right);

/*
    std::cout << "on key " << id << ": "
              << height(node->left) << "-" << height(node->right) 
              << "=" << balance << std::endl; 
*/
    
    if (balance > 1 && id < node->left->id) { // Left Left
        return rightRotate(node);
    }
    
    if (balance > 1 && id > node->left->id) { // Left Right
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }
    
    if (balance < -1 && id < node->right->id) { // Right Left
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    if (balance < -1 && id > node->right->id) { // Right Right
        return leftRotate(node);
    }
    
    return node;
}

template <class T>
typename avl_tree<T>::Node * avl_tree<T>::remove (T id, Node * node) {
    if (!node) {
        return node;
    }
    
    if (id < node->id) { // left case
        node->left = remove(id, node->left);
    } else if (id > node->id) { // right case
        node->right = remove(id, node->right);
    } else { // same id means this is the node, we want to delete
        if (!node->left && !node->right) { // no children, simply remove the node
            delete node;
            count--;
            return nullptr;
        } else if (!node->left || !node->right) { // one child: make the child a child of its
                                                  // grandparent, remove the parent (=node)
            Node * tmp = node->left ? node->left : node->right;
            
            delete node;
            count--;
            return tmp;
        } else {
            Node * tmp = getMax(node->left);
            
            ASSERT_TRUE(!tmp->right);
            
            node->id = tmp->id; // overwrite the id of node with the id of tmp
            remove (tmp->id, node->left);
        }
    }
    
    node->height = max(height(node->left), height(node->right)) + 1;
    
    int balance = height(node->left) - height(node->right);

/*
    std::cout << "on key " << id << ": "
              << height(node->left) << "-" << height(node->right) 
              << "=" << balance << std::endl; 
*/
    
    if (balance > 1 && id < node->left->id) { // Left Left
        return rightRotate(node);
    }
    
    if (balance > 1 && id > node->left->id) { // Left Right
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }
    
    if (balance < -1 && id < node->right->id) { // Right Left
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    if (balance < -1 && id > node->right->id) { // Right Right
        return leftRotate(node);
    }
    
    return node;
}

template <class T>
typename avl_tree<T>::Node * avl_tree<T>::get(T id, Node * node) {
    if (!node) {
        return nullptr;
    }
    
    if (id < node->id) {
        get(id, node->left);
    } else if (id > node->id) {
        get(id, node->right);
    } else  { // id == node->id
        return node;
    }
    
}

template <class T>
typename avl_tree<T>::Node * avl_tree<T>::getMin (Node * node) {
    if (!node->left) {
        return node;
    } else {
        return getMin (node->left);
    }
}

template <class T>
typename avl_tree<T>::Node * avl_tree<T>::getMax (Node * node) {
    if (!node->right) {
        return node;
    } else {
        return getMax (node->right);
    }
}

template <class T>
std::vector<T>& avl_tree<T>::fill (Node * node, std::vector<T>& content) {
    if (!node) {
        return content;
    }
    
    content = fill(node->left, content);
    
    content.push_back(node->id);
    
    content = fill(node->right, content);
    
    return content;
}


template <class T>
void avl_tree<T>::print (Node * node) {
    if (!node) return;
    
    for (size_t i = node->height; i < root->height; ++i) {
        std::cout << "|  ";
    }
    std::cout << node->id << std::endl;
    
    print(node->right);
    print(node->left);
}

