/*****************************************************************************\
 Rachel -- Reparing Service Choreographies

 Copyright (c) 2008, 2009 Niels Lohmann

 Rachel is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Rachel is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Rachel.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

#include <config.h>
#include <libgen.h>
#include <climits>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <stack>
#include <string>
#include "Action.h"
#include "Graph.h"
#include "Matching.h"
#include "Simulation.h"
#include "cmdline.h"
#include "helpers.h"
#include "verbose.h"
#include "Output.h"
#include "config-log.h"


using std::endl;
using std::stack;
using std::pair;
using std::map;
using std::ofstream;
using std::string;



/***********
 * globals *
 ***********/

// lexer and parser
extern int og_yyparse();
extern FILE *og_yyin;

/// the automaton
Graph A("");

/// the operating guideline
Graph B("");

/// the parsed graph
Graph G_parsedGraph("");

/// the filename of the current file
char *G_filename;

/// a map caching the best script for all action pairs
map<Node, map<Node, ActionScript> > G_script_cache;

/*!
 * \brief the command line options
 *
 * Stores all information on the command line options.
 */
gengetopt_args_info args_info;





/*************
 * functions *
 *************/

/// output complete edit script
/// \todo move me somewhere else
Graph outputEditScript(const Graph &g1, const Graph &g2) {
    typedef pair<Node, Node> NodePair;

    Graph g3("edit");

    stack<NodePair> todo;
    todo.push(NodePair(g1.getRoot(), g2.getRoot()));

    while (!todo.empty()) {
        // pop top element
        NodePair index = todo.top();
        ActionScript current = G_script_cache[index.first][index.second];
        todo.pop();

        // show action script and push successors
        for (size_t i = 0; i < current.script.size(); ++i) {
            todo.push(NodePair(current.script[i].stateA, current.script[i].stateB));

            g3.addNode(index.first);
            g3.addNode(current.script[i].stateA);

            if (current.script[i].getType() == DELETE) {
                g3.addEdge(index.first, current.script[i].stateA, current.script[i].label_old, current.script[i].getType());
            } else {
                g3.addEdge(index.first, current.script[i].stateA, current.script[i].label_new, current.script[i].getType());
            }
        }
    }

    return g3;
}


/// evaluate the command line parameters
void evaluateParameters(int argc, char** argv) {
    // set default values
    cmdline_parser_init(&args_info);

    // initialize the parameters structure
    struct cmdline_parser_params *params = cmdline_parser_params_create();

    // call the cmdline parser
    if (cmdline_parser (argc, argv, &args_info) != 0)
        exit(EXIT_FAILURE);

    if (args_info.conf_file_given) {
        // initialize the config file parser
        params->initialize = 0;
        params->override = 0;

        // call the config file parser
        if (cmdline_parser_config_file (args_info.conf_file_arg, &args_info, params) != 0)
            exit(EXIT_FAILURE);
    }

    // debug option
    if (args_info.bug_flag) {
        { Output debug_output("bug.log", "configuration information");
          debug_output.stream() << CONFIG_LOG << std::flush; }
        message("please send file 'bug.log' to %s!", PACKAGE_BUGREPORT);
        exit(EXIT_SUCCESS);
    }

    // tidy memory (is this right)
    free(params);
}


int main(int argc, char** argv) {
    evaluateParameters(argc, argv);

    // modes that read a service automaton
    if (args_info.mode_arg == mode_arg_matching or
        args_info.mode_arg == mode_arg_simulation or
        args_info.mode_arg == mode_arg_sa) {

        // care about the automaton file input
        if (!args_info.automaton_given) {
            abort(1, ": '--automaton' ('-a') option required in mode '%s'",
                cmdline_parser_mode_values[args_info.mode_arg]);
        } else {
            G_filename = args_info.automaton_arg;
            og_yyin = fopen(G_filename, "r");
            if (og_yyin == NULL) {
                abort(2, "automaton '%s' not found", args_info.automaton_arg);
            }

            // call parser and copy read graph
            G_parsedGraph = Graph(G_filename);
            og_yyparse();
            fclose(og_yyin);
            A = G_parsedGraph;
        }
    }


    // modes that read an operating guideline
    if (args_info.mode_arg == mode_arg_matching or
        args_info.mode_arg == mode_arg_simulation or
        args_info.mode_arg == mode_arg_og) {

        // care about the OG file input
        if (!args_info.og_given) {
            abort(3, "'--og' ('-o') option required in mode '%s'",
                cmdline_parser_mode_values[args_info.mode_arg]);
        } else {
            G_filename = args_info.og_arg;
            og_yyin = fopen(G_filename, "r");
            if (og_yyin == NULL) {
                abort(4, "og '%s' not found", args_info.og_arg);
            }

            // call parser and copy read graph
            G_parsedGraph = Graph(G_filename);
            og_yyparse();
            fclose(og_yyin);
            B = G_parsedGraph;
        }
    }


    // check if graphs are cyclic
    if (args_info.mode_arg == mode_arg_matching or args_info.mode_arg == mode_arg_simulation) {
        if (A.isCyclic()) {
            abort(5, "automaton '%s' is cyclic; aborting", args_info.automaton_arg);
        }
        if (B.isCyclic()) {
            abort(6, "OG '%s' is cyclic; aborting", args_info.og_arg);
        }

        // statistical output
        status("calculating %s", cmdline_parser_mode_values[args_info.mode_arg]);
        status("source (SA): %s\t%u nodes",
                basename(args_info.automaton_arg), static_cast<unsigned int>(A.nodes.size()));
        status("target (OG): %s\t%u nodes",
                basename(args_info.og_arg), static_cast<unsigned int>(B.nodes.size()));
    }


    // more statistical output
    if (args_info.mode_arg == mode_arg_matching) {
        status("OG characterizes %.3Le deterministic acyclic services (up to tree isomorphism)",
                B.countServices());
        status("OG has an average size of satisfying assignments of %f",
                B.averageSatSize());
    }

    if (args_info.mode_arg == mode_arg_simulation) {
        status("pairs to check: %lu", (A.nodes.size() * B.nodes.size()));
    }


    // do what you're told via "--mode" parameter
    switch (args_info.mode_arg) {
        case(mode_arg_simulation): {
            message("similarity: %.2f", Simulation::simulation());
            break;
        }

        case(mode_arg_matching): {
            message("matching: %.2f\n", Matching::matching());
            break;
        }

        default: {}
    }


    // create dot if requested
    if (args_info.dot_given) {
        switch (args_info.mode_arg) {
            case(mode_arg_matching):
            case(mode_arg_simulation): {
                Graph C = outputEditScript(A, B);
                C.createDotFile();
                break;
            }

            case(mode_arg_og): {
                B.createDotFile();
                break;
            }

            case(mode_arg_sa): {
                A.createDotFile();
                break;
            }
        }
    }


    return EXIT_SUCCESS;
}
