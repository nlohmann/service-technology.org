#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <cassert>
#include <sstream>
#include "config.h"
#include "cmdline.h"
#include "pnapi.h"
#include "helpers.h"
#include "bounds.h"

/// the command line parameters
gengetopt_args_info args_info;

typedef set<pnapi::Place* >  pps;

typedef Bounds<pnapi::Place*> EventBound;
typedef Bounds<pps> EventSetBound;
//extern 	BOUND getBoundFromLPSolveString(std::string);


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
	std::cerr << "Processing: " << args_info.inputs[0] << std::endl;

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

	/*----------------------.
    | 2. Build up the LES   |
    `----------------------*/

	const pnapi::formula::Conjunction* finalmarking = dynamic_cast<const pnapi::formula::Conjunction*> (&(net->finalCondition().formula()));

	if (finalmarking == NULL) {

		std::cerr << "Finalmarking not recognizable." << std::endl;
		exit(EXIT_FAILURE);
	}

	const pnapi::Place* omegaPlaces[finalmarking->children().size()];
	int omegaTokens[finalmarking->children().size()];

	int om = 0;
	for (std::set<const pnapi::formula::Formula *>::iterator fIt = finalmarking->children().begin(); fIt != finalmarking->children().end(); ++fIt) {
		const pnapi::formula::Formula& f = (**fIt);
		const pnapi::formula::Proposition* p = dynamic_cast<const pnapi::formula::Proposition*> (&f);

		if (p == NULL) {

			std::cerr << "Finalmarking not recognizable." << std::endl;
			exit(EXIT_FAILURE);
		}

		const pnapi::Place* place = &(p->place());
		if (net->getInterfacePlaces().find(const_cast<pnapi::Place *>(place)) == net->getInterfacePlaces().end()) {
			omegaPlaces[om] = &(p->place());
			omegaTokens[om] = p->tokens();
			++om;
		}
	}


	string decls = "";
	std::ofstream out("tmp.stub");


	for (int counter = 0; counter < om; ++counter) {

		out << omegaPlaces[counter]->getName() << ": 0 ";

		std::set<pnapi::Arc *> preset = omegaPlaces[counter]->getPresetArcs();
		for (set<pnapi::Arc *>::iterator pIt = preset.begin(); pIt != preset.end();
		++pIt) {
			pnapi::Transition& t = (*pIt)->getTransition();
			out << " +" << (*pIt)->getWeight() << " * " << t.getName();
		}

		std::set<pnapi::Arc *> postset = omegaPlaces[counter]->getPostsetArcs();
		for (set<pnapi::Arc *>::iterator pIt = postset.begin(); pIt != postset.end(); ++pIt) {
			pnapi::Transition& t = (*pIt)->getTransition();
			out << " -" << (*pIt)->getWeight() << " * " << t.getName();
		}

		int val = omegaTokens[counter] - omegaPlaces[counter]->getTokenCount() ;
		out << " = ";
		out << val << ";\n";
	}

	for (set<pnapi::Transition *>::iterator it = net->getTransitions().begin(); it != net->getTransitions().end(); ++it) {
		decls += (*it)->getName() + ", ";
	}
	out.close();

	std::ofstream outDecl("tmp.decl");

	if (decls.length() > 0) {
		outDecl << "\nint " << decls.substr(0,decls.length()-2) << ";";
	}

	outDecl.close();

	/*---------------------------------.
    | 2. Call LP_SOLVE in variations   |
    `---------------------------------*/

	vector<EventBound> bounds, exacts;

//	string result = "";

#define MIN_CALL popen("cat tmp.obj tmp.stub tmp.events tmp.known tmp.decl | lp_solve -S1","r");
#define MAX_CALL popen("cat tmp.obj tmp.stub tmp.events tmp.known tmp.decl | lp_solve -S1 -max","r");

	system("rm -f tmp.known; touch tmp.known");
	system("rm -f tmp.events; touch tmp.events");

	for (set<pnapi::Place *>::iterator it = net->getInterfacePlaces().begin(); it != net->getInterfacePlaces().end(); ++it) {

		std::ofstream outObjective("tmp.obj");

		outObjective << "min: " << (*it)->getName() << ";" << std::endl;
		outObjective.close();

		out.open("tmp.events",std::ios::app);

		out << "Event_" << (*it)->getName() << ": 0 ";

		std::set<pnapi::Node *> preset = (*it)->getPreset();
		for (set<pnapi::Node *>::iterator pIt = preset.begin(); pIt != preset.end(); ++pIt) {
			pnapi::Transition* t = dynamic_cast< pnapi::Transition*>(*pIt);
			out << "+" << t->getName();
		}

		std::set<pnapi::Node *> postset = (*it)->getPostset();
		for (set<pnapi::Node *>::iterator pIt = postset.begin(); pIt != postset.end();	++pIt) {
			pnapi::Transition* t = dynamic_cast< pnapi::Transition*>(*pIt);
			out << "+" << t->getName();
		}

		out << " = " << (*it)->getName() << ";" << std::endl;

		out.close();

		FILE* solution;

		EventBound e(*it);

		solution = MIN_CALL;
		string t = getText(solution);
//		result += t + " <= " + (*it)->getName() + " <= ";
		e.min = getBoundFromLPSolveString(t);

		pclose(solution);

		solution = MAX_CALL;
			//popen("cat tmp.obj tmp.stub tmp.events tmp.known tmp.decl | lp_solve -S1 -max","r");
		t = getText(solution);
//		result += t + (char) 1;
		e.max = getBoundFromLPSolveString(t);

		pclose(solution);

		bool maxUnbounded = e.max != UNBOUNDED;
		bool minUnbounded = e.min != UNBOUNDED;

		if ((minUnbounded || maxUnbounded)) {
			std::ofstream outKnowledge("tmp.known",std::ios::app);
			if (minUnbounded) {
				outKnowledge << "Known_min_" << e.key->getName() << ": " << e.key->getName() << " >= " << e.min << ";\n";
			}
			if (maxUnbounded) {
				outKnowledge << "Known_max_" << e.key->getName() << ": " << e.key->getName() << " <= " << e.max << ";\n";
			}
			outKnowledge.close();
		}
		if (!e.isExact()) {
			bounds.push_back(e);
		} else {
			exacts.push_back(e);
		}
	}

//	cleanstring(result);

	vector<EventSetBound> eventSetBounds;
	set<set<int> > alreadyCalculated;
	for (int i = 0; i < bounds.size(); ++i) {
		EventBound& b1 = bounds.at(i);
		for (int j = 0; j < bounds.size(); ++j) {

			set<int> thisOne;
			thisOne.insert(i);
			thisOne.insert(j);

			if (i != j && (alreadyCalculated.find(thisOne) == alreadyCalculated.end())) {

			alreadyCalculated.insert(thisOne);

			EventBound& b2 = bounds.at(j);

			EventSetBound esb;

			esb.key.insert(b1.key);
			esb.key.insert(b2.key);

			std::ofstream outObjective("tmp.obj");
			outObjective << "min: " << b1.key->getName() << " + " << b2.key->getName() << ";" << std::endl;
			outObjective.close();

			FILE* solution;

			string t;

			if ((b1.min != UNBOUNDED && b2.min != UNBOUNDED)) {
				solution = MIN_CALL;
				t = getText(solution);
				esb.min = getBoundFromLPSolveString(t);
				pclose(solution);
			}

			if (b1.max != UNBOUNDED && b2.max != UNBOUNDED) {

				solution = MAX_CALL;
				t = getText(solution);
				esb.max = getBoundFromLPSolveString(t);

				pclose(solution);
			}

			if (esb.min != (b1.min + b2.min) || esb.max != (b1.max + b2.max)) {
				eventSetBounds.push_back(esb);
			}

			}

		}


	}



	time(&end_time);

//	std::cout << result << std::endl;

	for (int i = 0; i < exacts.size(); ++i) {
		EventBound& b = exacts.at(i);
		std::cerr << b.key->getName() << " = " << b.min << std::endl;
	}
	for (int i = 0; i < bounds.size(); ++i) {
		EventBound& b = bounds.at(i);
		std::cerr << b.min << " <= " << b.key->getName() << " <= " << b.max << std::endl;
	}

	for (int i = 0; i < eventSetBounds.size(); ++i) {
		EventSetBound& b = eventSetBounds.at(i);

		string s;
		for (pps::iterator it = b.key.begin(); it != b.key.end(); ++it) {
			s += (*it)->getName() + " + ";
		}

		if (b.isExact()) {

		std::cerr << s.substr(0,s.length()-3) << " = " << b.min << std::endl;

		} else {

		std::cerr << b.min << " <= " << s.substr(0,s.length()-3)<< " <= " << b.max << std::endl;

		}

	}


	if (args_info.verbose_given) {
		fprintf(stderr, "%s: Calculation took %.0f seconds.\n", PACKAGE, difftime(end_time, start_time));
	}

	return EXIT_SUCCESS;
}
