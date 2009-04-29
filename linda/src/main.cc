#include <cstdlib>
#include <ctime>
#include <cassert>
#include <sstream>
#include "config.h"
#include "cmdline.h"
#include "helpers.h"
#include "lp_lib.h"

/// the command line parameters
gengetopt_args_info args_info;

/// evaluate the command line parameters
void evaluateParameters(int argc, char** argv) {
	// set default values
	cmdline_parser_init(&args_info);

	// initialize the parameters structure
	struct cmdline_parser_params *params = cmdline_parser_params_create();

	// call the cmdline parser
	cmdline_parser(argc, argv, &args_info);

	// check whether at most one file is given
	if (args_info.inputs_num > 1) {
		fprintf(stderr, "%s: at most one input file must be given -- aborting\n", PACKAGE);
		exit(EXIT_FAILURE);
	}
	free(params);
}


int main(int argc, char** argv) {
	time_t start_time, end_time;

	time(&start_time);
	/*--------------------------------------.
	| 0. parse the command line parameters  |
	`--------------------------------------*/

	evaluateParameters(argc, argv);

	std::cerr << PACKAGE << "%s: Processing" << args_info.inputs[0] << ".\n";

	pnapi::PetriNet* net = new pnapi::PetriNet;

	/*----------------------.
	| 1. parse the open net |
	`----------------------*/

	try {
		// parse either from standard input or from a given file
		if (args_info.inputs_num == 0) {
			std::cin >> pnapi::io::owfn >> *(net);
		} else {
			assert (args_info.inputs_num == 1);
			std::ifstream inputStream;
			inputStream.open(args_info.inputs[0]);
			inputStream >> pnapi::io::owfn >> *(net);
			inputStream.close();
		}
		if (args_info.verbose_given) {
			std::cerr << PACKAGE << ": read net " << pnapi::io::stat << *(net) << std::endl;
		}
	} catch (pnapi::io::InputError error) {
		std::cerr << PACKAGE << error << std::endl;
		exit(EXIT_FAILURE);
	}

	/*--------------------.
	| 2. Build the System |
	`--------------------*/

	// Trying to retrieve a final marking from the stored final condition.
	// Basically, only conjunctions are supported by now.
	const pnapi::formula::Conjunction* finalmarking = dynamic_cast<const pnapi::formula::Conjunction*> (&(net->finalCondition().formula()));

	if (finalmarking == NULL) {
		fprintf(stderr, "%s: Finalmarking not recognizable.\n", PACKAGE);
		exit(EXIT_FAILURE);
	}

	// Create to arrays that represent the final marking:
	// For each place that is specified in the final marking, we store the pointer and the number of tokens
	const pnapi::Place* omegaPlaces[finalmarking->children().size()];
	int omegaTokens[finalmarking->children().size()];
	int om = 0; // Current index.

	// Build a map for quick transition referencing
	std::map<pnapi::Transition*, unsigned int> transitionID;
	unsigned int current = 0;
	for (std::set<pnapi::Transition*>::iterator tIt = net->getTransitions().begin(); tIt != net->getTransitions().end(); ++tIt) {
		transitionID[*tIt] = current;
	}

	const unsigned int NR_OF_COLS = net->getTransitions().size() + net->getInterfacePlaces().size() + finalmarking->children().size();
	const unsigned int START_TRANSITIONS = 1;
	const unsigned int START_EVENTS = START_TRANSITIONS + net->getTransitions().size();
	const unsigned int START_OMEGAS = START_EVENTS + net->getInterfacePlaces().size();

	if (args_info.verbose_given) {
		fprintf(stderr, "%s: Number of columns: %i (%i transitions, %i events, %i final places)\n", PACKAGE, NR_OF_COLS, START_EVENTS-1, START_OMEGAS-START_EVENTS, NR_OF_COLS-START_OMEGAS+1);
	}

	lprec *lp;

	lp = make_lp(0, NR_OF_COLS);

	if(lp == NULL) {
		fprintf(stderr, "Unable to create new LP model\n");
		return(1);
	}
	set_verbose(lp,0);

	set_add_rowmode(lp, TRUE);

	// Go through the final marking and create an equation for each place specified
	for (std::set<const pnapi::formula::Formula *>::iterator fIt = finalmarking->children().begin(); fIt != finalmarking->children().end(); ++fIt) {

		// Retrieve the place.
		const pnapi::formula::Formula& f = (**fIt);
		const pnapi::formula::Proposition* p = dynamic_cast<const pnapi::formula::Proposition*> (&f);

		if (p == NULL) {
			fprintf(stderr, "%s: Finalmarking not recognizable.\n", PACKAGE);
			exit(EXIT_FAILURE);
		}

		const pnapi::Place* place = &(p->place());

		// Right now we are ignoring it, if an interface place is specified in the final marking.
		if (net->getInterfacePlaces().find(const_cast<pnapi::Place *>(place)) == net->getInterfacePlaces().end()) {

			// First, add the marking as a range for the corresponding place column.
			omegaPlaces[om] = &(p->place());
			omegaTokens[om] = p->tokens();
			int intermediate = p->tokens() - p->place().getTokenCount();
			set_bounds(lp,START_OMEGAS + om,intermediate,intermediate);

			// Now, add the constraint for the place.
			std::set<pnapi::Arc *> preset = p->place().getPresetArcs();
			std::set<pnapi::Arc *> postset = p->place().getPostsetArcs();
			int transCol[preset.size() + postset.size() + 1];
			REAL transVal[preset.size() + postset.size() + 1];

			int tr = 0;

			// We now iterate over the preset of the place to retrieve all transitions positively effecting the place.
			for (set<pnapi::Arc *>::iterator pIt = preset.begin(); pIt != preset.end();
			++pIt) {
				pnapi::Transition& t = (*pIt)->getTransition();
				// We add the transition, with the weight as a factor.
				transCol[tr] = START_TRANSITIONS + transitionID[&t];
				transVal[tr++] = (*pIt)->getWeight();
			}

			// We now iterate over the postset of the place to retrieve all transitions negatively effecting the place.
			for (set<pnapi::Arc *>::iterator pIt = postset.begin(); pIt != postset.end(); ++pIt) {
				pnapi::Transition& t = (*pIt)->getTransition();
				// We add the transition, with the weight as a factor.
				transCol[tr] = START_TRANSITIONS + transitionID[&t];
				transVal[tr++] = -1 * (int) (*pIt)->getWeight();
			}

			// The corresponding final place
			transCol[tr] = START_OMEGAS + om;
			transVal[tr] = -1;

			add_constraintex(lp, preset.size() + postset.size() + 1, transVal, transCol, EQ, 0);

			++om;
		}
	}


	// Now add all the events

	pnapi::Place* EventPtr[net->getInterfacePlaces().size()];
	std::map<EVENT,unsigned int> EventID;

	int ev = 0;
	for (set<pnapi::Place *>::iterator it = net->getInterfacePlaces().begin(); it != net->getInterfacePlaces().end(); ++it) {

		pnapi::Place* p = *it;

		EventPtr[ev+START_EVENTS] = p;
		EventID[p] = ev+START_EVENTS;

		std::set<pnapi::Arc *> preset = p->getPresetArcs();
		std::set<pnapi::Arc *> postset = p->getPostsetArcs();

		int transCol[preset.size() + postset.size() + 1];
		REAL transVal[preset.size() + postset.size() + 1];
		int tr = 0;

		// We now iterate over the preset of the place to retrieve all transitions positively effecting the place.
		for (set<pnapi::Arc *>::iterator pIt = preset.begin(); pIt != preset.end();
		++pIt) {
			pnapi::Transition& t = (*pIt)->getTransition();
			// We add the transition, with the weight as a factor.
			transCol[tr] = START_TRANSITIONS + transitionID[&t];
			transVal[tr++] = (*pIt)->getWeight();
		}

		// We now iterate over the postset of the place to retrieve all transitions negatively effecting the place.
		for (set<pnapi::Arc *>::iterator pIt = postset.begin(); pIt != postset.end(); ++pIt) {
			pnapi::Transition& t = (*pIt)->getTransition();
			// We add the transition, with the weight as a factor.
			transCol[tr] = START_TRANSITIONS + transitionID[&t];
			transVal[tr++] = (*pIt)->getWeight();
		}
		transCol[tr] = START_EVENTS + ev++;
		transVal[tr] = -1;
		add_constraintex(lp, preset.size() + postset.size() + 1, transVal, transCol, EQ, 0);
	}

	set_add_rowmode(lp, FALSE);
	for (int i = START_TRANSITIONS; i < START_OMEGAS; ++i) {
		set_int(lp,i,'1');
	}

	L0MessageProfile mp(net->getInterfacePlaces().size());
	EBV candidate;

	for (int i = START_EVENTS; i < START_OMEGAS; ++i) {

		int objCol[1];
		REAL objVal[1];

		objCol[0] = i;
		objVal[0] = 1;
		set_obj_fnex(lp, 1, objVal, objCol);

		Bounds<EVENT> b(EventPtr[i]);
		set_minim(lp);

		int result = solve(lp);
		if (result == 0) {
			b.min = (int) get_objective(lp);
		}

		set_maxim(lp);
		result = solve(lp);
		if (result == 0) {
			b.max = (int) get_objective(lp);
		}

		mp.bounds[i-START_EVENTS] = b;
		candidate.push_back(b);
	}

	vector<EventSetBound> results;
	set<PPS> alreadyTested;

	if (args_info.level_1_given) {
		findMaxDependenciesR(candidate, results, alreadyTested, lp, EventID);
	}

	delete_lp(lp);

	time(&end_time);

	mp.out();

	if (args_info.verbose_given) {
		fprintf(stderr, "%s: Calculation took %.0f seconds.\n", PACKAGE, difftime(end_time, start_time));
	}
}
