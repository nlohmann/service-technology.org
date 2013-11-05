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
#ifndef SARALIB
#include "cmdline.h"
#endif
#include "problem.h"
#include "verbose.h"
#include "sthread.h"
#include "JSON.h"
#include "lp_solve_5.5/lp_lib.h"

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


/// parser supplement (sara) for problem files
extern FILE *sara_in;
extern int sara_parse();

namespace sara {
extern vector<Problem> pbls;

/// Global ordering of transitions for lp_solve and the PathFinder
vector<Transition*> transitionorder;
/// Global ordering of places for lp_solve and determining a scapegoat (in the stubborn set method)
vector<Place*> placeorder;
/// inverted transitionorder for back references
map<const Transition*,int> revtorder;
/// inverted placeorder for back references
map<Place*,int> revporder;
/// for property checks via owfn2sara: if the properties hold so far
map<string,bool> results;
/// for property checks via owfn2sara: if lp_solve fails the result can be indecisive
map<string,bool> indecisive;
/// for ordering the properties correctly
vector<string> resorder;
/// if the result of several problems should be ORed (or ANDed)
map<string,bool> orresult;
/// if no computation should be done at all (dummy for model checking contest)
map<string,bool> donotcompute;
int debug(0);

bool flag_droppast(false);
int val_droppast(0);
bool flag_verbose(false);
bool flag_show(false);
bool flag_forceprint(false);
bool flag_lookup(false);
int val_lookup(0);
bool flag_output(false);
bool flag_treemaxjob(false);
bool val_treemaxjob(0);
bool flag_continue(false);
bool flag_scapegoat(false);
bool flag_break(false);
bool flag_witness(false);
bool flag_joborder(false);
bool flag_yesno(false);
int val_maxdepth(0);
bool flag_incremental(false);

} // end of namespace sara

/*
template <class T>
inline std::string to_string (const T& t)
{
	std::stringstream ss;
	ss << t;
	return ss.str();
}
*/

#ifndef SARALIB
// this part will be compiled if sara is compiled into an executable file.
namespace sara {

/// the command line parameters
gengetopt_args_info args_info;

/*! Evaluate the command line parameters.
	\param argc Number of arguments.
	\param argv Argument list.
*/
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

void parse_constraint(string in, bool cov, map<string,int>& lhs, int& comp, int& rhs) {
	size_t comppos(in.find_last_of("<>:="));
	if (comppos == string::npos) { comp = (cov ? GE : EQ); rhs = 1; comppos = in.size(); }
	else { switch (in.at(comppos)) {
		case '<': comp = LE; break;
		case '>': comp = GE; break;
		case '=':
		case ':': comp = EQ; break;
		}
		rhs = std::strtol(in.substr(comppos+1,string::npos).c_str(),NULL,10);
	}
	size_t fpos(0);
	while (fpos<comppos) {
		int lhsval(1);
		size_t startname(in.find_first_not_of("+- 0123456789",fpos));
		size_t lastplus(in.find_last_of("+",startname-1));
		if (lastplus == string::npos || lastplus < fpos) lastplus = fpos;
		else ++lastplus;
		if (lastplus < startname) lhsval = std::strtol(in.substr(lastplus,startname-lastplus).c_str(),NULL,10);
		fpos = in.find_first_of("+- ");
		if (fpos > comppos) fpos = comppos;
		lhs[in.substr(startname,fpos-startname)] = lhsval;
	}
}

void parse_marking(string in, bool cov, map<string,int>& m, map<string, int>& comp) {
	size_t fpos(0);
	while (fpos < in.size()) {
		map<string,int> mtmp;
		int cmp, rhs;
		size_t next(in.find_first_of(",",fpos));
		if (next == string::npos) next = in.size();
		parse_constraint(in.substr(fpos,next-fpos),cov,mtmp,cmp,rhs);
		fpos = next+1;
		m[mtmp.begin()->first] = rhs;
		comp[mtmp.begin()->first] = cmp;
	}
}

void json_parse(JSON& jin) {
	for(JSON::iterator jit=jin.begin(); jit!=jin.end(); ++jit)
	{
		Problem pbl;
//		std::map<string,int> clhs;
//		int ccomp,crhs;

		pbl.setName(jit.key());
		if ((*jit)["GOAL"] == "REACHABILITY")
			pbl.setGoal(Problem::REACHABLE);
		else if ((*jit)["GOAL"] == "REALIZABILITY")
			pbl.setGoal(Problem::REALIZABLE);
		else pbl.setGoal(Problem::DUMMY);
		pbl.setFilename((*jit)["FILE"]);
		if ((*jit)["TYPE"] == "OWFN")
			pbl.setNetType(Problem::OWFN);
		else if ((*jit)["TYPE"] == "LOLA")
			pbl.setNetType(Problem::LOLA);
		else
			pbl.setNetType(Problem::PNML);
		if (!(*jit)["INITIAL"].empty()) {
			map<string,int> tmp1,tmp2;
			parse_marking((*jit)["INITIAL"],false,tmp1,tmp2);
			map<string,int>::iterator mit;
			for(mit=tmp1.begin(); mit!=tmp1.end(); ++mit)
				pbl.setInit(mit->first,mit->second);
		}
		bool cov(false);
		if ((*jit).find("COVER")!=(*jit).end()) 
			if ((*jit)["COVER"]) cov = true;
		pbl.setGeneralCover(cov);
		if (!(*jit)["FINAL"].empty()) {
			map<string,int> tmp1,tmp2;
			parse_marking((*jit)["FINAL"],cov,tmp1,tmp2);
			map<string,int>::iterator mit;
			for(mit=tmp1.begin(); mit!=tmp1.end(); ++mit)
				pbl.setFinal(mit->first,tmp2[mit->first],mit->second);
		}
		if ((*jit).find("CONSTRAINT")!=(*jit).end()) {
			for(unsigned int i=0; i<(*jit)["CONSTRAINT"].size(); ++i)
			{
				map<string,int> tmp;
				int cmp, rhs;
				parse_constraint((*jit)["CONSTRAINT"][i],false,tmp,cmp,rhs);
				pbl.addConstraint(tmp,cmp,rhs);
			}
		}
		if ((*jit).find("RESULT")!=(*jit).end()) {
			if ((*jit)["RESULT"]["OR"]) pbl.setOrResult(true); else pbl.setOrResult(false);
			if ((*jit)["RESULT"]["NEGATE"]) pbl.setNegateResult(true); else pbl.setNegateResult(false);
			pbl.setResultText((*jit)["RESULT"]["TEXT"]);
		}

		pbls.push_back(pbl);
	}
}

/*! Main method of Sara.
	\param argc Number of arguments.
	\param argv Argument list.
*/
int main(int argc, char** argv) {
	// start timer
	clock_t starttime;
	starttime = clock();

	// evaluate command line
    evaluateParameters(argc, argv);
	flag_droppast = args_info.droppast_given;
	val_droppast = args_info.droppast_arg;
	flag_verbose = args_info.verbose_given;
	flag_show = args_info.show_given;
	flag_forceprint = args_info.forceprint_given;
	flag_lookup = args_info.lookup_given;
	val_lookup = args_info.lookup_arg;
	flag_output = args_info.output_given;
	flag_treemaxjob = args_info.treemaxjob_given;
	val_treemaxjob = args_info.treemaxjob_arg;
	flag_continue = args_info.continue_given;
	flag_scapegoat = args_info.scapegoat_given;
	flag_break = args_info.break_given;
	flag_witness = args_info.witness_given;
	flag_joborder = args_info.joborder_given;
	flag_yesno = args_info.yesno_given;
	if (args_info.maxdepth_given) val_maxdepth = args_info.maxdepth_arg;
	flag_incremental = args_info.incremental_given;

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
	pbl.setFilename(args_info.quasilive_arg); // select filename and net type
	int nettype(Problem::PNML);
	if (strstr(args_info.quasilive_arg,"owfn") || strstr(args_info.quasilive_arg,"OWFN")) nettype=Problem::OWFN;
	if (strstr(args_info.quasilive_arg,"llnet") || strstr(args_info.quasilive_arg,"LLNET")) nettype=Problem::LOLA;
	if (args_info.owfn_given) nettype = Problem::OWFN;
	if (args_info.lola_given) nettype = Problem::LOLA;
	if (args_info.pnml_given) nettype = Problem::PNML;
	pbl.setNetType(nettype);
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
		outfile << "FILE " << args_info.quasilive_arg << " TYPE ";
		switch(nettype) {
			case Problem::OWFN: outfile << "OWFN"; break;
			case Problem::LOLA: outfile << "LOLA"; break;
			case Problem::PNML: outfile << "PNML"; break;
		}
		outfile << ";" << endl << "INITIAL ";
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
	char* filename((args_info.reachable_given ? args_info.reachable_arg : args_info.realize_arg));
	pbl.setFilename(filename);
	int nettype(Problem::PNML);
	if (strstr(filename,"owfn") || strstr(filename,"OWFN")) nettype=Problem::OWFN;
	if (strstr(filename,"llnet") || strstr(filename,"LLNET")) nettype=Problem::LOLA;
	if (args_info.owfn_given) nettype = Problem::OWFN;
	if (args_info.lola_given) nettype = Problem::LOLA;
	if (args_info.pnml_given) nettype = Problem::PNML;
	pbl.setNetType(nettype);
	PetriNet* pn(pbl.getPetriNet());
	Marking m(Marking(*pn,false));
	// open the .sara file for output
	string outname = pbl.getFilename() + ".sara";
	if (args_info.json_given) outname += ".json";
	problemfile = outname; // redirect later pipe input
	ofstream outfile(outname.c_str(), ofstream::trunc);
	if (!outfile.is_open()) abort(3,"error: could not write to file '%s'",outname.c_str());
	// print the problem
	if (args_info.json_given) {
		JSON jout;
		std::stringstream pbname;

		if (args_info.reachable_given) {
			pbname << "reachability_in_" << pbl.getFilename();
			jout[pbname.str()]["GOAL"] = "REACHABILITY";
		} else {
			pbname << "realizability_in_" << pbl.getFilename();
			jout[pbname.str()]["GOAL"] = "REALIZABILITY";
		}
		jout[pbname.str()]["FILE"] = pbl.getFilename();
		switch(nettype) {
			case Problem::OWFN: jout[pbname.str()]["TYPE"] = "OWFN"; break;
			case Problem::LOLA: jout[pbname.str()]["TYPE"] = "LOLA"; break;
			case Problem::PNML: jout[pbname.str()]["TYPE"] = "PNML"; break;
		}
		bool first = true;
		std::stringstream init;
		map<const Place*,unsigned int>::const_iterator pit;
		for(pit=m.begin(); pit!=m.end(); ++pit)
			if (pit->second>0) {
				if (!first) init << ",";
				init << pit->first->getName() << ":" << pit->second;
				first = false;
			}
		jout[pbname.str()]["INITIAL"] = init.str();
		if (args_info.final_given) {
			bool cover(strstr(args_info.final_arg,"COVER"));
			jout[pbname.str()]["FINAL"] = args_info.final_arg + (cover ? 5 : 0);
			if (cover) jout[pbname.str()]["COVER"] = true; 
			else jout[pbname.str()]["COVER"] = false;
		}
		if (args_info.constraint_given) {
			for(unsigned int i=0; i<args_info.constraint_given; ++i)
				jout[pbname.str()]["CONSTRAINT"] += args_info.constraint_arg[i];
		}

		outfile << jout << endl;
	} else {
		outfile << "PROBLEM " << (args_info.reachable_given?"reach":"realiz") << "ability_in_" << pbl.getFilename() << ":" << endl;
		if (args_info.reachable_given) outfile << "GOAL REACHABILITY;" << endl;
		else outfile << "GOAL REALIZABILITY;" << endl;
		outfile << "FILE " << pbl.getFilename() << " TYPE ";
		switch(nettype) {
			case Problem::OWFN: outfile << "OWFN"; break;
			case Problem::LOLA: outfile << "LOLA"; break;
			case Problem::PNML: outfile << "PNML"; break;
		}
		outfile << ";" << endl << "INITIAL ";
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
		outfile << ";" << endl;
		if (args_info.constraint_given) {
			outfile << "CONSTRAINTS ";
			first = true;
			for(unsigned int i=0; i<args_info.constraint_given; ++i)
			{
				if (!first) outfile << ", ";
				outfile << args_info.constraint_arg[i];
				first = false;
			}
			outfile << ";" << endl;
		}
		outfile << endl;
	}
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
	JSON json;
	if (args_info.json_given) {
		if (args_info.input_given) {
			std::ifstream inputStream(args_info.input_arg);
	        if (!inputStream) abort(1, "could not read problem information");
			inputStream >> json;
		} else std::cin >> json;
		json_parse(json);
	} else {
		if (args_info.input_given) problemfile = args_info.input_arg;
		if (problemfile=="stdin") sara_in = stdin;
		else sara_in = fopen(problemfile.c_str(),"r");
		if (!sara_in) abort(1, "could not read problem information");
		sara_parse();
		if (sara_in!=stdin) fclose(sara_in);
	}

	// set verbosity and debug mode
	bool verbose(args_info.verbose_given);

	if (args_info.debug_given) debug = args_info.debug_arg;
	// determine whether standard output still exists
	bool out = !(args_info.output_given);

	// start multi-threading
	initThreadData(args_info.threads_given?args_info.threads_arg:0);
	startThreads();

	// counters for number of problems and solutions found
	int loops = 0;
	int solcnt = 0;
	int maxtracelen = -1; // maximal solution length (-1=no solution)
	int avetracelen = 0; // average solution length

	// walk through the problem list
	for(unsigned int x=0; x<pbls.size(); ++x)
	{
		orresult[pbls.at(x).getResultText()]=pbls.at(x).isOrResult();
		if (results.find(pbls.at(x).getResultText())==results.end()) {
			results[pbls.at(x).getResultText()] = true;
			if (pbls.at(x).getResultText()!="")
				resorder.push_back(pbls.at(x).getResultText());
		} else if (pbls.at(x).getResultText()!="")
			if (!results[pbls.at(x).getResultText()]) continue;
		if (!flag_yesno) cout << "sara: Problem " << (++loops) << ": " << pbls.at(x).getName() << endl;
		if (args_info.log_given) // print info also to log file if one has been given
			cerr << "sara: Problem " << (loops) << ": " << pbls.at(x).getName() << endl;
		PetriNet* pn(pbls.at(x).getPetriNet()); // obtain the Petri net and its P/T-ordering
		IMatrix* im(pbls.at(x).getIMatrix());
		if (x+1<pbls.size()) pbls.at(x+1).checkForNetReference(pbls.at(x)); // possibly advance the net to the next problem (if it has the same net)
		Marking m1(pbls.at(x).getInitialMarking()); // get the initial marking
		Marking m2(pbls.at(x).getFinalMarking()); // get the extended final marking (if there is one)
		map<Place*,int> cover; // for the cover directives for the places 
		bool passedon(false); // set to true when the problem is passed on from PathFinder to Reachalyzer 

		if (verbose) // print the problem statement
		{
			cout << "sara: Trying to " << (pbls.at(x).getGoal()==Problem::REALIZABLE?"realize a transition vector ":"solve a reachability problem ") << "with inputs:" << endl;
			cout << "sara: - Petri net \"" << pbls.at(x).getFilename() << "\"" << endl;
			cout << "sara: - Initial marking "; 
			pbls.at(x).showInitial(); 
			cout << endl;
			if (pbls.at(x).getGoal()==Problem::REALIZABLE) {
				cout << "sara: - Transition vector "; 
				pbls.at(x).showTVector();
				cout << endl;
			} else {
				cout << "sara: - Final marking "; 
				pbls.at(x).showFinal(); 
				cout << endl;
			}
			if (pbls.at(x).getNumberOfConstraints()>0) {
				cout << "sara: - Global constraints ";
				pbls.at(x).showConstraints();
				cout << endl;
			}
		}

		cover = pbls.at(x).getCoverRequirement(); // get the cover directives for the places, if there are any 
		switch (pbls.at(x).getGoal()) { // check for reachability or realizability?

			case Problem::REALIZABLE: {
				map<Transition*,int> tv(pbls.at(x).getVectorToRealize()); // the vector to realize
//				IMatrix im(*pn); // incidence matrix of the net
				im->verbose = debug;
//				im.precompute();
				PartialSolution* ps(new PartialSolution(m1)); // create the initial job
				JobQueue tps(ps); // create a job list
				JobQueue solutions; // create a job list
				JobQueue failure; // create a dummy job list
				map<map<Transition*,int>,vector<PartialSolution> > dummy; // dummy, will be filled and immediately free'd
				// create an instance of the realizability solver
				PathFinder pf(m1,tv,tps,tps,solutions,failure,*im,dummy,NULL);
				pf.verbose = debug;
				initPathFinderThread(0,ps,m1,tv,pf);
				bool solved = pf.recurse(0);
				pf.waitForThreads(0,solved); // wait for the helper threads (or cancel them in case of a solution)
				waitForAllIdle(debug);
				if (!solutions.almostEmpty()) // solve the problem and print a possible solution
				{ 
					int mtl = solutions.printSolutions(avetracelen,pbls.at(x),x,json); // get the solution length for this problem
					if (mtl>maxtracelen) maxtracelen=mtl; // and maximize over all problems
					solcnt+=solutions.size();
					if (pbls.at(x).isNegateResult() ^ pbls.at(x).isOrResult())
						results[pbls.at(x).getResultText()] = false;
				}
				else { 
					if (!flag_yesno) cout << "sara: INFEASIBLE: the transition multiset is not realizable." << endl;
					if ((!pbls.at(x).isNegateResult()) ^ pbls.at(x).isOrResult())
						results[pbls.at(x).getResultText()] = false;
				}
				if ((!solutions.almostEmpty() || !verbose) && !args_info.continue_given) break;
				// if witnesses are sought and we have no solution, we pass the problem on
				passedon = true;
				m2 = m1; // but first, calculate the final marking
				map<Place*,int> change(im->getChange(tv));
				map<Place*,int>::iterator mit;
				for(mit=change.begin(); mit!=change.end(); ++mit)
					m2[*(mit->first)]+=mit->second;
			}
			// no break here! We might want to pass on the problem to the reachability solver

			case Problem::REACHABLE: {
				// obtain an instance of the reachability solver
				Reachalyzer reach(*pn,m1,m2,cover,pbls.at(x),verbose,debug,out,(args_info.break_given?args_info.break_arg:0),passedon,im);
				if (reach.getStatus()!=Reachalyzer::LPSOLVE_INIT_ERROR) {
					reach.start(); // if everything is ok, solve the problem
					clock_t mytime(reach.getTime()); // ... and measure the CPU time for that
					// count the solutions
					if (reach.getStatus()==Reachalyzer::SOLUTION_FOUND) {
						solcnt+=reach.numberOfSolutions(); 
						if (pbls.at(x).isNegateResult() ^ pbls.at(x).isOrResult())
							results[pbls.at(x).getResultText()] = false;
					} else if (reach.getStatus()==Reachalyzer::SOLUTIONS_LOST
								|| reach.getStatus()==Reachalyzer::LPSOLVE_RUNTIME_ERROR) {
						indecisive[pbls.at(x).getResultText()] = true;
					} else {
						if ((!pbls.at(x).isNegateResult()) ^ pbls.at(x).isOrResult())
							results[pbls.at(x).getResultText()] = false;
					}
					if (!flag_yesno) reach.printResult(x+1,json); // ... and print the result
					int mtl = reach.getMaxTraceLength(); // get the maximal solution length for this problem
					if (mtl>maxtracelen) maxtracelen=mtl; // and maximize over all problems
					avetracelen += reach.getSumTraceLength(); // sum up solution lengths for average calculation
					if (debug>0) { // debug info, P/T orderings
						cerr << "Transition Order: ";
						for(unsigned int o=0; o<transitionorder.size(); ++o)
							cerr << transitionorder[o]->getName() << " ";
						cerr << endl;
						cerr << "Place Order: ";
						for(unsigned int o=0; o<placeorder.size(); ++o)
							cerr << placeorder[o]->getName() << " ";
						cerr << endl;
					}
					if (args_info.time_given) // if --time was specified, print the time used
						cout << "sara: Time: " << (float)(mytime)/CLOCKS_PER_SEC << " sec." << endl;
				} else { if (!flag_yesno)
						cout << "sara: UNSOLVED: error while initializing lp_solve" << endl;
					indecisive[pbls.at(x).getResultText()] = true;
				}
				break;
			}

			case Problem::DUMMY: donotcompute[pbls.at(x).getResultText()] = true; break;
		}
		if (!flag_yesno) cout << endl;
	}

	clock_t endtime = clock();
	if (args_info.verbose_given)
	{
		cout << "sara: " << solcnt << " Solution" << (solcnt!=1?"s":"") << " produced." << endl;
		if (solcnt>1 && maxtracelen>=0 && !flag_yesno)
		{
			cout << "sara: Maximal solution length is " << maxtracelen;
			if (avetracelen!=maxtracelen*solcnt) cout << ", average is " << (avetracelen%solcnt==0?"":"about ") << avetracelen/solcnt+(avetracelen%solcnt>=solcnt/2?1:0);
			cout << "." << endl;
		}
	}

	if (flag_yesno) { // for reduced output (Model checking contest 2011)

		for(unsigned int i=0; i<resorder.size(); ++i)
			if (resorder[i]!="")
			{
				bool tech(true);
				if (donotcompute.find(resorder[i])!=donotcompute.end())
					{ cout << "DO_NOT_COMPETE" << endl; tech=false; }
				else if (indecisive.find(resorder[i])==indecisive.end()) {
					if (results[resorder[i]] ^ !orresult[resorder[i]]) cout << "FORMULA " << resorder[i] << " FALSE ";
					else cout << "FORMULA " << resorder[i] << " TRUE ";
				} else {
					if (results[resorder[i]]) { 
						cout << "CANNOT_COMPUTE" << endl;
						tech = false;
					} else
						if (!orresult[resorder[i]]) cout << "FORMULA " << resorder[i] << " FALSE ";
						else cout << "FORMULA " << resorder[i] << " TRUE ";
				}
				if (tech) cout << "TECHNIQUES ABSTRACTIONS PARTIAL_ORDERS OTHERS" << endl;
			}

	} else { // normal output

		for(unsigned int i=0; i<resorder.size(); ++i)
			if (resorder[i]!="" && donotcompute.find(resorder[i])==donotcompute.end())
			{
				cout << "sara: The property of " << resorder[i];
				if (indecisive.find(resorder[i])==indecisive.end()) {
					if (results[resorder[i]] ^ orresult[resorder[i]]) {
						cout << " is fulfilled." << endl;
						json["PROPERTIES"][resorder[i]] = true;
					} else {
						cout << " does not hold." << endl;
						json["PROPERTIES"][resorder[i]] = false;
					}
				} else {
					if (results[resorder[i]]) {
						cout << " could not be decided." << endl;
						json["PROPERTIES"][resorder[i]] = "error";
					} else { 
						if (orresult[resorder[i]]) {
							cout << " is fulfilled." << endl;
							json["PROPERTIES"][resorder[i]] = true;
						} else {
							cout << " does not hold." << endl;
							json["PROPERTIES"][resorder[i]] = false;
						}
					}
				}
			}
	}

	if (args_info.time_given) // print time use if --time was specified
		cout << "sara: Used " << (float)(endtime-starttime)/CLOCKS_PER_SEC << " seconds overall." << endl;

	// stop the threads (if any)
	stopThreads();
	destroyThreadData();

	// write the JSON object with solution/counter examples to a file
	if (args_info.json_arg) {
		std::ofstream outputStream(args_info.json_arg);
        if (!outputStream) abort(3, "could not write to file '%s'", args_info.json_arg);
		outputStream << json << endl;
	}
}

/*************************
* REDIRECTING OUTPUT/LOG *
*************************/
if (args_info.output_given) std::cout.rdbuf(cout_sbuf); // restore the original stream buffer
if (args_info.log_given) std::cerr.rdbuf(cerr_sbuf); // restore the original stream buffer


    return EXIT_SUCCESS;
}

} // end namespace sara

/*! Main method of Sara (outside namespace sara).
	\param argc Number of arguments.
	\param argv Argument list.
*/
int main(int argc, char** argv) {
	sara::main(argc,argv);
}

#else
// this part of Sara will be compiled when constructing a library
namespace sara {

// Use this method to call Sara's reachability test in a library:
vector<Transition*> ReachabilityTest(PetriNet& pn, Marking& m0, Marking& mf, map<Place*,int>& cover) {
	Reachalyzer reach(pn,m0,mf,cover); // the net, initial and final marking, and cover info (if token numbers should be covered instead of reached exactly)
	if (reach.getStatus()!=Reachalyzer::LPSOLVE_INIT_ERROR) {
		reach.start(); // if everything is ok, solve the problem
		if (reach.getStatus()==Reachalyzer::SOLUTION_FOUND) {
			status("called Sara, found a firing sequence");
			return reach.getOneSolution();
		} else if (reach.getStatus()==Reachalyzer::SOLUTIONS_LOST) {
			status("called Sara, no firing sequence found -- result is indecisive");
			vector<Transition*> result(2);
			result[0]=NULL; result[1]=NULL; // two null pointers == indecisive result
			return result;
		} else {
			status("called Sara, firing sequence does not exist");
			vector<Transition*> result(1);
			result[0]=NULL; // one null pointer == no solution exists
		}
	} else {
		status("called Sara, lp_solve initialization failed");
		vector<Transition*> result(2);
		result[0]=NULL; result[1]=NULL;
		return result;
	}
}

// Use this method to call Sara's realizability test in a library:
vector<Transition*> RealizabilityTest(PetriNet& pn, Marking& m0, map<Transition*,int>& parikh) {
	IMatrix im(pn); // incidence matrix of the net
	im.verbose = 0;
	im.precompute();
	PartialSolution* ps(new PartialSolution(m0)); // create the initial job
	JobQueue tps(ps); // create a job list
	JobQueue solutions; // create a job list
	JobQueue failure; // create a dummy job list
	map<map<Transition*,int>,vector<PartialSolution> > dummy; // dummy, will be filled and immediately free'd
	// create an instance of the realizability solver
	allocTarjan();
	PathFinder pf(m0,parikh,tps,tps,solutions,failure,im,dummy,NULL);
	pf.verbose = 0;
	initThread(0,ps,m1,tv,pf);
	bool solved = pf.recurse(0);
	pf.waitForThreads(0,solved); // wait for the helper threads (or cancel them in case of a solution)
	freeTarjan();
	if (!solutions.almostEmpty()) // solve the problem and print a possible solution
	{ 
		status("called Sara, found a firing sequence");
		return solutions.getOneSolution();
	}
	status("called Sara, firing sequence does not exist");
	vector<Transition*> noresult;
	noresult.push_back(NULL); // non-existence of a firing sequence is expressed by a single NULL pointer in a vector
	return noresult;
}

void setVerbose() { flag_verbose = true; }

} // end namespace sara
#endif

