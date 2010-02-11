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
    	static int maxId;		/// maximal id of all nodepair objects
    public:
        Labels outLabels; 		/// the labels of outgoing arcs
        int id;
        Node* node1;
        Node* node2;
        Formula *formula;  		/// a formula assigned to this node
        list<Nodepair*> *outEdges; 	/// outgoing edges

    public:
        /// constructor
        Nodepair(Formula* _f, Node* _node1, Node* _node2);

        /// destructor
        ~Nodepair();

        /// add an outoing edge to the node
        void addEdge(int label, Nodepair *target);

};

#endif
