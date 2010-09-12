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
Classes and functions that are commonly used throughout Yasmina.
 */

// Given an unsigned integer representing a bit field, returns the bit on the given position. 
unsigned int getBit(unsigned int field, unsigned int position);
// Transforms an int to a string
std::string intToStr(int i);

// Provides static members to store and find identifiers.
// An identifier is a transition or channel name.
// Each identifier has a unique numerical id.
// All constraints use the numerical ids to reference on identifiers.
// Numerical ids start at 1 for more convenient lp_solve-integration. 
// Therefore, the highest numerical id coincides with the number of identifiers.
class Universe {
  // stores the identifiers together with their numIDs. Adding and looking up the numIDs is fast. 
  // Finding an identifier for a given numID is slow.
  static std::map<std::string, int> identifiers;
public:
  // Adds an identifier to the universe and returns its numID.
  // If this identifier already exists, it is not added again and keeps its numID.
  static int addIdentifier (std::string* identifier) {
    if (identifiers.find(*identifier) != identifiers.end()) {
      return identifiers[*identifier];
    } else {
      int val = identifiers.size();
      identifiers[*identifier] = val+1;
  }
    return identifiers.size();
  }

  // Looks up if an identifier with this numID can be found and, if yes, returns it. If not, it returns the empty string.
  // Rather slow.
  static std::string getIdentifier (int numID) {
    std::string s = "";
    if (identifiers.size() >= numID) {
      for (std::map<std::string, int>::iterator it = identifiers.begin(); it != identifiers.end(); ++it) {
        if ((*it).second == numID) {
          s = (*it).first;
          return s;
        }
      }
    }
    return s;
  }
  
  // Looks up the num ID for an identifier.
  static int getNumID(std::string* identifier) {
    return identifiers[*identifier];
  }
  
  // Returns the size of the universe.
  static int getUniverseSize() {
    return identifiers.size();
  }
  
  
};

// Abstract constraint over some domain. 
// A constraint is a term, possibly together with bounds.
// The domain is an array of integers which are numerical ids of identifiers.
// The term is specified by an array of REAL. values[i] = x means that domain[i] has value x in this term.
// A constraint has a virtual member function responsible for adding the constraint to a given lp system.
class Constraint {
protected:
  int* domain;
  REAL* values;
  int domainsize;
public:
  // Adds this constraint to an lp-system and returns the number of lp constraints added.
  virtual int addToLP(lprec* lp) = 0;
};

// Custom constraint, having a specific operator that compares the term with a rhs value
// Results in one lp constraint.
class CustomConstraint : public Constraint {
  REAL rhs;
  int op;
public:
  int addToLP(lprec* lp);
  CustomConstraint(std::vector<int>* aDomain, std::vector<int>* someValues, int op, int rhs);
};

// A left and right bounded constraint.
// Results in two lp constraints.
class BoundedConstraint : public Constraint {
  REAL lBound, rBound;
public:
  int addToLP(lprec* lp);
  BoundedConstraint(std::vector<int>* identifiers, std::vector<int>* values, int aLeftBound, int aRightBound);
};

// A left bounded constraint.
// Results in one lp constraint.
class LeftBoundedConstraint : public Constraint {
  REAL bound;
public:
  int addToLP(lprec* lp);
  LeftBoundedConstraint(std::vector<int>* identifiers, std::vector<int>* values, int aBound);
};

// A right bounded constraint.
// Results in one lp constraint.
class RightBoundedConstraint : public Constraint {
  REAL bound;
public:
  int addToLP(lprec* lp);
  RightBoundedConstraint(std::vector<int>* identifiers, std::vector<int>* values, int aBound);
};

// A unbounded constraint which is equal to the empty constraint, or: true.
// Results in no lp constraints.
class UnboundedConstraint : public Constraint {
public:
  int addToLP(lprec* lp) {;}
  UnboundedConstraint(std::vector<int>* aDomain, std::vector<int>* someValues);
};

// A service model is a propositional formula having constraitns as propositions.
// It is given in disjunctive normal form without negations.
// Its domain, i.e. the identifiers occurring in all its constraitns, is split in two: inputs and outputs.
class ServiceModel {
public:
  // inputs domain
  std::vector<int> inputs;
  // outputs domain
  std::vector<int> outputs;
  // a name for this service model
  std::string name;
  // an integer alias for this service model for easier referencing in output (files)
  int alias;
  // The formula, given in dnf: constraints[x][y] is the y-th constraint in the x-th conjunctive clause
  std::vector<std::vector<Constraint*> > constraints;
};

// A result of the compatibility check of a series of models.
// Stores the models and the result.
// Can be converted to a string.
class Result {
public:
  // Models analyzed - their id corresponds to that in the YasminaLog.
  std::vector<int>* models;
  // The actual result.
  int result; // 0 = inconclusive, 1 = incompatible, -1 = syntactically incompatible
  // A constructor, simply setting the members.
  Result(std::vector<int>* composite, int res) {
    models = composite; result = res;      
  }
  // Convert this result to a string. 
  // Has the general form M1 (+) ... (+) Mn: maybe|no|undefined
  // undefined indicates that the composition of the models is not well-defined
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

// A log storing the analysis results.
class YasminaLog {
public:
  // All models occuring.
  static std::vector<ServiceModel*> models;
  // All results.
  static std::vector<Result> results;
  // Composes a string of this log. 
  // By default does not include lines for syntactically incompatible composites.
  // Contains of a declaration of all models and the string representations of the results.
  static std::string* toString(bool reportSyntacticallyIncompatible = false);
};

// An iterator for binary trees
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
