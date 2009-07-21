/*
 * stateEquation.cc
 *
 *  Created on: 17.06.2009
 *      Author: Jan
 */

#include "stateEquation.h"
#include "cmdline.h"
/// the command line parameters
extern gengetopt_args_info args_info;

bool ExtendedStateEquation::constructLP() {


	const unsigned int NR_OF_COLS = net->getTransitions().size() + net->getInterfacePlaces().size();
	const unsigned int START_TRANSITIONS = 1;
	const unsigned int START_EVENTS = START_TRANSITIONS + net->getTransitions().size();
	const unsigned int START_SYNCHRO = START_EVENTS + net->getSynchronizedTransitions().size();

	// Build a map for quick transition referencing
	std::map<pnapi::Transition*, unsigned int> transitionID;
	unsigned int current = 0;


	for (std::set<pnapi::Transition*>::iterator tIt = net->getTransitions().begin(); tIt != net->getTransitions().end(); ++tIt) {
		transitionID[*tIt] = current++;
	}
	lp = make_lp(0, NR_OF_COLS);

	for (int i = 1; i <= NR_OF_COLS; ++i) {
		set_int(lp,i,TRUE);
	}

	if(lp == NULL) {
		fprintf(stderr, "Unable to create new LP model\n");
		exit(1);
	}

	set_debug(lp,FALSE);
	set_verbose(lp,FALSE);

	set_add_rowmode(lp, TRUE);

	for (std::map<const pnapi::Place*,int>::iterator placesIt = omega->values.begin(); placesIt != omega->values.end(); ++placesIt) {

		const pnapi::Place* place = (*placesIt).first;

		if (net->getInterfacePlaces().find(const_cast<pnapi::Place*>(place)) != net->getInterfacePlaces().end()) {
			continue;
		}

		int value = (*placesIt).second;

		int number_of_transitions_for_this_place = place->getPresetArcs().size() + place->getPostsetArcs().size();

		int transCol[number_of_transitions_for_this_place];
		REAL transVal[number_of_transitions_for_this_place];

		int tr = 0;

		// We now iterate over the preset of the place to retrieve all transitions positively effecting the place.
		for (set<pnapi::Arc *>::iterator pIt = place->getPresetArcs().begin(); pIt != place->getPresetArcs().end();
		++pIt) {
			pnapi::Transition& t = (*pIt)->getTransition();
			// We add the transition, with the weight as a factor.

			assert(tr < number_of_transitions_for_this_place);

			transCol[tr] = START_TRANSITIONS + transitionID[&t];
			transVal[tr] = (*pIt)->getWeight();
			++tr;
		}

		// We now iterate over the postset of the place to retrieve all transitions negatively effecting the place.
		for (set<pnapi::Arc *>::iterator pIt = place->getPostsetArcs().begin(); pIt != place->getPostsetArcs().end(); ++pIt) {
			pnapi::Transition& t = (*pIt)->getTransition();

			assert(tr < number_of_transitions_for_this_place);

			// We add the transition, with the weight as a factor.
			transCol[tr] = START_TRANSITIONS + transitionID[&t];
			transVal[tr] = -1 * (int) (*pIt)->getWeight();
			++tr;
		}

		int diffval = value;
		int initialm = place->getTokenCount();
		diffval -= initialm;

		add_constraintex(lp, number_of_transitions_for_this_place, transVal, transCol, EQ, diffval);
	}

	// Now add all the events


	int ev = 0;


	for (set<pnapi::Place *>::iterator it = net->getInterfacePlaces().begin(); it != net->getInterfacePlaces().end(); ++it) {

		pnapi::Place* p = *it;

		EventID[p] = ev+START_EVENTS;

		int transCol[p->getPresetArcs().size() + p->getPostsetArcs().size() + 1];
		REAL transVal[p->getPresetArcs().size() + p->getPostsetArcs().size() + 1];
		int tr = 0;

		// We now iterate over the preset of the place to retrieve all transitions positively effecting the place.
		for (set<pnapi::Arc *>::iterator pIt = p->getPresetArcs().begin(); pIt != p->getPresetArcs().end();
		++pIt) {
			pnapi::Transition& t = (*pIt)->getTransition();
			// We add the transition, with the weight as a factor.
			transCol[tr] = START_TRANSITIONS + transitionID[&t];
			transVal[tr++] = (*pIt)->getWeight();
		}

		// We now iterate over the postset of the place to retrieve all transitions negatively effecting the place.
		for (set<pnapi::Arc *>::iterator pIt = p->getPostsetArcs().begin(); pIt != p->getPostsetArcs().end(); ++pIt) {
			pnapi::Transition& t = (*pIt)->getTransition();
			// We add the transition, with the weight as a factor.
			transCol[tr] = START_TRANSITIONS + transitionID[&t];
			transVal[tr++] = (*pIt)->getWeight();
		}

		transCol[tr] = START_EVENTS + ev++;
		transVal[tr] = -1;
		add_constraintex(lp, tr+1, transVal, transCol, EQ, 0);
	}

	set_add_rowmode(lp, FALSE);

	int ret = solve(lp);

	if (ret == INFEASIBLE) {
		std::cout << "Final marking not reachable from initial marking:" << std::endl;
		std::cout << "\t";
		omega->output();
		return false;
	}

	isFeasible = true;
	isConstructed = true;

}

void ExtendedStateEquation::evaluate(EventTerm* e) {



	string term = e->toString();
	std::map<pnapi::Place* const, int>* map = EventTerm::termToMap(e);

	int counter = 0;
	double obj_row[map->size()];
	int obj_cols[map->size()];

	std::string normalformterm = "";

	for (std::map<pnapi::Place* const,int>::iterator it = map->begin(); it != map->end(); ++it) {

		if ((*it).second > 0) {
			normalformterm += " +";
		} else {
			normalformterm += " ";
		}
		normalformterm += intToStr((*it).second);
		normalformterm += "*";
		normalformterm += (*it).first->getName();

		obj_row[counter] = (*it).second;
		obj_cols[counter] = EventID[(*it).first];
		++counter;
	}

	int ret;

	EventTermBound* bound = new EventTermBound();

	assert(set_obj_fnex(lp,map->size(),obj_row,obj_cols)== TRUE);

	set_minim(lp);

	ret = solve(lp);

//	std::cout << "\t";

	if (ret == UNBOUNDED) {
//		std::cout << "unbounded <= ";
		bound->lowerBounded = false;
	} else {
//		std::cout << get_objective(lp) << " <= ";
		bound->lowerBounded = true;
		bound->lowerBound = get_objective(lp);
	}

	set_maxim(lp);

	//print_lp(lp);

	ret = solve(lp);

	if (ret == UNBOUNDED) {
//		std::cout << " <= unbounded; ";
		bound->upperBounded = false;
	} else {
//		std::cout << " <= " << get_objective(lp) << "; ";
		bound->upperBounded = true;
		bound->upperBound = get_objective(lp);
	}

	calculated[e] = bound;

//	std::cout << "\n";
}
