/*!
 * 	\class interactionGraph
    \brief class for the generation of the interaction graph of oWFNs

	\author Daniela Weinberg
	\par History
		- 2006-01-24 creation
 */

#ifndef IG_H_
#define IG_H_

#include "mynew.h"
#include "reachGraph.h"

class oWFN;

class interactionGraph : public reachGraph {
	public:
		interactionGraph(oWFN *);
		~interactionGraph();
	
	    void buildGraph();
    	void buildGraph(vertex * );
    	void buildReducedGraph(vertex * );
    
    	setOfMessages getActivatedInputEvents(vertex *);
    	setOfMessages getActivatedOutputEvents(vertex *);

		setOfMessages combineReceivingEvents(vertex *);
		setOfMessages receivingBeforeSending(vertex *);

    	bool terminateBuildingGraph(vertex *);
    	
    	void calculateSuccStatesOutputSet(messageMultiSet, vertex *);
    	void calculateSuccStatesInputReduced(messageMultiSet, vertex *);
        
        // Provides user defined operator new. Needed to trace all new
        // operations on this class.
#undef new
        NEW_OPERATOR(interactionGraph)
#define new NEW_NEW
};

#endif /*IG_H_*/
