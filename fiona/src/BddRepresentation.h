/*!
 *  \class BddRepresentation
    \author Kathrin Kaschner
 */
#ifndef BDDREPRESENTATION_H_
#define BDDREPRESENTATION_H_
 
 
#include <vector>

#include "vertex.h"

#include "cudd.h" 
#include "cuddInt.h"

using namespace std;
typedef vector<bool>    BitVector;

class BddRepresentation{
	public:
		BddRepresentation(vertex * v, int numberOfLabels, Cudd_ReorderingType heuristic = CUDD_REORDER_SAME);
		~BddRepresentation();
		
		void generateRepresentation(vertex* v, bool visitedNodes[]);
		void reorder(Cudd_ReorderingType heuristic = CUDD_REORDER_SAME);
		void print();
		void printDotFile(char** names= NULL);
		
	private:	 
		DdManager* mgrMp;
		DdManager* mgrAnn;
		DdNode* bddMp;
		DdNode* bddAnn;
		int maxNodeBits;
		int maxLabelBits;
		unsigned int maxNodeNumber;
		map<unsigned int, unsigned int> nodeMap;
		
		DdNode* nodesToBddMp(unsigned int node1, unsigned int node2);
		DdNode* labelToBddMp(char* label);
		BitVector* numberToBin(unsigned int number, int count);
		unsigned int getBddNumber(unsigned int node);
		//DdNode* annotationToBddAnn(DdManager* mgr, int nodeNumber, DdNode* annotation, int maxNodeBits);
		//DdNode* labelToBddAnn(DdManager* mgr, char* channel, int first, int count, vector<char*> v_channel);
		
		int nbrBits(unsigned int i);
		string myitoa(unsigned int value, int base);
		
		void checkManager(DdManager* mgr, char* table);
};

#endif /*BDDREPRESENTATION_H_*/
