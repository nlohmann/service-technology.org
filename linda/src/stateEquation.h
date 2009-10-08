/*
 * stateEquation.h
 *
 *  Created on: 17.06.2009
 *      Author: Jan
 */

#ifndef STATEEQUATION_H_
#define STATEEQUATION_H_

#include "config.h"
#include <cassert>

#include "helpers.h"
#include "eventTerm.h"
#include "setsOfFinalMarkings.h"

/*!
 * A data structure for the extended state equation of an open net.
 */
class ExtendedStateEquation {
public:
	static BinaryTree<const pnapi::Place*,std::pair<int*,REAL*> >* lines; //!< For the analyzed open net, those are the lines in the equation system.
	lprec* lp; //!< The lp system for this state equation
	pnapi::PetriNet* net; //!< The open net that is to be analyzed
	PartialMarking* omega; //!< The partial final marking to be assumed

	/*!
	 * Basic constructor, assigning/initializing fields.
	 */
	ExtendedStateEquation(pnapi::PetriNet* aNet, PartialMarking* aFinalMarking) {
		net = aNet;
		omega = aFinalMarking;
		lp = NULL;
		isFeasible = false;
		isConstructed = false;
	}

	/*!
	 * Constructs the lp system from ExtendedStateEquation::lines and the final marking, returns true if no error occured.
	 */
	bool constructLP();

	bool isFeasible; //!< the lp system is feasible.
	bool isConstructed; //!< the lp system was constructed.

	/*!
	 * Outputs the lp system.
	 */
	void output() {
		print_lp(lp);
	}

	/*!
	 * Evaluates a given event term and returns the bounds.
	 */
	EventTermBound* evaluate(EventTerm*);

	/*!
	 * Deletes the underlying lp system.
	 */
	~ExtendedStateEquation() {
		delete_lp(lp);
	}


	std::vector<EventTerm*> calculatedEventTerms; //!< Contains all evaluated event terms.
	std::vector<EventTermBound*> calculatedBounds; //!< Contains all computed bounds.

};

#endif /* STATEEQUATION_H_ */
