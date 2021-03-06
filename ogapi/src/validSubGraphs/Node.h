#ifndef _NODE_H
#define _NODE_H

#include <map>
#include <vector>
#include <string>
#include "types.h"
#include "Formula.h"


class Node {
    private:        
        /// satisfying assignments for the node's formula
        Assignments *sat;
    
    public:
        /// the labels of outgoing arcs
        Labels outLabels;

        /// an identifier as given by the .og file
        unsigned int id;
        
        /// a formula assigned to this node
        Formula *formula;

        /// is the node in the current search path?
        bool isActive;

        bool hasCycles;

        unsigned int lastSearchId;

        /// list of cycle-ids that the node is in (divided into the outgoing edges)
        std::map<std::string, std::vector<int> > cycleList;

        ///  a bit assigned to this node
        enum Bit { NONE, S, F, T } bit;

        /// incoming edges
        std::map<std::string, std::vector<Node*> > inEdges;

        /// outgoing edges
        std::map<std::string, std::vector<Node*> > outEdges;

    public:
        /// constructors
        Node(unsigned int _id, Formula *_formula = NULL);

        /// destructor
        ~Node();
        
        /// add an outoing edge to the node
        void addEdge(std::string label, Node *target);
        
        /// add a formula to the node
        void setFormula(Formula *f);
        
        /// set the node's bit
        void setBit(Bit b);

        /// returns the first successors reachable by the given label
        Node *successor(const std::string &label);
        
        /// returns all assignments that satisfy the formula
        Assignments *Sat();
        
        /// returns a minimized DNF representation of the formula
        std::string dnf() const;
        
//        /// replace this node with a given one
//        void replaceWith(Node *n);

        /// returns the bit string
        const std::string bitString() const;
};

#endif
