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

#include "cmdline.h"
#include "verbose.h"

#ifndef PNAPI_PNAPI_H
#include "pnapi/pnapi.h"
#endif
#include "problem.h"

using std::cerr;
using std::cout;
using std::cin;
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

/// Global ordering of transitions for lp_solve and the PathFinder
vector<Transition*> transitionorder;
/// Global ordering of places for lp_solve and determining a scapegoat (in the stubborn set method)
vector<Place*> placeorder;
/// inverted transitionorder for back references
map<Transition*,int> revtorder;
/// inverted placeorder for back references
map<Place*,int> revporder;


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

	// evaluate command line
    evaluateParameters(argc, argv);


/***************************
* CONSTRUCT SINGLE PROBLEM *
***************************/

string in;
while (cin >> in) 
{
	char* seq = new char[strlen(in.c_str())+1];
	strcpy(seq,in.c_str()); 
	char* tok = strtok(seq," :,;"); 
	while (tok!=NULL) {
		char* filename(tok);
/*
		// print the filename
		cout << filename << endl;
*/

		// load the Petri net
		Problem pbl;
		pbl.setFilename(filename);
		int nettype(Problem::PNML);
		if (strstr(filename,"llnet") || strstr(filename,"LLNET")) nettype=Problem::LOLA;
		if (strstr(filename,"owfn") || strstr(filename,"OWFN")) nettype=Problem::OWFN;
		pbl.setNetType(nettype);
		PetriNet* pn(pbl.getPetriNet());
		Marking m(Marking(*pn,false));

		// get the final marking
		map<Place*,int> final;
		const set<Place*> places(pn->getPlaces());
		set<Place*>::const_iterator cit;
		for(cit=places.begin(); cit!=places.end(); ++cit)
			if ((*cit)->getPostset().empty()) final[(*cit)] = 1;

		// print the problem
		cout << "PROBLEM " << "final_marking_in_" << pbl.getFilename() << ":" << endl;
		cout << "GOAL REACHABILITY;" << endl;
		cout << "FILE " << pbl.getFilename() << " TYPE ";
		switch(nettype) {
			case Problem::OWFN: cout << "OWFN"; break;
			case Problem::LOLA: cout << "LOLA"; break;
			case Problem::PNML: cout << "PNML"; break;
		}
		cout << ";" << endl << "INITIAL ";
		bool first = true;
		map<const Place*,unsigned int>::const_iterator pit;
		for(pit=m.begin(); pit!=m.end(); ++pit)
			if (pit->second>0) {
				if (!first) cout << ",";
				cout << pit->first->getName() << ":" << pit->second;
				first = false;
			}
		cout << ";" << endl;
		cout << "FINAL ";
		first = true;
		map<Place*,int>::iterator fit;
		for(fit=final.begin(); fit!=final.end(); ++fit)
			if (fit->second>0) {
				if (!first) cout << ",";
				cout << fit->first->getName() << ":" << fit->second;
				first = false;
			}
		cout << ";" << endl;
		cout << endl;


		// on to the next filename
		tok = strtok(NULL," :,;");
	}
	delete seq;
}

    return EXIT_SUCCESS;
}

