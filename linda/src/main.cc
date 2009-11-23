#include "config.h"
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <sstream>
#include "cmdline.h"
#include "helpers.h"
#include "setsOfFinalMarkings.h"
#include "eventTerm.h"
#include "stateEquation.h"
#include "files.h"
#include "eventTermParser.h"

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

	if (args_info.about_given) {
		LindaHelpers::printAsciiArtImage();
		exit(0);
	}

	// check whether at least one file is given
	if (args_info.inputs_num != 1) {
		abort(1, "Exactly one input file (open net) must be given.");
	}

	free(params);
}

/// the parser vector
std::vector<EventTermConstraint*>* constraint_vec = 0;
bool stop_interaction = false;

extern int etc_yylineno;
extern int etc_yydebug;
extern int etc_yy_flex_debug;
extern FILE* etc_yyin;
extern int etc_yyerror();
extern int etc_yyparse();

#ifdef YY_FLEX_HAS_YYLEX_DESTROY
extern int etc_yylex_destroy();
#endif

int main(int argc, char** argv) {

	/*
	 * The Linda Workflow in short:
	 * ----------------------------
	 * Evaluate command line parameters
	 * Parse an Open Net
	 * If specified, create level 0 message profile.
	 * If specified, create message profile from file.
	 * If specified, create message profile from constraint file and try to decide compliance.
	 * If specified, create an output file that contains all computed terms/bounds.
	 */

	// Start the stopwatch
	clock_t start_clock = clock();

	// Evaluate parameters
	evaluateParameters(argc, argv);

	if (args_info.quiet_flag) {

		std::cerr.rdbuf(0);

		args_info.verbose_flag = 0;

	}

	// Parsing the open net, using the PNAPI
	message("Processing %s", args_info.inputs[0]);
	pnapi::PetriNet* net = new pnapi::PetriNet;

	try {

		// parse either from standard input or from a given file
		if (args_info.inputs_num == 0) {
			std::cin >> pnapi::io::owfn >> *(net);
		} else {
			assert(args_info.inputs_num == 1);
			std::ifstream inputStream;
			inputStream.open(args_info.inputs[0]);
			inputStream >> pnapi::io::owfn >> *(net);
			inputStream.close();
		}

		std::stringstream pnstats;
		pnstats << pnapi::io::stat << *(net);

		status("read net %s", pnstats.str().c_str());
	} catch (pnapi::io::InputError error) {
		std::stringstream inputerror;
		inputerror << error;
		abort(1, "pnapi error %i", inputerror.str().c_str());
	}

	// Create the global event IDs
	LindaHelpers::NR_OF_EVENTS = net->getInterfacePlaces().size();
	LindaHelpers::EVENT_STRINGS = new std::string[LindaHelpers::NR_OF_EVENTS]();
	LindaHelpers::EVENT_PLACES
	= new pnapi::Place*[LindaHelpers::NR_OF_EVENTS]();

	int counter = 0;
	for (std::set<pnapi::Place *>::iterator it =
		net->getInterfacePlaces().begin(); it
		!= net->getInterfacePlaces().end(); ++it) {
		LindaHelpers::EVENT_STRINGS[counter] = (*it)->getName();
		LindaHelpers::EVENT_PLACES[counter] = (*it);
		++counter;
	}






	// Calculate the final markings from the final condition

	int bound = args_info.bound_arg;

	SetOfPartialMarkings* fSet = SetOfPartialMarkings::create(net,
			&(net->finalCondition().formula()), bound);

	ExtendedStateEquation** systems = new ExtendedStateEquation*[fSet->size()];

	status("Computed final markings:");
	for (std::vector<PartialMarking*>::iterator finalMarkingIt =
		fSet->partialMarkings.begin(); finalMarkingIt
		!= fSet->partialMarkings.end(); ++finalMarkingIt) {
		status("    %s", (*finalMarkingIt)->toString().c_str());
	}

	// Create the linear program for each final marking

	for (int i = 0; i < fSet->size(); ++i) {
		// Create a new state equation for this final marking
		ExtendedStateEquation* XSE = new ExtendedStateEquation(net,fSet->partialMarkings[i]);
		if (XSE->constructLP()) {
			systems[i] = XSE;

			// If flag is on, output the lps.

			if (args_info.show_lp_flag) {
				message("    Processing final marking: %s",
						fSet->partialMarkings[i]->toString().c_str());
				XSE->output();
			}
		} else {
			abort(1, "Construction of a system failed!");

		}
	}

	// Cached incidence matrix is not needed anymore
	delete ExtendedStateEquation::lines;
	delete[] ExtendedStateEquation::eventLines;
	delete ExtendedStateEquation::transitionID;

	// Verbose output of the number of linear programs created
	status("Number of lp systems: %i", fSet->size());

	if (args_info.structure_flag) {
		// Calculate t-invariants
		FlowMatrix f(net);
		f.computeTInvariants();
		f.output();
		f.createTerms();

		// For each system, evaluate all event terms
		for (int i = 0; i < fSet->size(); ++i) {

			status("    Processing final marking: %s",
					fSet->partialMarkings[i]->toString().c_str());

			ListElement<EventTerm*>* currentTerm = f.terms;
			while (currentTerm != 0) {
				EventTermBound* b = systems[i]->evaluate(currentTerm->element);
				currentTerm = currentTerm->next;
			}

		}
	}




	// MODE Level 0 Message Profile
	// We evaluate a term (1*a) for each event (a).
	if (args_info.level_0_flag) {
		message("Evaluating basic terms.");
		status("Number of basic terms: %i", LindaHelpers::NR_OF_EVENTS);

		// Iterate over the finalmarkings/lps
		for (int x = 0; x < fSet->size(); ++x) {

			status("    Processing final marking: %s",
					fSet->partialMarkings[x]->toString().c_str());

			// For each event, we create a term and evaluate it
			for (int i = 0; i < LindaHelpers::NR_OF_EVENTS; ++i) {
				BasicTerm* bt = new BasicTerm(i);
				EventTermBound* b = systems[x]->evaluate(bt);
			}

		}

	}

	// MODE Level 1 Message Profile
	// We evaluate a term (1*a + 1*b) for any two events a,b with a != b.
	if (args_info.level_1_flag) {
		message("Evaluating mutual exclusion terms.");
		status("Number of mutual exclsion terms: %i",
				(LindaHelpers::NR_OF_EVENTS * LindaHelpers::NR_OF_EVENTS)
				- LindaHelpers::NR_OF_EVENTS);

		// Iterate over the finalmarkings/lps
		for (int x = 0; x < fSet->size(); ++x) {

			status("    Processing final marking: %s",
					fSet->partialMarkings[x]->toString().c_str());

			// For each event, we create a term and evaluate it
			for (int i = 0; i < LindaHelpers::NR_OF_EVENTS; ++i) {
				BasicTerm* bt1 = new BasicTerm(i);
				for (int j = i + 1; j < LindaHelpers::NR_OF_EVENTS; ++j) {
					BasicTerm* bt2 = new BasicTerm(j);
					AddTerm* at = new AddTerm(bt1, bt2);
					EventTermBound* b = systems[x]->evaluate(at);
					if (b->lowerBounded && b->upperBounded && b->lowerBound
							<= 1) {
						if (b->upperBound == 1) {
							status(
									"        Events '%s' and '%s' are mutual exclusive.",
									LindaHelpers::EVENT_STRINGS[i].c_str(),
									LindaHelpers::EVENT_STRINGS[j].c_str());
						} else if (b->upperBound == 0) {
							status(
									"        Events '%s' and '%s' do not occur.",
									LindaHelpers::EVENT_STRINGS[i].c_str(),
									LindaHelpers::EVENT_STRINGS[j].c_str());
						}
					}
				}
			}

		}

	}

	// MODE Event term file
	// We read a file that contains event terms and evaluate them
	if (args_info.file_given) {

		message("Evaluating terms from file <%s>", args_info.file_arg);

		// Initialize Parser.
		EventTermParser::initialize();
		vector<EventTerm*>* term_vec = EventTermParser::parseFile(std::string(args_info.file_arg));

		// For each system, evaluate all event terms
		for (int i = 0; i < fSet->size(); ++i) {

			status("    Processing final marking: %s",
					fSet->partialMarkings[i]->toString().c_str());

			// For each parsed event term...
			for (std::vector<EventTerm*>::iterator it = term_vec->begin(); it
			!= term_vec->end(); ++it) {
				EventTermBound* b = systems[i]->evaluate((*it));
			}
		}

	}

	// MODE Constraint file
	// A constraint file contains event terms with bounds.
	// We evaluate the terms and then try to decide if the given bounds are correct.
	if (args_info.constraint_file_given) {

		message("Evaluating constraints from file <%s>",
				args_info.constraint_file_arg);

		// Initialize parser
		etc_yylineno = 1;
		etc_yydebug = 0;
		etc_yy_flex_debug = 0;

		etc_yyin = fopen(args_info.constraint_file_arg, "r");

		// If the file can not be read we warn the user and cancel the mode.
		if (!etc_yyin) {
			abort(1, "cannot open ETC file '%s' for reading!",
					args_info.constraint_file_arg);
		} else {

			constraint_vec = new std::vector<EventTermConstraint*>();

			etc_yyparse();
			fclose(etc_yyin);

			// For each system...
			for (int i = 0; i < fSet->size(); ++i) {

				message("    Processing final marking: %s",
						fSet->partialMarkings[i]->toString().c_str());

				bool thereWereMaybes = false;

				// For each term...
				for (std::vector<EventTermConstraint*>::iterator it =
					constraint_vec->begin(); it != constraint_vec->end(); ++it) {
					// Evaluate the term
					EventTermBound* b = systems[i]->evaluate(
							(*it)->getEventTerm());

					// Try to decide if the given bounds are correct or not
					if ((*it)->holds(b) == EventTermConstraint::is_false) {
						message("        Constraint %s falsified",
								(*it)->toString().c_str());
					} else if ((*it)->holds(b) == EventTermConstraint::is_true) {
						message("        Constraint %s verified",
								(*it)->toString().c_str());
					} else if ((*it)->holds(b) == EventTermConstraint::is_maybe) {
						status(
								"        Constraint %s could neither be verified nor falsified",
								(*it)->toString().c_str());
						thereWereMaybes = true;
					}

				}

				// Print out the overall result.
				if (thereWereMaybes) {
					message(
							"        Some constraints could neither be verified nor falsified.");
				}
			}

			delete constraint_vec;

		}
	}

	// OUTPUT FILE GENERATION

	if (args_info.output_given) {

		if (std::string(args_info.output_arg) != std::string("-")) {
			// Initialize stream
			std::ofstream file;
			file.open(args_info.output_arg);

			// Create the file and output it to the stream
			ProfileFile* outputFile = new ProfileFile(systems, net,fSet->size());
			outputFile->output(file, args_info.show_terms_as_given_flag);

			// Close the file
			file.close();
			delete outputFile;

			// Creation complete.
			message("Output file %s created.", args_info.output_arg);

		} else {

			// Create the file and output it to the stream
			ProfileFile* outputFile = new ProfileFile(systems, net,fSet->size());
			outputFile->output(std::cout, args_info.show_terms_as_given_flag);
			delete outputFile;

			std::cout << std::flush;

			// Output complete.
			message("Output written to standard out.");
		}
	}
	// Verbose output the result of the stop watch
	status("runtime: %.2f sec", ((double(clock()) - double(start_clock))
			/ CLOCKS_PER_SEC));
	if (args_info.verbose_given) {
		fprintf(stderr, "%s: memory consumption: ", PACKAGE);
		system(
				(std::string("ps | ") + TOOL_GREP + " " + PACKAGE + " | "
						+ TOOL_AWK
						+ " '{ if ($1 > max) max = $1 } END { print max \" KB\" }' 1>&2").c_str());
	}
}
