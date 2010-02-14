#ifndef FORMULATREE_H_
#define FORMULATREE_H_

#include <vector>
#include "Node.h"

class FormulaTree {
	private:
		static int maxId;		/// maximal id of all objects
	public:
	    int id;
		vector<bool>* satAssignment;
		Node* node;
		FormulaTree* yes;
		FormulaTree* no;

		///Constructors
		FormulaTree();
		FormulaTree(Node * _node);
		FormulaTree(vector<bool>* _satAssigment, FormulaTree* _yes, FormulaTree* _no);
};


#endif /* FORMULATREE_H_ */
