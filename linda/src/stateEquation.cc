#include "stateEquation.h"
#include "cmdline.h"

/// the command line parameters
extern gengetopt_args_info args_info;

/// Initialize the static members.
BinaryTree<const pnapi::Place*, std::pair<int, std::pair<int*, REAL*> > >
		* ExtendedStateEquation::lines = new BinaryTree<const pnapi::Place*,
				std::pair< int, std::pair<int*, REAL*>  > > ();
std::pair<int*, REAL*>* ExtendedStateEquation::eventLines = 0;
BinaryTree<pnapi::Transition*, unsigned int>
		* ExtendedStateEquation::transitionID = 0;

bool ExtendedStateEquation::constructLP() {

	// Our LP system has columns T1,T2,...,Tn,E1,...,Ek. We determine where events start and the like.
	const unsigned int NR_OF_COLS = net->getTransitions().size()
			+ LindaHelpers::NR_OF_EVENTS;
	const unsigned int START_TRANSITIONS = 1;
	const unsigned int START_EVENTS = START_TRANSITIONS
			+ net->getTransitions().size();
	const unsigned int START_SYNCHRO = START_EVENTS
			+ net->getSynchronizedTransitions().size();

	// Build a mapping between an lp column (int) to the pointer of the associated transition
	if (transitionID == 0) {
		unsigned int current = 0;
		transitionID = new BinaryTree<pnapi::Transition*, unsigned int> ();
		for (std::set<pnapi::Transition*>::iterator tIt =
				net->getTransitions().begin(); tIt
				!= net->getTransitions().end(); ++tIt) {
			transitionID->insert(*tIt, current++, false);
		}
	}

	// Create the lp_system
	lp = make_lp(0, NR_OF_COLS);

	// Set each column to integer since we want to solve an ILP
	for (int i = 1; i <= NR_OF_COLS; ++i) {
		set_int(lp, i, TRUE);
	}

	// If for some reason the lp system is null, we exit.
	if (lp == NULL) {
		abort(1, "Unable to create new LP model");
	}

	// Set debug and verbose of this linear problem to FALSE
	set_debug(lp, FALSE);
	set_verbose(lp, FALSE);

	// Enter the "add row mode" for faster row insertion
	set_add_rowmode(lp, TRUE);

	// Iterate over the places in this final marking
	for (BinaryTreeIterator<const pnapi::Place*, int>* placesIt =
			omega->values.begin(); placesIt->valid(); placesIt->next()) {

		// Prepare the arrays for the column ids and values
		int* transCol;
		REAL* transVal;

		// Get the place pointer
		const pnapi::Place* place = placesIt->getKey();

		// Get the value of the place in the partial marking
		int value = placesIt->getValue();

		// See how many transitions are connected to this place
		int number_of_transitions_for_this_place =
				place->getPresetArcs().size() + place->getPostsetArcs().size();

		// See if this line was already created before (and thus is stored in the static member).
		BinaryTreeNode<const pnapi::Place*, std::pair<int , std::pair<int*, REAL*> > >* line =
				lines->find(place);

		// Counter for the columns with values.
		int tr = 0;

		// If the line did not exist before...
		if (line == 0) {


			// Allocate memory for the arrays
			transCol = new int[number_of_transitions_for_this_place]();
			transVal = new REAL[number_of_transitions_for_this_place]();


			// We now iterate over the preset of the place to retrieve all transitions positively effecting the place.
			for (std::set<pnapi::Arc *>::iterator pIt =
					place->getPresetArcs().begin(); pIt
					!= place->getPresetArcs().end(); ++pIt) {

				pnapi::Transition& t = (*pIt)->getTransition();

				// We add the transition, with the weight as a factor.
				transCol[tr] = START_TRANSITIONS
						+ transitionID->find(&t)->value;
				transVal[tr] = (*pIt)->getWeight();
				++tr;
			}

			// We now iterate over the postset of the place to retrieve all transitions negatively effecting the place.
			for (std::set<pnapi::Arc *>::iterator pIt =
					place->getPostsetArcs().begin(); pIt
					!= place->getPostsetArcs().end(); ++pIt) {

				pnapi::Transition& t = (*pIt)->getTransition();
				if (place->getPreset().find(&t) != place->getPreset().end()) {
					int thisID = START_TRANSITIONS + transitionID->find(&t)->value;
					for (int i = 0; i < tr; ++i) {
						if (transCol[i] == thisID) {
							transVal[i] +=  -1 * (int) (*pIt)->getWeight();
						}
					}

				} else {

				// We add the transition, with the weight as a factor.
				transCol[tr] = START_TRANSITIONS
						+ transitionID->find(&t)->value;
				transVal[tr] = -1 * (int) (*pIt)->getWeight();

				++tr;

				}
			}

			// Store this line for further use in the static member
			lines->insert(place, std::pair<int, std::pair<int*, REAL*> > (tr, std::pair<int*, REAL*>(transCol, transVal)),
					false);

		} else { // The line already exist, we just set our pointers to those in the static member
			tr = line->value.first;
			transCol = line->value.second.first;
			transVal = line->value.second.second;
		}

		// Find the difference between initial and final marking for this place (and final marking)
		int diffval = value;
		int initialm = place->getTokenCount();
		diffval -= initialm;

		// Add the row to the linear program (name of lp, number of columns affected, the columns affected,
		//                                    the values for each affected column, the comparison operator,
		//                                    the right hand side value.)
		add_constraintex(lp, tr, transVal,
				transCol, EQ, diffval);
	}

	// If the event line array has not been initialized yet, do that now.
	if (eventLines == 0) {
		eventLines = new std::pair<int*, REAL*>[LindaHelpers::NR_OF_EVENTS]();
		for (int i = 0; i < LindaHelpers::NR_OF_EVENTS; ++i) {

			pnapi::Label* p = LindaHelpers::EVENT_LABELS[i];

			int* transCol = new int[p->getTransitions().size() + 1];
			REAL* transVal = new REAL[p->getTransitions().size() + 1];

			int tr = 0;

			for (std::set<pnapi::Transition*>::iterator tIt = p->getTransitions().begin(); tIt != p->getTransitions().end(); ++tIt) {
				transCol[tr] = START_TRANSITIONS + transitionID->find(*tIt)->value;
				transVal[tr++] = 1;
			}
			
			transCol[tr] = START_EVENTS + i;
			transVal[tr] = -1;

			eventLines[i] = std::pair<int*, REAL*>(transCol, transVal);
		}
	}

	// Add the rows for each event, retrieved from the event lines.
	for (int i = 0; i < LindaHelpers::NR_OF_EVENTS; ++i) {

		int* transCol = eventLines[i].first;
		REAL* transVal = eventLines[i].second;

		add_constraintex(
				lp, LindaHelpers::EVENT_LABELS[i]->getTransitions().size() + 1, transVal, transCol, EQ, 0);
	}

	// We are done adding rows, so we leave the "add row mode" and set the flag.
	set_add_rowmode(lp, FALSE);
	isConstructed = true;

	print_lp(lp);
	
	// We solve the linear program to see if it is feasible. objective function is min 0 at this point.
	int ret = solve(lp);

	// Set the flag for feasibility
	isFeasible = ret != INFEASIBLE;

	// We find that the linear program is infeasible, so we do not take it into account anymore and return false.
	if (!isFeasible) {
		message("Final marking not reachable from initial marking: %s",
				omega->toString().c_str());
		return false;
	}

	return true;
}

EventTermBound* ExtendedStateEquation::evaluate(EventTerm* e) {
	// Create a mapping from the term
	int* map = EventTerm::termToMap(e);
	evaluate(map);
}

EventTermBound* ExtendedStateEquation::evaluate(int* e) {

	// Determine the first event column
	const unsigned int START_EVENTS = 1 + net->getTransitions().size();

	// Create the arrays for the objective function
	double* obj_row = new double[LindaHelpers::NR_OF_EVENTS]();
	int* obj_cols = new int[LindaHelpers::NR_OF_EVENTS]();

	// Fill the array with the values from the mapping
	for (int i = 0; i < LindaHelpers::NR_OF_EVENTS; ++i) {
		obj_row[i] = e[i];
		obj_cols[i] = START_EVENTS + i;
	}

	// Create a new EventTermBound, neither lower nor upper bounded by default
	EventTermBound* bound = new EventTermBound();

	// Set the objective function
	set_obj_fnex(lp, LindaHelpers::NR_OF_EVENTS, obj_row, obj_cols);

	// Set the objective two minimize
	set_minim(lp);

	// Solve the linear problem
	int ret = solve(lp);

	// If lp_solve found a lower bound, set the bounds object to lower bounded and set the lower bound
	if (ret != UNBOUNDED) {
		bound->lowerBounded = true;
		bound->lowerBound = (int) get_objective(lp);
	}

	// Set the objective two maximize, the objective function stays the same
	set_maxim(lp);

	// Solve the linear problem
	ret = solve(lp);

	// If lp_solve found an upper bound, set the bounds object to upper bounded and set the upper bound
	if (ret != UNBOUNDED) {
		bound->upperBounded = true;
		bound->upperBound = (int) get_objective(lp);
	}

	if (storeHistory) {
		// Add this term and bound to the "archive".
		calculatedBounds.push_back(bound);
		calculatedEventTerms.push_back(e);
	}

	// Return the pointer to the calculated bound
	return bound;

}
