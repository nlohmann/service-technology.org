#include "FormulaTree.h"


int FormulaTree::maxId = 0;

FormulaTree::FormulaTree() :
		satAssignment(NULL), node(NULL), yes(NULL), no(NULL){
	id = maxId++;
}

FormulaTree::FormulaTree(Node * _node):
		satAssignment(NULL), node(_node), yes(NULL), no(NULL){
	id = maxId++;
}

FormulaTree::FormulaTree(vector<bool>* _satAssigment, FormulaTree* _yes, FormulaTree* _no):
		satAssignment(_satAssigment), node(NULL), yes(_yes), no(_no){
	id = maxId++;
}
