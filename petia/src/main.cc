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

	// setup lp_solve
	LPWrapper lp(net,false);
	// and create the basic system of equations we need
	lp.createIEquation();
	// establish representation for the equivalence classes
	InvEqRel ier(net);
	// precompute some equivalences (not using lp_solve)
	ier.simpleEquivalences();
	// two placeholders for yet-to-decide equivalence of two places
	Place* p1;
	Place* p2;
	// as long as we find two places for which equivalence has not been decided ...
	while (ier.getUndecided(p1,p2))
	{
		// add the equations p1=0 and p2>0 to the system
		if (!lp.stripConstraints()) abort(12,"failed to remove constraints from LP model");
		if (!lp.addConstraint(p1,true)) abort(12,"failed to add constraint to LP model");
		if (!lp.addConstraint(p2,false)) abort(12,"failed to add constraint to LP model");
		// and solve it
		int res(lp.solveSystem());
		if (res>2) status("lp_solve failure -- resulting relation may be too coarse");
		else if (res<2)
		{
			// p1 and p2 can be distinguished
			map<Place*,int> sol(lp.getPVector());
			// use the computed invariant to find non-equivalent places (including p1&p2)
			ier.split(sol);
		} else {
			// if the first check fails, now test with p1>0 and p2=0
			if (!lp.stripConstraints()) abort(12,"failed to remove constraints from LP model");
			if (!lp.addConstraint(p2,true)) abort(12,"failed to add constraint to LP model");
			if (!lp.addConstraint(p1,false)) abort(12,"failed to add constraint to LP model");
			res = lp.solveSystem();
			if (res>2) status("lp_solve failure -- resulting relation may be too coarse");
			else if (res<2)
			{
				// p1 and p2 can be distinguished
				map<Place*,int> sol(lp.getPVector());
				// use the computed invariant to find non-equivalent places (including p1&p2)
				ier.split(sol);
			// otherwise, p1&p2 are equivalent, so join their classes
			} else if (!ier.join(p1,p2)) status("internal failure -- resulting relation may be too coarse");
		}
	}

    /*--------------------------------.
    | 3. Write result to standard out |
    `--------------------------------*/

	// output in the format used by the tool Snoopy ...
	if (args_info.snoopy_given) {
		cout << " equivalence classes ( place ) = " << endl << endl;
		vector<set<Place*> > vp(ier.getClasses(true));
		for(unsigned int i=0; i<vp.size(); ++i)
		{
			cout << (i+1);
			bool comma(false);
			set<Place*>::iterator pit;
			for(pit=vp[i].begin(); pit!=vp[i].end(); ++pit, comma=true)
			{
				if (comma) cout << "," << endl;
				cout << "\t|" << (*pit)->getName() << "\t:1";
			}
			cout << endl;
		}
	} else {
	// or standard output ...
		if (flag_verbose)
		{
			cout << ier.preJoinsDone() << " pre-joins found, ";
			cout << lp.getCalls() << " calls to lp_solve made." << endl;
		}
		vector<set<Place*> > vp(ier.getClasses(true));
		cout << "The equivalence relation has " << vp.size() << " classes:" << endl;
		for(unsigned int i=0; i<vp.size(); ++i)
		{
			cout << "Class " << (i+1) << ": ";
			set<Place*>::iterator pit;
			for(pit=vp[i].begin(); pit!=vp[i].end(); ++pit)
				cout << (*pit)->getName() << " ";
			cout << endl;
		}
	}

    return EXIT_SUCCESS;
}


/* <<-- CHANGE END -->> */
