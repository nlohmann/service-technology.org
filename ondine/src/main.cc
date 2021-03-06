/*****************************************************************************\
 Ondine -- Divides open nets into two components with as small as possible interfaces

 Copyright (c) 2011 Harro Wimmel

 Ondine is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Ondine is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Ondine.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

// include header files
#include <config.h>
#include <ctime>
#include <libgen.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>

#include <pnapi/pnapi.h>
#include "config-log.h"
#include "cmdline.h"
#include "Output.h"
#include "verbose.h"

using namespace pnapi;
using std::vector;
using std::set;
using std::map;
using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::stringstream;
using std::ostream;
using std::ofstream;
using std::ifstream;

// call for the PB-MiniSAT solver (number of variable, optimisation function, constraints with lhs, operator, and rhs)
extern map<string,bool> pbminisat(int vars, map<string,int>& goal, vector<map<string,int> >& constr, vector<int>& ineq, vector<int>& rhs);
// for a given (new) interface sort the places by input/output and collect all the transitions in the component (one transition must be given)
void completeTransitions(set<Transition*>& tset, const set<Place*>& interface, set<Place*>& input, set<Place*>& output);
// get one transition that must belong to the other component
Transition* findOneOppositeTransition(const set<Transition*>& tset, const set<Place*>& interface);
// collect all transitions adjacent to the given sets of inner and interface places
set<Transition*> collectAdjacentTransitions(const set<Place*>& pset, const set<Label*>& lset);
// remove places and adjacent arcs from a net
void deletePlaces(PetriNet& net, const set<Place*>& pset);
// delete transitions and adjacent arcs from a net
void deleteTransitions(PetriNet& net, const set<Transition*>& tset);
// shift a set of places from the inner to the interface
void moveToInterface(PetriNet& net, Port& port, const set<Place*>& pset, bool in);

/// the command line parameters
gengetopt_args_info args_info;

/// verbose flag
bool flag_verbose(false);

/// the invocation string
std::string invocation;

/// a file to store a mapping from marking ids to actual Petri net markings
Output* markingoutput = NULL;

/// a variable holding the time of the call
clock_t start_clock = clock();

/// check if a file exists and can be opened for reading
inline bool fileExists(const std::string& filename) {
    std::ifstream tmp(filename.c_str(), std::ios_base::in);
    return tmp.good();
}

/// evaluate the command line parameters
void evaluateParameters(int argc, char** argv) {
    // overwrite invocation for consistent error messages
    argv[0] = basename(argv[0]);

    // store invocation in a std::string for meta information in file output
    for (int i = 0; i < argc; ++i) {
        invocation += std::string(argv[i]) + " ";
    }

    // initialize the parameters structure
    struct cmdline_parser_params* params = cmdline_parser_params_create();

    // call the cmdline parser
    if (cmdline_parser(argc, argv, &args_info) != 0) {
        abort(7, "invalid command-line parameter(s)");
    }

    // debug option
    if (args_info.bug_flag) {
        {
            Output debug_output("bug.log", "configuration information");
            debug_output.stream() << CONFIG_LOG << std::flush;
        }
        message("please send file 'bug.log' to %s!", PACKAGE_BUGREPORT);
        exit(EXIT_SUCCESS);
    }

    // read a configuration file if necessary
    if (args_info.config_given) {
        // initialize the config file parser
        params->initialize = 0;
        params->override = 0;

        // call the config file parser
        if (cmdline_parser_config_file(args_info.config_arg, &args_info, params) != 0) {
            abort(14, "error reading configuration file '%s'", args_info.config_arg);
        } else {
            status("using configuration file '%s'", args_info.config_arg);
        }
    } else {
        // check for configuration files
        std::string conf_generic_filename = std::string(PACKAGE) + ".conf";
        std::string conf_filename = fileExists(conf_generic_filename) ? conf_generic_filename :
                                    (fileExists(std::string(SYSCONFDIR) + "/" + conf_generic_filename) ?
                                     (std::string(SYSCONFDIR) + "/" + conf_generic_filename) : "");

        if (conf_filename != "") {
            // initialize the config file parser
            params->initialize = 0;
            params->override = 0;
            if (cmdline_parser_config_file(const_cast<char*>(conf_filename.c_str()), &args_info, params) != 0) {
                abort(14, "error reading configuration file '%s'", conf_filename.c_str());
            } else {
                status("using configuration file '%s'", conf_filename.c_str());
            }
        } else {
            status("not using a configuration file");
        }
    }


    // check whether at most one input and two output files are given
    // (the input file comes first)
    if (args_info.inputs_num > 3) {
        abort(4, "at most one input and two output files must be given");
    }

    free(params);
}


/// a function collecting calls to organize termination (close files, ...)
void terminationHandler() {
    /* [USER] Add code here */

    // print statistics
    if (args_info.stats_flag) {
        message("runtime: %.2f sec", (static_cast<double>(clock()) - static_cast<double>(start_clock)) / CLOCKS_PER_SEC);
        fprintf(stderr, "%s: memory consumption: ", PACKAGE);
        system((std::string("ps -o rss -o comm | ") + TOOL_GREP + " " + PACKAGE + " | " + TOOL_AWK + " '{ if ($1 > max) max = $1 } END { print max \" KB\" }' 1>&2").c_str());
    }
}

/// main-function
int main(int argc, char** argv) {

    // set the function to call on normal termination
    atexit(terminationHandler);

    /*--------------------------------------.
    | 0. parse the command line parameters  |
    `--------------------------------------*/
    evaluateParameters(argc, argv);
    Output::setTempfileTemplate(args_info.tmpfile_arg);
    Output::setKeepTempfiles(args_info.noClean_flag);
	flag_verbose = args_info.verbose_given;

    /*-----------------------.
    | 1. parse the Petri net |
    `-----------------------*/
    pnapi::PetriNet net;

    try {
        // parse either from standard input or from a given file
        if (args_info.inputs_num%2 == 0) {
            // parse the open net from standard input
            if (args_info.pnml_given) std::cin >> pnapi::io::pnml >> net;
            else if (args_info.lola_given) std::cin >> pnapi::io::lola >> net;
            else std::cin >> pnapi::io::owfn >> net;
        } else {
            // strip suffix from input filename (if necessary: uncomment next line for future use)
            //inputFilename = std::string(args_info.inputs[0]).substr(0, std::string(args_info.inputs[0]).find_last_of("."));

            // open input file as an input file stream
            std::ifstream inputStream(args_info.inputs[0]);
            // ... and abort, if an error occurs
            if (!inputStream) {
                abort(1, "could not open file '%s'", args_info.inputs[0]);
            }

            // parse the open net from the input file stream
			if (args_info.pnml_given)
	            inputStream >> meta(pnapi::io::INPUTFILE, args_info.inputs[0])
    	                    >> pnapi::io::pnml >> net;
			else if (args_info.lola_given)
	            inputStream >> meta(pnapi::io::INPUTFILE, args_info.inputs[0])
    	                    >> pnapi::io::lola >> net;
			else 
	            inputStream >> meta(pnapi::io::INPUTFILE, args_info.inputs[0])
    	                    >> pnapi::io::owfn >> net;
        }
        // additional output if verbose-flag is set
        if (args_info.verbose_flag) {
            std::ostringstream s;
            s << pnapi::io::stat << net;
            status("read net: %s", s.str().c_str());
        }
    } catch (const pnapi::exception::InputError& error) {
        std::ostringstream s;
        s << error;
        abort(2, "\b%s", s.str().c_str());
    }

    /*-------------------------------------------.
    | 2. Construct the formula for the PBSolver  |
    `-------------------------------------------*/

	// following is the code needed to create the PB-MiniSAT formula

	// the code could be better structured and moved into several methods,
	// but this would slow down execution as several loops would have to be
	// copied and run more than once. There is also not much code that 
	// could be reused for optimisation

	vector<map<string,int> > cs; // PB constraints
	vector<int> ineq,rhs; // with inequality operator and right hand side
	set<pnapi::Arc*> tmp,tmp2;
	set<pnapi::Arc*>::iterator ait,ait2;
	set<Place*> pset(net.getPlaces()); // all places of the net
	set<Place*> prei,posti; // internal places adjacent to pre/postset of current place
	set<Label*> lprei,lposti,ltmp; // adjacent interface places
	set<Place*>::iterator pit,pit2;
	set<Label*>::iterator lit,lit2;	
	for(pit=pset.begin(); pit!=pset.end(); ++pit)
	{
		map<string,int> singlecs;
		// variable name for io_i ("place i is in interface between the two components") 
		string name((*pit)->getName()+"_I");
		// namej as shortcut for x_i and not io_i ("place i is in first component but not in common interface")
		string namej((*pit)->getName()+"_J");
		// now construct this shortcut via three clauses
		// 1. x_i and not io_i -> namej_i  |(namej_i + io_i - x_i >= 0)
		singlecs[namej]=1;
		singlecs[name]=1;
		singlecs[(*pit)->getName()]=-1;
		cs.push_back(singlecs);
		ineq.push_back(1);
		rhs.push_back(0);
		// 2. namej_i -> x_i  |(x_i - namej_i >= 0)
		singlecs.clear();		
		singlecs[namej]=-1;
		singlecs[(*pit)->getName()]=1;
		cs.push_back(singlecs);
		ineq.push_back(1);
		rhs.push_back(0);
		// 3. namej_i -> not io_i  |(- namej_i - io_i >= -1)
		singlecs.clear();		
		singlecs[namej]=-1;
		singlecs[name]=-1;
		cs.push_back(singlecs);
		ineq.push_back(1);
		rhs.push_back(-1);

		// get the pre- and postsets (places and interface labels)
		// preset here: all places reachable by going backwards 1 arc, 
		// then 1 arc in arbitrary direction
		// postset here: 1 forward arc, then 1 arc arbitrarily
		prei.clear(); // preset, inner places
		posti.clear(); // postset, inner places
		lprei.clear(); // preset, labels
		lposti.clear(); // postset, labels
		tmp = (*pit)->getPresetArcs(); // go through transitions in the preset of the current place
		for(ait=tmp.begin(); ait!=tmp.end(); ++ait)
		{
			// collect all inner places in the preset ...
			Transition* t(&((*ait)->getTransition()));
			tmp2 = t->getPresetArcs();
			for(ait2=tmp2.begin(); ait2!=tmp2.end(); ++ait2)
				prei.insert(&((*ait2)->getPlace()));
			// ... and those in the postset
			tmp2 = t->getPostsetArcs();
			for(ait2=tmp2.begin(); ait2!=tmp2.end(); ++ait2)
				if (*pit!=&((*ait2)->getPlace()))
					prei.insert(&((*ait2)->getPlace()));
			// as well as the interface place, input ...
			ltmp = t->getInputLabels();
			for(lit=ltmp.begin(); lit!=ltmp.end(); ++lit)
				lprei.insert(*lit);
			// ... and output
			ltmp = t->getOutputLabels();
			for(lit=ltmp.begin(); lit!=ltmp.end(); ++lit)
				lprei.insert(*lit);
		}
		// ban the current place from the interface if it has a loop
		if (prei.find(*pit)!=prei.end()) {
			singlecs.clear();
			singlecs[name]=-1;
			cs.push_back(singlecs);
			ineq.push_back(1);
			rhs.push_back(0);
		}
		// but do not collect the current place itself
		prei.erase(*pit);
		// now do the same for the postset of the current place
		tmp = (*pit)->getPostsetArcs();
		for(ait=tmp.begin(); ait!=tmp.end(); ++ait)
		{
			Transition* t(&((*ait)->getTransition()));
			tmp2 = t->getPresetArcs();
			for(ait2=tmp2.begin(); ait2!=tmp2.end(); ++ait2)
				posti.insert(&((*ait2)->getPlace()));
			tmp2 = t->getPostsetArcs();
			for(ait2=tmp2.begin(); ait2!=tmp2.end(); ++ait2)
				posti.insert(&((*ait2)->getPlace()));
			ltmp = t->getInputLabels();
			for(lit=ltmp.begin(); lit!=ltmp.end(); ++lit)
				lposti.insert(*lit);
			ltmp = t->getOutputLabels();
			for(lit=ltmp.begin(); lit!=ltmp.end(); ++lit)
				lposti.insert(*lit);
		}
		posti.erase(*pit);
		// component2 implication for io_i-pre/postset
		// (c_i: all places in PRE of the new interface are in component 2) 
		// not c_i -> OR_{j in PRE(i)} (x_j and not io_j)  |(c_i + sum_{j in PRE(i)} (x_j and not io_j) >= 1)
		singlecs.clear();
		singlecs["c_"+(*pit)->getName()]=1;
		for(pit2=prei.begin(); pit2!=prei.end(); ++pit2)
			singlecs[(*pit2)->getName()+"_J"]=1;
		for(lit2=lprei.begin(); lit2!=lprei.end(); ++lit2)
			singlecs[(*lit2)->getName()]=1;
		cs.push_back(singlecs);
		ineq.push_back(1);
		rhs.push_back(1);
		// now go over PRE(i) for the reverse clauses, yielding c_i -> not OR_{j in PRE(i)} (x_j and not io_j)
		for(pit2=prei.begin(); pit2!=prei.end(); ++pit2)
		{
			// c_i -> not(x_j and not io_j)  |(-c_i -x_j +io_j >= -1)
			singlecs.clear();
			singlecs["c_"+(*pit)->getName()]=-1;
			singlecs[(*pit2)->getName()]=-1;
			singlecs[(*pit2)->getName()+"_I"]=1;
			cs.push_back(singlecs);
			ineq.push_back(1);
			rhs.push_back(-1);
		}
		for(lit2=lprei.begin(); lit2!=lprei.end(); ++lit2)
		{
			// c_i -> not(x_j and not io_j)  |(-c_i -x_j +io_j >= -1)
			// as we are in the former interface, io_j can never be set anyway)
			singlecs.clear();
			singlecs["c_"+(*pit)->getName()]=-1;
			singlecs[(*lit2)->getName()]=-1;
			cs.push_back(singlecs);
			ineq.push_back(1);
			rhs.push_back(-1);
		}
		// (d_i: all places in POST of the new interface are in component 2) 
		// not d_i -> OR_{j in POST(i)} (x_j and not io_j)  |(d_i + sum_{j in POST(i)} (x_j and not io_j) >= 1)
		singlecs.clear();
		singlecs["d_"+(*pit)->getName()]=1;
		for(pit2=posti.begin(); pit2!=posti.end(); ++pit2)
			singlecs[(*pit2)->getName()+"_J"]=1;
		for(lit2=lposti.begin(); lit2!=lposti.end(); ++lit2)
			singlecs[(*lit2)->getName()]=1;
		cs.push_back(singlecs);
		ineq.push_back(1);
		rhs.push_back(1);
		// now go over POST(i) for the reverse clauses
		for(pit2=posti.begin(); pit2!=posti.end(); ++pit2)
		{
			// d_i -> not(x_j and not io_j)  |(-d_i -x_j +io_j >= -1)
			singlecs.clear();
			singlecs["d_"+(*pit)->getName()]=-1;
			singlecs[(*pit2)->getName()]=-1;
			singlecs[(*pit2)->getName()+"_I"]=1;
			cs.push_back(singlecs);
			ineq.push_back(1);
			rhs.push_back(-1);
		}
		for(lit2=lposti.begin(); lit2!=lposti.end(); ++lit2)
		{
			// d_i -> not(x_j and not io_j)  |(-d_i -x_j +io_j >= -1)
			// as we are in the former interface, io_j can never be set anyway)
			singlecs.clear();
			singlecs["d_"+(*pit)->getName()]=-1;
			singlecs[(*lit2)->getName()]=-1;
			cs.push_back(singlecs);
			ineq.push_back(1);
			rhs.push_back(-1);
		}
		// for new interface places, either c_i or d_i must hold
		// io_i -> c_i or d_i  |(c_i + d_i - io_i >= 0)
		singlecs.clear();
		singlecs[name]=-1;
		singlecs["c_"+(*pit)->getName()]=1;
		singlecs["d_"+(*pit)->getName()]=1;
		cs.push_back(singlecs);
		ineq.push_back(1);
		rhs.push_back(0);

		// implication io_i -> x_i, every new interface place is in component 1
		singlecs.clear();
		singlecs[name]=-1;
		singlecs[(*pit)->getName()]=1;
		cs.push_back(singlecs);
		ineq.push_back(1);
		rhs.push_back(0);

		// implications for x_i and not io_i resp. not x_i (places adjacent to inner places)
		// here: if the current place is an inner place, adjacent places cannot be inner places of the other component
		for(pit2=prei.begin(); pit2!=prei.end(); ++pit2)
		{
			// not x_i or io_i or x_j
			singlecs.clear();
			singlecs[name]=1;
			singlecs[(*pit)->getName()]=-1;
			singlecs[(*pit2)->getName()]=1;
			cs.push_back(singlecs);
			ineq.push_back(1);
			rhs.push_back(0);
			// x_i or not x_j or io_j
			singlecs.clear();
			singlecs[(*pit)->getName()]=1;
			singlecs[(*pit2)->getName()]=-1;
			singlecs[(*pit2)->getName()+"_I"]=1;
			cs.push_back(singlecs);
			ineq.push_back(1);
			rhs.push_back(0);
		}
		for(pit2=posti.begin(); pit2!=posti.end(); ++pit2)
		{
			// not x_i or io_i or x_j
			singlecs.clear();
			singlecs[name]=1;
			singlecs[(*pit)->getName()]=-1;
			singlecs[(*pit2)->getName()]=1;
			cs.push_back(singlecs);
			ineq.push_back(1);
			rhs.push_back(0);
			// x_i or not x_j or io_j
			singlecs.clear();
			singlecs[(*pit)->getName()]=1;
			singlecs[(*pit2)->getName()]=-1;
			singlecs[(*pit2)->getName()+"_I"]=1;
			cs.push_back(singlecs);
			ineq.push_back(1);
			rhs.push_back(0);
		}
		for(lit2=lprei.begin(); lit2!=lprei.end(); ++lit2)
		{
			// not x_i or io_i or x_j
			singlecs.clear();
			singlecs[name]=1;
			singlecs[(*pit)->getName()]=-1;
			singlecs[(*lit2)->getName()]=1;
			cs.push_back(singlecs);
			ineq.push_back(1);
			rhs.push_back(0);
			// x_i or not x_j
			singlecs.clear();
			singlecs[(*pit)->getName()]=1;
			singlecs[(*lit2)->getName()]=-1;
			cs.push_back(singlecs);
			ineq.push_back(1);
			rhs.push_back(0);
		}
		for(lit2=lposti.begin(); lit2!=lposti.end(); ++lit2)
		{
			// not x_i or io_i or x_j
			singlecs.clear();
			singlecs[name]=1;
			singlecs[(*pit)->getName()]=-1;
			singlecs[(*lit2)->getName()]=1;
			cs.push_back(singlecs);
			ineq.push_back(1);
			rhs.push_back(0);
			// x_i or not x_j
			singlecs.clear();
			singlecs[(*pit)->getName()]=1;
			singlecs[(*lit2)->getName()]=-1;
			cs.push_back(singlecs);
			ineq.push_back(1);
			rhs.push_back(0);
		}

		// implication for io_i, pre/postset in component 1
		// (a_i: all places in PRE of the new interface are in component 1) 
		// not a_i -> OR_{j in PRE(i)} not x_j
		singlecs.clear();
		singlecs["a_"+(*pit)->getName()]=1;
		for(pit2=prei.begin(); pit2!=prei.end(); ++pit2)
			singlecs[(*pit2)->getName()]=-1;
		for(lit2=lprei.begin(); lit2!=lprei.end(); ++lit2)
			singlecs[(*lit2)->getName()]=-1;
		cs.push_back(singlecs);
		ineq.push_back(1);
		rhs.push_back(1-prei.size()-lprei.size());
		// a_i -> AND_{j in PRE(i)} x_j
		for(pit2=prei.begin(); pit2!=prei.end(); ++pit2)
		{
			singlecs.clear();
			singlecs["a_"+(*pit)->getName()]=-1;
			singlecs[(*pit2)->getName()]=1;
			cs.push_back(singlecs);
			ineq.push_back(1);
			rhs.push_back(0);
		}
		for(lit2=lprei.begin(); lit2!=lprei.end(); ++lit2)
		{
			singlecs.clear();
			singlecs["a_"+(*pit)->getName()]=-1;
			singlecs[(*lit2)->getName()]=1;
			cs.push_back(singlecs);
			ineq.push_back(1);
			rhs.push_back(0);
		}
		// (b_i: all places in POST of the new interface are in component 1) 
		// not b_i -> OR_{j in POST(i)} not x_j
		singlecs.clear();
		singlecs["b_"+(*pit)->getName()]=1;
		for(pit2=posti.begin(); pit2!=posti.end(); ++pit2)
			singlecs[(*pit2)->getName()]=-1;
		for(lit2=lposti.begin(); lit2!=lposti.end(); ++lit2)
			singlecs[(*lit2)->getName()]=-1;
		cs.push_back(singlecs);
		ineq.push_back(1);
		rhs.push_back(1-posti.size()-lposti.size());
		// b_i -> AND_{j in POST(i)} x_j
		for(pit2=posti.begin(); pit2!=posti.end(); ++pit2)
		{
			singlecs.clear();
			singlecs["b_"+(*pit)->getName()]=-1;
			singlecs[(*pit2)->getName()]=1;
			cs.push_back(singlecs);
			ineq.push_back(1);
			rhs.push_back(0);
		}
		for(lit2=lposti.begin(); lit2!=lposti.end(); ++lit2)
		{
			singlecs.clear();
			singlecs["b_"+(*pit)->getName()]=-1;
			singlecs[(*lit2)->getName()]=1;
			cs.push_back(singlecs);
			ineq.push_back(1);
			rhs.push_back(0);
		}
		// io_i -> a_i or b_i, at least one of pre/postset of a new interface place is fully contained in component 1 
		singlecs.clear();
		singlecs["a_"+(*pit)->getName()]=1;
		singlecs["b_"+(*pit)->getName()]=1;
		singlecs[name]=-1;
		cs.push_back(singlecs);
		ineq.push_back(1);
		rhs.push_back(0);
	}
	map<string,int> singlecs;
	set<Label*> lin(net.getInterface().getInputLabels());
	set<Label*> lout(net.getInterface().getOutputLabels());
	set<Label*> lall(lin);
	for(lit=lout.begin(); lit!=lout.end(); ++lit) lall.insert(*lit);
	// implications for x_i and not io_i resp. not x_i (places adjacent to inner places are in the same component)
	// here: if the current place is in the old interface, adjacent places cannot be in the inner of the other component
	for(lit=lall.begin(); lit!=lall.end(); ++lit)
	{
		set<Transition*> tset((*lit)->getTransitions());
		set<Transition*>::iterator tit;
		set<Place*> pset2;
		set<Label*> lset2;
		for(tit=tset.begin(); tit!=tset.end(); ++tit)
		{
			tmp2 = (*tit)->getPresetArcs();
			for(ait2=tmp2.begin(); ait2!=tmp2.end(); ++ait2)
				pset2.insert(&((*ait2)->getPlace()));
			tmp2 = (*tit)->getPostsetArcs();
			for(ait2=tmp2.begin(); ait2!=tmp2.end(); ++ait2)
				pset2.insert(&((*ait2)->getPlace()));
			ltmp = (*tit)->getInputLabels();
			for(lit2=ltmp.begin(); lit2!=ltmp.end(); ++lit2)
				lset2.insert(*lit2);
			ltmp = (*tit)->getOutputLabels();
			for(lit2=ltmp.begin(); lit2!=ltmp.end(); ++lit2)
				lset2.insert(*lit2);
		}
		lset2.erase(*lit);
		for(pit2=pset2.begin(); pit2!=pset2.end(); ++pit2)
		{
			// not x_i or x_j
			singlecs.clear();
			singlecs[(*lit)->getName()]=-1;
			singlecs[(*pit2)->getName()]=1;
			cs.push_back(singlecs);
			ineq.push_back(1);
			rhs.push_back(0);
			// x_i or not x_j or io_j
			singlecs.clear();
			singlecs[(*lit)->getName()]=1;
			singlecs[(*pit2)->getName()]=-1;
			singlecs[(*pit2)->getName()+"_I"]=1;
			cs.push_back(singlecs);
			ineq.push_back(1);
			rhs.push_back(0);
		}
		for(lit2=lset2.begin(); lit2!=lset2.end(); ++lit2)
		{
			// not x_i or x_j
			singlecs.clear();
			singlecs[(*lit)->getName()]=-1;
			singlecs[(*lit2)->getName()]=1;
			cs.push_back(singlecs);
			ineq.push_back(1);
			rhs.push_back(0);
			// x_i or not x_j
			singlecs.clear();
			singlecs[(*lit)->getName()]=1;
			singlecs[(*lit2)->getName()]=-1;
			cs.push_back(singlecs);
			ineq.push_back(1);
			rhs.push_back(0);
		}
	}

	// Ensure at least one element per component
		// 1.component: one direct variable is positive or one new element with nonempty pre- And postset in the interface
	singlecs.clear();
	for(pit=pset.begin(); pit!=pset.end(); ++pit) singlecs[(*pit)->getName()+"_J"]=1;
	for(lit=lall.begin(); lit!=lall.end(); ++lit) singlecs[(*lit)->getName()]=1;
	// a place in the new interface must have non-empty pre- and postset
	// so places with empty pre- or postset will not be in the interface (due to optimisation)
	for(pit=pset.begin(); pit!=pset.end(); ++pit)
		if ((*pit)->getPreset().size()>0 && (*pit)->getPostset().size()>0)
			singlecs[(*pit)->getName()+"_I"]=1; 
	cs.push_back(singlecs);
	ineq.push_back(1);
	rhs.push_back(1);
		// 2.component: one direct variable negative oder one new element with nonempty pre- AND postset in the interface
	singlecs.clear();
	for(pit=pset.begin(); pit!=pset.end(); ++pit) singlecs[(*pit)->getName()]=-1;
	for(lit=lall.begin(); lit!=lall.end(); ++lit) singlecs[(*lit)->getName()]=-1;
	for(pit=pset.begin(); pit!=pset.end(); ++pit)
		if ((*pit)->getPreset().size()>0 && (*pit)->getPostset().size()>0)
			singlecs[(*pit)->getName()+"_I"]=1; 
	cs.push_back(singlecs);
	ineq.push_back(1);
	rhs.push_back(1-pset.size()-lall.size());

	// selection of the first component: the component with more (old) interface places
	singlecs.clear();
	if (!lall.empty()) {
		for(lit=lall.begin(); lit!=lall.end(); ++lit) singlecs[(*lit)->getName()]=1;
		cs.push_back(singlecs);
		ineq.push_back(1);
		rhs.push_back((lall.size()+1)/2);
	}
	// minimisation: sum of old and new interface as small as possible for component 1
	for(pit=pset.begin(); pit!=pset.end(); ++pit)
	{
		string name((*pit)->getName()+"_I");
		singlecs[name]=1;
	}
	// singlecs contains goal function now
	// we count the variables used:
	int numvars(7*pset.size()+lall.size());
	map<string,bool> res;
	res = pbminisat(numvars,singlecs,cs,ineq,rhs); // solve PB-SAT problem
	// if the result vector contains the empty string, no solution was found ...
	if (res.find("")!=res.end())
	{
		// either because there is none or because there was a failure
		if (res[""]) abort(10,"net cannot be split into components");
		else abort(12,"failure of MiniSAT+");
	}
        if (args_info.verbose_flag) status("create formula");

    /*------------------------------------------------.
    | 3. Collect places/transitions from the formula  |
    `------------------------------------------------*/

	// the formula contains only the names of places and some additional variables;
	// to construct a component we must first collect the names of the places and
	// determine which transitions belong to it (not part of the formula)
	set<Place*> cp1,cp2,ci,ci1,ci2;
	set<Label*> cl1,cl2;
	set<Transition*> ct1,ct2;
	// First determine for places (inner and interface) where they belong:
	// cp1=only component 1, ci=interface of 1+2, cp2=only component 2
	// cl1,cl2=old interface, only in component 1/2
	map<string,bool>::iterator rit;
	for(rit=res.begin(); rit!=res.end(); ++rit)
	{
		// test if a variable name is an inner/interface place name
		Place* p(net.findPlace(rit->first));
		Label* l(net.getInterface().findLabel(rit->first));
		if (p)
		{
			// for inner places, check which component they belong to
			if (rit->second)
			{
				// and possibly if they are in the new interface
				if (res[rit->first+"_I"]) ci.insert(p);
				else cp1.insert(p);
			}
			else cp2.insert(p);
		}
		else if (l)
		{
			// for old interface places, check to which component they belong
			if (rit->second) cl1.insert(l);
			else cl2.insert(l);
		}
	}

	// Determine transitions for the components.
	// first, collect all transitions adjacent to places that are in one component only
	ct1 = collectAdjacentTransitions(cp1,cl1);
	ct2 = collectAdjacentTransitions(cp2,cl2);
	// if there are only new interface places, select one arbitrary transition into component 1
	if (cp1.empty() && cl1.empty() && cp2.empty() && cl2.empty())
	{
		// obtain an arbitrary transition and put it into component 1
		Transition* t(*(net.getTransitions().begin()));
		ct1.insert(t);
	}
	// further transitions may be in a component if they are connected to the interface
	// between the components only. To which component they belong depends on the
	// arc directions, which have to be the same as for the other 
	// transitions in that component, regarding
	// the interface. If the transition set is empty so far, we must first determine
	// any one transition in the component by crossing the interface from a transition
	// in the other component.
	if (ct1.empty()) ct1.insert(findOneOppositeTransition(ct2,ci));
	// now determine the transitions missing so far (connected to the new interface only)
	completeTransitions(ct1,ci,ci1,ci2);
	// the same as above, but for the other component
	if (ct2.empty()) ct2.insert(findOneOppositeTransition(ct1,ci));
	completeTransitions(ct2,ci,ci2,ci1);
        // additional output if verbose-flag is set
        if (args_info.verbose_flag) status("collect places and transitions");

    /*---------------------------------.
    | 4. Construct the two components  |
    `---------------------------------*/

	// we construct the components by copying the original net,
	// deleting inner elements of the other components, and
	// modifying the places of the new part of the interface
	PetriNet c1(net); // component 1
	// Delete all inner places not in component 1
	deletePlaces(c1,cp2);
	// Delete all transitions not in component 1
	deleteTransitions(c1,ct2);
	// Create a new port, this will contain the new part of the interface
	string portname("compose");
	if (args_info.inputs_num>1) 
	{
		string tmpname1(args_info.inputs[1]);
		string name1(tmpname1.substr(0, string(tmpname1).find_last_of(".")));
		string tmpname2(args_info.inputs[2]);
		string name2(tmpname2.substr(0, string(tmpname2).find_last_of(".")));
		portname+="_"+name1+"+"+name2;
	}
	if (args_info.inputs_num%2==1)
	{
		string tmpname0(args_info.inputs[0]);
		string name0(tmpname0.substr(0, string(tmpname0).find_last_of(".")));
		portname += "_to_"+name0;
	}
	Port* myport(&(c1.getInterface().addPort(portname)));
	// Replace places entering the interface by using labels (input places)
	moveToInterface(c1,*myport,ci1,true);
	moveToInterface(c1,*myport,ci2,false);
	// Remove interface places (labels) appearing in component 2 only
	for(lit=cl2.begin(); lit!=cl2.end(); ++lit)
	{
		Port* port(c1.getInterface().getPort((*lit)->getPort().getName()));
		port->removeLabel((*lit)->getName());
		// delete a port if it becomes empty
		if (port->isEmpty()) myport=&(c1.getInterface().mergePorts(*myport,*port));
	}
        // additional output if verbose-flag is set
        if (args_info.verbose_flag) {
            std::ostringstream s;
            s << pnapi::io::stat << c1;
            status("create first component: %s", s.str().c_str());
        }

	PetriNet c2(net); // component 2
	// Delete all inner places not in component 2
	deletePlaces(c2,cp1);
	// Delete all transitions not in component 2
	deleteTransitions(c2,ct1);
	// Create a new port, with the same name as for the first component
	Port* myport2(&(c2.getInterface().addPort(portname)));
	// Replace places entering the interface by labels (input=ci2, output=ci1)
	moveToInterface(c2,*myport2,ci2,true);
	moveToInterface(c2,*myport2,ci1,false);
	// Remove interface places (labels) appearing in component 1 only
	for(lit=cl1.begin(); lit!=cl1.end(); ++lit)
	{
		Port* port(c2.getInterface().getPort((*lit)->getPort().getName()));
		port->removeLabel((*lit)->getName());
		// delete a port if it becomes empty
		if (port->isEmpty()) myport2=&(c2.getInterface().mergePorts(*myport2,*port));
	}
        // additional output if verbose-flag is set
        if (args_info.verbose_flag) {
            std::ostringstream s;
            s << pnapi::io::stat << c1;
            status("create second component: %s", s.str().c_str());
        }

    /*--------------------------------.
    | 5. Write the components to file |
    `--------------------------------*/

        // write either to standard output or to given files
        if (args_info.inputs_num < 2) {
            // parse the open nets to standard output
            cout << pnapi::io::owfn << c1;
	    cout << endl;
            cout << pnapi::io::owfn << c2;
        } else {
            int sn(args_info.inputs_num%2); // where the first output file can be found
            // open output file as an output file stream
            std::ofstream outputStream(args_info.inputs[sn]);
            // ... and abort, if an error occurs
            if (!outputStream) {
                abort(1, "could not open file '%s'", args_info.inputs[sn]);
            }
            // print the first component to the output file stream
	            outputStream << meta(pnapi::io::OUTPUTFILE, args_info.inputs[sn])
    	                    << pnapi::io::owfn << c1;
            // open second output file as an output file stream
            std::ofstream outputStream2(args_info.inputs[sn+1]);
            // ... and abort, if an error occurs
            if (!outputStream2) {
                abort(1, "could not open file '%s'", args_info.inputs[sn+1]);
            }
            // print the second component to the output file stream
	            outputStream2 << meta(pnapi::io::OUTPUTFILE, args_info.inputs[sn+1])
    	                    << pnapi::io::owfn << c2;
        }
        // additional output if verbose-flag is set
        if (args_info.verbose_flag) {
            std::ostringstream s;
            s << pnapi::io::stat << c1;
            status("write net: %s", s.str().c_str());
            std::ostringstream s2;
            s2 << pnapi::io::stat << c2;
            status("write net: %s", s2.str().c_str());
        }

    return EXIT_SUCCESS;
}

/** Complete the set of transitions belonging to a component and determine if interface places are input or output.
	@param tset A set of transition to be filled. Initially containing at least all inner transitions (not connected to the new interface)
	@param interface The new portion of the interface.
	@param input A initially empty set that will finally contain all input places in interface.
	@param output A initially empty set that will finally contain all output places in interface.
*/
void completeTransitions(set<Transition*>& tset, const set<Place*>& interface, set<Place*>& input, set<Place*>& output)
{
	set<Transition*> todo(tset);
	while (!todo.empty())
	{
		Transition* t(*(todo.begin()));
		todo.erase(t);
		set<pnapi::Arc*> arcs(t->getPresetArcs());
		set<pnapi::Arc*>::iterator ait,ait2;
		for(ait=arcs.begin(); ait!=arcs.end(); ++ait)
			if (interface.find(&((*ait)->getPlace()))!=interface.end())
			{
				input.insert(&((*ait)->getPlace()));
				set<pnapi::Arc*> arcs2((*ait)->getPlace().getPostsetArcs());
				for(ait2=arcs2.begin(); ait2!=arcs2.end(); ++ait2)
					if (tset.find(&((*ait2)->getTransition()))==tset.end())
					{
						tset.insert(&((*ait2)->getTransition()));
						todo.insert(&((*ait2)->getTransition()));
					}
			}
		arcs = t->getPostsetArcs();
		for(ait=arcs.begin(); ait!=arcs.end(); ++ait)
			if (interface.find(&((*ait)->getPlace()))!=interface.end())
			{
				output.insert(&((*ait)->getPlace()));
				set<pnapi::Arc*> arcs2((*ait)->getPlace().getPresetArcs());
				for(ait2=arcs2.begin(); ait2!=arcs2.end(); ++ait2)
					if (tset.find(&((*ait2)->getTransition()))==tset.end())
					{
						tset.insert(&((*ait2)->getTransition()));
						todo.insert(&((*ait2)->getTransition()));
					}
			}
	}
}

/** Find a transition not in this component.
	@param tset A set of transition belonging to this component. At least one transition must be adjacent to the new interface.
	@param interface The new portion of the interface.
	@return A transition contained in the other component.
*/
Transition* findOneOppositeTransition(const set<Transition*>& tset, const set<Place*>& interface)
{
	// tset must contain at least one transition connected to the common part of the interface
	set<Transition*>::iterator tit;
	for(tit=tset.begin(); tit!=tset.end(); ++tit)
	{
		set<pnapi::Arc*>::iterator ait;
		set<pnapi::Arc*> arcs((*tit)->getPresetArcs());
		for(ait=arcs.begin(); ait!=arcs.end(); ++ait)
		{
			Place* p(&((*ait)->getPlace()));
			if (interface.find(p)!=interface.end())
			{
				set<pnapi::Arc*> arcs2(p->getPresetArcs());
				return &((*(arcs2.begin()))->getTransition());
			}
		}
		arcs = (*tit)->getPostsetArcs();
		for(ait=arcs.begin(); ait!=arcs.end(); ++ait)
		{
			Place* p(&((*ait)->getPlace()));
			if (interface.find(p)!=interface.end())
			{
				set<pnapi::Arc*> arcs2(p->getPostsetArcs());
				return &((*(arcs2.begin()))->getTransition());
			}
		}
	}
	return NULL; // should never occur
}

/** Collect all transitions adjacent to a given set of places.
	@param pset The inner portion of the given set of places.
	@param lset The (old) interface portion of the given set of places.
	@return The adjacent transitions.
*/
set<Transition*> collectAdjacentTransitions(const set<Place*>& pset, const set<Label*>& lset)
{
	set<Transition*> tset;
	set<Place*>::iterator pit;
	for(pit=pset.begin(); pit!=pset.end(); ++pit)
	{
		set<pnapi::Arc*> arcs((*pit)->getPresetArcs());
		set<pnapi::Arc*>::iterator ait;
		for(ait=arcs.begin(); ait!=arcs.end(); ++ait)
			tset.insert(&((*ait)->getTransition()));
		arcs = (*pit)->getPostsetArcs();
		for(ait=arcs.begin(); ait!=arcs.end(); ++ait)
			tset.insert(&((*ait)->getTransition()));
	}
	set<Label*>::iterator lit;
	for(lit=lset.begin(); lit!=lset.end(); ++lit)
	{
		set<Transition*> ttmp((*lit)->getTransitions());
		set<Transition*>::iterator tit;
		for(tit=ttmp.begin(); tit!=ttmp.end(); ++tit)
			tset.insert(*tit);
	}
	return tset;
}

/** Remove a set of places from a net, including all adjacent arcs.
	@param net The Petri net.
	@param pset The set of (inner) places to be deleted.
*/
void deletePlaces(PetriNet& net, const set<Place*>& pset)
{
	set<Place*>::iterator pit;
	for(pit=pset.begin(); pit!=pset.end(); ++pit)
	{
		set<pnapi::Arc*>::iterator ait;
		set<pnapi::Arc*> arcs(net.findPlace((*pit)->getName())->getPresetArcs());
		for(ait=arcs.begin(); ait!=arcs.end(); ++ait) net.deleteArc(**ait);
		arcs = net.findPlace((*pit)->getName())->getPostsetArcs();
		for(ait=arcs.begin(); ait!=arcs.end(); ++ait) net.deleteArc(**ait);
		net.deletePlace(*(net.findPlace((*pit)->getName())));
	}
}

/** Remove a set of transitions from a net, including all adjacent arcs.
	@param net The Petri net.
	@param tset The transitions to be removed.
*/
void deleteTransitions(PetriNet& net, const set<Transition*>& tset)
{
	set<Transition*>::iterator tit;
	for(tit=tset.begin(); tit!=tset.end(); ++tit)
	{
		set<pnapi::Arc*>::iterator ait;
		set<pnapi::Arc*> arcs(net.findTransition((*tit)->getName())->getPresetArcs());
		for(ait=arcs.begin(); ait!=arcs.end(); ++ait) net.deleteArc(**ait);
		arcs = net.findTransition((*tit)->getName())->getPostsetArcs();
		for(ait=arcs.begin(); ait!=arcs.end(); ++ait) net.deleteArc(**ait);
		net.deleteTransition(*(net.findTransition((*tit)->getName())));
	}
}

/** Move a set of inner places to the interface, including all modifications needed
	for the adjacent arcs and transitions.
	@param net The Petri net in which to do this.
	@param port The port to which the interface places are to be added.
	@param pset The places to be moved.
	@param in Whether the new places should be input or output places.
*/ 
void moveToInterface(PetriNet& net, Port& port, const set<Place*>& pset, bool in)
{
	set<Place*>::iterator pit;
	for(pit=pset.begin(); pit!=pset.end(); ++pit)
	{
		map<Transition*,unsigned int> weight;
		set<pnapi::Arc*>::iterator ait;
		set<pnapi::Arc*> arcs;
		if (in) arcs=net.findPlace((*pit)->getName())->getPostsetArcs();
		else    arcs=net.findPlace((*pit)->getName())->getPresetArcs();
		for(ait=arcs.begin(); ait!=arcs.end(); ++ait)
		{
			weight[&((*ait)->getTransition())]=(*ait)->getWeight();
			net.deleteArc(**ait);
		}		
		net.deletePlace(*(net.findPlace((*pit)->getName())));
		Label& l0(port.addLabel((*pit)->getName(),(in?Label::INPUT:Label::OUTPUT)));
		map<Transition*,unsigned int>::iterator mit;
		for(mit=weight.begin(); mit!=weight.end(); ++mit)
		{
			l0.addTransition(*(mit->first));
			mit->first->addLabel(l0,mit->second);
		}
	}
}

/* <<-- CHANGE END -->> */
