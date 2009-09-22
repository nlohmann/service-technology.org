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


BinaryTree<const pnapi::Place*,std::pair<int*,REAL*> >* ExtendedStateEquation::lines = new BinaryTree<const pnapi::Place*,std::pair<int*,REAL*> >();


bool ExtendedStateEquation::constructLP() {


	const unsigned int NR_OF_COLS = net->getTransitions().size() + net->getInterfacePlaces().size();
	const unsigned int START_TRANSITIONS = 1;
	const unsigned int START_EVENTS = START_TRANSITIONS + net->getTransitions().size();
	const unsigned int START_SYNCHRO = START_EVENTS + net->getSynchronizedTransitions().size();

	// Build a map for quick transition referencing
	BinaryTree<pnapi::Transition*, unsigned int> transitionID;
	unsigned int current = 0;


	for (std::set<pnapi::Transition*>::iterator tIt = net->getTransitions().begin(); tIt != net->getTransitions().end(); ++tIt) {
		transitionID.insert(*tIt,current++,false);
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


	for (BinaryTreeIterator<const pnapi::Place*,int>* placesIt = omega->values.begin(); placesIt->valid(); placesIt->next()) {

		const pnapi::Place* place = placesIt->getKey();

		int* transCol;
		REAL* transVal;
			int value = placesIt->getValue();

			int number_of_transitions_for_this_place = place->getPresetArcs().size() + place->getPostsetArcs().size();

		BinaryTreeNode<const pnapi::Place*,std::pair<int*,REAL*> >* line = lines->find(place);
		if (line == 0) {

			transCol = new int[number_of_transitions_for_this_place]();
			transVal = new REAL[number_of_transitions_for_this_place]();

			int tr = 0;

			// We now iterate over the preset of the place to retrieve all transitions positively effecting the place.
			for (set<pnapi::Arc *>::iterator pIt = place->getPresetArcs().begin(); pIt != place->getPresetArcs().end();
			++pIt) {
				pnapi::Transition& t = (*pIt)->getTransition();
				// We add the transition, with the weight as a factor.

				assert(tr < number_of_transitions_for_this_place);

				transCol[tr] = START_TRANSITIONS + transitionID.find(&t)->value;
				transVal[tr] = (*pIt)->getWeight();
				++tr;
			}

			// We now iterate over the postset of the place to retrieve all transitions negatively effecting the place.
			for (set<pnapi::Arc *>::iterator pIt = place->getPostsetArcs().begin(); pIt != place->getPostsetArcs().end(); ++pIt) {
				pnapi::Transition& t = (*pIt)->getTransition();

				assert(tr < number_of_transitions_for_this_place);

				// We add the transition, with the weight as a factor.
				transCol[tr] = START_TRANSITIONS + transitionID.find(&t)->value;
				transVal[tr] = -1 * (int) (*pIt)->getWeight();
				++tr;
			}


			lines->insert(place,std::pair<int*,REAL*>(transCol,transVal),false);

		} else {
			transCol = line->value.first;
			transVal = line->value.second;
		}
		int diffval = value;
		int initialm = place->getTokenCount();
		diffval -= initialm;

		add_constraintex(lp, number_of_transitions_for_this_place, transVal, transCol, EQ, diffval);
	}

	// Now add all the events


	//int ev = 0;


	for (int i = 0; i < NR_OF_EVENTS; ++i) {

		pnapi::Place* p = EVENT_PLACES[i];

		//EventID[p] = ev+START_EVENTS;

		int transCol[p->getPresetArcs().size() + p->getPostsetArcs().size() + 1];
		REAL transVal[p->getPresetArcs().size() + p->getPostsetArcs().size() + 1];
		int tr = 0;

		// We now iterate over the preset of the place to retrieve all transitions positively effecting the place.
		for (set<pnapi::Arc *>::iterator pIt = p->getPresetArcs().begin(); pIt != p->getPresetArcs().end();
		++pIt) {
			pnapi::Transition& t = (*pIt)->getTransition();
			// We add the transition, with the weight as a factor.
			transCol[tr] = START_TRANSITIONS + transitionID.find(&t)->value;
			transVal[tr++] = (*pIt)->getWeight();
		}

		// We now iterate over the postset of the place to retrieve all transitions negatively effecting the place.
		for (set<pnapi::Arc *>::iterator pIt = p->getPostsetArcs().begin(); pIt != p->getPostsetArcs().end(); ++pIt) {
			pnapi::Transition& t = (*pIt)->getTransition();
			// We add the transition, with the weight as a factor.
			transCol[tr] = START_TRANSITIONS + transitionID.find(&t)->value;
			transVal[tr++] = (*pIt)->getWeight();
		}

		transCol[tr] = START_EVENTS + i;
		transVal[tr] = -1;
		add_constraintex(lp, tr+1, transVal, transCol, EQ, 0);
	}

	set_add_rowmode(lp, FALSE);

	int ret = solve(lp);

	if (ret == INFEASIBLE) {
		std::cout << "Final marking not reachable from initial marking:" << std::endl;
		std::cout << "\t";
		omega->output(std::cout);
		return false;
	}

	isFeasible = true;
	isConstructed = true;

	return true;
}

EventTermBound* ExtendedStateEquation::evaluate(EventTerm* e) {



	std::string term = e->toString();
	int* map = EventTerm::termToMap(e);

	const unsigned int START_TRANSITIONS = 1;
	const unsigned int START_EVENTS = START_TRANSITIONS + net->getTransitions().size();


	double obj_row[NR_OF_EVENTS];
	int obj_cols[NR_OF_EVENTS];

	std::string normalformterm = "";

	for (int i = 0; i < NR_OF_EVENTS; ++i) {

		if (map[i] > 0) {
			normalformterm += " +";
		} else {
			normalformterm += " ";
		}
		normalformterm += map[i];
		normalformterm += "*";
		normalformterm += EVENT_STRINGS[i];

		obj_row[i] = map[i];
		obj_cols[i] = START_EVENTS + i;
	}

	int ret;

	EventTermBound* bound = new EventTermBound();

	assert(set_obj_fnex(lp,NR_OF_EVENTS,obj_row,obj_cols)== TRUE);

	set_minim(lp);

	ret = solve(lp);

	//	std::cout << "\t";

	if (ret == UNBOUNDED) {
		//		std::cout << "unbounded <= ";
		bound->lowerBounded = false;
	} else {
		//		std::cout << get_objective(lp) << " <= ";
		bound->lowerBounded = true;
		bound->lowerBound = (int) get_objective(lp);
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
		bound->upperBound = (int) get_objective(lp);
	}

	calculatedBounds.push_back(bound);
	calculatedEventTerms.push_back(e);

	return bound;

	//	std::cout << "\n";
}
