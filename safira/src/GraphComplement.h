#ifndef _GRAPHCOMPLEMENT_H
#define _GRAPHCOMPLEMENT_H

#include <map>
#include "Graph.h"

using namespace std;

class GraphComplement : public Graph {
	private:
		map<int, Node*> addedNodes;
		Node* trap;

		void toDot_addedNodes(FILE* out) const;

        void printAddedNodes(ostream& o) const;

        /// prints the global Formula of the complement (it is required that makeTotal and makeComplete was executed before)
        void printGlobalFormulaForComplement(ostream& o) const;

        Node* getNode(Formula *f);
        bool formulaFounded(Formula *f, Formula *g);
//        void getGlobalFormula(ostream& o) const;


    public:

    	///constructor
    	GraphComplement();

    	///destructor
    	~GraphComplement();

    	/// computes the complement for the extended annotated graph
    	void complement();

        /// complete the extended annotated graph
        void makeComplete();

        /// make the extended annotated graph total
        void makeTotal();

        /// Graphviz dot output
        void toDot(FILE* out, string title = "") const;

        /// Graph output as complement (it is required that makeTotal and makeComplete was executed before)
        void print(ostream& o) const;

        void getGlobalFormulaForComplement(ostream& o) const;

        /// get the number of new nodes in the complement
        int getSizeOfAddedNodes();


//        /// adds a label to the graphs
//        void addLabel(string label, unsigned int id);

};

#endif /* GRAPHCOMPLEMENT_H_ */
