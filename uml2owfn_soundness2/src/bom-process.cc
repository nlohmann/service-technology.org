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

// ------------------------ Petri Net related methods --------------------------

/*!
 * \brief construct a formula the describes the legal final states of the
 *        process considering the different termination semantics due to stop
 *        nodes and end nodes, the construction of the formula is based on the
 *        fields of this object that distinguish internal nodes, stop nodes, and
 *        end nodes
 *
 * \param PN  a net that formalizes the process of this object
 */
FormulaState* BomProcess::createFinalStatePredicate(PetriNet* PN) const
{
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

    // build disjunction: pStop > 0 OR (other final state formulas)
		mainF->subFormulas.insert(pnLit);
	}

	// create all end node formulae, iterate over all end nodes
	//
	// end node formula:   pEnd > 0 AND p1 = 0 AND ... AND p17 = 0 (pXY internal node only)
	for (set<Place *>::iterator it = process_endNodes.begin(); it != process_endNodes.end(); it++) {
		Place* p = static_cast<Place*>(*it);
		PetriNetLiteral* pnLit = new PetriNetLiteral(p, COMPARE_GREATER, 0);
		FormulaState* subF = new FormulaState(LOG_AND);

		subF->subFormulas.insert(pnLit);	   // endNode > 0 AND

		if (processNodesLit_zero.size() > 0) // all other internal places = 0
		  subF->subFormulas.insert(processNodesLit_zero.begin(),
                               processNodesLit_zero.end());

		// build disjunction: endNodeFormula OR (other final state formulas)
		mainF->subFormulas.insert(subF);
	}
	return mainF;
}

/*!
 * \brief construct a formula the describes the legal final states of the
 *        process, the construction of the formula is based on the declaration
 *        of Petri net places as "final places"
 *
 * \param PN  a net that formalizes a workflow
 * \param stopNodes  switch whether final places shall be treated as stopNodes
 *                   which clear the tokens on all internal places, this
 *                   functionality is currently not implemented and returns NULL
 */
FormulaState* BomProcess::createOmegaPredicate(PetriNet* PN, bool stopNodes) const
{
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

		// create a literal "> 0" for all final places (= omega places)
		set<Place *>	finalPlaces = PN->getFinalPlaces();
		for (set<Place*>::iterator it = finalPlaces.begin(); it != finalPlaces.end(); it++)
		{
			Place* p = static_cast<Place*>(*it);
			// pOmega > 0
			omegaF->subFormulas.insert(new PetriNetLiteral(p, COMPARE_GREATER, 0));
		}

		// and build the conjunction
    // ( omega1 > 0 OR ... OR omegaN > 0) AND p1 = 0 AND ... AND pM = 0
    mainF = new FormulaState(LOG_AND);
		if (omegaF->size() > 0)
		  mainF->subFormulas.insert(omegaF);
		mainF->subFormulas.insert(processNodesLit_zero.begin(), processNodesLit_zero.end());
	}

	return mainF;
}

FormulaState* BomProcess::createSafeStatePredicate (PetriNet* PN, bool mustBeNonEmpty=false) const
{
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

  // no place that could become unsafe
  if (processNodesLit_zero.empty()) {
    if (mustBeNonEmpty)
    {
      // create a literal p > 1 for an arbitrary place
      Place* p = (Place*)(*PN->getInternalPlaces().begin());
      processNodesLit_zero.insert(new PetriNetLiteral(p, COMPARE_GREATER, 1));
    }
    else
      return NULL;
  }
  assert(!processNodesLit_zero.empty());

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
 * \brief remove places and transitions representing unused output pinsets
 *
 * \param  PN  the net to be modified
 */
void BomProcess::removeEmptyOutputPinSets (PetriNet *PN) {
  set<Transition*> removeT;
  set<Place*> removeP;

  for (set<Transition*>::iterator it = process_outputPinSets.begin()
        ; it != process_outputPinSets.end(); it++)
  {
    Transition*   t   = static_cast<Transition*>(*it);
    set<Node*>    pre = t->getPreSet();

    bool          pinPlaceFound = false;
    for (set<Node*>::iterator it2 = pre.begin(); it2 != pre.end(); it2++) {
      Place*   p   = static_cast<Place*>(*it2);
      if (pinPlaces.find(p) != pinPlaces.end()) {
        pinPlaceFound = true; break;
      }
    }
    if (!pinPlaceFound) {
      // this output pinset has no more incoming pins,
      // remove it and all of its post-places (the true outgoing interface)
      set<Node*>    post = t->getPostSet();
      removeT.insert(t);
      for (set<Node*>::iterator it2 = post.begin(); it2 != post.end(); it2++) {
        Place*   p   = static_cast<Place*>(*it2);
        removeP.insert(p);
      }
    }
  }

  // remove all transitions and places
  for (set<Transition*>::iterator it=removeT.begin(); it!=removeT.end(); it++) {
    Transition*   t   = static_cast<Transition*>(*it);
    PN->removeTransition(t);
    process_outputPinSets.erase(t);
  }
  for (set<Place*>::iterator it=removeP.begin(); it!=removeP.end(); it++) {
    Place*   p   = static_cast<Place*>(*it);
    PN->removePlace(p);
  }
  removeP.clear();

  // remove dangling places that remember which input pinset was used
  for (set<Place*>::iterator pUsed = process_inputCriterion_used.begin()
         ; pUsed != process_inputCriterion_used.end(); pUsed++)
  {
    if ((*pUsed)->getPostSet().empty()) {
      removeP.insert(*pUsed);
    }
  }

  for (set<Place*>::iterator it=removeP.begin(); it!=removeP.end(); it++) {
    Place*   p   = static_cast<Place*>(*it);
    PN->removePlace(p);
    process_internalPlaces.erase(p);
    pinPlaces.erase(p);
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
 * \param keepPinsets  ignore termination according to output pinsets
 * \param wfNet  attempt to create a workflow net structure
 */
void BomProcess::soundness_terminalPlaces(PetriNet *PN, bool liveLocks, bool stopNodes, bool keepPinsets=true, bool wfNet=false)
{
  set<Place*> 		 places_to_remove;
  set<Transition*> transitions_to_remove;
  set<Node*> 		   noMoreOutput;
  set<Place*>		   omegaPlaces;	     // set of new omega places

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

    if (keepPinsets) {
      // keep pinsets: create omega post-place of the pinset-transition,
      // and add a live-locking loop if necessary
      Place* outputPlace = PN->newPlace((*outPS)->nodeFullName() + "_omega");
      PN->newArc((*outPS), outputPlace);
      outputPlace->isFinal = true;		// make it a final place

      omegaPlaces.insert(outputPlace);	// is one of the new final places

      if (liveLocks) {	// introduce live-lock at omega-place
        Transition* tLoop = PN->newTransition(outputPlace->nodeFullName() + ".loop");
        PN->newArc(outputPlace, tLoop, STANDARD, 1);
        PN->newArc(tLoop, outputPlace, STANDARD, 1);

      }

    } else {
      // do not keep pinsets: remove pinset transition
      transitions_to_remove.insert(*outPS);

      // and add a token consuming transition to each pre-place
      set<Node *> inputPins = PN->preset(*outPS); // and the output pins
      for (set<Node *>::iterator it = inputPins.begin(); it != inputPins.end(); it++)
      {
        Place* place = static_cast<Place*>(*it);
        Transition* tConsume = PN->newTransition(place->nodeFullName()+"_consume");
        PN->newArc(place, tConsume);
        // adding live-locks is not necessary
      }
    }
  }

  // connect the end- and stop- nodes to the output transitions
  if (keepPinsets)
  {
    // implement the BOM process termination semantics in petri nets:
    // when checking soundness, transition which consume from
    // end-nodes must be made looping, this is necessary to make sure
    // that the soundness-formula for correct termination on end nodes
    //   AG EF (endNode > 0 AND p1 = 0 AND ... p17 = 0)
    // remains true once an endNode carries a token
    // (consuming from endNode must not violate the formula)

    // include all end nodes in the process termination semantics
    for (set<Place *>::iterator it = process_endNodes.begin(); it != process_endNodes.end(); it++) {
      Place* p = static_cast<Place*>(*it);

      // get post-transition ".eat" of place p
      Transition* t = static_cast<Transition*>(*(PN->postset(p).begin()));
      transitions_to_remove.insert(t);
      //PN->newArc(t, p, STANDARD, 1);				// add arc back to p

      if (!wfNet) {
        // not creating a workflow net, add a transition that cleans the end node
        // for each omega place
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
      } else {
        // try to create a workflow net: each output pinset has its own
        // omega transition and its own omega-place, add each end node
        // to the preset of each output pinset-omega transition
        for (set<Transition *>::iterator outPS = process_outputPinSets.begin();
             outPS != process_outputPinSets.end(); outPS++)
        {
          PN->newArc(p, (*outPS));
        }
      }

    }

    // include all stop nodes in the process termination semantics
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

        if (!wfNet) {
          // not creating a workflow net, add a transition that cleans the end node
          // for each omega place
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
        } else {
          // try to create a workflow net: each output pinset has its own
          // omega transition and its own omega-place, add each end node
          // to the preset of each output pinset-omega transition
          for (set<Transition *>::iterator outPS = process_outputPinSets.begin();
               outPS != process_outputPinSets.end(); outPS++)
          {
            PN->newArc(p, (*outPS));
          }
        }
      }
    }

    // a workflow net may have only one omega place
    if (wfNet) {
      Place* processOmega = PN->newPlace("omega");
      for (set<Place *>::iterator omega = omegaPlaces.begin();
                    omega != omegaPlaces.end(); omega++)
      {
        Transition* tOmega = PN->newTransition((*omega)->nodeFullName() + ".finish");
        PN->newArc(*omega, tOmega);
        PN->newArc(tOmega, processOmega);
        (*omega)->isFinal = false;  // local omega is no longer a final place
      }
      processOmega->isFinal = true; // is the only final place in the process
    }
  } // end: connect end/stop nodes with pinsets

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
