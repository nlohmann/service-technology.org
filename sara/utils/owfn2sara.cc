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
using pnapi::Node;

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


/******************************
* CONSTRUCT PROBLEM INSTANCES *
******************************/

string in;
while (cin >> in) 
{
	char* seq = new char[strlen(in.c_str())+1];
	strcpy(seq,in.c_str()); 
	char* tok = strtok(seq," :,;"); 
	while (tok!=NULL) {
		char* filename(tok);

		// load the Petri net
		Problem pbl;
		pbl.setFilename(filename);
		int nettype(Problem::PNML);
		if (strstr(filename,"llnet") || strstr(filename,"LLNET")
			|| strstr(filename, "lola") || strstr(filename,"LOLA")) nettype=Problem::LOLA;
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

		// adapt the net if necessary
		bool adapted(false);
		if (args_info.adapt_given)
		{
			const set<Node*> nodes(pn->getNodes());
			set<Node*>::const_iterator nit;
			for(nit=nodes.begin(); nit!=nodes.end(); ++nit)
			{
				char* nname = new char[strlen((*nit)->getName().c_str())+1];
				strcpy(nname,(*nit)->getName().c_str());
				for(unsigned int z=0; nname[z]; ++z)
				{
					switch (nname[z])
					{
						case ':': nname[z]='.'; adapted=true; break;
						case '+': nname[z]='P'; adapted=true; break;
						case '-': nname[z]='M'; adapted=true; break;
						case '<': nname[z]='('; adapted=true; break;
						case '>': nname[z]=')'; adapted=true; break;
						case ' ': nname[z]='_'; adapted=true; break;
						case ',': nname[z]='´'; adapted=true; break;
						case ';': nname[z]='|'; adapted=true; break;
					}
				}
				string newname(nname);
				try { (*nit)->setName(newname); }
				catch (pnapi::exception::UserCausedError error) {
					cerr << "sara: error: " << error << endl;
					cerr << "sara: renaming the node " << (*nit)->getName();
					cerr << " in the Petri net " << filename << " to ";
					cerr << newname << " failed; probably the new name already exists." << endl;
				}
				delete[] nname;
			}
			if (adapted)
			{
				pbl.setFilename(pbl.getFilename()+".adapt");
				pbl.savePetriNet();
			}
		}

		// print the problem
		if (args_info.quasilive_given)
		{
			// get all transitions
			set<Transition*> tset(pn->getTransitions());
			if (tset.empty()) cerr << "sara: no transitions in Petri net " << filename << endl;
			set<Transition*>::iterator it;
			for(it=tset.begin(); it!=tset.end(); ++it)
			{
				char* tname = new char[strlen((*it)->getName().c_str())+1];
				strcpy(tname,(*it)->getName().c_str());
				for(unsigned int z=0; z<strlen(tname); ++z)
					if (tname[z]==':') tname[z]='.'; 
				cout << "PROBLEM enabling_of_" << tname << "_in_" << filename << ":" << endl;
				delete[] tname;
				cout << "GOAL REACHABILITY;" << endl;
				cout << "FILE " << filename << (adapted?".adapt":"") << " TYPE ";
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
				cout << "FINAL COVER ";
				set<pnapi::Arc*> arcs((*it)->getPresetArcs());
				set<pnapi::Arc*>::iterator ait;
				first = true;
				for(ait=arcs.begin(); ait!=arcs.end(); ++ait) {
					if (!first) cout << ",";
					cout << (*ait)->getPlace().getName() << ">" << (*ait)->getWeight();
					first = false;
				}
				cout << ";" << endl;
				cout << "RESULT quasiliveness;" << endl;
				cout << endl;
			}
		}
		if (args_info.propercompletion_given)
		{
			cout << "PROBLEM " << "proper_completion_in_" << filename << ":" << endl;
			cout << "GOAL REACHABILITY;" << endl;
			cout << "FILE " << filename << (adapted?".adapt":"") << " TYPE ";
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
			cout << "FINAL COVER ";
			first = true;
			map<Place*,int>::iterator fit;
			int fsum(1);
			for(fit=final.begin(); fit!=final.end(); ++fit)
				if (fit->second>0) {
					if (!first) cout << ",";
					cout << fit->first->getName() << ">" << fit->second;
					fsum += fit->second;
					first = false;
				}
			cout << ";" << endl;
			cout << "CONSTRAINTS ";
			first = true;
			for(cit=places.begin(); cit!=places.end(); ++cit)
			{
				if (!first) cout << "+";
				cout << (*cit)->getName();
				first = false;
			}
			cout << ">" << fsum << ";" << endl;
			cout << "RESULT NEGATE proper_completion;" << endl;
			cout << endl;
		}
		if (args_info.lazysound_given)
		{
			cout << "PROBLEM " << "lazy_soundness_in_" << filename << ":" << endl;
			cout << "GOAL REACHABILITY;" << endl;
			cout << "FILE " << filename << (adapted?".adapt":"") << " TYPE ";
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
			cout << "FINAL COVER ";
			first = true;
			map<Place*,int>::iterator fit;
			for(fit=final.begin(); fit!=final.end(); ++fit)
				if (fit->second>0) {
					if (!first) cout << ",";
					cout << fit->first->getName() << ":" << fit->second;
					first = false;
				}
			cout << ";" << endl;
			cout << "RESULT lazy_soundness;" << endl;
			cout << endl;
		}
		if (args_info.relaxedsound_given)
		{
			// get all transitions
			set<Transition*> tset(pn->getTransitions());
			if (tset.empty()) cerr << "sara: no transitions in Petri net " << filename << endl;
			set<Transition*>::iterator it;
			for(it=tset.begin(); it!=tset.end(); ++it)
			{
				char* tname = new char[strlen((*it)->getName().c_str())+1];
				strcpy(tname,(*it)->getName().c_str());
				for(unsigned int z=0; z<strlen(tname); ++z)
					if (tname[z]==':') tname[z]='.';
				cout << "PROBLEM " << "relaxed_soundness_of_" << tname << "_in_" << filename << ":" << endl;
				delete[] tname;
				cout << "GOAL REACHABILITY;" << endl;
				cout << "FILE " << filename << (adapted?".adapt":"") << " TYPE ";
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
				cout << "CONSTRAINTS " << (*it)->getName() << ">1;" << endl;
				cout << "RESULT relaxed_soundness;" << endl;
				cout << endl;
			}
		}
		if (args_info.finalmarking_given)
		{
			cout << "PROBLEM " << "final_marking_in_" << filename << ":" << endl;
			cout << "GOAL REACHABILITY;" << endl;
			cout << "FILE " << filename << (adapted?".adapt":"") << " TYPE ";
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
			cout << "RESULT reachability_of_the_final_marking;" << endl;
			cout << endl;
		}

		// on to the next filename
		tok = strtok(NULL," :,;");
	}
	delete[] seq;
}

    return EXIT_SUCCESS;
}

