#ifndef _GRAPHCOMPLEMENT_H
#define _GRAPHCOMPLEMENT_H

#include <map>
#include "FormulaTree.h"
//#include "Node.h"
#include "Graph.h"

using namespace std;

class GraphComplement : public Graph {
	private:
		map<int, Node*> addedNodes;
		Node* trap;
		FormulaTree* root;

		void toDot_addedNodes(FILE* out) const;

        void printAddedNodes(ostream& o) const;

        /// prints the global Formula of the complement (it is required that makeTotal and makeComplete was executed before)
        void printGlobalFormulaForComplement(ostream& o) const;

        Node* getNode(Formula *f);
        Node* getNode_stupid(Formula *f);
        bool formulaFound(Formula *f, Formula *g);
        Node* searchNode(Formula *f, FormulaTree *n);
//        void getGlobalFormula(ostream& o) const;



    public:

    	///constructor
    	GraphComplement();

    	///destructor
    	~GraphComplement();

    	/// computes the complement for the extended annotated graph
    	void complement();

        /// complete the extended annotated graph - fast algorithm, but unreduced result
        void makeComplete_fast();

        /// complete the extended annotated graph - stupid algorithm (simple reduction, slow)
        void makeComplete_stupid();

        /// complete the extended annotated graph - efficient algorithm (intelligent reduction, fast)
        void makeComplete_efficient();

        /// make the extended annotated graph total
        void makeTotal();

        /// Graphviz dot output
        void toDot(FILE* out, string title = "") const;

        /// Graph output as complement (it is required that makeTotal and makeComplete was executed before)
        void print(ostream& o) const;

        //TODO: diese Funktion private?
        void getGlobalFormulaForComplement(ostream& o) const;

        /// get the number of new nodes in the complement
        int getSizeOfAddedNodes();
};

#endif /* GRAPHCOMPLEMENT_H_ */
