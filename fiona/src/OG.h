/*!
 * 	\class operatingGuidelines
    \brief class for the generation of the operating guidelines of oWFNs

	\author Daniela Weinberg
	\par History
		- 2006-01-24 creation
 */

#ifndef OG_H_
#define OG_H_

#include "reachGraph.h"
#include "BddRepresentation.h" 

#ifdef LOG_NEW
#include "mynew.h"
#endif
//#include <map>

class oWFN;


class operatingGuidelines : public reachGraph {
	
	public:
		operatingGuidelines(oWFN *);
		~operatingGuidelines();
	
	    void buildGraph();
    	void buildGraph(vertex * );
    
    	void getInputEvents(vertex *);
    	void getActivatedOutputEvents(vertex *);
    	 
    	bool terminateBuildingGraph(vertex * );
		void convertToBdd();
		
		BddRepresentation * bdd;
};


#endif /*OG_H_*/
