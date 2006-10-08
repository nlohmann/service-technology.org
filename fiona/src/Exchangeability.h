/*!
 *  \class exchange
    \author Kathrin Kaschner
 */
 
#ifndef EXCHANGEABILITY_H_
#define EXCHANGEABILITY_H_


#include "mynew.h" 
#include <list>
//#include <vector>
//#include <map>

//#include "vertex.h"
//#include "CNF.h"

#include "util.h"
#include "cudd.h" 
#include "cuddInt.h"
#include "dddmp.h"

using namespace std;
//typedef vector<bool> BitVector;


class Exchangeability{
	public:
		Exchangeability(char* filename);
		~Exchangeability();
		
		bool check(Exchangeability* bdd);

        // Provides user defined operator new. Needed to trace all new
        // operations on this class.
#undef new
        NEW_OPERATOR(Exchangeability)
#define new NEW_NEW

	private:	  
		static DdManager* mgrMp;
		static DdManager* mgrAnn;
		
		DdNode* bddMp;
		DdNode* bddAnn;
		char** names; //varibles names of bddAnn
		int nbrVarAnn; //size of array names
		list<char*> labelList;
		
		void loadBdd(char* filename);
		void loadHeader(FILE* fp, char*** names, int* nVars, int** permids);
		DdNode* loadDiagram(FILE* fp, DdManager* mgr, int size, int* permids);
		void printDotFile(char* filename, char** varNames, DdNode* bddMp, DdNode* bddAnn);
		//void checkManager(DdManager* mgr, char* table);
};

#endif /*EXCHANGEABILIY_H_*/
