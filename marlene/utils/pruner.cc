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
#include "pruner-statespace.h"

// include PN API headers
#include "pnapi/pnapi.h"

typedef std::tr1::shared_ptr<pnapi::PetriNet> PetriNet_ptr;

PetriNet_ptr net(new pnapi::PetriNet());

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

    if (args_info.interface_given) {
        time(&start_time);
        status("reading interface information from file \"%s\"",
               args_info.interface_arg);

        extern FILE * communication_yyin;
        extern int communication_yyparse();
        extern int communication_yylineno;
        extern int communication_yylex_destroy();

        communication_yylineno = 1;

        std::tr1::shared_ptr<FILE> interfacefile(fopen(
                    args_info.interface_arg, "r"), fclose);
        if (interfacefile.get() != NULL) {
            communication_yyin = interfacefile.get();
            communication_yyparse();
        } else {
            abort(2, "Interface file %s could not be opened for reading",
                  args_info.interface_arg);
        }
        time(&end_time);
        status("reading interface file done [%.0f sec]", difftime(
                   end_time, start_time));
        communication_yylex_destroy();
    }
    //	graph_yyin = inputStream;

    //reading open net file
    if (args_info.net_given) {
        time(&start_time);

        std::string filename(args_info.net_arg);
        status("reading open net from file \"%s\"",
               filename.c_str());
        try {
            std::ifstream inputfile(filename.c_str(),
                                    std::ios_base::in);
            if (inputfile.good()) {
                inputfile >> meta(pnapi::io::INPUTFILE, filename)
                          >> meta(pnapi::io::CREATOR, PACKAGE_STRING)
                          >> meta(pnapi::io::INVOCATION, invocation)
                          >> pnapi::io::owfn >> *net;

                //std::cerr << pnapi::io::owfn << *net;

            } else {
                abort(
                    2,
                    "Open net file %s could not be opened for reading",
                    filename.c_str());
            }
        } catch (const pnapi::exception::InputError& error) {
            std::cerr << PACKAGE << ":" << error << std::endl;
            displayFileError(error.filename.c_str(), error.line,
                             error.token.c_str());
            abort(2, " ");
        }
        time(&end_time);
        status("reading interface file done [%.0f sec]", difftime(
                   end_time, start_time));

    }


    time(&start_time);
    extern FILE * graph_yyin;
    extern int graph_yyparse();
    extern int graph_yylineno;
    extern int graph_yylex_destroy();

    graph_yylineno = 1;

    // opening stream to lola-statespace
#warning "Lola is hard-coded for now"
    std::string lola_command = "lola-statespace -M ";
    if (args_info.inputs_num > 0) {
        lola_command += args_info.inputs[0];
    }
    status("running lola: \"%s\"", lola_command.c_str());
    {
        std::tr1::shared_ptr<FILE> lolafile(popen(
                                                lola_command.c_str(), "r"), pclose);

        graph_yyin = lolafile.get();

        graph_yyparse();
        graph_yylex_destroy();
    }
    time(&end_time);
    status("running lola done [%.0f sec]", difftime(end_time, start_time));
    status("lola found %d states.", State::stateSpace.size());

    time_t start_parttime, end_parttime;

    status("starting pruning of reachability graph");
    time(&start_time);
    time(&start_parttime);
    /* do the pruning here and now */
    State::checkFinalReachable();
    time(&end_parttime);
    status("reachability of final states done [%.0f sec]", difftime(end_parttime, start_parttime));

    time(&start_parttime);
    // now: tau abstraction
    State::tauFolding();
    time(&end_parttime);
    status("tau folding done [%.0f sec]", difftime(end_parttime, start_parttime));

    time(&start_parttime);
    // now: actual pruning
    State::prune();
    time(&end_parttime);
    status("pruning done [%.0f sec]", difftime(end_parttime, start_parttime));
    time(&end_time);
    status("all phases done [%.0f sec]", difftime(end_time, start_time));

    //State::calculateSCC();
    State::output();

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

