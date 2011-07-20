/*****************************************************************************\
 Cosme -- Checking Simulation, Matching, and Equivalence

 Copyright (c) 2010 Andreas Lehmann

 Cosme is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Cosme is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Cosme.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <cstdio>
#include <cstdlib>
#include <libgen.h>
#include <ctime>
#include "Label.h"
#include "Service.h"
#include "OperatingGuideline.h"
#include "cmdline.h"
#include "Output.h"
#include "verbose.h"
#include "util.h"
#include "Results.h"
#include "config-log.h"
#include <pnapi/pnapi.h>

// lexer and parser
extern int og_yyparse();
extern int og_yylex_destroy();
extern int graph_yyparse();
extern int graph_yylex_destroy();
extern FILE* og_yyin;
extern FILE* graph_yyin;

// the command line parameters
gengetopt_args_info args_info;
// the invocation string
string invocation;
// a variable holding the time of the call
clock_t start_clock = clock();

Label GlobalLabels;
std::map <std::string, label_id_t> TransitionLabels;
std::set<label_id_t> OGInterface;
std::set<label_id_t> ServiceInterface;
std::map<og_service_index_t, OGMarking*> OGMarkings;
std::map<og_service_index_t, ServiceMarking*> ServiceMarkings;

pnapi::PetriNet tmpNet;

OperatingGuideline* A;
OperatingGuideline* B;
Service* C;

/// check if a file exists and can be opened for reading
inline bool fileExists(const std::string& filename) {
    std::ifstream tmp(filename.c_str(), std::ios_base::in);
    return tmp.good();
}

// evaluate the command line parameters
void evaluateParameters(int argc, char** argv) {
    // overwrite invocation for consistent error messages
    argv[0] = basename(argv[0]);

    // store invocation in a string for meta information in file output
    for (int i = 0; i < argc; ++i) {
        invocation += string(argv[i]) + " ";
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

    // check what to do
    if ((args_info.matching_flag + args_info.simulation_flag + args_info.equivalence_flag) > 1) {
        abort(12, "don't know what to do (matching, simulation or equivalence)");
    }
    if ((args_info.matching_flag + args_info.simulation_flag + args_info.equivalence_flag) == 0) {
        abort(12, "don't know what to do (matching, simulation or equivalence)");
    }

    // check whether two files are given
    if (args_info.matching_flag) {
        if (!args_info.ServiceC_given or !args_info.OGA_given) {
            abort(4, "for matching a service and an operating guideline must be given");
        }
    } else {
        if (!args_info.OGA_given or !args_info.OGB_given) {
            abort(4, "for simulation/equivalence two operating guidelines must be given");
        }
    }

    free(params);
}

OperatingGuideline* parseOG(char* file) {
    og_yyin = fopen(file, "r");

    if (!og_yyin) { // if an error occurred
        abort(1, "could not open file '%s'", file);
    }

    // actual parsing
    og_yyparse();

    // close input (output is closed by destructor)
    fclose(og_yyin);
    og_yylex_destroy();

    OperatingGuideline* tmpOG = new OperatingGuideline(OGMarkings, OGInterface);
    OGMarkings.clear();
    OGInterface.clear();

    return tmpOG;
}

inline void parseService_LabelHelper() {

    GlobalLabels.mapLabel("Tau");
    label_id_t curLabel;
    std::pair<std::map<std::string, label_id_t>::iterator, bool> ret;

    const std::set<pnapi::Label*> outputLabels(tmpNet.getInterface().getOutputLabels());
    FOREACH(l, outputLabels) {
        // save current !-event
        string tmpStr("!");
        tmpStr += (*l)->getName();
        curLabel = GlobalLabels.mapLabel(tmpStr);
        ServiceInterface.insert(curLabel);

        const std::set<pnapi::Transition*> preset((**l).getTransitions());
        FOREACH(t, preset) {
            // save current Transitionlabel
            ret = TransitionLabels.insert(make_pair((*t)->getName(), curLabel));
            assert(ret.second);
        }
    }

    const std::set<pnapi::Label*> inputLabels(tmpNet.getInterface().getInputLabels());
    FOREACH(l, inputLabels) {
        // save current ?-event
        string tmpStr("?");
        tmpStr += (*l)->getName();
        curLabel = GlobalLabels.mapLabel(tmpStr);
        ServiceInterface.insert(curLabel);

        const std::set<pnapi::Transition*> postset((**l).getTransitions());
        FOREACH(t, postset) {
            ret = TransitionLabels.insert(make_pair((*t)->getName(), curLabel));
            assert(ret.second);
        }
    }

}

Service* parseService(char* file) {
    std::ifstream InputStream(file);
    time_t start_time, end_time;

    if (!InputStream) {
        abort(1, "could not open file '%s'", file);
    }

    // parse OWFN
    InputStream >> pnapi::io::owfn >> tmpNet;
    InputStream.close();

    if (tmpNet.getTransitions().empty()) {
        abort(3, "the input open net has no transitions");
    }
    if (!tmpNet.isNormal()) {
        abort(3, "the input open net must be normal");
    }

    parseService_LabelHelper();

    //TODO: ggf. allgemeinen Constructor nutzen
#if defined(__MINGW32__)
    Output temp(basename(args_info.tmpfile_arg), "");
#else
    Output temp(args_info.tmpfile_arg, "");
#endif

    temp.stream() << pnapi::io::lola << tmpNet;

    // select LoLA binary and build LoLA command
#if defined(__MINGW32__)
    // MinGW does not understand pathnames with "/", so we use the basename
    string command_line = string(basename(args_info.lola_arg)) + " " + temp.name() + " -M" + (args_info.verbose_flag ? "" : " 2> nul");
#else
    string command_line = string(args_info.lola_arg) + " " + temp.name() + " -M" + (args_info.verbose_flag ? "" : " 2> /dev/null");
#endif

    status("calling %s: '%s'", _ctool_("LoLA"), command_line.c_str());
    time(&start_time);
    graph_yyin = popen(command_line.c_str(), "r");
    graph_yyparse();
    pclose(graph_yyin);
    graph_yylex_destroy();
    time(&end_time);
    status("%s is done [%.0f sec]", _ctool_("LoLA"), difftime(end_time, start_time));

    Service* tmpService = new Service(ServiceMarkings, ServiceInterface);

    ServiceMarkings.clear();
    ServiceInterface.clear();

    return tmpService;
}

// a function collecting calls to organize termination (close files, ...)
void terminationHandler() {

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

// main function
int main(int argc, char** argv) {
    // set the function to call on normal termination
    atexit(terminationHandler);

    // set a standard filename
    std::string filename = std::string(PACKAGE) + "_output";

    // return value
    bool ret = false;

    evaluateParameters(argc, argv);

    if (args_info.matching_flag) {
        C = parseService(args_info.ServiceC_arg);
        A = parseOG(args_info.OGA_arg);

        status("Service contains %d markings", C->size());
        status("Operating guideline contains %d markings", A->size());

        C->calculateBitSets(GlobalLabels);
        A->calculateBitSets(GlobalLabels);

        ret = A->isMatching(*C);

        if (ret) {
            message("%s: %s", _cimportant_("Matching"), _cgood_("completed"));
        } else {
            message("%s: %s", _cimportant_("Matching"), _cbad_("failed"));
        }

        // release memory (used to detect memory leaks)
        if (args_info.finalize_flag) {
            time_t start_time, end_time;
            time(&start_time);
            A->finalize();
            C->finalize();
            delete A;
            delete C;
            cmdline_parser_free(&args_info);
            time(&end_time);
            status("released memory [%.0f sec]", difftime(end_time, start_time));
        }

    } else if ((args_info.simulation_flag) or(args_info.equivalence_flag)) {
        OperatingGuideline* A, *B;

        A = parseOG(args_info.OGA_arg);
        B = parseOG(args_info.OGB_arg);

        status("Operating guideline A contains %d markings", A->size());
        status("Operating guideline B contains %d markings", B->size());

        if (args_info.simulation_flag) {
            ret = A->isSimulation(*B);
            if (ret) {
                message("%s: %s", _cimportant_("Simulation"), _cgood_("completed"));
            } else {
                message("%s: %s", _cimportant_("Simulation"), _cbad_("failed"));
            }
        } else {
            ret = A->isEquivalent(*B);
            if (ret) {
                message("%s: %s", _cimportant_("Equivalence"), _cgood_("completed"));
            } else {
                message("%s: %s", _cimportant_("Equivalence"), _cbad_("failed"));
            }
        }

        // release memory (used to detect memory leaks)
        if (args_info.finalize_flag) {
            time_t start_time, end_time;
            time(&start_time);
            A->finalize();
            B->finalize();
            delete A;
            delete B;
            cmdline_parser_free(&args_info);
            time(&end_time);
            status("released memory [%.0f sec]", difftime(end_time, start_time));
        }

    }

    // results output
    if (args_info.resultFile_given) {
        std::string results_filename = args_info.resultFile_arg ? args_info.resultFile_arg : filename + ".results";
        Results results(results_filename);

        results.add("meta.package_name", (char*)PACKAGE_NAME);
        results.add("meta.package_version", (char*)PACKAGE_VERSION);
        results.add("meta.svn_version", (char*)VERSION_SVN);
        results.add("meta.invocation", invocation);

        if (args_info.matching_flag) {
            results.add("matching.result", ret);
        } else if (args_info.simulation_flag) {
            results.add("simulation.result", ret);
        } else if (args_info.equivalence_flag) {
            results.add("equivalence.result", ret);
        }

    }

    return EXIT_SUCCESS;
}

