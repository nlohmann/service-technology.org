#include <iostream>
#include "Graph.h"

// used namespaces
using std::map;
using std::cout;
using std::endl;


//! \brief a basic constructor of AnnotadedGraph
Graph::Graph() :
    root(NULL) {
}


//! \brief a basic destructor of AnnotadedGraph
Graph::~Graph() {

    root = NULL;
    nodes.clear();
}


void Graph::recolorInefficientNodes() {

    map< Node*, list<Node*> > inefficientSuccessors;

    // compute cost and inefficient successors
    root->computeInefficientSuccessors(inefficientSuccessors);

    // recolor inefficient successors
    root->recolorInefficientSuccessors(inefficientSuccessors);
    inefficientSuccessors.clear();
}


void Graph::printToStdout() {

    cout << "graph has " << nodes.size() << " nodes" << endl;
    cout << "root node is node id '";
    if ( root == NULL ) cout << "NULL'" << endl; else cout << root->getID() << "'" << endl;

    for (map<unsigned int, Node*>::iterator iter = nodes.begin();
         iter != nodes.end(); ++iter) {

        Node* currentNode = iter->second;
        currentNode->printToStdout();
    }
}
