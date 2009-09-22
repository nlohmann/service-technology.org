#ifndef LINDA_HELPERS_H
#define LINDA_HELPERS_H

#include <sstream>
#include <iostream>
#include <fstream>
#include <set>
#include <vector>

#include "config.h"
#include <pnapi/pnapi.h>

#include "lp_lib.h"


using std::set;
using std::vector;


const std::string intToStr(const int);

extern int NR_OF_EVENTS;
extern std::string* EVENT_STRINGS;
extern pnapi::Place** EVENT_PLACES;

extern int GET_EVENT_ID(std::string* s);

template <class KeyType, class ValueType>
class BinaryTreeIterator;

template <class KeyType, class ValueType>
class BinaryTreeNode {
public:
	KeyType key;
	ValueType value;
	BinaryTreeNode<KeyType,ValueType>* left;
	BinaryTreeNode<KeyType,ValueType>* right;
	BinaryTreeNode<KeyType,ValueType>* next;
	BinaryTreeNode<KeyType,ValueType>* prev;
	BinaryTreeNode (KeyType k, ValueType v) {
		key = k;
		value = v;
		left = 0;
		right = 0;
		next = 0;
		prev = 0;
	}
};

template <class KeyType, class ValueType>
class BinaryTree {
public:
	unsigned int size;
	BinaryTreeNode<KeyType,ValueType>* root;
	BinaryTreeNode<KeyType,ValueType>* rootIterator;
	BinaryTree () {
		root = 0;
		rootIterator = 0;
		size = 0;
	}

	bool insert(KeyType k, ValueType v,bool update) {
		bool result = insertR(0,false,root,k,v,update);
		if (result) {
			++size;
		}
		return result;
	}

	BinaryTreeNode<KeyType,ValueType>* find(KeyType k) {
		return retrieveR(root,k);
	}

	BinaryTreeIterator<KeyType,ValueType>* begin() {
		return new BinaryTreeIterator<KeyType,ValueType>(rootIterator);
	}

	~BinaryTree() {
		deleteNodesR(rootIterator);
	}

private:

	void deleteNodesR(BinaryTreeNode<KeyType,ValueType>* node) {
		if (node->next != 0) deleteNodesR(node->next);

		delete node;
	}

	bool insertR(BinaryTreeNode<KeyType,ValueType>* referrer, bool left, BinaryTreeNode<KeyType,ValueType>*& node, KeyType k, ValueType v,bool update) {
		if (node == 0) {
			node = new BinaryTreeNode<KeyType,ValueType>(k,v);
			// Change the next/prev pointers accordingly.
			if (referrer != 0) {
				if (left) {
					node->next = referrer;
					if (referrer->prev != 0) {
						node->prev = referrer->prev;
						referrer->prev->next = node;
					} else {
						rootIterator = node;
					}
					referrer->prev = node;
				} else {
					node->prev = referrer;
					if (referrer->next != 0) {
						node->next = referrer->next;
						referrer->next->prev = node;
					}
						referrer->next = node;
				}
			} else {
				rootIterator = node;
			}
			return true;
		} else if (k < node->key) {
			return insertR(node,true,node->left,k,v,update);
		} else if (k > node->key) {
			return insertR(node,false,node->right,k,v,update);
		} else if (update = true && node->key == k) {
			node->value = v;
			return true;
		}
		return false;
	}

	BinaryTreeNode<KeyType,ValueType>* retrieveR(BinaryTreeNode<KeyType,ValueType>* node, KeyType k) {
		if (node == 0) {
			return 0;
		} else if (node->key == k) {
			return node;
		} else if (k < node->key) {
			return retrieveR(node->left,k);
		} else if (k > node->key) {
			return retrieveR(node->right,k);
		}
		return 0;
	}

};

template <class KeyType,class ValueType>
class BinaryTreeIterator {
	BinaryTreeNode<KeyType,ValueType>* current;
public:
	BinaryTreeIterator(BinaryTreeNode<KeyType,ValueType>* rootIterator) {
		current = rootIterator;
	}
	bool valid() {
		return current != 0;
	}

	void next() {
		current = current->next;
	}

	KeyType getKey() {
		return current->key;
	}

	ValueType getValue() {
		return current->value;
	}

};


#endif /* HELPERS_H */
