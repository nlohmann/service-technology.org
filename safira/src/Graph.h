#ifndef _GRAPH_H
#define _GRAPH_H

#include <iostream>
#include <map>
#include <set>
#include "Node.h"
#include "Formula.h"
#include "types.h"

using namespace std;

class Graph {
	protected:

		void toDot_Header(FILE* out, string title) const;
		void toDot_Nodes(FILE* out) const;

        void printInterface(ostream& o) const;
        void printInitialNodes(ostream& o) const;
        void printGlobalFormula(ostream& o) const;
        void printNodes(ostream& o) const;

    public:

        list<int> initialNodes;

        /// the nodes indexed by an identifier
        map<int, Node*> nodes;


        /// the global formula of the graph
        Formula *globalFormula;
        string globalFormulaAsString;
        set<int> lits; //all literals in the global Formula


    	///constructor
    	Graph();

    	///destructor
    	~Graph();

    	/// add an initial node to the grpah
    	void addInitialNode(int id);

    	/// Graphviz dot output
        virtual void toDot(FILE* out, string title = "") const;

        /// Graph output
        virtual void print(ostream& o) const;

};

#endif
