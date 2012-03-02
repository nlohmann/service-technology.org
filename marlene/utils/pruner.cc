/*****************************************************************************\
 Marlene -- synthesizing behavioral adapters

 Copyright (C) 2009  Christian Gierds <gierds@informatik.hu-berlin.de>

 Marlene is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Marlene is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Marlene.  If not, see <http://www.gnu.org/licenses/>.
 \*****************************************************************************/

// include standard C++ headers
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <libgen.h>
#include <ctime>

// include Marlene's own headers
// #include "config.h"
#include "pruner-cmdline.h"
#include "pruner-helper.h"
// #include "macros.h"
// #include "adapter.h"
#include "Output.h"
#include "pruner-verbose.h"

// include PN API headers
// #include "pnapi/pnapi.h"

/// a variable holding the time of the call
clock_t start_clock = clock();

/// a function collecting calls to organize termination (close files, ...)
void terminationHandler() {
    // release memory (used to detect memory leaks)
    // if (args_info.finalize_flag)
    // print statistics

	if (args_info.stats_flag) {
        message("runtime: %.2f sec", (static_cast<double> (clock())
                - static_cast<double> (start_clock)) / CLOCKS_PER_SEC);
        fprintf(stderr, "%s: memory consumption: ", PACKAGE);
        int result = system(
                (std::string("ps -o rss -o comm | ") + TOOL_GREP + " "
                        + PACKAGE + " | " + TOOL_AWK
                        + " '{ if ($1 > max) max = $1 } END { print max \" KB\" }' 1>&2").c_str());
        if (result != 0) {
            status("error while figuring out memory consumption\n");
        }
    }

}

int main(int argc, char* argv[]) {

	argv[0] = basename(argv[0]);

        // set the function to call on normal termination
        atexit(terminationHandler);

        /*************************************************\
    * Parsing the command line and evaluation options *
         \*************************************************/
        // evaluate command line options
        evaluate_command_line(argc, argv);
        Output::setTempfileTemplate(args_info.tmpfile_arg);
        Output::setKeepTempfiles(args_info.noClean_flag);

        // store invocation in a string for meta information in file output
        std::string invocation;
        for (int i = 0; i < argc; ++i) {
            invocation += std::string(argv[i]) + " ";
        }
        status("Pruner was called with the following arguments: `%s'", invocation.c_str());

        time_t start_time, end_time;


		//	graph_yyin = inputStream;

        {
        	extern FILE * graph_yyin;
        	extern int graph_yyparse();
        	extern int graph_yylineno;
        	extern int graph_yylex_destroy();

			graph_yylineno = 1;

			graph_yyparse();
			graph_yylex_destroy();

        }

        /*************************\
    * All necessary variables *
         \*************************/

        /*
        //! #rs contains a RuleSet that is provided for the adapter
        RuleSet rs;
        //! #adapter
        Adapter adapter(nets, rs, contType, messageBound, useCompPlaces);

        if (args_info.rulefile_given) {
            time(&start_time);
            status("reading transformation rules from file \"%s\"",
                    args_info.rulefile_arg);
#ifdef USE_SHARED_PTR
            std::tr1::shared_ptr<FILE> rulefile(fopen(
                    args_info.rulefile_arg, "r"), fclose);
            if (rulefile.get() != NULL) {
#else
                FILE * rulefile = fopen(args_info.rulefile_arg, "r");
                if (rulefile != NULL) {
#endif
                rs.addRules(rulefile);
            } else {
                abort(2, "Rule file %s could not be opened for reading",
                        args_info.rulefile_arg);
            }
            time(&end_time);
            status("reading all rules file done [%.0f sec]", difftime(
                    end_time, start_time));
#ifndef USE_SHARED_PTR
            fclose(rulefile);
#endif
        }
        */

    // cmdline_parser_free(&args_info);
    {
        time_t start_time, end_time;
        time(&start_time);
        cmdline_parser_free(&args_info);
        time(&end_time);
        status("released memory [%.0f sec]", difftime(end_time, start_time));
    }

    return 0;
}

