#include <cstdlib>
#include <cstdio>
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

/// the parser vector
std::vector<EventTerm*>* term_vec = 0;
bool stop_interaction = false;

extern int et_yylineno;
extern int et_yydebug;
extern int et_yy_flex_debug;
extern FILE* et_yyin;
extern int et_yyerror();
extern int et_yyparse();

#ifdef YY_FLEX_HAS_YYLEX_DESTROY
extern int et_yylex_destroy();
#endif


int main(int argc, char** argv) {
	time_t start_time, end_time;

	time(&start_time);
	/*--------------------------------------.
	| 0. parse the command line parameters  |
	`--------------------------------------*/

	evaluateParameters(argc, argv);

	std::cerr << PACKAGE << ": Processing " << args_info.inputs[0] << ".\n";

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
		if (args_info.verbose_flag) {
			std::cerr << PACKAGE << ": read net " << pnapi::io::stat << *(net) << std::endl;
		}
	} catch (pnapi::io::InputError error) {
		std::cerr << PACKAGE << error << std::endl;
		exit(EXIT_FAILURE);
	}

	/*----------------------------.
	| 2. Calculate final markings |
	`----------------------------*/


	int bound = args_info.bound_arg;

	SetOfPartialMarkings* fSet = SetOfPartialMarkings::create(&(net->finalCondition().formula()),bound);

	std::vector<std::pair<PartialMarking*,ExtendedStateEquation*> > systems;

	for (std::vector<PartialMarking*>::iterator finalMarkingIt = fSet->partialMarkings.begin();
	finalMarkingIt != fSet->partialMarkings.end();
	++finalMarkingIt) {
		ExtendedStateEquation* XSE = new ExtendedStateEquation(net,(*finalMarkingIt));
		if (XSE->constructLP()) {
			systems.push_back(std::pair<PartialMarking*,ExtendedStateEquation*>(*finalMarkingIt,XSE));
		}
	}




	if (args_info.level_0_flag) {

		std::cout << "\nLevel 0 message profile:" << std::endl;


		/*-----------------------------------.
		| Calculate and flatten event terms. |
		`-----------------------------------*/

		vector<EventTerm*>* etermvec = EventTerm::createBasicTermSet(net);

		for (std::set<pnapi::Place*>::iterator it = net->getInterfacePlaces().begin(); it != net->getInterfacePlaces().end(); ++it) {
			for (std::set<pnapi::Place*>::iterator it2 = net->getInterfacePlaces().begin(); it2 != net->getInterfacePlaces().end(); ++it2) {

				if (*it == *it2) continue;

				BasicTerm* b1 = new BasicTerm((*it));
				BasicTerm* b2 = new BasicTerm((*it2));

				AddTerm* a = new AddTerm(b1,b2);

				etermvec->push_back(a);
			}
		}


		/*-----------------------------------------------------------.
		| For each REACHABLE final marking evaluate the event terms. |
		`-----------------------------------------------------------*/

		for (std::vector<std::pair<PartialMarking*,ExtendedStateEquation*> >::iterator systemsIt = systems.begin();
		systemsIt != systems.end();
		++systemsIt) {

			std::cout << "Final marking: ";

			(*systemsIt).first->output();

			for (vector<EventTerm*>::iterator it = etermvec->begin(); it != etermvec->end(); ++it) {
				(*systemsIt).second->evaluate((*it));
			}


		}

	}


	if (args_info.file_given) {

		et_yylineno = 1;
		et_yydebug = 0;
		et_yy_flex_debug = 0;

		et_yyin = fopen(args_info.file_arg, "r");
		if (!et_yyin) {
			std::cerr << "cannot open ET file '" << args_info.file_arg << "' for reading'\n" << std::endl;
		}
		term_vec = new std::vector<EventTerm*>();

		EventTerm::events.clear();
		for (set<pnapi::Place *>::iterator it = net->getInterfacePlaces().begin(); it != net->getInterfacePlaces().end(); ++it) {
			(EventTerm::events)[(*it)->getName()] = (*it);
		}

		et_yyparse();
		fclose(et_yyin);

		for (std::vector<std::pair<PartialMarking*,ExtendedStateEquation*> >::iterator systemsIt = systems.begin();
		systemsIt != systems.end();
		++systemsIt) {

			std::cout << "Final marking: ";

			(*systemsIt).first->output();

			for (std::vector<EventTerm*>::iterator it = term_vec->begin(); it != term_vec->end(); ++it) {
				(*systemsIt).second->evaluate((*it));
			}
		}

	}


	if (args_info.random_given) {

		std::cout << "\nRandom message profile (" <<  args_info.random_arg <<" terms):" << std::endl;

		srand(time(NULL));

		int number = args_info.random_arg;

		vector<EventTerm*>* randomvec = new vector<EventTerm*>();

		for (int i = 0; i < number; ++i) {
			randomvec->push_back(EventTerm::createRandomEventTerm(net));
		}

		for (std::vector<std::pair<PartialMarking*,ExtendedStateEquation*> >::iterator systemsIt = systems.begin();
		systemsIt != systems.end();
		++systemsIt) {

			std::cout << "Final marking: ";

			(*systemsIt).first->output();

			for (vector<EventTerm*>::iterator it = randomvec->begin(); it != randomvec->end(); ++it) {
				(*systemsIt).second->evaluate((*it));
			}

		}




	}


	if (args_info.interactive_flag) {

		stop_interaction = false;

		while (!stop_interaction) {

				std::cerr << "Enter a number of terms, seperated by <;> or <:q> to quit the interaction mode." << std::endl;

				et_yylineno = 1;
				et_yydebug = 0;
				et_yy_flex_debug = 0;

				et_yyin = stdin;

				delete term_vec;

				term_vec = new std::vector<EventTerm*>();

				EventTerm::events.clear();
				for (set<pnapi::Place *>::iterator it = net->getInterfacePlaces().begin(); it != net->getInterfacePlaces().end(); ++it) {
					(EventTerm::events)[(*it)->getName()] = (*it);
				}

				et_yyparse();

				if (term_vec->size() == 0) {
					continue;
				}

				for (std::vector<std::pair<PartialMarking*,ExtendedStateEquation*> >::iterator systemsIt = systems.begin();
				systemsIt != systems.end();
				++systemsIt) {

					std::cout << "Final marking: ";

					(*systemsIt).first->output();

					for (std::vector<EventTerm*>::iterator it = term_vec->begin(); it != term_vec->end(); ++it) {
						(*systemsIt).second->evaluate((*it));
					}
				}

		}

	}

	time(&end_time);

	if (args_info.verbose_flag) {
		std::cerr << "\n" << PACKAGE << ": calculation took " << difftime( end_time,start_time) << " seconds" << std::endl << std::endl;
	}

}
