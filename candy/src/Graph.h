#ifndef GRAPH_H_
#define GRAPH_H_

#include "settings.h"
#include "Node.h"
#include "Event.h"



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

        /// remove inefficient nodes and events
        void removeInefficientNodesAndEvents();

        /// output graph information as file header
        void outputFileHeader(std::ostream&);

        /// output this graph in og format
        void outputOG(std::ostream&);

        /// output this graph in sa format
		void outputSA(std::ostream&);

        /// output this graph as debug information
		void outputDebug(std::ostream&);
};

#endif /* GRAPH_H_ */
