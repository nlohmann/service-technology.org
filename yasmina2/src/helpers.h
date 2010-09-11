#ifndef YASMINA_HELPERS_H
#define YASMINA_HELPERS_H

#include <sstream>
#include <iostream>
#include <fstream>
#include <set>
#include <vector>
#include <typeinfo>

#include "config.h"
#include "verbose.h"
#include <pnapi/pnapi.h>

#include <lp_solve/lp_lib.h>

/*
Classes that are commonly used throughout Yasmina.
 */

unsigned int getBit(unsigned int field, unsigned int position);
std::string intToStr(int i);


class Universe {
  static std::vector<std::string> identifiers;
public:
  static int addIdentifier (std::string* identifier) {
    for (int i = 1; i < identifiers.size(); ++i) {
      if (identifiers[i].compare(*identifier) == 0) {
        return i;
      } 
    }
    identifiers.push_back(*identifier);
    return identifiers.size()-1;
  }

  static std::string getIdentifier (int numID) {
    return identifiers[numID];
  }
  
  static int getNumID(std::string* identifier) {
    for (int i = 1; i < identifiers.size(); ++i) {
        if (identifiers[i].compare(*identifier) == 0) {
          return i;
        } 
    }
    return 0;
  }
  
  static int getUniverseSize() {
    return identifiers.size();
  }
  
  
};

class Constraint {
protected:
  int* domain;
  REAL* values;
  int domainsize;
public:
  virtual int addToLP(lprec* lp) = 0;
};

class CustomConstraint : public Constraint {
  REAL rhs;
  int op;
public:
  int addToLP(lprec* lp);
  CustomConstraint(std::vector<int>* aDomain, std::vector<int>* someValues, int op, int rhs);
};


class BoundedConstraint : public Constraint {
  REAL lBound, rBound;
public:
  int addToLP(lprec* lp);
  BoundedConstraint(std::vector<int>* identifiers, std::vector<int>* values, int aLeftBound, int aRightBound);
};


class LeftBoundedConstraint : public Constraint {
  REAL bound;
public:
  int addToLP(lprec* lp);
  LeftBoundedConstraint(std::vector<int>* identifiers, std::vector<int>* values, int aBound);
};

class RightBoundedConstraint : public Constraint {
  REAL bound;
public:
  int addToLP(lprec* lp);
  RightBoundedConstraint(std::vector<int>* identifiers, std::vector<int>* values, int aBound);
};

class UnboundedConstraint : public Constraint {
public:
  int addToLP(lprec* lp) {;}
  UnboundedConstraint(std::vector<int>* aDomain, std::vector<int>* someValues);
};


class ServiceModel {
public:
  std::vector<int> inputs;
  std::vector<int> outputs;
  std::string name;
  int alias;
  std::vector<std::vector<Constraint*> > constraints;
};

class Result {
public:
  std::vector<int>* models;
  int result; // 0 = inconclusive, 1 = incompatible, -1 = syntactically incompatible
  Result(std::vector<int>* composite, int res) {
    models = composite; result = res;      
  }
  std::string* toString() {
    std::string* resultstring = new std::string();
    for (int i = 0; i < models->size(); ++i) {
      if (i != 0) {
        (*resultstring) += " (+) ";
      }
      (*resultstring) += "M" + intToStr((*models)[i]+1);
    }
    (*resultstring) += ": ";
    if (result == 0) {
      (*resultstring) += "maybe";
    } else if (result == 1) {
      (*resultstring) += "no";
    } else if (result == -1) {
      (*resultstring) += "undefined";
    }
    return resultstring; 
  }
      
};

class YasminaLog {
public:
  static std::vector<ServiceModel*> models;
  static std::vector<Result> results;
  static std::string* toString(bool reportSyntacticallyIncompatible = false);
};

template <class KeyType, class ValueType>
class BinaryTreeIterator;

/*!
 * \brief A node of a binary tree, using two template parameters: sort-key-type and value-type.
 * It knows its left and right child as well as its pre- and postdecessor in the order of the binary tree.
 */
template <class KeyType, class ValueType>
class BinaryTreeNode {
public:
	KeyType key; //!< The type of the sort key
	ValueType value; //!< The type of the value
	BinaryTreeNode<KeyType,ValueType>* left; //!< Left child
	BinaryTreeNode<KeyType,ValueType>* right; //!< Right child
	BinaryTreeNode<KeyType,ValueType>* next; //!< Next node in the order of the binary tree.
	BinaryTreeNode<KeyType,ValueType>* prev; //!< Previous node in the order of the binary tree.

	///Basic constructor, assigning the key and the value of the node as well as the null pointer to all connections.
	BinaryTreeNode (KeyType k, ValueType v) {
		key = k;
		value = v;
		left = 0;
		right = 0;
		next = 0;
		prev = 0;
	}
};

/// A binary tree consisting of nodes, using two template parameters: sort-key-type and value-type.
template <class KeyType, class ValueType>
class BinaryTree {
public:

	unsigned int size; //!< Number of nodes in the tree. Is updated when a new node was inserted.
	BinaryTreeNode<KeyType,ValueType>* root; //!< The root node
	BinaryTreeNode<KeyType,ValueType>* rootIterator; //!< The first node in the global order. Is updated when a new node was inserted and is the smallest element.

	///Basic constructor, assigning the null pointer to all node-pointers.
	BinaryTree () : root(0),rootIterator(0),size(0) {}

	///Inserts a new node into the tree if no node with this key exists. If there exists a node and update is true, the value of this node is updated.
	bool insert(KeyType k, ValueType v,bool update) {
		bool result = insertR(0,false,root,k,v,update);
		if (result) {
			++size;
		}
		return result;
	}

	///Return the node with this key, if it exists, else return the null pointer.
	BinaryTreeNode<KeyType,ValueType>* find(KeyType k) {
		return retrieveR(root,k);
	}

	///Returns the iterator.
	BinaryTreeIterator<KeyType,ValueType>* begin() {
		return new BinaryTreeIterator<KeyType,ValueType>(rootIterator);
	}

	///Deletes all nodes in the tree.
	~BinaryTree() {
		deleteNodesR(rootIterator);
	}

private:

	///Recursive helper to delete all nodes.
	void deleteNodesR(BinaryTreeNode<KeyType,ValueType>* node) {
		if (node->next != 0) deleteNodesR(node->next);

		delete node;
	}

	///Recursive helper to insert a node into the tree.
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
		} else if (update == true && node->key == k) {
			node->value = v;
			return true;
		}
		return false;
	}

	///Recursive helper to find a node in the tree.
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

/// Iterator for a binary tree.
template <class KeyType,class ValueType>
class BinaryTreeIterator {
	BinaryTreeNode<KeyType,ValueType>* current; //<! Pointer to the current node.
public:
	///Basic constructor, assigning the current node.
	BinaryTreeIterator(BinaryTreeNode<KeyType,ValueType>* rootIterator) {
		current = rootIterator;
	}

	///Return true iff the current node is not the null pointer.
	bool valid() {
		return current != 0;
	}

	///Iterates.
	void next() {
		current = current->next;
	}

	///Returns the key of the current node.
	KeyType getKey() {
		return current->key;
	}

	///Returns the value of the current node.
	ValueType getValue() {
		return current->value;
	}

};


template <class ElemType>
class ListElement {
public:
	ListElement<ElemType>* prev;
	ListElement<ElemType>* next;
	ElemType element;
	ListElement<ElemType>(ElemType element, ListElement<ElemType>* prev, ListElement<ElemType>* next) {
		this->element = element;
		this->prev = prev;
		this->next = next;
	}
	bool hasPrev() {
		return prev != 0;
	}
	bool hasNext() {
		return next != 0;
	}

};

std::pair<BinaryTree<pnapi::Place*,std::pair<int,int> >**,int> translateFinalCondition(pnapi::PetriNet* net);



#endif /* YASMINA_HELPERS_H */
