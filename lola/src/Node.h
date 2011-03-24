#pragma once

#include <iostream>
class Arc;

using std::ostream;


/// Petri net nodes, including places and transitions
class Node {
    public:
        unsigned int nr;
        char* name;   ///< external (string) representation
        unsigned short int NrOfArriving; ///< number of incoming arcs
        unsigned short int NrOfLeaving; ///< number of outgoing arcs
        Arc** ArrivingArcs;  ///< array with pointers to each incoming arc
        Arc** LeavingArcs;  ///< array with pointers to each outgoing arc
        Node(char*);  ///< constructor
        ostream& operator << (ostream&);  ///< output procedure
        ~Node(); ///< destructor
        void NewArriving(Arc&); ///< insertion of a new incoming arc during net construction
        void NewLeaving(Arc&); ///< insertion of a new outgoing arc during net construction
        unsigned int pos[2]; ///< index in array of all nodes, two values for appearance in two arrays
#ifdef CYCLE
        Node* parent ;  ///< for organizing a list that serves as a stack in a depth-first search through the net
        ///< idea: cycles in the net are necessary for seeing cycles in the state space
#endif
};

/// insert a new incoming arc
inline void Node::NewArriving(Arc& a) {
    Arc** Old = ArrivingArcs;  /// remember original array
    NrOfArriving++; /// increment size
    ArrivingArcs = new Arc* [NrOfArriving]; /// enlarge array
    for (int i = 0; i < NrOfArriving - 1; i++) { ///copy old entries
        ArrivingArcs[i] = Old[i];
    }
    ArrivingArcs[NrOfArriving - 1] = & a; /// add new arc
    delete [] Old; /// free old array
}

/// insert a new outgoing arc
inline void Node::NewLeaving(Arc& a) {
    Arc** Old = LeavingArcs;  /// remember old array
    NrOfLeaving++; /// increment size
    LeavingArcs = new Arc* [NrOfLeaving]; ///enlarge array
    for (int i = 0; i < NrOfLeaving - 1; i++) { /// copy old entries
        LeavingArcs[i] = Old[i];
    }
    LeavingArcs[NrOfLeaving - 1] = & a; /// add new arc
    delete [] Old; /// free old arry
}

/// destructor
inline Node::~Node() {
    /// release nontrivial ingredients
    delete [] name;
    delete []  ArrivingArcs;
    delete [] LeavingArcs;
}

/// constructor
inline Node::Node(char* n) {
    /// copy external representation
    name = new char [strlen(n) + 5];
    strcpy(name, n);
    /// initialize arcs
    pos[0] = pos[1] = NrOfArriving = NrOfLeaving = 0;
    /// dummy arrays to start with
    ArrivingArcs = new Arc* [1];
    LeavingArcs = new Arc*  [1];
}

/// output operator
inline ostream& operator << (ostream& str, Node n) {
    str << n.name;
    return str;
}
