#include <iostream>
#include "Graph.h"

// used namespaces
using std::map;
using std::cout;
using std::endl;


//! \brief a basic constructor of Graph
Graph::Graph() :
    root(NULL) {
}


//! \brief a basic destructor of Graph
Graph::~Graph() {
    root = NULL;
    nodes.clear();
}


void Graph::recolorInefficientNodes() {

    map< Node*, list<Node*> > inefficientSuccessors;

    // compute cost and inefficient successors
    root->computeEfficientSuccessors();

    // recolors inefficient successors
    root->recolorInefficientSuccessors(inefficientSuccessors);
    inefficientSuccessors.clear();
}


void Graph::printToStdout() {

    cout << "graph has " << nodes.size() << " nodes and "
         << (events.size()) << " events\n" << endl;

    for (map<string, Event*>::const_iterator iter = events.begin();
         iter != events.end(); ++iter) {
    	Event* currentEvent = iter->second;
    	currentEvent->printToStdout();
    }

    cout << "\nroot node is node id '";
    if ( root == NULL ) cout << "NULL'" << endl; else cout << root->getID() << "'\n" << endl;
    for (map<unsigned int, Node*>::iterator iter = nodes.begin();
         iter != nodes.end(); ++iter) {

        Node* currentNode = iter->second;
        currentNode->printToStdout();
    }
}
