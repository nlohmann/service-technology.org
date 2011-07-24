/*****************************************************************************\
 Petia -- Place Equivalence Through (semipositive) Invariant Analysis

 Copyright (c) 2011 Harro Wimmel

 Petia is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Petia is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Petia.  If not, see <http://www.gnu.org/licenses/>.
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
#include "inveqrel.h"
#include "lpwrapper.h"

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


    // check whether at most one file is given
    if (args_info.inputs_num > 1) {
        abort(4, "at most one input file must be given");
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
        if (system((std::string("ps -o rss -o comm | ") + TOOL_GREP + " " + PACKAGE + " | " + TOOL_AWK + " '{ if ($1 > max) max = $1 } END { print max \" KB\" }' 1>&2").c_str())) {};
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
        if (args_info.inputs_num == 0) {
            // parse the open net from standard input
            if (args_info.owfn_given) std::cin >> pnapi::io::owfn >> net;
            else if (args_info.lola_given) std::cin >> pnapi::io::lola >> net;
            else std::cin >> pnapi::io::pnml >> net;
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
			if (args_info.owfn_given)
	            inputStream >> meta(pnapi::io::INPUTFILE, args_info.inputs[0])
    	                    >> pnapi::io::owfn >> net;
			else if (args_info.lola_given)
	            inputStream >> meta(pnapi::io::INPUTFILE, args_info.inputs[0])
    	                    >> pnapi::io::lola >> net;
			else 
	            inputStream >> meta(pnapi::io::INPUTFILE, args_info.inputs[0])
    	                    >> pnapi::io::pnml >> net;
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


    /*------------------------------------.
    | 2. Compute the equivalence classes  |
    `------------------------------------*/

	vector<map<Node*,int> > inv;
	bool ntype(!args_info.transition_given);
	// setup lp_solve
	LPWrapper lp(net,ntype,false);
	// and create the basic system of equations we need
	lp.createIEquation();
	// establish representation for the equivalence classes
	InvEqRel ier(net,ntype,!args_info.optimize_given);
	// precompute some equivalences (not using lp_solve)
	ier.simpleEquivalences(args_info.full_given);
	// two placeholders for yet-to-decide equivalence of two places
	Node* n1;
	Node* n2;
	// as long as we find two nodes for which equivalence has not been decided ...
	while (ier.getUndecided(n1,n2))
	{
		// add the equations n1=0 and n2>0 to the system (or n1-n2>0 for full invariants)
		if (!lp.stripConstraints()) abort(12,"failed to remove constraints from LP model");
		if (!args_info.full_given) {
			if (!lp.addConstraint(n1,true)) abort(12,"failed to add constraint to LP model");
			if (!lp.addConstraint(n2,false)) abort(12,"failed to add constraint to LP model");
		} else if (!lp.addConstraint(n1,n2)) abort(12,"failed to add constraint to LP model");
		// and solve it
		int res(lp.solveSystem());
		if (res>2) status("lp_solve failure -- resulting relation may be too coarse");
		else if (res<2)
		{
			// n1 and n2 can be distinguished
			map<Node*,int> sol(lp.getNVector());
			inv.push_back(sol);
			// use the computed invariant to find non-equivalent places (including p1&p2)
			ier.split(sol);
		} else {
			// if the first check fails, now test with n1>0 and n2=0 (resp. n2-n1>0)
			if (!lp.stripConstraints()) abort(12,"failed to remove constraints from LP model");
			if (!args_info.full_given) {
				if (!lp.addConstraint(n2,true)) abort(12,"failed to add constraint to LP model");
				if (!lp.addConstraint(n1,false)) abort(12,"failed to add constraint to LP model");
			} else if (!lp.addConstraint(n2,n1)) abort(12,"failed to add constraint to LP model");
			res = lp.solveSystem();
			if (res>2) status("lp_solve failure -- resulting relation may be too coarse");
			else if (res<2)
			{
				// n1 and n2 can be distinguished
				map<Node*,int> sol(lp.getNVector());
				inv.push_back(sol);
				// use the computed invariant to find non-equivalent places (including p1&p2)
				ier.split(sol);
			// otherwise, n1&n2 are equivalent, so join their classes
			} else if (!ier.join(n1,n2)) status("internal failure -- resulting relation may be too coarse");
		}
	}

    /*--------------------------------.
    | 3. Write result to standard out |
    `--------------------------------*/

	status("%d priority joins found",ier.preJoinsDone());
	status("%d calls to lp_solve made",lp.getCalls());
	status("%d invariants computed",inv.size());
	// output in the format used by the tool Snoopy ...
	if (args_info.snoopy_given) {
		if (args_info.invariants_given) cout << " invariants ( ";
		else cout << " equivalence classes ( ";
		if (ntype) cout << "place";
		else cout << "transition";
		cout << " ) = " << endl << endl;
		if (args_info.invariants_given)
		{
			for(unsigned int i=0; i<inv.size(); ++i)
			{
				cout << (i+1);
				bool comma(false);
				map<Node*,int>::iterator nit;
				for(nit=inv[i].begin(); nit!=inv[i].end(); ++nit)
				{
					if (nit->second==0) continue;
					if (comma) cout << "," << endl;
					cout << "\t|" << nit->first->getName() << "\t:" << nit->second;
					comma = true;
				}
				cout << endl;
			}
		} else {
			vector<set<Node*> > vp(ier.getClasses(!args_info.fine_given));
			for(unsigned int i=0; i<vp.size(); ++i)
			{
				cout << (i+1);
				bool comma(false);
				set<Node*>::iterator nit;
				for(nit=vp[i].begin(); nit!=vp[i].end(); ++nit, comma=true)
				{
					if (comma) cout << "," << endl;
					cout << "\t|" << (*nit)->getName() << "\t:1";
				}
				cout << endl;
			}
		}
	} else {
	// or standard output ...
		if (args_info.invariants_given) {
			if (inv.size()>0) cout << "Invariants used to prove non-equivalence:" << endl;
			else cout << "No invariants have been constructed." << endl;
			for(unsigned int i=0; i<inv.size(); ++i)
			{
				cout << (i+1) << ") ";
				bool comma(false);
				map<Node*,int>::iterator nit;
				for(nit=inv[i].begin(); nit!=inv[i].end(); ++nit)
				{
					if (nit->second==0) continue;
					if (comma) cout << "+";
					cout << nit->second << nit->first->getName();
					comma = true;
				}
				cout << endl;
			}
		}
		vector<set<Node*> > vp(ier.getClasses(!args_info.fine_given));
		cout << "The equivalence relation has " << vp.size() << " class" << (vp.size()!=1?"es:":"") << endl;
		for(unsigned int i=0; i<vp.size(); ++i)
		{
			cout << "Class " << (i+1) << ": ";
			set<Node*>::iterator nit;
			for(nit=vp[i].begin(); nit!=vp[i].end(); ++nit)
				cout << (*nit)->getName() << " ";
			cout << endl;
		}
	}

    return EXIT_SUCCESS;
}

/* <<-- CHANGE END -->> */
