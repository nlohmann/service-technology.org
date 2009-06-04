//#include <cassert>
//#include <queue>
//#include <utility>
#include <iostream>
#include "Graph.h"
//#include "GraphNode.h"

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

void Graph::printToStdout() {

    cout << "root node is node id '";
    if ( root == NULL ) cout << "NULL'" << endl; else cout << root->id << "'" << endl;

    for (map<unsigned int, Node*>::iterator iter = nodes.begin();
         iter != nodes.end(); ++iter) {

        Node* currentNode = iter->second;
        currentNode->printToStdout();
    }
}
