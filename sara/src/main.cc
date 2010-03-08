#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <time.h>


#ifndef PNAPI_PNAPI_H
#include "pnapi/pnapi.h"
#endif
#include "partialsolution.h"
#include "pathfinder.h"
#include "reachalyzer.h"
#include "problem.h"
#include "cmdline.h"
#include "verbose.h"

using std::cerr;
using std::cout;
using std::endl;
using std::vector;
using std::deque;
using std::map;
using std::set;
using std::string;
using std::ifstream;
using std::ofstream;
using std::ostringstream;
using std::setw;
//using namespace pnapi;

/// the command line parameters
gengetopt_args_info args_info;

/// parser supplement (sara) for problem files
extern FILE *sara_in;
extern int sara_parse();
extern vector<Problem> pbls;

/// parser for Petri nets
pnapi::parser::owfn::Parser paowfn;
pnapi::parser::lola::Parser palola;

/// Global ordering of transitions for lp_solve and the PathFinder
vector<Transition*> transitionorder;
/// Global ordering of places for lp_solve and determining a scapegoat (in the stubborn set method)
vector<Place*> placeorder;
/// inverted transitionorder for back references
map<Transition*,int> revtorder;
/// inverted placeorder for back references
map<Place*,int> revporder;

/*
template <class T>
inline std::string to_string (const T& t)
{
	std::stringstream ss;
	ss << t;
	return ss.str();
}
*/

/// evaluate the command line parameters
void evaluateParameters(int argc, char** argv) {
    // set default values
    cmdline_parser_init(&args_info);

    // initialize the parameters structure
    struct cmdline_parser_params *params = cmdline_parser_params_create();

    // call the cmdline parser
    if (cmdline_parser (argc, argv, &args_info) != 0) {
        fprintf(stderr, "       see 'sara --help' for more information\n");
        exit(EXIT_FAILURE);
    }

    free(params);
}

/// main
int main(int argc, char** argv) {
	// start timer
	clock_t starttime;
	starttime = clock();

	// evaluate command line
    evaluateParameters(argc, argv);

/****************
* NO ARGS GIVEN *
****************/

// can't use groups or modes in gengetopt; required options form two groups
if (!args_info.input_given && !args_info.pipe_given
	&& !args_info.quasilive_given && !args_info.reachable_given && !args_info.realize_given)
{
	cerr << "sara: Use options -i or -p for input, -q, -r, or -R for problem creation." << endl;
}

/*************************
* REDIRECTING OUTPUT/LOG *
*************************/
std::streambuf* cout_sbuf = std::cout.rdbuf(); // save original sbuf
std::ofstream   fout((args_info.output_given?args_info.output_arg:"outfile"));
if (args_info.output_given) std::cout.rdbuf(fout.rdbuf()); // redirect 'cout' to 'fout'

std::streambuf* cerr_sbuf = std::cerr.rdbuf(); // save original sbuf
std::ofstream   ferr((args_info.log_given?args_info.log_arg:"logfile"));
if (args_info.log_given) std::cerr.rdbuf(ferr.rdbuf()); // redirect 'cerr' to 'ferr'

string problemfile("stdin"); // name of the problem file to be executed (input)

/*****************
* QUASI-LIVENESS *
*****************/
if (args_info.quasilive_given) {
	// load the Petri net
	Problem pbl;
	pbl.setFilename(args_info.quasilive_arg);
	pbl.setNetType((args_info.owfn_given?(Problem::OWFN):(Problem::LOLA)));
	PetriNet* pn(pbl.getPetriNet());
	Marking m(Marking(*pn,false));
	// get all transitions
	set<Transition*> tset(pn->getTransitions());
	if (tset.empty()) cerr << "sara: no transitions in Petri net " << args_info.quasilive_arg << endl;
	set<Transition*>::iterator it;
	// open the .sara file for output
	string outname = (string)args_info.quasilive_arg + ".sara";
	problemfile = outname; // redirect later pipe input
	ofstream outfile(outname.c_str(), ofstream::trunc);
	if (!outfile.is_open()) abort(3,"error: could not write to file '%s'",outname.c_str());
	for(it=tset.begin(); it!=tset.end(); ++it)
	{
		outfile << "PROBLEM enabling_of_" << (*it)->getName() << ":" << endl;
		outfile << "GOAL REACHABILITY;" << endl;
		outfile << "FILE " << args_info.quasilive_arg << " TYPE " << (args_info.owfn_given?"OWFN":"LOLA") << ";" << endl;
		outfile << "INITIAL ";
		bool first = true;
		map<const Place*,unsigned int>::const_iterator pit;
		for(pit=m.begin(); pit!=m.end(); ++pit)
			if (pit->second>0) {
				if (!first) outfile << ",";
				outfile << pit->first->getName() << ":" << pit->second;
				first = false;
			}
		outfile << ";" << endl;
		outfile << "FINAL COVER ";
		set<pnapi::Arc*> arcs((*it)->getPresetArcs());
		set<pnapi::Arc*>::iterator ait;
		first = true;
		for(ait=arcs.begin(); ait!=arcs.end(); ++ait) {
			if (!first) outfile << ",";
			outfile << (*ait)->getPlace().getName() << ">" << (*ait)->getWeight();
			first = false;
		}
		outfile << ";" << endl << endl;
	}
	outfile.close();
}

/***************************
* CONSTRUCT SINGLE PROBLEM *
***************************/
if (args_info.reachable_given || args_info.realize_given) {
	// load the Petri net
	Problem pbl;
	if (args_info.reachable_given) pbl.setFilename(args_info.reachable_arg);
	else pbl.setFilename(args_info.realize_arg);
	pbl.setNetType((args_info.owfn_given?(Problem::OWFN):(Problem::LOLA)));
	PetriNet* pn(pbl.getPetriNet());
	Marking m(Marking(*pn,false));
	// open the .sara file for output
	string outname = pbl.getFilename() + ".sara";
	problemfile = outname; // redirect later pipe input
	ofstream outfile(outname.c_str(), ofstream::trunc);
	if (!outfile.is_open()) abort(3,"error: could not write to file '%s'",outname.c_str());
	// print the problem
	outfile << "PROBLEM " << (args_info.reachable_given?"reach":"realiz") << "ability_in_" << pbl.getFilename() << ":" << endl;
	if (args_info.reachable_given) outfile << "GOAL REACHABILITY;" << endl;
	else outfile << "GOAL REALIZABILITY;" << endl;
	outfile << "FILE " << pbl.getFilename() << " TYPE " << (args_info.owfn_given?"OWFN":"LOLA") << ";" << endl;
	outfile << "INITIAL ";
	bool first = true;
	map<const Place*,unsigned int>::const_iterator pit;
	for(pit=m.begin(); pit!=m.end(); ++pit)
		if (pit->second>0) {
			if (!first) outfile << ",";
			outfile << pit->first->getName() << ":" << pit->second;
			first = false;
		}
	outfile << ";" << endl;
	outfile << "FINAL ";
	if (args_info.final_given) outfile << args_info.final_arg;
	outfile << ";" << endl << endl;
	outfile.close();
}

/*********************
* SOLVE THE PROBLEMS *
*********************/
if (args_info.input_given || args_info.pipe_given) {

	if (args_info.input_given) status("try reading problem file %s.",args_info.input_arg);
	else if (problemfile=="stdin") status("reading problem from standard input.");
	else status("passing on problems.");

	// try to open file to read problem from
	if (args_info.input_given) problemfile = args_info.input_arg;
	if (problemfile=="stdin") sara_in = stdin;
	else sara_in = fopen(problemfile.c_str(),"r");
	if (!sara_in) abort(1, "could not read problem information");
	sara_parse();
	if (sara_in!=stdin) fclose(sara_in);

	// set verbosity and debug mode
	bool verbose(args_info.verbose_given);
	int debug = 0;
	if (args_info.debug_given) debug = args_info.debug_arg;
	// determine whether standard output still exists
	bool out = !(args_info.output_given);

	// counters for number of problems and solutions found
	int loops = 0;
	int solcnt = 0;
	// walk through the problem list
	for(unsigned int x=0; x<pbls.size(); ++x)
	{
		cout << "sara: Problem " << ++loops << ": " << pbls.at(x).getName() << endl;
		if (args_info.log_given)
			cerr << "sara: Problem " << ++loops << ": " << pbls.at(x).getName() << endl;
		PetriNet* pn(pbls.at(x).getPetriNet()); // obtain the Petri net and its P/T-ordering
		if (x+1<pbls.size()) pbls.at(x+1).checkForNetReference(pbls.at(x)); // possibly advance it to the next problem
		Marking m1(pbls.at(x).getInitialMarking()); // get the initial marking
		switch (pbls.at(x).getGoal()) { // check for reachability or realizability
			case Problem::REACHABLE: {
				Marking m2(pbls.at(x).getFinalMarking()); // get the extended final marking ...
				map<Place*,int> cover(pbls.at(x).getCoverRequirement()); // including the directives for the places 
				// obtain an instance of the reachability solver
				Reachalyzer reach(*pn,m1,m2,cover,verbose,debug,out,(args_info.break_given?args_info.break_arg:0));
				if (reach.getStatus()!=Reachalyzer::LPSOLVE_INIT_ERROR) {
					reach.start(); // if everything is ok, solve the problem
					clock_t mytime(reach.getTime()); // ... and measure the time for that
					if (reach.getStatus()==Reachalyzer::SOLUTION_FOUND) ++solcnt;
					reach.printResult(); // ... and print the result
					if (debug>0) {
						cerr << "Transition Order: ";
						for(unsigned int o=0; o<transitionorder.size(); ++o)
							cerr << transitionorder[o]->getName() << " ";
						cerr << endl;
						cerr << "Place Order: ";
						for(unsigned int o=0; o<placeorder.size(); ++o)
							cerr << placeorder[o]->getName() << " ";
						cerr << endl;
					}
					if (args_info.time_given)
						cout << "sara: Time: " << (float)(mytime)/CLOCKS_PER_SEC << " sec." << endl;
				} else cout << "sara: UNSOLVED: error while initializing lp_solve" << endl;
				break;
			}
			case Problem::REALIZABLE: {
				map<Transition*,int> tv(pbls.at(x).getVectorToRealize()); // the vector to realize
				IMatrix im(*pn); // incidence matrix of the net
				im.verbose = debug;
				PartialSolution* ps(new PartialSolution(m1)); // create initial job
				JobQueue tps(ps); // create a job list
				JobQueue solutions; // create a job list
				map<map<Transition*,int>,vector<PartialSolution> > dummy; // dummy, will be filled and immediately free'd
				// create an instance of the realizability solver
				PathFinder pf(m1,tv,pn->getTransitions().size(),tps,solutions,im,dummy);
				pf.verbose = debug;
				if (pf.recurse()) { pf.printSolution(); ++solcnt; } // solve the problem and print a possible solution
				else cout << "sara: INFEASIBLE: the transition multiset is not realizable." << endl;
				break;
			}
		}
		cout << endl;
	}

	clock_t endtime = clock();
	if (args_info.verbose_given)
		cout << "sara: " << solcnt << " Solution" << (solcnt!=1?"s":"") << " produced." << endl;
	if (args_info.time_given)
		cout << "sara: Used " << (float)(endtime-starttime)/CLOCKS_PER_SEC << " seconds overall." << endl;
}

/*************************
* REDIRECTING OUTPUT/LOG *
*************************/
if (args_info.output_given) std::cout.rdbuf(cout_sbuf); // restore the original stream buffer
if (args_info.log_given) std::cerr.rdbuf(cerr_sbuf); // restore the original stream buffer


    return EXIT_SUCCESS;
}

