#ifndef GRAPH_H_
#define GRAPH_H_

#include <map>
#include "Node.h"


class Graph {

    public:

        /// basic constructor
        Graph();

        /// basic deconstructor
        ~Graph();

        Node* root;

        std::map< unsigned int, Node* > nodes;

        void printToStdout();
};

#endif /* GRAPH_H_ */
