/*****************************************************************************\
 Wendy -- Calculating Operating Guidelines

 Copyright (C) 2009  Niels Lohmann <niels.lohmann@uni-rostock.de>

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
#include <cassert>
#include "config.h"
#include "StoredKnowledge.h"
#include "Label.h"


/// the input file
extern FILE *graph_in;

/// the parser
extern int graph_parse();

/// the command line parameters
gengetopt_args_info args_info;


/*!
 \brief abort with an error message and an error code
 
 The codes are documented in Wendy's manual.
 
 \param code    the error code
 \param format  the error message formatted as printf string
*/
void abort(unsigned int code, const char* format, ...) {
    fprintf(stderr, "%s: ", PACKAGE);

    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end (args);

    fprintf(stderr, " -- aborting [#%02d]\n", code);

    if (args_info.verbose_given) {
        fprintf(stderr, "%s: see manual for a documentation of this error\n", PACKAGE);
    }

    exit(EXIT_FAILURE);
}


/// evaluate the command line parameters
void evaluateParameters(int argc, char** argv) {
    // overwrite invokation for consistent error messages
    argv[0] = PACKAGE;

    // set default values
    cmdline_parser_init(&args_info);

    // initialize the parameters structure
    struct cmdline_parser_params *params = cmdline_parser_params_create();

    // call the cmdline parser
    if (cmdline_parser(argc, argv, &args_info) != 0) {
        abort(7, "invalid command-line parameter(s)");
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

    // check whether a LoLA executable is given either in file "config.h" or
    // with a command line parameter "--lola"
#ifndef BINARY_LOLA
    if (!args_info.lola_given) {
        abort(5, "LoLA executable was not found");
    }
#endif

    // check the message bound
    if ((args_info.messagebound_arg < 1) or (args_info.messagebound_arg > UINT8_MAX)) {
        abort(9, "message bound must be between 1 and %d", UINT8_MAX);
    }

    // evaluate Fiona's '--type/-t' parameter
    if (args_info.type_given and
        (args_info.type_arg == type_arg_og or args_info.type_arg == type_arg_OG)) {
        args_info.og_given = 1;
    }

    // evaluate Fiona's '--show/-s' parameter
    for (unsigned int i = 0; i < args_info.show_given; ++i) {
        switch (args_info.show_arg[i]) {
            case(show_arg_empty):
                args_info.showEmptyNode_given = 1;
                break;

            case(show_arg_deadlocks):
                args_info.showDeadlocks_given = 1;
                break;

            case(show_arg_allstates):
                args_info.showDeadlocks_given = 1;
                args_info.showTransients_given = 1;
                break;
        }
    }

    // the '--showTransients' parameter implies '--diagnosis'
    if (args_info.showTransients_given) {
        args_info.diagnosis_given = 1;
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
            assert (args_info.inputs_num == 1);

            // strip suffix from input filename
            filename = string(args_info.inputs[0]).substr(0,string(args_info.inputs[0]).find_last_of("."));

            std::ifstream inputStream(args_info.inputs[0]);
            if (!inputStream) {
                abort(1, "could not open file '%s'", args_info.inputs[0]);
            }
            inputStream >> pnapi::io::owfn >> *(InnerMarking::net);
            inputStream.close();
        }
        if (args_info.verbose_given) {
            std::cerr << PACKAGE << ": read net " << pnapi::io::stat << *(InnerMarking::net) << std::endl;
        }
    } catch (pnapi::io::InputError error) {
        std::stringstream temp;
        temp << error;
        abort(2, "\b\b%s", temp.str().c_str());
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


    /*--------------------------------------------.
    | 3. write inner of the open net to LoLA file |
    `--------------------------------------------*/
    std::ofstream lolaFile;
    lolaFile.open("tmp.lola", std::ofstream::out | std::ofstream::trunc);
    lolaFile << pnapi::io::lola << *(InnerMarking::net);
    lolaFile.close();


    /*------------------------------------------.
    | 4. call LoLA and parse reachability graph |
    `------------------------------------------*/
    time(&start_time);

    // choose the LoLA binary
#ifdef BINARY_LOLA
    string command_line(args_info.lola_given ? args_info.lola_arg : BINARY_LOLA);
#else
    string command_line(args_info.lola_arg);
#endif

    // read from a pipe or from a file
#if defined(HAVE_POPEN) && defined(HAVE_PCLOSE)
    command_line += " tmp.lola -M 2> /dev/null";
    graph_in = popen(command_line.c_str(), "r");
    graph_parse();
    pclose(graph_in);
    system("rm -f tmp.lola");
#else
    command_line += " tmp.lola -m &> /dev/null";
    system(command_line.c_str());
    graph_in = fopen("tmp.graph", "r");
    graph_parse();
    fclose(graph_in);
    system("rm -f tmp.graph tmp.lola");
#endif
    time(&end_time);


    /*-------------------------------.
    | 5. organize reachability graph |
    `-------------------------------*/
    InnerMarking::initialize();
    delete InnerMarking::net;

    if (args_info.verbose_given) {
        fprintf(stderr, " [%.0f sec]\n", difftime(end_time, start_time));
    }


    /*-------------------------------.
    | 6. calculate knowledge bubbles |
    `-------------------------------*/
    time(&start_time);
    Knowledge *K0 = new Knowledge(0);
    StoredKnowledge::root = new StoredKnowledge(K0);
    StoredKnowledge::root->store();

    StoredKnowledge::calcRecursive(K0, StoredKnowledge::root);
    delete K0;
    time(&end_time);

    if (args_info.verbose_given) {
        fprintf(stderr, "%s: stored %d knowledges [%.0f sec]\n",
            PACKAGE, StoredKnowledge::storedKnowledges, difftime(end_time, start_time));
        fprintf(stderr, "%s: used %d of %d hash buckets, maximal bucket size: %d\n",
            PACKAGE, static_cast<unsigned int>(StoredKnowledge::hashTree.size()),
            (1 << (8*sizeof(hash_t))), static_cast<unsigned int>(StoredKnowledge::maxBucketSize));
    }


    /*----------------------------.
    | 7. add predecessor relation |
    `----------------------------*/
    time(&start_time);
    unsigned int edges = StoredKnowledge::addPredecessors();
    time(&end_time);

    if (args_info.verbose_given) {
        fprintf(stderr, "%s: added predecessor relation (%d edges) [%.0f sec]\n",
            PACKAGE, edges, difftime(end_time, start_time));
    }


    /*----------------------------------.
    | 8. detect and delete insane nodes |
    `----------------------------------*/
    time(&start_time);
    unsigned int redNodes = StoredKnowledge::removeInsaneNodes();
    time(&end_time);

    if (args_info.verbose_given) {
        fprintf(stderr, "%s: removed %d red nodes in %d iterations [%.0f sec]\n",
            PACKAGE, redNodes, StoredKnowledge::iterations, difftime(end_time, start_time));
    }

    // analyze root node
    fprintf(stderr, "%s: net is controllable: %s\n",
        PACKAGE, (StoredKnowledge::root->is_sane) ? "YES" : "NO");


    /*------------------.
    | 9. output options |
    `------------------*/
    // dot output
    if (args_info.dot_given) {
        string dot_filename = args_info.dot_arg ? args_info.dot_arg : filename + ".dot";
        std::ofstream dot_file(dot_filename.c_str(), std::ofstream::out | std::ofstream::trunc);
        StoredKnowledge::dot(dot_file);
        if (args_info.verbose_given) {
            fprintf(stderr, "%s: wrote dot representation to file '%s'\n", PACKAGE, dot_filename.c_str());
        }
    }

    // operating guidelines output
    if (args_info.og_given) {
        string og_filename = args_info.og_arg ? args_info.og_arg : filename + ".og";
        std::ofstream og_file(og_filename.c_str(), std::ofstream::out | std::ofstream::trunc);
        StoredKnowledge::OGoutput(og_file);
        if (args_info.verbose_given) {
            fprintf(stderr, "%s: wrote OG to file '%s'\n", PACKAGE, og_filename.c_str());
        }
    }


    return EXIT_SUCCESS;
}
