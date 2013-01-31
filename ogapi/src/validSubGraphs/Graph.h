#ifndef _GRAPH_H
#define _GRAPH_H

#include <map>
#include <cstdio>
#include <iostream>
#include "Node.h"
#include "types.h"

class Graph {
    public:
        enum Type { BIT = 1, FORMULA = 0, NOTDEFINED };

    private:
        /// the root node of the graph
        Node *root;

        /// the nodes indexed by an identifier
        std::map<unsigned int, Node*> nodes;

        /// the labels
        Labels labels;

        /// the maximal node id
        static unsigned int maxId;

        Type type;

    public:
        /// constructor
        Graph();

//        int testAllAssignments(std::map<unsigned int, Labels> AssignmentMap, std::vector<Node*> nodes);

        /// set the root of the graph
        void setRoot(Node *n);

        /// set the graph's type
        void setType(Type t);

        /// adds a label to the graphs
        void addLabel(std::string l);

        /// add a node to the graph
        void addNode(Node *n);

        /// count the valid subgraphs of the given OG
        int countValidSubgraphs();

        /// help function
        AssignCount countFromNode(Node *node, Node *from);

        int wasVisitedByNode(Node *node, Node *from);

    private:
        /// input check
        Labels input() const;

        /// output check
        Labels output() const;

        /// synchronous check
        Labels synchronous() const;
};

#endif
