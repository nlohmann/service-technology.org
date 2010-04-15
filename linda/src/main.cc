#include "config.h"
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <sstream>
#include "cmdline.h"
#include "helpers.h"
#include "lindaAgent.h"
#include "staticAnalysis.h"
#include "setsOfFinalMarkings.h"
#include "eventTerm.h"
#include "stateEquation.h"
#include "eventTermParser.h"
#include "Output.h"
#include "verbose.h"

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <libgen.h>
#include <fstream>
#include <sstream>
#include <string>

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


/// Output a message profile.
void output(std::ostream& file, ExtendedStateEquation** systems, pnapi::PetriNet* net, uint8_t nr) {

	file << "PLACE";
	file << std::endl << "  INTERNAL" << std::endl << "    ";

	bool first = true;

	for (std::set<pnapi::Place *>::iterator it =
			net->getPlaces().begin(); it
			!= net->getPlaces().end(); ++it) {

		if (first) {
			first = false;
		} else {
			file << ",";
		}

		file << (*it)->getName();

	}

	file << ";";

	file << std::endl << "  INPUT";

	first = true;

	std::set<pnapi::Label *> inlabels = net->getInterface().getInputLabels();

	for (std::set<pnapi::Label *>::iterator it = inlabels.begin(); it
			!= inlabels.end(); ++it) {

		if (first) {
			first = false;
		} else {
			file << ",";
		}

		file << std::endl << "    ";

		file << (*it)->getName();

	}

	
	
	file << ";";
	file << std::endl << "  OUTPUT";

	first = true;

	std::set<pnapi::Label *> outlabels = net->getInterface().getInputLabels();

	for (std::set<pnapi::Label *>::iterator it = outlabels.begin(); it
			!= outlabels.end(); ++it) {

		if (first) {
			first = false;
		} else {
			file << ",";
		}

		file << std::endl << "    ";

		file << (*it)->getName();

	}


	file << ";";

	file << std::endl << std::endl << "FINALMARKINGS";

	for (int i = 0; i < nr; ++i) {
		file << std::endl << "    F" << i+1 << ": "
				<< systems[i]->omega->toString();
	}

	int counter = 1;

	file << std::endl << std::endl << "TERMS";

	for (std::vector<int*>::iterator termsIt =
			systems[0]->calculatedEventTerms.begin(); termsIt
			!= systems[0]->calculatedEventTerms.end(); ++termsIt) {
		file << std::endl << "    T" << counter << ": ";
		for (int i = 0; i < LindaHelpers::NR_OF_EVENTS; ++i) {
			if ((*termsIt)[i] == 0) {
				continue;
			}

			if ((*termsIt)[i] > 0) {
				file << "+";
			}
			file << (*termsIt)[i] << "*" << LindaHelpers::EVENT_STRINGS[i] << " ";
		}
		file << ";";
		++counter;

	}

	file << std::endl << std::endl << "BOUNDS";
	int tCounter = 1;
	int fCounter = 1;

	for (int i = 0; i < nr; ++i) {

		for (std::vector<EventTermBound*>::iterator termsIt =
				systems[i]->calculatedBounds.begin(); termsIt
				!= systems[i]->calculatedBounds.end(); ++termsIt) {

			if ((*termsIt)->upperBounded || (*termsIt)->lowerBounded) {

			file << std::endl << "    F" << fCounter << ",T" << tCounter
					<< ": ";
			file << (*termsIt)->getLowerBoundString() << ","
					<< (*termsIt)->getUpperBoundString() << ";";

			}

			++tCounter;
		}

		tCounter = 1;
		++fCounter;
	}

	file << std::endl;

}


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
    Output::setTempfileTemplate(args_info.tmpfile_arg);
    Output::setKeepTempfiles(args_info.noClean_flag);


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
	} catch (pnapi::exception::InputError error) {
		std::stringstream inputerror;
		inputerror << error;
		abort(1, "pnapi error %i", inputerror.str().c_str());
	}




	// Create the linear program for each final marking

	LindaAgent::initialize(net, args_info.output_given);

	LindaAgent::addFinalMarkingsFromFinalCondition(args_info.bound_arg);

	unsigned int sysCounter = LindaAgent::getNumberOfSystems();

	// Verbose output of the number of linear programs created
	status("Number of lp systems: %i", sysCounter);

	if (sysCounter == 0) {
		abort(1,"No final markings are reachable from the initial marking.");
	}

	// MODE Level 0 Message Profile
	// We evaluate a term (1*a) for each event (a).
	EventTermBound*** level0 = 0;
	if (args_info.level_0_flag) {
		message("Evaluating basic terms.");
		status("Number of basic terms: %i", LindaHelpers::NR_OF_EVENTS);

		level0 = new EventTermBound**[sysCounter];

		// Iterate over the finalmarkings/lps
		for (int x = 0; x < sysCounter; ++x) {

			status("    Processing final marking: %s",
					LindaAgent::getFinalMarking(x)->toString().c_str());

			level0[x] = new EventTermBound*[LindaHelpers::NR_OF_EVENTS];


			// For each event, we create a term and evaluate it
			for (int i = 0; i < LindaHelpers::NR_OF_EVENTS; ++i) {
				BasicTerm* bt = new BasicTerm(i);
				level0[x][i] = LindaAgent::getSystem(x)->evaluate(bt);
			}

		}

	}


	// MODE Structural Analysis
	// For each found t-invariant, we check a number of terms.
	if (args_info.structure_flag) {
		// Calculate t-invariants
		FlowMatrix f(net);
		f.computeTInvariants();
		//		f.output();
		f.createTerms(level0);

		// For each system, evaluate all event terms
		for (int i = 0; i <sysCounter; ++i) {

			status("    Processing final marking: %s",
					LindaAgent::getFinalMarking(i)->toString().c_str());

			ListElement<int*>* currentTerm = f.terms;
			while (currentTerm != 0) {
				EventTermBound* b = LindaAgent::getSystem(i)->evaluate(currentTerm->element);
				currentTerm = currentTerm->next;
			}

		}

	}


	// MODE Level 1 Message Profile
	// We evaluate a term (1*a + 1*b) for any two events a,b with a != b.
	if (args_info.level_1_flag) {
		message("Evaluating level 1 terms.");

		// Iterate over the finalmarkings/lps
		for (int x = 0; x < sysCounter; ++x) {

			status("    Processing final marking: %s",
					LindaAgent::getFinalMarking(x)->toString().c_str());

			// For each event, we create a term and evaluate it
			for (int i = 0; i < LindaHelpers::NR_OF_EVENTS; ++i) {
				BasicTerm* bt1 = new BasicTerm(i);
				for (int j = i + 1; j < LindaHelpers::NR_OF_EVENTS; ++j) {



					BasicTerm* bt2 = new BasicTerm(j);
					AddTerm* at = new AddTerm(bt1, bt2);
					AddTerm* at2 = new AddTerm(bt1, new MultiplyTerm(bt2,-1));

					EventTermBound* b = LindaAgent::getSystem(x)->evaluate(at);
					EventTermBound* b2 = LindaAgent::getSystem(x)->evaluate(at2);
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


	// MODE Oct Message Profile
	if (args_info.oct_flag) {
		message("Evaluating octagon terms.");

		// Iterate over the finalmarkings/lps
		for (int x = 0; x < sysCounter; ++x) {

			status("    Processing final marking: %s",
					LindaAgent::getFinalMarking(x)->toString().c_str());

			// For each event, we create a term and evaluate it
			for (int i = 0; i < LindaHelpers::NR_OF_EVENTS; ++i) {
				BasicTerm* bt1 = new BasicTerm(i);
				
				EventTermBound* bsingle = LindaAgent::getSystem(x)->evaluate(bt1);
				
				for (int j = i + 1; j < LindaHelpers::NR_OF_EVENTS; ++j) {

					BasicTerm* bt2 = new BasicTerm(j);
					AddTerm* at1 = new AddTerm(bt1, bt2);
					AddTerm* at2 = new AddTerm(bt1, new MultiplyTerm(bt2,-1));

					EventTermBound* b1 = LindaAgent::getSystem(x)->evaluate(at1);
					EventTermBound* b2 = LindaAgent::getSystem(x)->evaluate(at2);
				}
			}

		}

	}

		
	// MODE DBM Message Profile
	if (args_info.dbm_flag) {
		message("Evaluating dbm terms.");
		status("Number of mutual exclsion terms: %i",
				(LindaHelpers::NR_OF_EVENTS * LindaHelpers::NR_OF_EVENTS)
				- LindaHelpers::NR_OF_EVENTS);

		// Iterate over the finalmarkings/lps
		for (int x = 0; x < sysCounter; ++x) {

			status("    Processing final marking: %s",
					LindaAgent::getFinalMarking(x)->toString().c_str());

			// For each event, we create a term and evaluate it
			for (int i = 0; i < LindaHelpers::NR_OF_EVENTS; ++i) {
				BasicTerm* bt1 = new BasicTerm(i);
				
				EventTermBound* bsingle = LindaAgent::getSystem(x)->evaluate(bt1);
				
				for (int j = i + 1; j < LindaHelpers::NR_OF_EVENTS; ++j) {

					BasicTerm* bt2 = new BasicTerm(j);
					AddTerm* at2 = new AddTerm(bt1, new MultiplyTerm(bt2,-1));

					EventTermBound* b2 = LindaAgent::getSystem(x)->evaluate(at2);
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
		std::vector<int*>* term_vec = EventTermParser::parseFile(std::string(args_info.file_arg));

		// For each system, evaluate all event terms
		for (int i = 0; i <sysCounter; ++i) {

			status("    Processing final marking: %s",
					LindaAgent::getFinalMarking(i)->toString().c_str());

			// For each parsed event term...
			for (std::vector<int*>::iterator it = term_vec->begin(); it
			!= term_vec->end(); ++it) {
				EventTermBound* b = LindaAgent::getSystem(i)->evaluate((*it));
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
			for (int i = 0; i <sysCounter; ++i) {

				message("    Processing final marking: %s",
						LindaAgent::getFinalMarking(i)->toString().c_str());

				bool thereWereMaybes = false;

				// For each term...
				for (std::vector<EventTermConstraint*>::iterator it =
					constraint_vec->begin(); it != constraint_vec->end(); ++it) {
					// Evaluate the term
					EventTermBound* b = LindaAgent::getSystem(i)->evaluate(
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

		{
			Output out( std::string(args_info.output_arg), "message profile" );
			output(out, LindaAgent::getSystemsArray(), net, sysCounter);
		}

		if (std::string(args_info.output_arg) != std::string("-")) {
			// Creation complete.
			message("Output file %s created.", args_info.output_arg);
		} else {
			// Output complete.
			message("Output written to standard out.");
		}

	} else {
		message("Computation finished. No output produced, since no output target was given.");
		message("To produce output to a file, use \"-o <path/to/file>\", to output to standard out, use \"-o-\". ");
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
