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


// for UINT8_MAX
#define __STDC_LIMIT_MACROS

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <libgen.h>
#include "config.h"
#include "config-log.h"
#include "StoredKnowledge.h"
#include "PossibleSendEvents.h"
#include "Label.h"
#include "Cover.h"
#include "verbose.h"

// detect MinGW compilation under Cygwin
#ifdef WIN32
#ifndef _WIN32
#define CYGWIN_MINGW
#endif
#endif

using std::string;


/// input files
extern FILE *graph_in;
extern FILE *cover_in;

/// the parsers
extern int graph_parse();
extern int cover_parse();

/// the command line parameters
gengetopt_args_info args_info;

/// the invocation string
string invocation;

/// a file to store a mapping from marking ids to actual Petri net markings
std::ofstream *markingfile;


bool fileExists(std::string filename) {
    FILE *tmp = fopen(filename.c_str(), "r");
    if (tmp) {
        fclose(tmp);
        return true;
    } else {
        return false;
    }
}


/// evaluate the command line parameters
void evaluateParameters(int argc, char** argv) {
    // overwrite invocation for consistent error messages
    argv[0] = (char*)PACKAGE;

    // store invocation in a string for meta information in file output
    for (int i = 0; i < argc; ++i) {
        invocation += string(argv[i]) + " ";
    }

    // set default values
    cmdline_parser_init(&args_info);

    // initialize the parameters structure
    struct cmdline_parser_params *params = cmdline_parser_params_create();

    // call the cmdline parser
    if (cmdline_parser(argc, argv, &args_info) != 0) {
        abort(7, "invalid command-line parameter(s)");
    }

    // debug option
    if (args_info.bug_flag) {
        FILE *debug_output = fopen("bug.log", "w");
        fprintf(debug_output, "%s\n", CONFIG_LOG);
        fclose(debug_output);
        fprintf(stderr, "Please send file 'bug.log' to %s.\n", PACKAGE_BUGREPORT);
        exit(EXIT_SUCCESS);
    }

    // read a configuration file if necessary
    if (args_info.config_given) {
        // initialize the config file parser
        params->initialize = 0;
        params->override = 0;

        // call the config file parser
        if (cmdline_parser_config_file (args_info.config_arg, &args_info, params) != 0) {
            abort(14, "error reading configuration file '%s'", args_info.config_arg);
        } else {
            status("using configuration file '%s'", args_info.config_arg);
        }
    } else {
        // check for configuration files
        string conf_filename = fileExists("wendy.conf") ? "wendy.conf" :
                               (fileExists(string(SYSCONFDIR) + "/wendy.conf") ?
                               (string(SYSCONFDIR) + "/wendy.conf") : "");

        if (conf_filename != "") {
            // initialize the config file parser
            params->initialize = 0;
            params->override = 0;
            if (cmdline_parser_config_file ((char*)conf_filename.c_str(), &args_info, params) != 0) {
                abort(14, "error reading configuration file '%s'", conf_filename.c_str());
            } else {
                status("using configuration file '%s'", conf_filename.c_str());
            }
        } else {
            status("not using a configuration file");
        }
    }

    // initialize the report frequency
    if (args_info.reportFrequency_arg < 0) {
        abort(8, "report frequency must not be negative");
    }
    StoredKnowledge::reportFrequency = args_info.reportFrequency_arg;

    // check whether at most one file is given
    if (args_info.inputs_num > 1) {
        abort(4, "at most one input file must be given");
    }

    if (args_info.sa_given and args_info.og_given) {
        abort(12, "'--og' and '--sa' parameter are mutually exclusive");
    }

    // check the message bound
    if ((args_info.messagebound_arg < 1) or (args_info.messagebound_arg > UINT8_MAX)) {
        abort(9, "message bound must be between 1 and %d", UINT8_MAX);
    }

    free(params);
}


int main(int argc, char** argv) {
    // set a standard filename
    string filename("wendy_output");
    time_t start_time, end_time;

    /*--------------------------------------.
    | 0. parse the command line parameters  |
    `--------------------------------------*/
    evaluateParameters(argc, argv);


    /*----------------------.
    | 1. parse the open net |
    `----------------------*/
    try {
        // parse either from standard input or from a given file
        if (args_info.inputs_num == 0) {
            std::cin >> pnapi::io::owfn >> *(InnerMarking::net);
        } else {
            // strip suffix from input filename
            filename = string(args_info.inputs[0]).substr(0,string(args_info.inputs[0]).find_last_of("."));

            std::ifstream inputStream(args_info.inputs[0]);
            if (!inputStream) {
                abort(1, "could not open file '%s'", args_info.inputs[0]);
            }
            inputStream >> meta(pnapi::io::INPUTFILE, args_info.inputs[0])
                        >> pnapi::io::owfn >> *(InnerMarking::net);
        }
        if (args_info.verbose_flag) {
            std::cerr << PACKAGE << ": read net " << pnapi::io::stat << *(InnerMarking::net) << std::endl;
        }
    } catch (pnapi::io::InputError error) {
        std::stringstream temp;
        temp << error;
        abort(2, "\b%s", temp.str().c_str());
    }

    // only normal nets are supported so far
    if (not InnerMarking::net->isNormal()) {
        abort(3, "the input open net must be normal");
    }


    /*--------------------------------------------.
    | 2. initialize labels and interface markings |
    `--------------------------------------------*/
    Label::initialize();
    InterfaceMarking::initialize(args_info.messagebound_arg);
    PossibleSendEvents::initialize();

//    PossibleSendEvents *a = new PossibleSendEvents(true, 1);
//    PossibleSendEvents *b = new PossibleSendEvents();
//
//    //a->labelPossible(4);
//    b->labelPossible(8);
//
//    //*a |= *b;
//
//
//    a = b;
//
//    char *foo = a->decode();
//    for (Label_ID l = 0; l < Label::send_events; ++l) {
//        fprintf(stderr, "%d", foo[l]);
//    }
//    fprintf(stderr, "\n");


    /*----------------------------.
    | 3. read cover file if given |
    `----------------------------*/
    if(args_info.cover_given) {
        cover_in = fopen(args_info.cover_arg, "r");
        if(cover_in == NULL) {
            abort(15, "could not open cover file '%s'", args_info.cover_arg);
        }
        cover_parse();
        fclose(cover_in);

        status("read cover file '%s'", args_info.cover_arg);
        status("%d nodes to cover", Cover::nodeCount);
    }


    /*--------------------------------------------.
    | 4. write inner of the open net to LoLA file |
    `--------------------------------------------*/
    // create a unique temporary file name
    char *tmp = args_info.tmpfile_arg;
#ifdef HAVE_MKSTEMP
    if (mkstemp(tmp) == -1) {
        abort(13, "could not create a temporary file '%s'", tmp);
    }
#endif
#ifdef CYGWIN_MINGW
    status("setting tempfile name for MinGW to 'wendy.tmp'");
    tmp = "wendy.tmp";
#endif
    std::string tmpname(tmp);

    std::ofstream lolaFile((tmpname + ".lola").c_str(), std::ofstream::out | std::ofstream::trunc);
    if (!lolaFile) {
        abort(11, "could not write to file '%s.lola'", tmpname.c_str());
    }
    lolaFile << pnapi::io::lola << *(InnerMarking::net);
    lolaFile.close();
    status("created file '%s.lola'", tmpname.c_str());


    /*------------------------------------------.
    | 5. call LoLA and parse reachability graph |
    `------------------------------------------*/
    time(&start_time);

    // marking information output
    if (args_info.mi_given) {
        string mi_filename = args_info.mi_arg ? args_info.mi_arg : filename + ".mi";
        markingfile = new std::ofstream(mi_filename.c_str(), std::ofstream::out | std::ofstream::trunc);
    }

    // read from a pipe or from a file
#if defined(HAVE_POPEN) && defined(HAVE_PCLOSE) && !defined(CYGWIN_MINGW)
    string command_line = string(args_info.lola_arg) + " " + tmpname + ".lola -M 2> /dev/null";
    status("creating a pipe to LoLA by calling '%s'", command_line.c_str());
    graph_in = popen(command_line.c_str(), "r");
    graph_parse();
    pclose(graph_in);
#else
#ifdef CYGWIN_MINGW
    string command_line = string(basename(args_info.lola_arg)) + " " + tmpname + ".lola -m";
#else
    string command_line = string(args_info.lola_arg) + " " + tmpname + ".lola -m &> /dev/null";
#endif
    status("calling LoLA with '%s'", command_line.c_str());
    system(command_line.c_str());
    graph_in = fopen((tmpname + ".graph").c_str(), "r");
    graph_parse();
    fclose(graph_in);
#endif
    time(&end_time);

    // close marking information output file
    if (args_info.mi_given) {
        markingfile->close();
        delete markingfile;
    }

    /*-------------------------------.
    | 6. organize reachability graph |
    `-------------------------------*/
    InnerMarking::initialize();
    delete InnerMarking::net;
    if(args_info.cover_given) {
        Cover::clear();
    }

    status("LoLA is done [%.0f sec]", difftime(end_time, start_time));


    /*-------------------------------.
    | 7. calculate knowledge bubbles |
    `-------------------------------*/
    time(&start_time);
    Knowledge *K0 = new Knowledge(0);
    StoredKnowledge::root = new StoredKnowledge(K0);
    StoredKnowledge::root->store();

    StoredKnowledge::processRecursively(K0, StoredKnowledge::root);
    delete K0;
    time(&end_time);

    status("stored %d knowledges [%.0f sec]",
        StoredKnowledge::stats_storedKnowledges, difftime(end_time, start_time));
    status("used %d of %d hash buckets, maximal bucket size: %d",
        static_cast<unsigned int>(StoredKnowledge::hashTree.size()),
        (1 << (8*sizeof(hash_t))), static_cast<unsigned int>(StoredKnowledge::stats_maxBucketSize));
    status("at most %d interface markings per inner marking",
        StoredKnowledge::stats_maxInterfaceMarkings);


    /*----------------------------.
    | 8. add predecessor relation |
    `----------------------------*/
    time(&start_time);
    unsigned int edges = StoredKnowledge::addPredecessors();
    time(&end_time);

    status("added predecessor relation (%d edges) [%.0f sec]",
        edges, difftime(end_time, start_time));


    /*----------------------------------.
    | 9. detect and delete insane nodes |
    `----------------------------------*/
    time(&start_time);
    unsigned int redNodes = StoredKnowledge::removeInsaneNodes();
    time(&end_time);

    status("removed %d insane nodes (%3.2f%%) in %d iterations [%.0f sec]",
        redNodes, 100.0 * ((double)redNodes / (double)StoredKnowledge::stats_storedKnowledges),
        StoredKnowledge::stats_iterations, difftime(end_time, start_time));
    if (redNodes != 0) {
        status("%d insane nodes (%3.2f%%) were prematurely detected",
            StoredKnowledge::stats_builtInsaneNodes,
            100.0 * ((double)StoredKnowledge::stats_builtInsaneNodes / (double)redNodes));
    }
    status("%d nodes left", StoredKnowledge::seen.size());

    // analyze root node
    fprintf(stderr, "%s: net is controllable: %s\n",
        PACKAGE, (StoredKnowledge::root->is_sane) ? "YES" : "NO");

    /*------------------------------.
    | 10. calculate cover contraint |
    `-------------------------------*/
    if(args_info.cover_given) {
        Cover::calculate(StoredKnowledge::seen);

        fprintf(stderr, "%s: cover constraint is satisfiable: %s\n",
            PACKAGE, (Cover::satisfiable) ? "YES" : "NO");
    }

    /*-------------------.
    | 11. output options |
    `-------------------*/
    // operating guidelines output
    if (args_info.og_given) {
        string og_filename = args_info.og_arg ? args_info.og_arg : filename + ".og";
        std::ofstream og_file(og_filename.c_str(), std::ofstream::out | std::ofstream::trunc);
        if (!og_file) {
            abort(11, "could not write to file '%s'", og_filename.c_str());
        }
        if (args_info.fionaFormat_flag) {
            StoredKnowledge::output_old(og_file);
        } else {
            StoredKnowledge::output(og_file);
        }
        status("wrote OG to file '%s'", og_filename.c_str());

        if(args_info.cover_given) {
            Cover::write(og_filename + ".cover");
        }
    }

    // service automaton output
    if (args_info.sa_given) {
        string sa_filename = args_info.sa_arg ? args_info.sa_arg : filename + ".sa";
        std::ofstream sa_file(sa_filename.c_str(), std::ofstream::out | std::ofstream::trunc);
        if (!sa_file) {
            abort(11, "could not write to file '%s'", sa_filename.c_str());
        }
        StoredKnowledge::output(sa_file);
        status("wrote service automaton to file '%s'", sa_filename.c_str());
    }

    // dot output
    if (args_info.dot_given) {
        string dot_filename = args_info.dot_arg ? args_info.dot_arg : filename + ".dot";
        std::ofstream dot_file(dot_filename.c_str(), std::ofstream::out | std::ofstream::trunc);
        if (!dot_file) {
            abort(11, "could not write to file '%s'", dot_filename.c_str());
        }
        StoredKnowledge::dot(dot_file);
        status("wrote dot representation to file '%s'", dot_filename.c_str());
    }

    // migration output
    if (args_info.im_given) {
        string im_filename = args_info.im_arg ? args_info.im_arg : filename + ".im";
        std::ofstream im_file(im_filename.c_str(), std::ofstream::out | std::ofstream::trunc);
        if (!im_file) {
            abort(11, "could not write to file '%s'", im_filename.c_str());
        }

        time(&start_time);
        StoredKnowledge::migration(im_file);
        time(&end_time);

        status("wrote migration information to file '%s' [%.0f sec]",
            im_filename.c_str(), difftime(end_time, start_time));
    }


    return EXIT_SUCCESS;
}
