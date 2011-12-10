/*****************************************************************************\
 Chie -- CHeckIng tEst cases

 Copyright (c) 2011 Christian Sura

 Chie is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Chie is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Chie.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

// include header files
#include <config.h>
#include <ctime>
#include <libgen.h>
#include <fstream>
#include <map>
#include <sstream>
#include <string>

#include "config-log.h"
#include "cmdline.h"
#include "automata.h"
#include "conformanceCheck.h"
#include "Output.h"
#include "verbose.h"

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

/// lexer and parser
extern int sa_yyparse();
extern int sa_yylex_destroy();
extern FILE* sa_yyin;

/// additional parser data
// whether parsing the second automaton
ServiceAutomaton * sa_specification = NULL;
ServiceAutomaton * sa_result = NULL;

// global variable for verbose output
std::string globalErrorMessage;


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
        abort(0, "invalid command-line parameter(s)");
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
            abort(0, "error reading configuration file '%s'", args_info.config_arg);
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
                abort(0, "error reading configuration file '%s'", conf_filename.c_str());
            } else {
                status("using configuration file '%s'", conf_filename.c_str());
            }
        } else {
            status("not using a configuration file");
        }
    }


    // check input
    if (!args_info.specification_given) {
      // count input files
      switch(args_info.inputs_num) {
      case 0:
        abort(0, "no specification given");
      case 1:
        abort(0, "no test chase given");
      default: /* all is fine */ ;
      }
    }

    free(params);
}


/// a function collecting calls to organize termination (close files, ...)
void terminationHandler() {
    /* [USER] Add code here */

    // clean lexer memory
    sa_yylex_destroy();

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
    std::string dotFileName;

    // set the function to call on normal termination
    atexit(terminationHandler);

    /*--------------------------------------.
    | 0. parse the command line parameters  |
    `--------------------------------------*/
    evaluateParameters(argc, argv);
    // no tempfiles needed
    //Output::setTempfileTemplate(args_info.tmpfile_arg);
    //Output::setKeepTempfiles(args_info.noClean_flag);


    /*---------------------------.
    | 1. parse service automaton |
    `---------------------------*/

    // allocate resulting automaton
    ServiceAutomaton specification;

    // open service automaton
    if(args_info.specification_given) // if service is specified by --service=foo.sa
    {
      // open given automaton
      sa_yyin = fopen(args_info.specification_arg, "r");
      if(!sa_yyin) {
        abort(0, "failed to open '%s'", args_info.specification_arg);
      }
    }
    else
    {
      // otherwise assume first file given as the service' automaton
      sa_yyin = fopen(args_info.inputs[0], "r");
      if(!sa_yyin) {
        abort(0, "failed to open '%s'", args_info.inputs[0]);
      }
    }

    // set result pointer
    sa_result = &specification;

    // actual parsing
    sa_yyparse();

    // close input
    fclose(sa_yyin);

    // set pointer for further parsing
    sa_specification = sa_result;


    /*----------------------.
    | 2. process test cases |
    `----------------------*/

    message("Checking whether the given test cases are conformance partners for the given specification:");

    // if --specification is given, the first file given is a test case
    int i = (args_info.specification_given ? 0 : 1);
    do
    {
      // set input
      sa_yyin = (args_info.inputs_num == 0) ? stdin : fopen(args_info.inputs[i], "r");
      if (!sa_yyin) {
          abort(0, "failed to open '%s'", (args_info.inputs_num == 0) ? "standard input stream" : args_info.inputs[i]);
      }

      // set result pointer
      ServiceAutomaton testCase;
      sa_result = &testCase;

      // parse test case
      sa_yyparse();

      // close input
      fclose(sa_yyin);


      // generate filename for dot output
      if(args_info.dot_given)
      {
        if(args_info.inputs_num == 0)
        {
          dotFileName = "stdin.dot";
        }
        else
        {
          dotFileName = args_info.inputs[i];
          dotFileName += ".dot";
        }
      }

      // do the test
      bool result = isConformancePartner(specification, testCase, dotFileName);
      message("%s: %s", ((args_info.inputs_num == 0) ? "stdin" : args_info.inputs[i]), (result ? "YES" : "NO"));
      if(!result)
      {
        status(globalErrorMessage.c_str()); // give more information when --verbose is given
      }
    } while (++i < args_info.inputs_num);

    return EXIT_SUCCESS;
}
