#ifndef _GRAPH_H
#define _GRAPH_H

#include <map>
#include "Node.h"
#include "types.h"

class Graph {
    private:
        /// the root node of the graph
        const Node *root;
        
        /// the nodes indexed by an identifier
        std::map<unsigned int, Node*> nodes;
        
        /// the labels
        Labels labels;
        
        /// the maximal node id
        static unsigned int maxId;
        
    public:
        /// set the root of the graph
        void setRoot(Node *n);
        
        /// adds a label to the graphs
        void addLabel(std::string l);
        
        /// add a node to the graph
        void addNode(Node *n);
        
        /// minimize the graph (OG)
        void minimize();
        
        /// Fiona OG output
        void ogOut() const;
};

#endif
