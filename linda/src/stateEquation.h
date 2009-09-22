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

class ExtendedStateEquation {
public:
	static BinaryTree<const pnapi::Place*,std::pair<int*,REAL*> >* lines;
	lprec* lp;
	pnapi::PetriNet* net;
	PartialMarking* omega;

	ExtendedStateEquation(pnapi::PetriNet* aNet, PartialMarking* aFinalMarking) {
		net = aNet;
		omega = aFinalMarking;
		lp = NULL;
		isFeasible = false;
		isConstructed = false;
	}

	bool constructLP();

	bool isFeasible;
	bool isConstructed;

	void output() {
		print_lp(lp);
	}

	EventTermBound* evaluate(EventTerm*);

	~ExtendedStateEquation() {
		delete_lp(lp);
	}

	std::vector<EventTerm*> calculatedEventTerms;
	std::vector<EventTermBound*> calculatedBounds;

};

#endif /* STATEEQUATION_H_ */
