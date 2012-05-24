/*****************************************************************************\
 Wendy -- Synthesizing Partners for Services

 Copyright (c) 2009 Niels Lohmann, Christian Sura, and Daniela Weinberg

 Wendy is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Wendy is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Wendy.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/


#include <config.h>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <libgen.h>
#include <fstream>
#include <sstream>
#include <string>
#include "config-log.h"
#include "InnerMarking.h"
#include "Label.h"
#include "cmdline.h"
#include "Results.h"
#include "Output.h"
#include "verbose.h"
#include "util.h"


// input files
extern FILE* graph_in;

// the parsers
extern int graph_parse();
extern int graph_lex_destroy();

/// the command line parameters
gengetopt_args_info args_info;

/// the invocation string
std::string invocation;

/// a file to store a mapping from marking ids to actual Petri net markings
Output* markingoutput = NULL;

/// a variable holding the time of the call
clock_t start_clock = clock();


///// replace one occurrence of a substring in a string
//std::string replaceOnce(std::string result, const std::string& replaceWhat,
//                        const std::string& replaceWithWhat) {
//    const size_t pos = result.find(replaceWhat);
//    if (pos == std::string::npos) {
//        return result;
//    }
//    result.replace(pos, replaceWhat.size(), replaceWithWhat);
//    return result;
//}


/// check if a file exists and can be opened for reading
inline bool fileExists(const std::string& filename) {
    std::ifstream tmp(filename.c_str(), std::ios_base::in);
    return tmp.good();
}


/// evaluate the command line parameters
void evaluateParameters(int argc, char** argv) {
    // overwrite invocation for consistent error messages
    // TODO: basename seems to allocated memory which is not freed afterwards,
    // i.e., valgrind 3.6.0.SVN reports a memory leak
    // a fix like "free(argv[0]);" after the call of the cmdline parser works an Mac OS 10.6.4
    // but unfortunately seems to fail on linux
    argv[0] = basename(argv[0]);

    // store invocation in a std::string for meta information in file output
    for (int i = 0; i < argc; ++i) {
        invocation += (i == 0 ? "" : " ") + std::string(argv[i]);
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
            debug_output.stream() << CONFIG_LOG;
        }
        message("Please send file '%s' to %s!", _cfilename_("bug.log"), _coutput_(PACKAGE_BUGREPORT));
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
            status("using configuration file '%s'", _cfilename_(args_info.config_arg));
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
                status("using configuration file '%s'", _cfilename_(conf_filename));
            }
        } else {
            status("not using a configuration file");
        }
    }

    // set LoLA if it is present in the environment and not set otherwise
    if (getenv("LOLA") != NULL and not args_info.lola_given) {
        status("using environment variable '%s' set to '%s'",
            _cfilename_("LOLA"), _cfilename_(getenv("LOLA")));
        free(args_info.lola_arg);
        args_info.lola_arg = strdup(getenv("LOLA"));
    }

    // check whether at most one file is given
    if (args_info.inputs_num > 1) {
        abort(4, "at most one input file must be given");
    }

    // \TODO: error codes
    if (args_info.count_arg < 0) {
    	abort(123, "Trace count has to be a positive integer.");
    }

    // \TODO: error codes
    if (args_info.icount_arg < -1) {
    	abort(123, "Interface count has to be a positive integer or '-1'.");
    }

    free(params);
}


/// a function collecting calls to organize termination (close files, ...)
void terminationHandler() {
    // release memory (used to detect memory leaks)
    if (args_info.finalize_flag) {
        time_t start_time, end_time;
        time(&start_time);
        cmdline_parser_free(&args_info);
        InnerMarking::finalize();

        time(&end_time);
        status("released memory [%.0f sec]", difftime(end_time, start_time));
    }

    // print statistics
    if (args_info.stats_flag) {
        message("runtime: %s%.2f sec%s", _bold_, (static_cast<double>(clock()) - static_cast<double>(start_clock)) / CLOCKS_PER_SEC, _c_);
        std::string call = std::string("ps -o rss -o comm | ") + TOOL_GREP + " " + PACKAGE + " | " + TOOL_AWK + " '{ if ($1 > max) max = $1 } END { print max \" KB\" }'";
        FILE* ps = popen(call.c_str(), "r");
        unsigned int memory;
        int res = fscanf(ps, "%u", &memory);
        assert(res != EOF);
        pclose(ps);
        message("memory consumption: %s%u KB %s", _bold_, memory, _c_);
    }
}


int main(int argc, char** argv) {
    time_t start_time, end_time;

    // set the function to call on normal termination
    atexit(terminationHandler);

    // set a standard filename
    std::string filename = std::string(PACKAGE) + "_output";

    /*--------------------------------------.
    | 0. parse the command line parameters  |
    `--------------------------------------*/
    evaluateParameters(argc, argv);
    Output::setTempfileTemplate(args_info.tmpfile_arg);
    Output::setKeepTempfiles(args_info.noClean_flag);


    /*----------------------.
    | 1. parse the open net |
    `----------------------*/
    try {
        // parse either from standard input or from a given file
        if (args_info.inputs_num == 0) {
            status("reading from stdin...");
            std::cin >> (args_info.tpn_flag ? pnapi::io::woflan : pnapi::io::owfn) >> *InnerMarking::net;
        } else {
            // strip suffix from input filename
            filename = std::string(args_info.inputs[0]).substr(0, std::string(args_info.inputs[0]).find_last_of("."));

            std::ifstream inputStream(args_info.inputs[0]);
            if (!inputStream) {
                abort(1, "could not open file '%s'", args_info.inputs[0]);
            }
            inputStream >> meta(pnapi::io::INPUTFILE, args_info.inputs[0])
                        >> (args_info.tpn_flag ? pnapi::io::woflan : pnapi::io::owfn)
                        >> *InnerMarking::net;
        }
        if (args_info.verbose_flag) {
            std::ostringstream s;
            s << pnapi::io::stat << *InnerMarking::net;
            status("read net: %s", s.str().c_str());
        }
    } catch (const pnapi::exception::InputError& error) {
        std::ostringstream s;
        s << error;
        abort(2, "\b%s", s.str().c_str());
    }

    // "fix" the net in order to avoid parse errors from LoLA (see bug #14166)
    if (InnerMarking::net->getTransitions().empty()) {
        status("net has no transitions -- adding dead dummy transition");
        InnerMarking::net->createArc(InnerMarking::net->createPlace(), InnerMarking::net->createTransition());
    }

    // only normal nets are supported so far
    if (not InnerMarking::net->isNormal()) {
        abort(3, "the input open net must be normal");
    }


    if (args_info.tpn_flag) {

    	InnerMarking::addInterface(args_info.icount_arg);

//    	std::stringstream tempstring (std::stringstream::in | std::stringstream::out);
//    	tempstring << pnapi::io::owfn << *InnerMarking::net;
//
//    	tempstring.str().find("FINALCONDITION");

    	std::string owfn_filename = filename + ".owfn";
    	Output output2(owfn_filename, "OWFN File");
    	output2.stream() << pnapi::io::owfn << *InnerMarking::net;

    }

    if (args_info.partnerView_flag) {
    	InnerMarking::changeView(args_info.messagebound_arg);
    }

    if (args_info.log_flag) {

    	/*--------------------------------------------.
    	| 2. initialize labels and interface markings |
    	`--------------------------------------------*/
    	Label::initialize();


    	/*--------------------------------------------.
    	| 3. write inner of the open net to LoLA file |
    	`--------------------------------------------*/
    	Output* temp = new Output();
    	std::stringstream ss;
    	ss << pnapi::io::lola << *InnerMarking::net;
    	std::string lola_net = ss.str();
    	//test output!!
    	//status("%s", lola_net.c_str());
    	temp->stream() << lola_net << std::endl;


    	/*------------------------------------------.
    	| 4. call LoLA and parse reachability graph |
    	`------------------------------------------*/
    	// select LoLA binary and build LoLA command
#if defined(__MINGW32__)
    	//    // MinGW does not understand pathnames with "/", so we use the basename
    	const std::string command_line = "\"" + std::string(args_info.lola_arg) + "\" " + temp->name() + " -M" + (args_info.verbose_flag ? "" : " 2> nul");
#else
    	const std::string command_line = std::string(args_info.lola_arg) + " " + temp->name() + " -M" + (args_info.verbose_flag ? "" : " 2> /dev/null");
#endif

    	// call LoLA
    	status("calling %s: '%s'", _ctool_("LoLA"), command_line.c_str());
    	time(&start_time);
    	graph_in = popen(command_line.c_str(), "r");

    	graph_parse();
    	pclose(graph_in);
    	graph_lex_destroy();
    	time(&end_time);
    	status("%s is done [%.0f sec]", _ctool_("LoLA"), difftime(end_time, start_time));
    	delete temp;


    	/*-------------------------------.
    	| 5. organize reachability graph |
    	`-------------------------------*/
    	InnerMarking::initialize();


    	/*-------------------------------.
    	| 6. create the XES log          |
    	`-------------------------------*/
    	std::string log_filename = args_info.logFile_arg ? args_info.logFile_arg : filename + ".xes";
    	Output output(log_filename, "XES Log");
    	InnerMarking::create_log(output, filename, args_info.count_arg, args_info.minLength_arg, args_info.maxLength_arg);

    }

    // delete the "counter places" if they were formerly created
    if (args_info.partnerView_flag) {
    	InnerMarking::deleteCounterPlaces();
    }

    if (args_info.pnmlFile_given) {
    	std::string pnml_filename = args_info.pnmlFile_arg ? args_info.pnmlFile_arg : filename + ".pnml";
    	Output output2(pnml_filename, "PNML File");
    	output2.stream() << pnapi::io::pnml << *InnerMarking::net;
    }

    //    std::string dot_filename = filename + ".dot";
    //    Output output3(dot_filename, "DOT File");
    //    output3.stream() << pnapi::io::dot << *InnerMarking::net;


    /*-------------------.
    | 7. output options |
    `-------------------*/
    // results output
    if (args_info.resultFile_given) {
    	std::string results_filename = args_info.resultFile_arg ? args_info.resultFile_arg : filename + ".results";
    	Results results(results_filename);
    	InnerMarking::output_results(results);
    	Label::output_results(results);

    	results.add("meta.package_name", (const char*)PACKAGE_NAME);
    	results.add("meta.package_version", (const char*)PACKAGE_VERSION);
    	results.add("meta.svn_version", (const char*)VERSION_SVN);
    	results.add("meta.invocation", invocation);
    }

    return EXIT_SUCCESS;
}
