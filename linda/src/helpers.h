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

#define MIN_CALL popen("cat tmp.obj tmp.stub tmp.events tmp.known tmp.decl | lp_solve -S1 ","r");
#define MAX_CALL popen("cat tmp.obj tmp.stub tmp.events tmp.known tmp.decl | lp_solve -S1 -max","r");

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
	BinaryTreeNode (KeyType k, ValueType v) {
		key = k;
		value = v;
		left = 0;
		right = 0;
	}
};

template <class KeyType, class ValueType>
class BinaryTree {
public:
	unsigned int size;
	BinaryTreeNode<KeyType,ValueType>* root;
	BinaryTree () {
		root = 0;
		size = 0;
	}

	bool insert(KeyType k, ValueType v,bool update) {
		bool result = insertR(root,k,v,update);
		if (result) {
			++size;
		}
		return result;
	}

	BinaryTreeNode<KeyType,ValueType>* find(KeyType k) {
		return retrieveR(root,k);
	}

	BinaryTreeIterator<KeyType,ValueType>* begin() {
		BinaryTreeNode<KeyType,ValueType>** result = new BinaryTreeNode<KeyType,ValueType>*[size];
		createNodeListR(result, root, 0);
		return new BinaryTreeIterator<KeyType,ValueType>(result,size);
	}

private:
	unsigned int createNodeListR(BinaryTreeNode<KeyType,ValueType>** result, BinaryTreeNode<KeyType,ValueType>* node, unsigned int counter) {
		if (node != 0) {
			int i = createNodeListR(result,node->left,counter);
			result[i] = node;
			++i;
			return createNodeListR(result,node->right,i);
		} else {
			return counter;
		}
	}

	bool insertR(BinaryTreeNode<KeyType,ValueType>*& node, KeyType k, ValueType v,bool update) {
		if (node == 0) {
			node = new BinaryTreeNode<KeyType,ValueType>(k,v);
			return true;
		} else if (k < node->key) {
			return insertR(node->left,k,v,update);
		} else if (k > node->key) {
			return insertR(node->right,k,v,update);
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
	int index;
	int last;
	BinaryTreeNode<KeyType,ValueType>** nodeList;
public:
	BinaryTreeIterator(BinaryTreeNode<KeyType,ValueType>** nodes, unsigned int size) {
		nodeList = nodes;
		index = 0;
		last = size-1;
	}
	bool valid() {
		return index <= last;
	}

	void next() {
		++index;
	}

	KeyType getKey() {
		return nodeList[index]->key;
	}

	ValueType getValue() {
		return nodeList[index]->value;
	}

};


#endif /* HELPERS_H */
