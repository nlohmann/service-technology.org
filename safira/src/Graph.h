#ifndef _GRAPH_H
#define _GRAPH_H

#include <map>
#include <stack>
#include "Node.h"
#include "Formula.h"
#include "types.h"
#include <iostream>

using namespace std;

class Graph {
	private:
		list<int> initialNodes;
		list<Intpair> initialNodepairs;
		map<int, Node*> addedNodes;
		Node *trap;
		stack<long> myStack;
		stack<Nodepair*> unproc;

        void printInterface(ostream& o) const;
        void printInitialNodes(ostream& o) const;
        void printNodes(ostream& o) const;
        void printNodepairs(ostream& o) const;
        /// prints the global Formula of the complement (it is required that makeTotal and makeComplete was executed before)
        void printGlobalFormulaForComplement(ostream& o) const;
        void printGlobalFormula(ostream& o) const;
        Node* getNode(Formula *f);
        bool formulaFounded(Formula *f, Formula *g);

        void product(Nodepair* qp);

    public:
        /// the nodes indexed by an identifier
        map<int, Node*> nodes;

        map<Intpair, Nodepair*, Cmp> nodepairs;


        /// the global formula of the graph
        Formula *globalFormula;

    	///constructor
    	Graph();

    	///destructor
    	~Graph();

    	/// add an initial node to the grpah
    	void addInitialNode(int id);

    	/// cumputes the intersection for the given graphs
    	void intersection(Graph *g1, Graph *g2);

    	/// computes the complement for the extended annotated graph
    	void complement();

        /// complete the extended annotated graph
        void makeComplete();

        /// make the extended annotated graph total
        void makeTotal();

        /// Graphviz dot output
        void toDot(FILE* out, string title = "") const;

        /// Graphviz dot output
        void toDotPair(FILE* out, string title = "") const;

        /// Graph output
        void print(ostream& o) const;

        /// Graph output as complement (it is required that makeTotal and makeComplete was executed before)
        void printComplement(ostream& o) const;

        void printIntersection(ostream& o) const;

        void getGlobalFormulaForComplement(ostream& o) const;
        void getGlobalFormula(ostream& o) const;

        /// get the number of new nodes in the complement
        int getSizeOfAddedNodes();


//        /// adds a label to the graphs
//        void addLabel(string label, unsigned int id);

};

#endif
