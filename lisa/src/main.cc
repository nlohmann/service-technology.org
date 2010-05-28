/*****************************************************************************\
 Lisa -- Implementing invariant calculus, state equation, reachability check

 Copyright (c) <<-- 2010 Georg Straube -->>

 Lisa is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Lisa is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Lisa.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

// include header files
#include <config.h>
#include <ctime>
#include <libgen.h>
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
#include "lpwrapper.h"

using std::vector;
using std::map;
using std::set;

/// the command line parameters
gengetopt_args_info args_info;

/// the invocation string
std::string invocation;

/// a file to store a mapping from marking ids to actual Petri net markings
Output* markingoutput = NULL;

/// a variable holding the time of the call
clock_t start_clock = clock();

/// Global ordering of transitions for lp_solve
vector<Transition*> transitionorder;
/// Global ordering of places for lp_solve and determining a scapegoat (in the stubborn set method)
vector<Place*> placeorder;
/// inverted transitionorder for back references
map<Transition*,int> revtorder;
/// inverted placeorder for back references
map<Place*,int> revporder;
/// set of arcs
set<pnapi::Arc*> arcs;

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
    time_t start_time, end_time;

    // set the function to call on normal termination
    atexit(terminationHandler);

    /*--------------------------------------.
    | 0. parse the command line parameters  |
    `--------------------------------------*/
    evaluateParameters(argc, argv);
    Output::setTempfileTemplate(args_info.tmpfile_arg);
    Output::setKeepTempfiles(args_info.noClean_flag);


    /*----------------------.
    | 1. parse the open net |
    `----------------------*/
    pnapi::PetriNet net;

    try {
        // parse either from standard input or from a given file
        if (args_info.inputs_num == 0) {
            // parse the open net from standard input
            std::cin >> pnapi::io::owfn >> net;
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
            inputStream >> meta(pnapi::io::INPUTFILE, args_info.inputs[0])
                        >> pnapi::io::owfn >> net;
        }
        // additional output if verbose-flag is set
        if (args_info.verbose_flag) {
            std::ostringstream s;
            s << pnapi::io::stat << net;
            status("read net: %s", s.str().c_str());
        }
    } catch (pnapi::io::InputError error) {
        std::ostringstream s;
        s << error;
        abort(2, "\b%s", s.str().c_str());
    }


    /*----------------------------------------.
    | 2. calculate and print t-invariant
    `----------------------------------------*/
    if(args_info.findTInv_flag){
      LPWrapper lpw(net.getTransitions().size(), &net);
      lpw.calcPTOrder();
      lpw.calcTInvariant(false);
    }

    /*----------------------------------------.
    | 3. calculate and print p-invariant
    `----------------------------------------*/

    if(args_info.findPInv_flag){
      LPWrapper lpw(net.getPlaces().size(), &net);
      lpw.calcPTOrder();
      lpw.calcPInvariant(false);
    }

    /*----------------------------------------.
    | 4. calculate and print trap
    `----------------------------------------*/

    if(args_info.findTrap_flag){
      LPWrapper lpw(net.getPlaces().size(), &net);
      lpw.calcPTOrder();
      lpw.calcTrap(false);
    }

    /*----------------------------------------.
    | 5. calculate and print siphon
    `----------------------------------------*/

    if(args_info.findSiphon_flag){
      LPWrapper lpw(net.getPlaces().size(), &net);
      lpw.calcPTOrder();
      lpw.calcSiphon(false);
    }	

    /*----------------------------------------.
    | 6. reachability check
    `----------------------------------------*/

    if(args_info.isReachable_given){
      LPWrapper lpw(net.getTransitions().size(), &net);
      lpw.calcPTOrder();
      Marking m1(net, false);
      Marking m2(net, true);
      //TODO: Implement input for marking to be reached.
      lpw.checkReachability(m1, m2, false);
    }	


    return EXIT_SUCCESS;
}

