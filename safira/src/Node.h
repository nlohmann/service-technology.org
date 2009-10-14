#ifndef _NODE_H
#define _NODE_H

#include <list>
#include "types.h"
#include "Formula.h"

using namespace std;


class Node {
    private:
    	static int maxId;

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
};

#endif
