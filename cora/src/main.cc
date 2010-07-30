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
#include "cora.h"
#include "pnloader.h"
#include "imatrix.h"
#include "cmdline.h"
#include "extmarking.h"
#include "cnode.h"
#include "rnode.h"
#include "covergraph.h"
#include "stubbornset.h"
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

/// the command line parameters
gengetopt_args_info args_info;

vector<Place*> po;

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

/****************
* NO ARGS GIVEN *
****************/

if (!args_info.input_given && !args_info.pipe_given)
{
	cerr << "cora: Use options -i or -p for input, -m, -s, or -f to define goal." << endl;
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

//string problemfile("stdin"); // name of the problem file to be executed (input)


/*********************
* SOLVE THE PROBLEMS *
*********************/

if (args_info.input_given || args_info.pipe_given) {

	if (args_info.input_given) status("try reading Petri net file %s.",args_info.input_arg);
	else status("try reading Petri net from stdin.");

	PNLoader pnl; // initialize the Petri net loader
	// then select the file/stdin and the net type
	pnl.setNetType(PNLoader::PNML);
	if (args_info.input_given) 
	{
		pnl.setFilename(args_info.input_arg);
		if (strstr(args_info.input_arg,"owfn") || strstr(args_info.input_arg,"OWFN"))
			pnl.setNetType(PNLoader::OWFN);
		if (strstr(args_info.input_arg,"llnet") || strstr(args_info.input_arg,"LLNET"))
			pnl.setNetType(PNLoader::LOLA);
	}
	else pnl.setFilename("stdin");
	if (args_info.owfn_given) pnl.setNetType(PNLoader::OWFN);
	else if (args_info.lola_given) pnl.setNetType(PNLoader::LOLA);
	else if (args_info.pnml_given) pnl.setNetType(PNLoader::PNML);
	PetriNet* pn(pnl.getPetriNet()); // obtain the Petri net
	vector<Place*> pord(pnl.getPOrder()); // and its place ordering
	po = pord;
	Marking m1(pnl.getInitialMarking()); // get the initial marking
	IMatrix im(*pn); // and incidence matrix of the net
	Cora cora(*pn,m1,pnl.getPOrder(),pnl.getTOrder()); // obtain a solver unit
	cora.setDebug(args_info.debug_given); // set the debug mode

	if (args_info.marking_given || args_info.cover_given) // if a final marking has been given
	{
		char* seq((args_info.marking_given ? args_info.marking_arg : args_info.cover_arg)); // get the string containing it
		vector<char*> subargs; // and split it, one string for each place
		char* tok = strtok(seq," ,;.");
		while (tok!=NULL) { subargs.push_back(tok); tok = strtok(NULL," ,;."); }
		for(unsigned int k=0; k<subargs.size(); ++k) // then go through all the places given
		{
			tok = strtok(subargs[k],":"); // to get their name
			Place* p(pn->findPlace(tok));
			if (p==NULL) abort(2,"wrong token '%s' in input marking",tok);
			tok = strtok(NULL," ,;.:"); // and their token number
			int i(1);
			if (tok!=NULL) i=atoi(tok);
			pnl.setFinal(p->getName(),i); // and set that in the final marking accordingly
		}		
	}

	Marking tmp(pnl.getFinalMarking()); // now get the final marking altogether
	deque<Transition*> path; // for storing a path in the coverability graph
	if (args_info.sequence_given) // if a sequence leading to a node (instead of a final marking) is given
	{
		char* seq(args_info.sequence_arg); // get the sequence of interest in the coverability graph
		char* tok = strtok(seq," :,;"); // split the sequence into tokens (transitions)
		vector<Transition*> tvec;
		while (tok!=NULL) { // create a vector of transitions from the sequence
			Transition* t(pn->findTransition(tok));
			if (t==NULL) abort(3,"wrong token '%s' in input transition sequence",tok);
			tvec.push_back(t);
			tok = strtok(NULL," :,;");
		}
		path = cora.findPath(tvec); // and finally look for a firable path to the same node, uses node splitting
	}
	else if (args_info.marking_given) path = cora.findReach(tmp); // try to reach a marking, uses node splitting
	else if (args_info.cover_given) path = cora.findCover(tmp); // try to cover a marking, uses path pumping

	if (args_info.verbose_given && !args_info.cover_given) 
	{ // print the final coverability graph including the node splitting
		cout << "Partial Coverability Graph with Split Nodes:" << endl;
		cora.printGraph(false);
	}
	if (args_info.verbose_given || args_info.fullgraph_given) 
	{ // print the coverability graph without the node splitting
		cout << endl << "Original "; // now print the "normal" coverability graph
		if (!args_info.fullgraph_given) cout << "Partial "; // but only those portions we have built
		cout << "Coverability Graph:" << endl;
		if (args_info.fullgraph_given) cora.buildFullGraph(); // if the user wants the full graph, we first have to complete it
		cora.printGraph(true);
		cout << endl;
	}

	if (path.empty() || path.at(0)!=NULL) { // a solution was found, so print it
		if (args_info.marking_given || args_info.cover_given || args_info.sequence_given || args_info.verbose_given)
		{
			cout << "SOLUTION(" << path.size() << "): ";
			for(unsigned int x=0; x<path.size(); ++x)
				cout << path[x]->getName() << " ";
			cout << endl;
		}
	}
	else if (path.size()>1) cout << "UNSOLVABLE: The given path does not exists in the coverability graph." << endl;
	else cout << "NO SOLUTION" << endl; // no solution was found

	if (args_info.verbose_given) // with -v print the number of tries (paths) we searched
	{
		if (args_info.marking_given) cora.printStatus(Cora::REACH);
		else if (args_info.cover_given) cora.printStatus(Cora::COVER);
		else if (args_info.sequence_given) cora.printStatus(Cora::PATH);
	}

	clock_t endtime = clock();
	if (args_info.time_given) // print time use if --time was specified
		cout << "cora: Used " << (float)(endtime-starttime)/CLOCKS_PER_SEC << " seconds overall." << endl;
}

/*************************
* REDIRECTING OUTPUT/LOG *
*************************/
if (args_info.output_given) std::cout.rdbuf(cout_sbuf); // restore the original stream buffer
if (args_info.log_given) std::cerr.rdbuf(cerr_sbuf); // restore the original stream buffer


    return EXIT_SUCCESS;
}

