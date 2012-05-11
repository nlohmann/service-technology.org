/*****************************************************************************\
 Patricia -- Scheduling Calls to Cerification Tools

 Copyright (c) 2012 Andre Moelle

 Patricia is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Patricia is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Patricia.  If not, see <http://www.gnu.org/licenses/>.

 Patricia contains the following important source files:
 - main.cc: This file. Handles command line evaluation and print the final
			result.
 - problem.cc: The main object. A problem is converted into a set of actions
			(parallel or sequential) by a plan(). Use the command line option
			--plan (or -p) to select a plan.
 - action.cc: An action can either be a call to a tool or a sequence or 
			parallel set of other actions.
 - tools.cc: The base class Tool for calls to a verification tool. Each tool
			has its own derived class to be found in the tools subdirectory.
			The classes handle the necessary preparations for the tool as
			well as the system call to the tool itself.
 - ptask.cc: The class PTask encapules the parameters needed by parallel
			processes/threads. This file additionally contains all purely
			thread-related stuff.
 - outcome.cc: Objects of type Outcome contain the result produced by an
			action, i.e. if a set of tools was able to verify of reject
			a given property.

\*****************************************************************************/

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <libgen.h>
#include <iostream>
#include <fstream>
#include <string>
#include <pthread.h>

#include "config-log.h"
#include "cmdline.h"
#include "Output.h"
#include "verbose.h"
#include "action.h"
#include "tools/lola.h"
#include "tools/sara.h"
#include "outcome.h"
#include "problem.h"

using std::cerr;
using std::endl;

/// the command line parameters
gengetopt_args_info args_info;
/// the invocation string
std::string invocation;
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
        abort(1, "invalid command-line parameter(s)");
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
            abort(2, "error reading configuration file '%s'", args_info.config_arg);
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
                abort(2, "error reading configuration file '%s'", conf_filename.c_str());
            } else {
                status("using configuration file '%s'", conf_filename.c_str());
            }
        } else {
            status("not using a configuration file");
        }
    }


    // check whether at most one file is given
    if (args_info.inputs_num > 3) {
        abort(4, "at most three input file can be given: a problem name, a Petri net file, and optionally a formula");
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


/*!
 Depending on the verification result, this function prints
 - "Verified (%s)" (%s is replaced by the tool's name)
 - "Refuted (%s)" (%s is replaced by the tool's name)
 - "Could neither verify nor refute"

 \param number of arguments
 \param arguments vector
 \return exit code (1 on invalid arguments passed)
 */
int main(int argc, char* argv[]) {

    // set the function to call on normal termination
    atexit(terminationHandler);

    /*--------------------------------------.
    | 1. parse the command line parameters  |
    `--------------------------------------*/
    evaluateParameters(argc, argv);
    Output::setTempfileTemplate(args_info.tmpfile_arg);
    Output::setKeepTempfiles(args_info.noClean_flag);

    /*--------------------------------------.
    | 2. check the given problem            |
    `--------------------------------------*/
    Problem problem(args_info.inputs_num, args_info.inputs);
	Action* act(problem.plan());
    Outcome outcome(act->perform(problem));

    if (outcome.state == Outcome::Positive) {
        printf("Verified (%s)\n", outcome.tool.c_str());
    } else if (outcome.state == Outcome::Negative) {
        printf("Refuted (%s)\n", outcome.tool.c_str());
    } else {
        printf("Could neither verify nor refute");
    }


	// delete the plan
	delete act;

    // ensure that each cleanup handler is called!
    pthread_exit(NULL);
}

