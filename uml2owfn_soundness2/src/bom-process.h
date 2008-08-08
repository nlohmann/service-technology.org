#ifndef BOMPROCESS_H_
#define BOMPROCESS_H_

#include "pnapi.h"
#include "block.h"


#include <utility>		// for pair
#include <list>
#include <map>
#include <set>


using std::map;
using std::list;
using std::pair;
using std::set;

class BomProcess
{
public:
	//set<Block*>			bom_nodes;
	//set<Node*>			pn_nodes;
	
	//map<Node*, Block*>	pn_to_bom;
	
	set<Place*>			pinPlaces;
	
	set<Transition *>	process_inputPinSets;
	set<Transition *>	process_outputPinSets;
	
	set<Place*>			process_endNodes;
	set<Place*>			process_stopNodes;
	set<Place*>			process_internalPlaces;
	
	// places denoting that an incriterion has fired
	set<Place*>     process_inputCriterion_used;

	/// create the formula describing the final state
	FormulaState* createFinalStatePredicate (PetriNet* PN);
	FormulaState* createOmegaPredicate (PetriNet* PN, bool stopNodes);
	FormulaState* createSafeStatePredicate (PetriNet* PN);
	
	/// remove places representing unconnected pins in the given net
	void removeUnconnectedPins (PetriNet *PN);
	
	/// remove places and transitions representing unused output pinsets
	void removeEmptyOutputPinSets (PetriNet *PN);
	
	/// make input places internal and introduce a unique initial place to the net
	void soundness_initialPlaces (PetriNet *PN);
	
	/// make output places internal and add live-locks at all terminal places
	void soundness_terminalPlaces (PetriNet *PN, bool liveLocks, bool stopNodes);
	
	/// constructor
	BomProcess();
	/// destructor
	virtual ~BomProcess();
};

#endif /*BOMPROCESS_H_*/
