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
		isFeasible = false;
		isConstructed = false;
	}

	bool constructLP();

	bool isFeasible;
	bool isConstructed;

	void output() {
		print_lp(lp);
	}

	void evaluate(EventTerm*);

	~ExtendedStateEquation() {
		delete_lp(lp);
	}

private:
	std::map<EVENT,unsigned int> EventID;

};

#endif /* STATEEQUATION_H_ */
