/*****************************************************************************\
 Hello -- <<-- Hello World -->>

 Copyright (c) <<-- 20XX Author1, Author2, ... -->>

 Hello is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Hello is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Hello.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

/* <<-- CHANGE START (main program) -->> */
// include header files
#include <config.h>
#include <ctime>
#include <libgen.h>
#include <fstream>
#include <sstream>
#include <string>

#include <pnapi/pnapi.h>
#include "config-log.h"
#include "cmdline.h"
#include "Output.h"
#include "verbose.h"

/// the command line parameters
gengetopt_args_info args_info;

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

/// lexer and parser
extern int wendy_yyparse();
extern int wendy_yylex_destroy();
extern FILE* wendy_yyin;

/// a string used by the parser
std::string wendy_out;


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

    /*---------------------------------.
    | 2. normalize the net (via PNAPI) |
    `---------------------------------*/

    net.normalize();

    /*----------------------------------------.
    | 3. set some output parameters for Wendy |
    `----------------------------------------*/

    // define variables
    std::string outputParam;
    std::string fileName;

    // if the verbose-flag is set, it is also given to Wendy
    if (args_info.verbose_flag) {
        outputParam += " -v ";
    }
    // for piping Wendy's output, we use " 2> " to read from std::cerr
    outputParam += " 2> ";
    if (args_info.output_given and args_info.output_arg) {
        fileName = args_info.output_arg;
    } else {
        // create a temporary file
#if defined(__MINGW32__)
        fileName = mktemp(basename(args_info.tmpfile_arg));
#else
        fileName = mktemp(args_info.tmpfile_arg);
#endif
    }
    // add the output filename
    outputParam += fileName;


    /*---------------------------------------------------.
    | 4. call Wendy and pipe the normalized net to Wendy |
    `---------------------------------------------------*/
    // select Wendy binary and build Wendy command
#if defined(__MINGW32__)
    // MinGW does not understand pathnames with "/", so we use the basename
    std::string command_line(basename(args_info.wendy_arg));
#else
    std::string command_line(args_info.wendy_arg);
#endif

    command_line += outputParam;
    // call Wendy
    message("creating a pipe to Wendy by calling '%s'", command_line.c_str());

    {
        // set start time
        time(&start_time);
        // create stringstream to store the open net
        std::stringstream ss;
        ss << pnapi::io::owfn << net << std::flush;
        // call Wendy and open a pipe
        FILE* fp = popen(command_line.c_str(), "w");
        // send the net to Wendy
        fprintf(fp, ss.str().c_str());
        // close the pipe
        pclose(fp);
        // set end time
        time(&end_time);
    }

    // status message
    status("Wendy is done [%.0f sec]", difftime(end_time, start_time));


    /*--------------------------------------------------------------.
    | 5. parse Wendy's output and write out the result to the shell |
    `--------------------------------------------------------------*/

    // if no output file is given
    if (not args_info.output_given) {

        // open Wendy's output file and link output file pointer
        wendy_yyin = fopen(fileName.c_str(), "r");
        if (!wendy_yyin) {
            std::cerr << PACKAGE << ": ERROR: failed to open file '"
                      << args_info.output_arg << "'" << std::endl;
            exit(EXIT_FAILURE);
        }

        /// actual parsing
        wendy_yyparse();

        // close input (output is closed by destructor)
        fclose(wendy_yyin);

        /// clean lexer memory
        wendy_yylex_destroy();

        // write parsed result to stderr
        message("parsed: %s", wendy_out.c_str());
    }

    return EXIT_SUCCESS;
}
/* <<-- CHANGE END -->> */
