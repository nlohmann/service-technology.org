#ifndef GRAPH_H_
#define GRAPH_H_

#include "Node.h"
#include <map>

using std::map;


class Graph {

    public:

        /// basic constructor
        Graph();

        /// basic deconstructor
        ~Graph();

        Node* root;

        map< unsigned int, Node* > nodes;

        void recolorInefficientNodes();

        void printToStdout();
};

#endif /* GRAPH_H_ */
