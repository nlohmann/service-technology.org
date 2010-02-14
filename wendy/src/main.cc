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
#include "StoredKnowledge.h"
#include "PossibleSendEvents.h"
#include "cmdline.h"
#include "Results.h"
#include "Output.h"
#include "Cover.h"
#include "Diagnosis.h"
#include "LivelockOperatingGuideline.h"
#include "AnnotationLivelockOG.h"
#include "Clause.h"
#include "verbose.h"
#include "SCSHandler.h"


// input files
extern FILE *graph_in;
extern FILE *cover_in;

// the parsers
extern int graph_parse();
extern int graph_lex_destroy();
extern int cover_parse();
extern int cover_lex_destroy();

/// the command line parameters
gengetopt_args_info args_info;

/// the invocation string
std::string invocation;

/// a file to store a mapping from marking ids to actual Petri net markings
Output *markingoutput = NULL;

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
        invocation += (i == 0 ? "" : " ") + std::string(argv[i]);
    }

    // initialize the parameters structure
    struct cmdline_parser_params *params = cmdline_parser_params_create();

    // call the cmdline parser
    if (cmdline_parser(argc, argv, &args_info) != 0) {
        abort(7, "invalid command-line parameter(s)");
    }

    // debug option
    if (args_info.bug_flag) {
        { Output debug_output("bug.log", "configuration information");
          debug_output.stream() << CONFIG_LOG; }
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

    // initialize the report frequency
    if (args_info.reportFrequency_arg < 0) {
        abort(8, "report frequency must not be negative");
    }

    // check whether at most one file is given
    if (args_info.inputs_num > 1) {
        abort(4, "at most one input file must be given");
    }

    if (args_info.sa_given and args_info.og_given) {
        abort(12, "'--og' and '--sa' parameter are mutually exclusive");
    }

    // --diagnose implies several reduction rules
    if (args_info.diagnose_given) {
        args_info.ignoreUnreceivedMessages_flag = 0;
        args_info.waitstatesOnly_flag = 1;
        args_info.receivingBeforeSending_flag = 1;
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
        StoredKnowledge::finalize();

        time(&end_time);
        status("released memory [%.0f sec]", difftime(end_time, start_time));
    }

    // print statistics
    if (args_info.stats_flag) {
        message("runtime: %s%.2f sec%s", (static_cast<double>(clock()) - static_cast<double>(start_clock)) / CLOCKS_PER_SEC, _bold_, _c_);
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
            std::cin >> (args_info.pnml_flag ? pnapi::io::pnml : pnapi::io::owfn) >> *InnerMarking::net;
        } else {
            // strip suffix from input filename
            filename = std::string(args_info.inputs[0]).substr(0, std::string(args_info.inputs[0]).find_last_of("."));

            std::ifstream inputStream(args_info.inputs[0]);
            if (!inputStream) {
                abort(1, "could not open file '%s'", args_info.inputs[0]);
            }
            inputStream >> meta(pnapi::io::INPUTFILE, args_info.inputs[0])
                        >> (args_info.pnml_flag ? pnapi::io::pnml : pnapi::io::owfn)
                        >> *InnerMarking::net;
        }
        if (args_info.verbose_flag) {
            std::ostringstream s;
            s << pnapi::io::stat << *InnerMarking::net;
            status("read net: %s", s.str().c_str());
        }
    } catch(pnapi::io::InputError error) {
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


    /*--------------------------------------------.
    | 2. initialize labels and interface markings |
    `--------------------------------------------*/
    Label::initialize();
    InterfaceMarking::initialize();
    PossibleSendEvents::initialize();


    /*----------------------------.
    | 3. read cover file if given |
    `----------------------------*/
    if (args_info.cover_given) {
        if (args_info.cover_arg) {
            if (!(cover_in = fopen(args_info.cover_arg, "r"))) {
                abort(15, "could not open cover file '%s'", args_info.cover_arg);
            }
            cover_parse();
            fclose(cover_in);
            cover_lex_destroy();

            status("read cover file '%s'", args_info.cover_arg);
        } else {
            Cover::coverAll();
            status("covering all nodes");
        }
        status("%d nodes to cover", Cover::nodeCount);
    }


    /*--------------------------------------------.
    | 4. write inner of the open net to LoLA file |
    `--------------------------------------------*/
    Output* temp = new Output();
    temp->stream() << pnapi::io::lola << *InnerMarking::net;
    // marking information output
    if (args_info.mi_given) {
        std::string mi_filename = args_info.mi_arg ? args_info.mi_arg : filename + ".mi";
        markingoutput = new Output(mi_filename, "marking information");
    }


    /*------------------------------------------.
    | 5. call LoLA and parse reachability graph |
    `------------------------------------------*/
    // select LoLA binary and build LoLA command
#if defined(__MINGW32__)
    // MinGW does not understand pathnames with "/", so we use the basename
    std::string command_line = std::string(basename(args_info.lola_arg)) + " " + temp->name() + " -M" + (args_info.verbose_flag ? "" : " 2> nul");
#else
    std::string command_line = std::string(args_info.lola_arg) + " " + temp->name() + " -M" + (args_info.verbose_flag ? "" : " 2> /dev/null");
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

    // close marking information output file
    delete markingoutput;


    /*-------------------------------.
    | 6. organize reachability graph |
    `-------------------------------*/
    InnerMarking::initialize();
    Cover::clear();


    /*-------------------------------.
    | 7. calculate knowledge bubbles |
    `-------------------------------*/
    time(&start_time);
    Knowledge* K0 = new Knowledge(0);
    StoredKnowledge::root = new StoredKnowledge(K0);

    StoredKnowledge::root->store();
    if (StoredKnowledge::root->is_sane) {
        StoredKnowledge::processNode(K0, StoredKnowledge::root);
    }
    delete K0;
    time(&end_time);

    // statistics output
    status("stored %d knowledges, %d edges [%.0f sec]",
        StoredKnowledge::stats.storedKnowledges, StoredKnowledge::stats.storedEdges, difftime(end_time, start_time));

    // traverse all nodes reachable from the root
    StoredKnowledge::root->traverse();
    status("%d knowledges reachable", StoredKnowledge::seen.size());

    ///\todo rausfinden, was mit dot ist (kann ich eine LL-OG dotten)
    // in case of livelock freedom and if the operating guideline shall be
    // calculated, we have to analyse every strongly connected set of knowledges
    if (args_info.correctness_arg == correctness_arg_livelock and args_info.og_given) {
        time(&start_time);
        LivelockOperatingGuideline::initialize();
        LivelockOperatingGuideline::generateLLOG();
        time(&end_time);
        status("generated LL-OG [%.0f sec]", difftime(end_time, start_time));
    }

    // analyze root node and print result
    message("%s: %s", _cimportant_("net is controllable"), (StoredKnowledge::root->is_sane ? _cgood_("YES") : _cbad_("NO")));
    if (not StoredKnowledge::root->is_sane and not args_info.diagnose_given) {
        message("use '%s' option for diagnosis information", _cparameter_("--diagnose"));
    }


    /*-------------------------------.
    | 8. calculate cover constraint |
    `--------------------------------*/
    if (args_info.cover_given) {
        Cover::calculate(StoredKnowledge::seen);
        message("%s: %s", _cimportant_("cover constraint is satisfiable"), (Cover::satisfiable ? _cgood_("YES") : _cbad_("NO")));
    }


    /*-------------------.
    | 9. output options |
    `-------------------*/
    // results output
    if (args_info.resultFile_given) {
        std::string results_filename = args_info.resultFile_arg ? args_info.resultFile_arg : filename + ".results";
        Results results(results_filename);
        StoredKnowledge::output_results(results);
        InnerMarking::output_results(results);
        Label::output_results(results);

        results.add("meta.package_name", (char *)PACKAGE_NAME);
        results.add("meta.package_version", (char *)PACKAGE_VERSION);
        results.add("meta.svn_version", (char *)VERSION_SVN);
        results.add("meta.invocation", invocation);
    }

    if (StoredKnowledge::root->is_sane or args_info.diagnose_given) {
        // operating guidelines output
        if (args_info.og_given) {
            std::string og_filename = args_info.og_arg ? args_info.og_arg : filename + ".og";
            Output output(og_filename, "operating guidelines");
            StoredKnowledge::output_og(output);

            if (args_info.cover_given) {
                std::string cover_filename = og_filename + ".cover";
                Output cover_output(cover_filename, "cover constraint");
                Cover::write(cover_output);
            }

            // print out some statistics gained by writing out the og file
            // (the decode() method of Clause is only called when we actually print out the annotations)
            /// \todo move me to class wherever
            /// \todo --dot should not need --og
            if (args_info.correctness_arg == correctness_arg_livelock and args_info.og_given) {

                status("number of strongly connected sets within knowledges: %d", LivelockOperatingGuideline::stats.numberOfSCSs);
                status("number of terminal strongly connected sets: %d", LivelockOperatingGuideline::stats.numberOfTSCCInSCSs);

                double averageSizeOfAnnotation = static_cast<double>(AnnotationElement::stats.cumulativeNumberOfClauses) / static_cast<double>(LivelockOperatingGuideline::stats.numberOfSCSs);
                status("average number of clauses in an annotation: %.2f", averageSizeOfAnnotation);

                double averageSizeOfLiteralsOfAnnotation = static_cast<double>(Clause::stats.cumulativeSizeAllClauses) / static_cast<double>(LivelockOperatingGuideline::stats.numberOfTSCCInSCSs);
                status("average number of literals of clause: %.2f", averageSizeOfLiteralsOfAnnotation);

                status("maximal number of clauses in an annotation: %d", AnnotationElement::stats.maximalNumberOfClauses);
                status("maximal number of literals of clause: %d", Clause::stats.maximalSizeOfClause);

                status("found %d bad subsets and skipped those", SCSHandler::countBadSubsystems);
            }
        }

        // service automaton output
        if (args_info.sa_given) {
            std::string sa_filename = args_info.sa_arg ? args_info.sa_arg : filename + ".sa";
            Output output(sa_filename, "service automaton");
            StoredKnowledge::output_og(output);
        }

        // dot output
        if (args_info.dot_given) {
            std::string dot_filename = args_info.dot_arg ? args_info.dot_arg : filename + ".dot";
            Output output(dot_filename, "dot representation");
            StoredKnowledge::output_dot(output);
        }

        // migration output
        if (args_info.im_given) {
            std::string im_filename = args_info.im_arg ? args_info.im_arg : filename + ".im";
            Output output(im_filename, "migration information");

            time(&start_time);
            StoredKnowledge::output_migration(output);
            time(&end_time);
            status("wrote migration information [%.0f sec]", difftime(end_time, start_time));
        }

        // diagnose output
        if (args_info.diagnose_given) {
            std::string diag_filename = args_info.diagnose_arg ? args_info.diagnose_arg : filename + ".diag.dot";
            Output output(diag_filename, "diagnosis information");
            Diagnosis::output_diagnosedot(output);
        }
    }

    return EXIT_SUCCESS;
}
