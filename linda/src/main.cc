#include "config.h"
#include <cassert>

#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <sstream>
#include "cmdline.h"
#include "helpers.h"
#include "lp_lib.h"
#include "setsOfFinalMarkings.h"
#include <typeinfo>
#include "eventTerm.h"
#include "stateEquation.h"
#include "files.h"

int NR_OF_EVENTS = 0;
std::string* EVENT_STRINGS = 0;
pnapi::Place** EVENT_PLACES = 0;
int GET_EVENT_ID(std::string* s) {
	for (int i = 0; i < NR_OF_EVENTS; ++i) {
		if (EVENT_STRINGS[i] == *s) {
			return i;
		}
	}
	return -1;
}


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
		fprintf(stderr, "%s: at most one input file (open net) must be given -- aborting\n", PACKAGE);
		exit(EXIT_FAILURE);
	}
	// check whether at least one file is given
	if (args_info.inputs_num != 1) {
		fprintf(stderr, "%s: Exactly one input file (open net) must be given -- aborting\n", PACKAGE);
		exit(EXIT_FAILURE);
	}

	free(params);
}

/// the parser vector
std::vector<EventTerm*>* term_vec = 0;
std::vector<EventTermConstraint*>* constraint_vec = 0;
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



extern int etc_yylineno;
extern int etc_yydebug;
extern int etc_yy_flex_debug;
extern FILE* etc_yyin;
extern int etc_yyerror();
extern int etc_yyparse();

#ifdef YY_FLEX_HAS_YYLEX_DESTROY
extern int etc_yylex_destroy();
#endif

void initialize_et_parser() {
	et_yylineno = 1;
	et_yydebug = 0;
	et_yy_flex_debug = 0;

}






int main(int argc, char** argv) {

	clock_t start_clock = clock();

	/*--------------------------------------.
	| 0. parse the command line parameters  |
	`--------------------------------------*/

	evaluateParameters(argc, argv);
	if (args_info.quiet_flag) {

		std::cerr.rdbuf(0);
		std::cout.rdbuf(0);

		args_info.verbose_flag = 0;

	}


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
		std::cerr << PACKAGE << ": " << error << std::endl;
		exit(EXIT_FAILURE);
	}

	/*
	 * Set up the event <-> nat mapping.
	 */

	NR_OF_EVENTS = net->getInterfacePlaces().size();
	EVENT_STRINGS = new std::string[NR_OF_EVENTS]();
	EVENT_PLACES = new pnapi::Place*[NR_OF_EVENTS]();

	int counter = 0;

	for (set<pnapi::Place *>::iterator it = net->getInterfacePlaces().begin(); it != net->getInterfacePlaces().end(); ++it) {
		EVENT_STRINGS[counter] = (*it)->getName();
		EVENT_PLACES[counter] = (*it);
		++counter;
	}

	/*----------------------------.
	| 2. Calculate final markings |
	`----------------------------*/

	int bound = args_info.bound_arg;

	SetOfPartialMarkings* fSet = SetOfPartialMarkings::create(net,&(net->finalCondition().formula()),bound);
	std::vector<std::pair<PartialMarking*,ExtendedStateEquation*> > systems;

	if (args_info.verbose_flag) {
		std::cerr << PACKAGE << ": Final markings:\n";
		for (std::vector<PartialMarking*>::iterator finalMarkingIt = fSet->partialMarkings.begin();
		finalMarkingIt != fSet->partialMarkings.end();
		++finalMarkingIt) {
			std::cerr << "    ";
			(*finalMarkingIt)->output(std::cerr);
		}
	}



	for (std::vector<PartialMarking*>::iterator finalMarkingIt = fSet->partialMarkings.begin();
	finalMarkingIt != fSet->partialMarkings.end();
	++finalMarkingIt) {
		ExtendedStateEquation* XSE = new ExtendedStateEquation(net,(*finalMarkingIt));
		if (XSE->constructLP()) {
			systems.push_back(std::pair<PartialMarking*,ExtendedStateEquation*>(*finalMarkingIt,XSE));

			// If flag is on, output the lps.

			if (args_info.show_lp_flag) {
				std::cout << "\nFinal marking:";
				(*finalMarkingIt)->output(std::cout);
				XSE->output();
			}
		} else {
			std::cerr << PACKAGE << ": WARNING: Construction of a system failed!" << std::endl;

		}
	}

	delete ExtendedStateEquation::lines;

	if (!args_info.output_given) { delete net; }

	if (args_info.verbose_flag) {
		std::cerr << PACKAGE << ": Number of lp systems: " << systems.size() << std::endl;
	}



	if (args_info.level_0_flag) {

		std::cerr << PACKAGE << ": Minimum and maximum occurence of single events." << std::endl;


		if (args_info.verbose_flag) {
			std::cerr << PACKAGE << ": Number of basic terms: " << NR_OF_EVENTS << std::endl;
		}

		for (std::vector<std::pair<PartialMarking*,ExtendedStateEquation*> >::iterator systemsIt = systems.begin();
		systemsIt != systems.end();
		++systemsIt) {

			std::cout << "\nFinal marking: ";

			(*systemsIt).first->output(std::cout);

			for (int i = 0; i < NR_OF_EVENTS; ++i) {
				BasicTerm* bt = new BasicTerm(i);
				EventTermBound* b = (*systemsIt).second->evaluate(bt);
				b->output(bt,args_info.show_terms_as_given_flag);
			}

		}

	}


	if (args_info.file_given) {

		std::cerr << PACKAGE << ": Event term file <" << args_info.file_arg << ">" << std::endl;

		initialize_et_parser();
		et_yyin = fopen(args_info.file_arg, "r");

		if (!et_yyin) {
			std::cerr << "cannot open ET file '" << args_info.file_arg << "' for reading'\n" << std::endl;
		}
		term_vec = new std::vector<EventTerm*>();

		et_yyparse();
		fclose(et_yyin);

		for (std::vector<std::pair<PartialMarking*,ExtendedStateEquation*> >::iterator systemsIt = systems.begin();
		systemsIt != systems.end();
		++systemsIt) {

			std::cout << "\nFinal marking: ";

			(*systemsIt).first->output(std::cout);

			for (std::vector<EventTerm*>::iterator it = term_vec->begin(); it != term_vec->end(); ++it) {
				EventTermBound* b = (*systemsIt).second->evaluate((*it));
				b->output(*it,args_info.show_terms_as_given_flag);
			}
		}

	}


	if (args_info.constraint_file_given) {

		std::cerr << PACKAGE << ": Constraint file <" << args_info.constraint_file_arg << ">" << std::endl;

		etc_yylineno = 1;
		etc_yydebug = 0;
		etc_yy_flex_debug = 0;

		etc_yyin = fopen(args_info.constraint_file_arg, "r");
		if (!etc_yyin) {
			std::cerr << "cannot open ETC file '" << args_info.constraint_file_arg << "' for reading'\n" << std::endl;
		}
		constraint_vec = new std::vector<EventTermConstraint*>();

		etc_yyparse();
		fclose(etc_yyin);

		for (std::vector<std::pair<PartialMarking*,ExtendedStateEquation*> >::iterator systemsIt = systems.begin();
		systemsIt != systems.end();
		++systemsIt) {

			std::cout << "\nFinal marking: ";

			(*systemsIt).first->output(std::cout);

			std::vector<EventTermConstraint*> maybes;
			int holds = EventTermConstraint::is_true;

			for (std::vector<EventTermConstraint*>::iterator it = constraint_vec->begin(); it != constraint_vec->end(); ++it) {
				EventTermBound* b = (*systemsIt).second->evaluate((*it)->getEventTerm());

				if ((*it)->holds(b) == EventTermConstraint::is_false) {
					std::cout << "\tConstraint " << (*it)->toString() << " violated." << std::endl;
					holds = holds*EventTermConstraint::is_false;
				} else if ((*it)->holds(b) == EventTermConstraint::is_true) {
					std::cout << "\tConstraint " << (*it)->toString() << " verified." << std::endl;
					holds = holds*EventTermConstraint::is_true;
				} else if ((*it)->holds(b) == EventTermConstraint::is_maybe) {
					std::cout << "\tConstraint " << (*it)->toString() << " could not be verified, but might hold." << std::endl;
					maybes.push_back((*it));
					holds = holds*EventTermConstraint::is_maybe;
				}

			}

			std::cout << "\n\n" << "Overall result:";

			if (holds == EventTermConstraint::is_true) {
				std::cout << "Verified.";
			} else if (holds == EventTermConstraint::is_false) {
				std::cout << "Violated.";
			} else if (holds == EventTermConstraint::is_maybe) {
				std::cout << "Verified iff all of the following constraints hold: \n";
				for (std::vector<EventTermConstraint*>::iterator it = maybes.begin(); it != maybes.end(); ++it) {
					std::cout  << "\t"<< (*it)->toString()<< "\n";
				}
			}
			std::cout << "\n\n";

		}

		delete constraint_vec;

	}




	if (args_info.interactive_flag) {

		std::cerr << PACKAGE << ": " << "Interactive mode." << std::endl;
		stop_interaction = false;

		while (!stop_interaction) {

			std::cerr << "Enter a number of terms, seperated by <;> or <:q> to quit the interaction mode." << std::endl;

			initialize_et_parser();

			et_yyin = stdin;

			delete term_vec;

			term_vec = new std::vector<EventTerm*>();


			et_yyparse();

			if (term_vec->size() == 0) {
				continue;
			}

			for (std::vector<std::pair<PartialMarking*,ExtendedStateEquation*> >::iterator systemsIt = systems.begin();
			systemsIt != systems.end();
			++systemsIt) {

				std::cout << "\nFinal marking: ";

				(*systemsIt).first->output(std::cout);

				for (std::vector<EventTerm*>::iterator it = term_vec->begin(); it != term_vec->end(); ++it) {
					EventTermBound* b = (*systemsIt).second->evaluate((*it));
					b->output(*it,args_info.show_terms_as_given_flag);

				}
			}

		}
		delete term_vec;
	}

	if (args_info.output_given) {

		std::ofstream file;
		file.open(args_info.output_arg);
		ProfileFile* outputFile = new ProfileFile(&systems,net);
		outputFile->output(file,args_info.show_terms_as_given_flag);

		file.close();
		delete outputFile;
		delete net;
		std::cerr << "\n" << PACKAGE << ": Output file " << args_info.output_arg << " created." << std::endl << std::endl;

	}

	if (args_info.verbose_flag) {
		std::cerr << PACKAGE << ": runtime: " << ((double(clock()) - double(start_clock)) / CLOCKS_PER_SEC) << " sec\n";
		std::cerr << PACKAGE << ": memory consumption: "; system((string("ps | ") + TOOL_GREP + " " + PACKAGE + " | " + TOOL_AWK + " '{ if ($1 > max) max = $1 } END { print max \" KB\" }' 1>&2").c_str());
	}

}
