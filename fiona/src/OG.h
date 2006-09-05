/*!
 * 	\class operatingGuidelines
    \brief class for the generation of the operating guidelines of oWFNs

	\author Daniela Weinberg
	\par History
		- 2006-01-24 creation
 */

#ifndef OG_H_
#define OG_H_

#include "mynew.h"
#include "communicationGraph.h"
#include "BddRepresentation.h" 

//#include <map>

class oWFN;

class operatingGuidelines : public communicationGraph {
	
	public:
		operatingGuidelines(oWFN *);
		~operatingGuidelines();
	
    	void buildGraph(vertex * );
    
    	void computeCNF(vertex *);
    	
    	bool terminateBuildingGraph(vertex * );
		void convertToBdd();
		
		BddRepresentation * bdd;

        // Provides user defined operator new. Needed to trace all new
        // operations on this class.
#undef new
        NEW_OPERATOR(operatingGuidelines)
#define new NEW_NEW
};


#endif /*OG_H_*/
