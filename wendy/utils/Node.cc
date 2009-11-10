#include <cstdio>
#include <cassert>
#include <algorithm>
#include <map>
#include <set>
#include <vector>
#include "Node.h"
#include "helpers.h"

using std::map;
using std::set;
using std::vector;

/// constructors
Node::Node(unsigned int _id, Formula *_formula) :
    sat(NULL), id(_id), formula(_formula) {
        outEdges.clear();
    }

/// destructor
Node::~Node() {
    // nodes that need to be tidied afterwards
    set<Node*> touchedNodes;

    // process successor nodes (follow outedges)
    for (map<std::string,vector<Node*> >::iterator e = inEdges.begin(); e != inEdges.end(); ++e) {
        for (size_t i = 0; i != e->second.size(); ++i) {
            for (vector<Node*>::iterator n = e->second[i]->outEdges[e->first].begin(); n != e->second[i]->outEdges[e->first].end(); ++n) {
                if (*n == this) {
                    *n = NULL;
                    touchedNodes.insert(e->second[i]);
                }
            }
        }
    }
    
    // process predecessor nodes (follow inedges)
    for (map<std::string,vector<Node*> >::iterator e = outEdges.begin(); e != outEdges.end(); ++e) {
        for (size_t i = 0; i != e->second.size(); ++i) {
            for (vector<Node*>::iterator n = e->second[i]->inEdges[e->first].begin(); n != e->second[i]->inEdges[e->first].end(); ++n) {
                if (*n == this) {
                    *n = NULL;
                    touchedNodes.insert(e->second[i]);
                }
            }
        }
    }

    delete formula;
}

/// add an outoing edge to the node
void Node::addEdge(std::string label, Node *target) {
    assert(target);
    
    // store this edge in my outEdges
    outEdges[label].push_back(target);
    std::sort(outEdges[label].begin(), outEdges[label].end());
    std::unique(outEdges[label].begin(), outEdges[label].end());
 
    // store this edge in target's inEdges
    target->inEdges[label].push_back(this);
    std::sort(target->inEdges[label].begin(), target->inEdges[label].end());
    std::unique(target->inEdges[label].begin(), target->inEdges[label].end());

    // store the label in outLabels
    outLabels.push_back(label);
    std::sort(outLabels.begin(), outLabels.end());
    std::unique(outLabels.begin(), outLabels.end());
}

/// add a formula to the node
void Node::setFormula(Formula *f) {
    assert(f);
    formula = f;
}

void Node::setBit(Bit b) {
    bit = b;
}

/// returns the first successors reachable by the given label
Node *Node::successor(const std::string &label) {
    if (outEdges[label].empty()) {
        return NULL;
    } else {
        return outEdges[label][0];        
    }
}

/// returns all assignments that satisfy the formula
Assignments *Node::Sat() {
    // recycle previously cached value
    if (sat) {
        return sat;
    }
    
    assert(formula);
    Assignments *result = new Assignments;
    
    // build vectors for indices and bounds
    vector<unsigned int> index;
    vector<unsigned int> bounds;
    unsigned int count = 1; // the total number of assignments (2**|phi|)
    unsigned int degree = outLabels.size();
    for (size_t i = 0; i < degree; ++i) {
        index.push_back(0);
        bounds.push_back(2); // could be changed to 1 when next_index is adjusted
        count *= 2;
    }
    
    // traverse all possible incides and check satisfiability
    for (size_t j = 0; j < count; ++j) {
        Labels tempLabels;
        
        // create an assignment from current index vector
        for (size_t i = 0; i < degree; ++i) {
            if (index[i] != 0) {
                tempLabels.push_back(outLabels[i]);
            }
        }
        
        // check if assignment satisfies the formula
        if (formula->sat(&tempLabels)) {
            result->push_back(tempLabels);
        }
        
        // update the index vector
        next_index(index, bounds);
    }

    // cache and return result
    return (sat = result);
}

/// returns a minimized DNF representation of the formula
std::string Node::dnf() const {
    std::set<std::set<std::string> > temp1;
    std::set<std::string> temp2;
    
    // get DNF representation (set of sets of literals)
    formula->dnf()->rep(temp1, temp2);
    if (!temp2.empty())
        temp1.insert(temp2);

    // find implicit terms
    std::set<std::set<std::string> > implicitSets;    
    for (std::set<std::set<std::string> >::iterator it1 = temp1.begin(); it1 != temp1.end(); ++it1) {
        for (std::set<std::set<std::string> >::iterator it2 = temp1.begin(); it2 != temp1.end(); ++it2) {
            if (it1 != it2) {
                if (std::includes(it1->begin(), it1->end(), it2->begin(), it2->end())) {
                    implicitSets.insert(*it1);
                }
            }
        }
    }

    // remove implicit terms
    for (std::set<std::set<std::string> >::iterator it = implicitSets.begin(); it != implicitSets.end(); ++it) {
        temp1.erase(temp1.find(*it));
    }
    
    // copy sets to string
    std::string result;
    for (std::set<std::set<std::string> >::iterator it1 = temp1.begin(); it1 != temp1.end(); ++it1) {
        for (std::set<std::string>::iterator it2 = it1->begin(); it2 != it1->end(); ++it2) {
            result += *it2;
        }
        result += " ";
    }

    return result;
}

/// replace this node with a given one
void Node::replaceWith(Node *newNode) {
    assert(newNode);
    
    // process successor nodes (follow outedges)
    for (map<std::string,vector<Node*> >::iterator e = inEdges.begin(); e != inEdges.end(); ++e) {
        for (size_t i = 0; i != e->second.size(); ++i) {
            for (vector<Node*>::iterator n = e->second[i]->outEdges[e->first].begin(); n != e->second[i]->outEdges[e->first].end(); ++n) {
                if (*n == this) {
                    vector<Node*>::iterator temp = n;
                    *temp = newNode;
                }
            }
        }
    }
    
    // process predecessor nodes (follow inedges)
    for (map<std::string,vector<Node*> >::iterator e = outEdges.begin(); e != outEdges.end(); ++e) {
        for (size_t i = 0; i != e->second.size(); ++i) {
            for (vector<Node*>::iterator n = e->second[i]->inEdges[e->first].begin(); n != e->second[i]->inEdges[e->first].end(); ++n) {
                if (*n == this) {
                    vector<Node*>::iterator temp = n;
                    *temp = newNode;
                }
            }
        }
    }
    
    delete this;
}


const std::string Node::bitString() const {
    switch (bit)
    {
    case S:
        return "S";
    case F:
        return "F";
    case T:
        return "T";
    default:
        return "";
    }
}
