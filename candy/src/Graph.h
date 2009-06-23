#ifndef GRAPH_H_
#define GRAPH_H_

#include "Node.h"
#include "Event.h"
#include <map>
#include <string>

using std::map;
using std::string;


class Graph {

    public:

        /*-----------.
        | attributes |
        `-----------*/

        Node* root;

        map< unsigned int, Node* > nodes;
        map< string, Event* > events;


        /*--------.
		| methods |
		`--------*/

		/// basic constructor
		Graph();

		/// basic deconstructor
		~Graph();

		/// TODO comment
        void recolorInefficientNodes();

        /// print information about this graph on std::cout
        void printToStdout();
};

#endif /* GRAPH_H_ */
