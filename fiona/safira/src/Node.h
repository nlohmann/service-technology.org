/*****************************************************************************\
 Safira -- Implementing a Set Algebra for Service Behavior

 Copyright (c) 2010 Kathrin Kaschner

 Safira is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Safira is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Safira.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

#ifndef _NODE_H
#define _NODE_H

#include <list>
#include "types.h"
#include "Formula.h"

using namespace std;


class Node {
    private:
    	static int maxId;		/// maximal id of all node objects

    public:
        Labels outLabels; 		/// the labels of outgoing arcs
        int id; 		        /// an identifier as given by the .og file
        Formula *formula;  		/// a formula assigned to this node
        list<Node*> *outEdges; 	/// outgoing edges

    public:
        /// constructor
        Node(Formula* _f, int _id);
        Node(Formula* _f);

        /// destructor
        ~Node();

        /// add an outoing edge to the node
        void addEdge(int label, Node *target);

        /// reset the maxId
        static void init();
};




class Nodepair {
    private:
    	//static int maxId;		/// maximal id of all nodepair objects
    public:
        //Labels outLabels; 		/// the labels of outgoing arcs
        //int id;
        Node* n1;
        Node* n2;
      //  Formula *formula;  		/// a formula assigned to this node
        Node* node;
      //  list<Nodepair*> *outEdges; 	/// outgoing edges

    public:
        /// constructor
        Nodepair(Formula* _f, Node* _n1, Node* _n2);
       // Nodepair(Node* _n1, Node* _n2);

        /// destructor
        ~Nodepair();

        /// add an outoing edge to the node
       // void addEdge(int label, Nodepair *target);

};

#endif
