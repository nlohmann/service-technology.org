#include "config.h"
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <sstream>
#include <cstdarg>
//#include <libgen.h>
#include <fstream>
#include <string>

#include "cmdline.h"
#include "helpers.h"
#include "lindaAgent.h"
#include "staticAnalysis.h"
#include "eventTerm.h"
#include "stateEquation.h"
#include "eventTermParser.h"
#include "Output.h"
#include "verbose.h"
//#include "costProfileParser.h"
//#include "requestParser.h"
#include "costs.h"

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

	if (args_info.costs_given && !(args_info.costprofile_given && args_info.request_given)) {
		abort(2, "In cost mode, both the 'costprofile' and 'request' parameters are obligatory.");
	}
	
	// check whether at least one file is given
	if (args_info.inputs_num != 1) {
		abort(2, "Exactly one input file (open net) must be given.");
	}

	free(params);
}

/// the parser vector
std::vector<EventTermConstraint*>* constraint_vec = 0;
bool stop_interaction = false;

extern int etc_yylineno;
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

	std::set<pnapi::Label *> outlabels = net->getInterface().getOutputLabels();

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
		<<  LindaAgent::getFinalMarkingString(i);
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




/// Pretty output of a message profile.
void prettyPrint(ExtendedStateEquation** systems, uint8_t nr) {
	std::string res = "Fingerprint: ";

	bool firstSystem = true;

	
	for (int syscount = 0; syscount < nr; ++syscount) {
	bool firstTerm = true;
		if (!firstSystem) {
			res += " OR ";
		} else {
			firstSystem = false;
		}
	res += "(";	
	for (int termcount = 0; termcount < systems[syscount]->calculatedEventTerms.size(); ++termcount) {
			if (!(systems[syscount]->calculatedBounds[termcount]->lowerBounded || systems[syscount]->calculatedBounds[termcount]->upperBounded)) {
				continue;
			}			

			if (!firstTerm) {
				res += " AND ";
			} else {
				firstTerm = false;
			}
			res += "(";
			if (systems[syscount]->calculatedBounds[termcount]->lowerBounded) {
				res += systems[syscount]->calculatedBounds[termcount]->getLowerBoundString() + " <= " ;
			}
			bool firstEvent = true;
			for (int i = 0; i < LindaHelpers::NR_OF_EVENTS; ++i) {
				if ((systems[syscount]->calculatedEventTerms[termcount])[i] == 0) {
					continue;
				}
				if ((systems[syscount]->calculatedEventTerms[termcount])[i] > 0 && !firstEvent) {res += "+";}
				if ((systems[syscount]->calculatedEventTerms[termcount])[i] == 1) {
					res += LindaHelpers::EVENT_STRINGS[i];
				} else if ((systems[syscount]->calculatedEventTerms[termcount])[i] == -1) {
					res += "-" + LindaHelpers::EVENT_STRINGS[i];
				} else {
					res += LindaHelpers::intToStr((systems[syscount]->calculatedEventTerms[termcount])[i]) + "*" + LindaHelpers::EVENT_STRINGS[i];
				}
				firstEvent = false;
			}
			if (systems[syscount]->calculatedBounds[termcount]->upperBounded) {
				res += " <= " + systems[syscount]->calculatedBounds[termcount]->getUpperBoundString() ;
			}
			res += ")";
		
		}
	res += ")";
	}
	message(res.c_str());
}

	


int main(int argc, char** argv) {
	bool dirty = false; // Setting this bit means that linda should exit with an exit code != 0, but should be further executed.



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


	if (!args_info.costs_given) {
		
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
			abort(3, "pnapi error %i", inputerror.str().c_str());
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
				LindaAgent::getFinalMarkingString(x).c_str());

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
				LindaAgent::getFinalMarkingString(i).c_str());

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
				LindaAgent::getFinalMarkingString(x).c_str());

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
				LindaAgent::getFinalMarkingString(x).c_str());
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
				LindaAgent::getFinalMarkingString(x).c_str());

				
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
				LindaAgent::getFinalMarkingString(i).c_str());

				
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

			etc_yyin = fopen(args_info.constraint_file_arg, "r");

			// If the file can not be read we warn the user and cancel the mode.
			if (!etc_yyin) {
				abort(3, "cannot open ETC file '%s' for reading!",
				args_info.constraint_file_arg);
			} else {

				constraint_vec = new std::vector<EventTermConstraint*>();

				etc_yyparse();
				fclose(etc_yyin);

				// For each system...
				for (int i = 0; i <sysCounter; ++i) {
					status("    Processing final marking: %s",
					LindaAgent::getFinalMarkingString(i).c_str());

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

		// PRETTY PRINT

		if (args_info.pretty_given) {
			prettyPrint(LindaAgent::getSystemsArray(), sysCounter);			
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
	} else { // The cost parameter was given
		
		// parse the open net
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
			abort(3, "pnapi error %i", inputerror.str().c_str());
		}	

		// build helpers

		CostAgent::places = new std::vector<pnapi::Place*>();
		CostAgent::initialMarking = new int[net->getPlaces().size()];
		for (std::set<pnapi::Place*>::iterator placeIt =  net->getPlaces().begin(); placeIt != net->getPlaces().end(); ++placeIt) {
			CostAgent::places->push_back(*placeIt);
			CostAgent::initialMarking[CostAgent::places->size()-1] = (int) (*placeIt)->getTokenCount();
		}

		CostAgent::transitions = new std::vector<pnapi::Transition*>(1);
		for (std::set<pnapi::Transition*>::iterator transitionIt =  net->getTransitions().begin(); transitionIt != net->getTransitions().end(); ++transitionIt) {
			CostAgent::transitions->push_back(*transitionIt);
		}
		
		std::set<pnapi::Label *> thelabels = net->getInterface().getAsynchronousLabels();
		CostAgent::labels = new std::vector<pnapi::Label*>();
		for (std::set<pnapi::Label*>::iterator labelIt = thelabels.begin(); labelIt != thelabels.end(); ++labelIt) {
			CostAgent::labels->push_back(*labelIt);
		}

		// parse cost profile
		extern CostProfile* profile;

	if (strncmp(args_info.costprofile_arg,"random",6) != 0) {
			status("Parsing cost profile `%s'", args_info.costprofile_arg);
			extern FILE * profile_yyin;
			if ( not (profile_yyin = fopen(args_info.costprofile_arg, "r")))
			{
				status("File not found: %s", args_info.costprofile_arg);
				exit(1);
			}

			extern int profile_yyparse();
			profile_yyparse();
			fclose(profile_yyin);

		} else {
		
			status("Using random profiles");
			
			std::string random(args_info.costprofile_arg);
			std::string relevant = random.substr(6);
			
			int pos1 = 0;
			int pos2 = relevant.find(".",pos1) + 1;
			int len1 = pos2 - pos1 - 1;
			
			int nrOfUseCases = atoi(relevant.substr(pos1,len1).c_str());
			int nrOfConstraintsPerUseCase = atoi(relevant.substr(pos2).c_str());

			profile = new CostProfile;
			
			for (int i = 0; i < nrOfUseCases; ++i) {
			
				UseCase* usecase = new UseCase();
				
				// use case policy
				
				int lowerbound = std::rand() % 2;
				int upperbound = std::rand() % 2;
				
				usecase->policyLB = lowerbound == 1;
				usecase->policyRB = upperbound == 1;
				int vorzeichen = std::rand() % 2;
				
				usecase->policyLHS = (-1*vorzeichen) + (std::rand() % 1000);
				usecase->policyRHS = std::rand() % 1000 + usecase->policyLHS;
				
				usecase->fixCosts = std::rand() % 100;
				
				for (int j = 0; j < CostAgent::places->size(); ++j) {
					usecase->marking[j] = std::rand() % 3;
				}
				
				for (int k = 0; k < nrOfConstraintsPerUseCase; ++k) {
						ElementalConstraint* e = new ElementalConstraint(CostAgent::transitions->size());
												
						for (int c = 1; c < CostAgent::transitions->size(); ++c) {
							int vorzeichen = std::rand() % 2;
							(e->lhs)[c] = (- vorzeichen) * (std::rand() % 10);
							vorzeichen = std::rand() % 2;
							e->rhs = (- vorzeichen) * (std::rand() % 10);
							e->sign = (std::rand() % 4) + 1;
						}
	
						usecase->constraint->push_back(e);
	
	
				}
	
					usecase->variableCosts = CostAgent::getCleanDoubleArray(CostAgent::transitions->size());
					for (int k = 1; k < CostAgent::transitions->size(); ++k) {
						int vorzeichen = std::rand() % 2;
						usecase->variableCosts[k] = (- vorzeichen) * (std::rand() % 100);
					}
	
				profile->usecases->push_back(usecase);
			}
		
		}
		// parse request
		extern Request* request;
		std::vector<Request*> requests;
		
		if (strncmp(args_info.request_arg,"random",6) != 0) {
			status("Parsing request `%s'", args_info.request_arg);
			extern FILE * request_yyin;
			if ( not (request_yyin = fopen(args_info.request_arg, "r")))
			{
				status("File not found: %s", args_info.request_arg);
				exit(1);
			}

			extern int request_yyparse();
			request_yyparse();
			fclose(request_yyin);
	
			requests.push_back(request);
			
		} else {
		
			status("Using random requests");
			
			std::string random(args_info.request_arg);
			std::string relevant = random.substr(6);
			
			
			int pos1 = 0;
			int pos2 = relevant.find(".",pos1) + 1;
			int pos3 = relevant.find(".",pos2) + 1;
			int pos4 = relevant.find(".",pos3) + 1;
			int pos5 = relevant.find(".",pos4) + 1;
			
			int len1 = pos2 - pos1 - 1;
			int len2 = pos3 - pos2 - 1;
			int len3 = pos4 - pos3 - 1;
			int len4 = pos5 - pos4 - 1;
			
			int nrOfRandomRequests = atoi(relevant.substr(pos1,len1).c_str());
			int nrOfAssertionsPerRequest = atoi(relevant.substr(pos2,len2).c_str());
			int nrOfConstraintsPerAssertion = atoi(relevant.substr(pos3,len3).c_str());
			int nrOfGrantsPerRequest = atoi(relevant.substr(pos4,len4).c_str());
			int nrOfConstraintsPerGrant = atoi(relevant.substr(pos5).c_str());
			
//			status("Nr of random requests: %i with each %i grants having %i constraints each.", nrOfRandomRequests, nrOfGrantsPerRequest, nrOfConstraintsPerGrant);
			
			for (int i = 0; i < nrOfRandomRequests; ++i) {
			
				request = new Request();
				
				request->assertions = new std::vector<std::vector<ElementalConstraint*>* >(); // A disjunction (outer vector) of conjunctions (inner vectors).
				
				for (int j = 0; j < nrOfAssertionsPerRequest; ++j) {
				
					std::vector<ElementalConstraint*>* assertion = new std::vector<ElementalConstraint*>;
					
					for (int k = 0; k < nrOfConstraintsPerAssertion; ++k) {
						ElementalConstraint* e = new ElementalConstraint(CostAgent::transitions->size());
												
						for (int c = 1; c < CostAgent::transitions->size(); ++c) {
//							std::cerr << "here";
							int vorzeichen = std::rand() % 2;
							(e->lhs)[c] = (- vorzeichen) * (std::rand() % 10);
							vorzeichen = std::rand() % 2;
							e->rhs = (- vorzeichen) * (std::rand() % 10);
							e->sign = (std::rand() % 4) + 1;
						}
						
						assertion->push_back(e);

					}
					
					request->assertions->push_back(assertion);
				
				}
				
				request->grants = new std::vector<Grant*>(); 

				for (int j = 0; j < nrOfGrantsPerRequest; ++j) {
					
					Grant* g = new Grant();
					delete g->variableCosts;
					g->variableCosts = CostAgent::getCleanDoubleArray(CostAgent::transitions->size());
					for (int k = 1; k < CostAgent::transitions->size(); ++k) {
						int vorzeichen = std::rand() % 2;
						g->variableCosts[k] = (- vorzeichen) * (std::rand() % 100);
					}
					int vorzeichen = std::rand() % 2;
					g->fixCosts = (- vorzeichen) * (std::rand() % 100);

					for (int x = 0; x < nrOfConstraintsPerGrant; ++x) {
						
						ElementalConstraint* e = new ElementalConstraint(CostAgent::transitions->size());
												
						for (int c = 1; c < CostAgent::transitions->size(); ++c) {
//							std::cerr << "here";
							vorzeichen = std::rand() % 2;
							(e->lhs)[c] = (- vorzeichen) * (std::rand() % 10);
							vorzeichen = std::rand() % 2;
							e->rhs = (- vorzeichen) * (std::rand() % 10);
							e->sign = (std::rand() % 4) + 1;
						}

						
						
						g->constraint->push_back(e);
					
					}
					
					request->grants->push_back(g);
					
				
				}
				
				requests.push_back(request);
			
			}
			
		}

		
		CostAgent::buildBasicStateEquation();	

		for (int reqcounter = 0; reqcounter < requests.size(); ++reqcounter) {
		
		request = requests[reqcounter];
		
		if (request->assertions->size() == 0) {
			ElementalConstraint* dummy = new ElementalConstraint(CostAgent::transitions->size());
			request->assertions->push_back(new std::vector<ElementalConstraint*>);
			(*request->assertions)[0]->push_back(dummy);
		}
		
		
		// for each use case in the cost profile...
		for (int usecaseCounter = 0; usecaseCounter < profile->usecases->size(); ++usecaseCounter) {
		
		status("Use case nr %i",usecaseCounter);
		
		CostAgent::buildStateEquationWithUseCase((*profile->usecases)[usecaseCounter]);	
			
			// for each conjunctive clause in the assertions-section of the request
		
			for (int clauseCounter = 0; clauseCounter < request->assertions->size(); ++clauseCounter) {
				std::vector<ElementalConstraint*>* clause = (*request->assertions)[clauseCounter];
				// augment state equation with conjunctive clause
				status("    Clause nr %i",clauseCounter);
				CostAgent::buildStateEquationWithAssertions((*profile->usecases)[usecaseCounter], clause);
				
				// for each grant			
				
				for (int grantCounter = 0; grantCounter < request->grants->size(); ++grantCounter) {
				status("        Grant nr %i", grantCounter);
				// augment state equation with grant-constraint
				CostAgent::buildStateEquationWithGrant((*profile->usecases)[usecaseCounter],(*request->grants)[grantCounter]);
				// solve program with cost-function - payment-function
				bool b = CostAgent::checkPolicy((*profile->usecases)[usecaseCounter],(*request->grants)[grantCounter]);
				if (!b) {
					dirty = true;
					message("Policy check failed! (use case %i, clause %i, grant %i)",usecaseCounter, clauseCounter, grantCounter);
				}
			}
			}
			
			if (dirty) {
				message("At least one policy check failed!");
			}
			// done.
			
			
			
		}
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
	
	if (dirty) return 1;
	
}
