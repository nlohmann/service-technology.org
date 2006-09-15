/*!
 *  \class BddRepresentation
    \author Kathrin Kaschner
 */
#ifndef BDDREPRESENTATION_H_
#define BDDREPRESENTATION_H_
 
#include "mynew.h" 
#include <vector>
#include <map>

#include "vertex.h"
#include "CNF.h"

#include "util.h"
#include "cudd.h" 
#include "cuddInt.h"
#include "dddmp.h"

using namespace std;
typedef vector<bool> BitVector;

class BddLabelTab;

class BddRepresentation{
	public:
		BddRepresentation(unsigned int numberOfLabels, Cudd_ReorderingType heuristic = CUDD_REORDER_SIFT);
		~BddRepresentation();
		
		void convertRootNode(vertex* root);
		void generateRepresentation(vertex* v, bool visitedNodes[]);
		void addOrDeleteLeavingEdges(vertex* v);
		void reorder(Cudd_ReorderingType heuristic = CUDD_REORDER_SIFT);
		void printDotFile(char** names= NULL);
		void print();
		void save();

		
        // Provides user defined operator new. Needed to trace all new
        // operations on this class.
#undef new
        NEW_OPERATOR(BddRepresentation)
#define new NEW_NEW

	private:	  
		DdManager* mgrMp;
		DdManager* mgrAnn;
		DdNode* bddMp;
		DdNode* bddAnn;
		int maxNodeBits;
		int maxLabelBits;
		unsigned int maxNodeNumber;
		unsigned int nbrLabels;
		map<unsigned int, unsigned int> nodeMap;
		BddLabelTab * labelTable;
		
		DdNode* labelToBddMp(char* label);
		DdNode* nodesToBddMp(unsigned int node1, unsigned int node2);
		DdNode* annotationToBddAnn(vertex * v);
		DdNode* CNFtoBddAnn(CNF * cl);
		unsigned int getBddNumber(unsigned int node);
		void addBddVars(unsigned int max);	
		BitVector numberToBin(unsigned int number, int cntBits);
		int nbrBits(unsigned int i);
		string myitoa(unsigned int value, int base);
		void checkManager(DdManager* mgr, char* table);
};

#endif /*BDDREPRESENTATION_H_*/
