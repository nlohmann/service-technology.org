#ifndef _GRAPH_H
#define _GRAPH_H

#include <map>
#include "Node.h"
#include "types.h"

using namespace std;

class Graph {
	private:
	list<int> initialNodes;
	map<int, Node*> addedNodes;
	Node *trap;

    public:
        /// the nodes indexed by an identifier
        map<int, Node*> nodes;
        Formula *globalFormula;

    	///constructor
    	Graph();

    	///destructor
    	~Graph();

    	/// add an initial node to the grpah
    	void addInitialNode(int id);

        /// complete the extended annotated graph
        void makeComplete();

        /// make the extended annotated graph total
        void makeTotal();

        /// Graphviz dot output
        void toDot(FILE* out, string title = "") const;
       // void toDot(string filename, string title = "") const;

        /// Fiona OG output
        void print() const;

        /// print the node numbers of the given map
        void printNodes(map<int, Node*> nodeMap);

        /// get the number of new nodes in the complement
        int getSizeOfAddedNodes();

//        /// adds a label to the graphs
//        void addLabel(string label, unsigned int id);
//
//        /// add a node to the graph
//        void addNode(Node *n);
//
//        /// returns true if the given node exits
//        bool nodeExists(unsigned int id);
//
//        /// returns a pointer to the node object for the given id
//        Node* getNode(unsigned int id);
//

//
//        /// remove a node from the graph (triggers edge deletion)
//        void removeNode(Node *n);



};

#endif
