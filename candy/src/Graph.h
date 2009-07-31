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

        /// output this graph as debug information
        void outputDebug(std::ostream&);

        /// output this graph in og format
        void output(std::ostream&);
};

#endif /* GRAPH_H_ */
