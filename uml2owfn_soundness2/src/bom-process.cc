#include "bom-process.h"
#include "debug.h"
#include "helpers.h"

#include <cstdio>
#include <cassert>
#include <sstream>
#include <string>

using std::stringstream;


#include <iostream>

using std::cerr;
using std::endl;
 

BomProcess::BomProcess()
{
}

BomProcess::~BomProcess()
{
}

FormulaState* BomProcess::createFinalStatePredicate(PetriNet* PN)
{
	// search for places with names containing "stopNode" and "endNode"
	FormulaState* mainF = new FormulaState(LOG_OR);
	
	// create a literal p = 0 for each internal place p of the process
	set<PetriNetLiteral *> 	processNodesLit_zero;
	for (set<Place *>::iterator it = process_internalPlaces.begin(); it != process_internalPlaces.end(); it++) {
		Place* p = static_cast<Place*>(*it);
		processNodesLit_zero.insert(new PetriNetLiteral(p, COMPARE_EQUAL, 0));
	}

	// create all stop node formulae, iterate over all stop nodes
	//
	// stop node formula:   pStop > 0	
	for (set<Place *>::iterator it = process_stopNodes.begin(); it != process_stopNodes.end(); it++) {
		Place* p = static_cast<Place*>(*it);

		PetriNetLiteral* pnLit = new PetriNetLiteral(p, COMPARE_GREATER, 0);
		mainF->subFormulas.insert(pnLit);	// add literal to the formula
	}

	// create all end node formulae, iterate over all end nodes
	//
	// end node formula:   pEnd > 0 AND p1 = 0 AND ... AND p17 = 0 (pXY internal node only)
	for (set<Place *>::iterator it = process_endNodes.begin(); it != process_endNodes.end(); it++) {
		Place* p = static_cast<Place*>(*it);
		PetriNetLiteral* pnLit = new PetriNetLiteral(p, COMPARE_GREATER, 0);
		FormulaState* subF = new FormulaState(LOG_AND);
				
		subF->subFormulas.insert(pnLit);		// endNode > 0 AND
		subF->subFormulas.insert(
				processNodesLit_zero.begin(),
				processNodesLit_zero.end());	// all other internal places = 0
		
		mainF->subFormulas.insert(subF);
	}
	return mainF;
}

FormulaState* BomProcess::createOmegaPredicate(PetriNet* PN, bool stopNodes)
{
	// search for places with names containing "stopNode" and "endNode"
	FormulaState* mainF;
	
	// create a literal p = 0 for each internal place p of the process
	set<Place*>	PN_P = PN->getInternalPlaces();
	set<PetriNetLiteral *> 	processNodesLit_zero;
	for (set<Place *>::iterator it = PN_P.begin(); it != PN_P.end(); it++) {
		Place* p = static_cast<Place*>(*it);
		
		if (p->isFinal)	// skip final places (= omega place)
			continue;
		// all non-final places must have no token in the terminal state
		processNodesLit_zero.insert(new PetriNetLiteral(p, COMPARE_EQUAL, 0));
	}

	if (stopNodes) {
		return NULL;
	} else {
		FormulaState* omegaF = new FormulaState(LOG_OR);
		
		// for all final places (= omega places)
		set<Place *>	finalPlaces = PN->getFinalPlaces();
		for (set<Place*>::iterator it = finalPlaces.begin(); it != finalPlaces.end(); it++)
		{
			Place* p = static_cast<Place*>(*it);
			// pOmega > 0
			omegaF->subFormulas.insert(new PetriNetLiteral(p, COMPARE_GREATER, 0));
		}
		// ( omega1 > 0 OR ... OR omegaN > 0) AND p1 = 0 AND ... AND pM = 0
		mainF = new FormulaState(LOG_AND);
		mainF->subFormulas.insert(omegaF);
		mainF->subFormulas.insert(processNodesLit_zero.begin(), processNodesLit_zero.end());
	}

	return mainF;
}

FormulaState* BomProcess::createSafeStatePredicate (PetriNet* PN) {
  // net is safe iff all internal places do not have more than one token
  // the analysis checks for the violation of the safe state
  FormulaState* mainF;
  
  // create a literal p = 0 for each internal place p of the process
  set<Place*> PN_P = PN->getInternalPlaces();
  set<PetriNetLiteral *>  processNodesLit_zero;
  for (set<Place *>::iterator it = PN_P.begin(); it != PN_P.end(); it++) {
    Place* p = static_cast<Place*>(*it);
    
    if (p->getPreSet().size() <= 1) {
      // this place is only unsafe if its predecessor is unsafe
      // or if it is unsafely marked
      // TODO add check for initial marking
      continue;
    }
    // all internal places must have not more than one token
    processNodesLit_zero.insert(new PetriNetLiteral(p, COMPARE_GREATER, 1));
  }
  // disjunction over all literals
  FormulaState* safeF = new FormulaState(LOG_OR);
  safeF->subFormulas.insert(processNodesLit_zero.begin(), processNodesLit_zero.end());
  return safeF;
}

/*!
 * \brief	remove places representing unconnected pins from the net
 * 
 * \param	PN  the net to be modified
 */
void BomProcess::removeUnconnectedPins(PetriNet *PN) {
	set<Place*> to_remove;	// list of nodes to be removed
	
	for (set<Place *>::iterator it = pinPlaces.begin(); it != pinPlaces.end(); it++) {
		Place* p = static_cast<Place*>(*it);
		
		// if the place has an empty pre-set or post-set, remove it
		if (PN->preset(p).empty() || PN->postset(p).empty())
			to_remove.insert(p);
	}
	
    // finally remove all gathered nodes
    for (set< Place * >::iterator place = to_remove.begin(); place != to_remove.end(); place ++) {
        //cerr << "Removing Place: " << (*place)->nodeFullName() << "\n";
        PN->removePlace(*place);
        // remove place from BOM process structure as well
        process_internalPlaces.erase(*place);	
        pinPlaces.erase(*place);
    }
}

/*!
 * \brief	makes all input places internal and add
 * 			an initially marked 'alpha' place to the net
 * 
 * \param	PN  the net to be modified
 */ 
void BomProcess::soundness_initialPlaces(PetriNet *PN) {
	// create new initially marked alpha-place
	Place* pAlpha = PN->newPlace("alpha", INTERNAL);
	pAlpha->mark();
	
    set<Place*> removeNodes;
    set<Node*> noMoreInput;
    unsigned int setNum = 1;
    
    // create an activating transition for each input pinset of the process
    for (set<Transition *>::iterator inPS = process_inputPinSets.begin(); inPS != process_inputPinSets.end(); inPS++)
    {
    	// the transition of the input pinset
    	Transition* pinset_transition = static_cast<Transition *>(*inPS);
    	set<Node *> inputPins = PN->preset(pinset_transition);	// and the input pins
    	
    	if (inputPins.size() == 0)	// if the pinset has no transition (process without data input)
    	{
    		PN->newArc(pAlpha, pinset_transition);	// add new arc to the pinset transition
    		continue;
    	}
    	
    	PN->newArc(pAlpha, pinset_transition);
    	noMoreInput.insert(pinset_transition);
    	
    	for (set<Node *>::iterator it = inputPins.begin(); it != inputPins.end(); it++)
    	{
    		Place* place = static_cast<Place*>(*it);
            // choose the 'old' input place for removal
	        removeNodes.insert(place);
    	}
    	setNum++;
    }

    // remove all chosen places
    for (set<Place*>::iterator p = removeNodes.begin(); p != removeNodes.end(); p++) {
    	//cerr << "removing place " << (*p)->nodeFullName() << endl;
        PN->removePlace(*p);
    }
    // make all post-transitions that had an input-place internal transitions
    for (set<Node*>::iterator t = noMoreInput.begin(); t != noMoreInput.end(); t++) {
    	//cerr << "making transition " << (*t)->nodeFullName() << " internal" << endl;
        (*t)->type = INTERNAL;
    }
}

/*!
 * \brief	makes all output places internal and adds
 * 			live-locks to all terminal places of the net
 * 
 * \param	PN  the net to be modified
 * \param	liveLocks  introduce live-locks at final state places
 * \param	stopNodes  distinguish stop nodes from end nodes
 */ 
void BomProcess::soundness_terminalPlaces(PetriNet *PN, bool liveLocks, bool stopNodes)
{
    set<Place*> 		places_to_remove;
    set<Transition*>	transitions_to_remove;
    set<Node*> 		noMoreOutput;
    set<Place*>		omegaPlaces;	// set of new omega places

    // make all final places (as found in the final marking) non-final
    set<Place *>	finalPlaces = PN->getFinalPlaces();
    for (set<Place *>::iterator place = finalPlaces.begin(); place != finalPlaces.end(); place ++)
    {
    	(*place)->isFinal = false;
        // possible extension: introduce omega-place as unqiue final place
    }

    // replace the post-set of the output pinset transition with an
    // omega place marked final
    
    // get all post-places of output pinset-transitions
    for (set<Transition *>::iterator outPS = process_outputPinSets.begin(); outPS != process_outputPinSets.end(); outPS++)
    {
    	set<Node *> outputPins = PN->postset(*outPS);	// and the output pins
    	for (set<Node *>::iterator it = outputPins.begin(); it != outputPins.end(); it++)
    	{
    		Place* place = static_cast<Place*>(*it);
   	        places_to_remove.insert(place);
    	}
    	noMoreOutput.insert((*outPS));
    	
        Place* outputPlace = PN->newPlace((*outPS)->nodeFullName() + "_omega");
        PN->newArc((*outPS), outputPlace);
        outputPlace->isFinal = true;		// make it a final place
        
        omegaPlaces.insert(outputPlace);	// is one of the new final places
        
        if (liveLocks) {	// introduce live-lock at omega-place 
	    	Transition* tLoop = PN->newTransition(outputPlace->nodeFullName() + "_omega.loop");
	    	PN->newArc(outputPlace, tLoop, STANDARD, 1);
	    	PN->newArc(tLoop, outputPlace, STANDARD, 1);
        }
    }
    
    // when checking soundness, transition which consume from
    // end-nodes must be made looping, this is necessary to make sure
    // that the soundness-formula for correct termination on end nodes
    //   AG EF (endNode > 0 AND p1 = 0 AND ... p17 = 0)
    // remains true once an endNode carries a token
    // (consuming from endNode must not violate the formula)
	for (set<Place *>::iterator it = process_endNodes.begin(); it != process_endNodes.end(); it++) {
		Place* p = static_cast<Place*>(*it);

		// get post-transition ".eat" of place p
		Transition* t = static_cast<Transition*>(*(PN->postset(p).begin()));
		transitions_to_remove.insert(t);
		//PN->newArc(t, p, STANDARD, 1);				// add arc back to p
		
		int outputSetNum = 1;
		for (set<Place *>::iterator omega = omegaPlaces.begin();
									omega != omegaPlaces.end(); omega++)
		{
			Transition *tEndClean =
				PN->newTransition( p->nodeFullName()+".clean."+toString(outputSetNum) );
			PN->newArc(p, tEndClean);
			// is not a live-lock, tEndClean consumes from endNode
			PN->newArc((*omega), tEndClean);	
			PN->newArc(tEndClean, (*omega));
			outputSetNum++;
		}
	}
	for (set<Place *>::iterator it = process_stopNodes.begin(); it != process_stopNodes.end(); it++) {
		Place* p = static_cast<Place*>(*it);
		
		// get post-transition ".eat" of place p
		Transition* t = static_cast<Transition*>(*(PN->postset(p).begin()));
		transitions_to_remove.insert(t);
        //PN->newArc(t,p);				// add arc back to p
		
		if (stopNodes) {
			// distinguish stop nodes from end-nodes
			
		} else {
			// treat stop nodes like end nodes
			int outputSetNum = 1;
			for (set<Place *>::iterator omega = omegaPlaces.begin();
										omega != omegaPlaces.end(); omega++)
			{
				Transition *tEndClean =
					PN->newTransition( p->nodeFullName()+".clean."+toString(outputSetNum) );
				PN->newArc(p, tEndClean);
				// is not a live-lock, tEndClean consumes from endNode
				PN->newArc((*omega), tEndClean);
				PN->newArc(tEndClean, (*omega));
				outputSetNum++;
			}
		}
	}
	
    // remove the old interface output places
    for (set<Place*>::iterator p = places_to_remove.begin(); p != places_to_remove.end(); p++) {
        PN->removePlace(*p);
    }
    // remove transitions, e.g. "endNode.eat"
    for (set<Transition*>::iterator t = transitions_to_remove.begin(); t != transitions_to_remove.end(); t++) {
        PN->removeTransition(*t);
    }
    // make all pre-transitions of former output places internal
    for (set<Node*>::iterator t = noMoreOutput.begin(); t != noMoreOutput.end(); t++) {
        (*t)->type = INTERNAL;
    }
    

}
