#include <cstdlib>
#include <ctime>
#include <cassert>
#include <sstream>
#include "config.h"
#include "cmdline.h"
#include "helpers.h"
#include "lp_lib.h"
#include "setsOfFinalMarkings.h"
#include <typeinfo>
#include "eventTerm.h"
#include "stateEquation.h"


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

	std::cerr << PACKAGE << ": Processing" << args_info.inputs[0] << ".\n";

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


	int bound = args_info.bound_arg;

	//	std::cout << bound << "\n";

	SetOfPartialMarkings* fSet = SetOfPartialMarkings::create(&(net->finalCondition().formula()),bound);

	vector<EventTerm*>* etermvec = EventTerm::createBasicTermSet(net);



	/*
	srand(time(NULL));

	for (int i = 0; i < 20; ++i) {
	EventTerm* rndEventTerm = EventTerm::createRandomEventTerm(net);
	std::cout <<"vorher: "<< rndEventTerm->toString() << "\n";

	std::map<pnapi::Place* const, int>* map = EventTerm::termToMap(rndEventTerm);
	std::cout <<"nachher: ";
	for (std::map<pnapi::Place* const,int>::iterator it = map->begin(); it != map->end(); ++it) {
		std::cout << (*it).first->getName() << " = " << (*it).second << "; ";
	}
	std::cout << "\n\n";
	}



	for (vector<EventTerm*>::iterator it = etermvec->begin(); it != etermvec->end(); ++it) {
		std::cout << (*it)->toString() << "; " ;
	}

	 */
//	fSet->output();



	/*--------------------.
	| 2. Build the System |
	`--------------------*/



	for (std::vector<PartialMarking*>::iterator finalMarkingIt = fSet->partialMarkings.begin();
	finalMarkingIt != fSet->partialMarkings.end();
	++finalMarkingIt) {

		ExtendedStateEquation* XSE = new ExtendedStateEquation(net,(*finalMarkingIt));
		XSE->constructLP();

		for (vector<EventTerm*>::iterator it = etermvec->begin(); it != etermvec->end(); ++it) {
			std::cout << "Evaluate..." << std::endl;

			XSE->evaluate((*it));

			std::cout << "...evaluated" << std::endl;

		}

	}


	std::cout << "Linda is done!" << std::endl;


}
