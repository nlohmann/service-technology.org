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

	if (args_info.marking_given) // if a final marking has been given
	{
		char* seq(args_info.marking_arg); // get the string containing it
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
	ExtMarking tmp2(tmp); // and extend it
	StubbornSet stubset(pnl.getTOrder(),tmp,im); // obtain the stubborn set methods for reachability
	CoverGraph cg(*pn,im,m1,(args_info.marking_given?&stubset:NULL)); // create the coverability graph with one (initial) node
	CNode* c(cg.getInitial()); // get the initial node
	CNode* aim(c);

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
		if (!cg.createPath(tvec)) { // create the path formed by the given sequence in the coverability graph
			cout << "UNSOLVABLE: The given path does not exist in the coverability graph." << endl; 
			exit(EXIT_SUCCESS); 
		}
	
		// Since the path exists, we can now obtain the last node on this path, which is our aim
		for(unsigned int i=0; i<tvec.size(); ++i)
			aim = aim->getSuccessor(*(tvec[i]));
	}

	deque<Transition*> path; // for storing a path in the coverability graph
	bool done(false); // if we are done, i.e. the path is firable
	int loops = 0; // just for counting
	while (!done) {
		if (args_info.marking_given) path = cg.findPath(tmp2); // find a shortest path to our final marking
		else path = cg.findPath(*aim); // or find a shortest path to our goal node
		if (args_info.debug_given) { // print the graph as it looks now
			cout << "Graph Structure ";
			if (loops==0) cout << "BEFORE "; else cout << "AFTER ";
			cout << "split:" << endl;
			cg.printGraph(pord,false);
			cout << endl;
			cout << "Path to Aim: "; // print the path chosen
			for(unsigned int i=0; i<path.size(); ++i)
				if (path[i]) cout << path[i]->getName() << " "; else cout << "NULL ";
			cout << endl;
		}
//cout << "Path to Aim: "; // print the path chosen
//for(unsigned int i=0; i<path.size(); ++i)
//	if (path[i]) cout << path[i]->getName() << " "; else cout << "NULL ";
//cout << endl;
		if (path.size()==0) done = cg.splitPath(path); // just check for the final marking if it is within the initial node
		else if (path.at(0)==NULL) // no path was found, we need to add new nodes/edges
		{
			if (args_info.debug_given) cout << "ADDING NEW SUCCESSORS" << endl;
			if (!cg.completeOneNode()) break; // stop if there is no new node, we have failed to find a solution
		}
		else done = cg.splitPath(path); // there is a path, so split the nodes along the chosen path
		// either the path becomes/is firable or it is destructed and cannot be found again
		++loops;
	}
	if (args_info.verbose_given) { // print the final coverability graph including the node splitting
		cout << "Partial Coverability Graph with Split Nodes:" << endl;
		cg.printGraph(pord,false);
		cout << "Initial Node: ";
		c->getMarking().show(cout,pord);
		cout << endl;
	}
	if (args_info.verbose_given || args_info.fullgraph_given) { // print the coverability graph without the node splitting
		cout << endl << "Original "; // now print the "normal" coverability graph
		if (!args_info.fullgraph_given) cout << "Partial "; // but only those portions we have built
		cout << "Coverability Graph:" << endl;
		if (args_info.fullgraph_given) // if the user wants the full graph, we first have to complete it
		{
			cg.useStubbornSetMethod(NULL); // we need all successors everywhere
			set<RNode>& allnodes(cg.getAllNodes());
			set<RNode>::iterator rit;
			for(rit=allnodes.begin(); rit!=allnodes.end(); ++rit)
			{
				cg.pushToDo((*rit).cnode); // put out an order to revisit all nodes
				(*rit).cnode->computeFirables(*pn,im); // and all edges from there
			}
			cg.completeGraph(); // now complete the graph
		}
		cg.printGraph(pord,true);
		cout << "Initial Node: ";
		c->getMarking().show(cout,pord);
		cout << endl << endl;
	}

	if (done) { // a solution was found, so we print it
		if (args_info.marking_given || args_info.sequence_given || args_info.verbose_given)
		{
			cout << "SOLUTION(" << path.size() << "): ";
			for(unsigned int x=0; x<path.size(); ++x)
				cout << path[x]->getName() << " ";
			cout << endl;
		}
	}
	else cout << "NO SOLUTION" << endl; // no solution was found
	if (args_info.verbose_given) // with -v print the number of tries (paths) we searched
	{
		cout << loops << " paths to the ";
		if (args_info.marking_given) { cout << "marking "; tmp2.show(cout,pord); }
		else { cout << "node "; aim->getMarking().show(cout,pord); }
		cout << " were investigated." << endl;
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

