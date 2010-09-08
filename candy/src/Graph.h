/*****************************************************************************\
 Candy -- Synthesizing cost efficient partners for services

 Copyright (c) 2010 Richard MŸller

 Candy is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Candy is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Candy.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/


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
