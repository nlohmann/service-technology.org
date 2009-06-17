/*
 * stateEquation.h
 *
 *  Created on: 17.06.2009
 *      Author: Jan
 */

#ifndef STATEEQUATION_H_
#define STATEEQUATION_H_

#include "helpers.h"
#include "eventTerm.h"
#include "setsOfFinalMarkings.h"

class ExtendedStateEquation {
public:
	lprec* lp;
	pnapi::PetriNet* net;
	PartialMarking* omega;

	ExtendedStateEquation(pnapi::PetriNet* aNet, PartialMarking* aFinalMarking) {
		net = aNet;
		omega = aFinalMarking;
		lp = NULL;
	}

	bool constructLP();

	void output() {
		print_lp(lp);
	}

	void evaluate(EventTerm*);

private:
	std::map<EVENT,unsigned int> EventID;

};

#endif /* STATEEQUATION_H_ */
