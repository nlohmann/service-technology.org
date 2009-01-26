/*****************************************************************************\
 Rachel -- Repairing Automata for Choreographies by Editing Labels
 
 Copyright (C) 2008, 2009  Niels Lohmann <niels.lohmann@uni-rostock.de>
 
 Rachel is free software; you can redistribute it and/or modify it under the
 terms of the GNU General Public License as published by the Free Software
 Foundation; either version 3 of the License, or (at your option) any later
 version.
 
 Rachel is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License along with
 Rachel (see file COPYING); if not, see http://www.gnu.org/licenses or write to
 the Free Software Foundation,Inc., 51 Franklin Street, Fifth
 Floor, Boston, MA 02110-1301  USA.
\*****************************************************************************/

#include <cstdio>
#include <fstream>
#include <stack>
#include <climits>
#include <cstdlib>
#include <string>
#include <libgen.h>
#include "Action.h"
#include "Graph.h"
#include "Matching.h"
#include "Simulation.h"
#include "cmdline.h"
#include "config.h"
#include "helpers.h"


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
Graph outputEditScript(Graph &g1, Graph &g2) {
    typedef pair<Node,Node> NodePair;
    
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
    
    // tidy memory (is this right)
    free(params);
}


int main(int argc, char** argv) {
    if (argc == 2 && string(argv[1]) == "--bug") {
	    printf("\n\n");
        printf("Please email the following information to %s:\n", PACKAGE_BUGREPORT);
        printf("- tool:               %s\n", PACKAGE_NAME);
        printf("- version:            %s\n", PACKAGE_VERSION);
        printf("- compilation date:   %s\n", __DATE__);
        printf("- compiler version:   %s\n", __VERSION__);
        printf("- platform:           %s\n", CONFIG_BUILDSYSTEM);
        printf("- config ASSERT:      %s\n", CONFIG_ENABLEASSERT);
        printf("- config ASPECTS:     %s\n", CONFIG_ENABLEASPECTS);
        printf("- config UNIVERSAL:   %s\n", CONFIG_ENABLEUNIVERSAL);
        printf("- config ENABLE64BIT: %s\n", CONFIG_ENABLE64BIT);
        printf("- config WIN32:       %s\n", CONFIG_ENABLEWIN32);
        printf("- external dot:       %s\n", CONFIG_DOT);
        printf("\n\n");
        exit(EXIT_SUCCESS);
    }


    evaluateParameters(argc, argv);


    // modes that read a service automaton
    if (args_info.mode_arg == mode_arg_matching ||
        args_info.mode_arg == mode_arg_simulation ||
        args_info.mode_arg == mode_arg_bpmn ||
        args_info.mode_arg == mode_arg_sa) {

        // care about the automaton file input
        if (!args_info.automaton_given) {
            fprintf(stderr, "%s: '--automaton' ('-a') option required in mode '%s'\n",
                PACKAGE, cmdline_parser_mode_values[args_info.mode_arg]);
            exit(EXIT_FAILURE);
        } else {
            G_filename = args_info.automaton_arg;
            og_yyin = fopen(G_filename, "r");
            if (og_yyin == NULL) {
                fprintf(stderr, "automaton '%s' not found\n", args_info.automaton_arg);
                exit(EXIT_FAILURE);
            }

            // call parser and copy read graph
            G_parsedGraph = Graph(G_filename);
            og_yyparse();
            fclose(og_yyin);
            A = G_parsedGraph;                
        }
    }


    // modes that read an operating guideline
    if (args_info.mode_arg == mode_arg_matching ||
        args_info.mode_arg == mode_arg_simulation ||
        args_info.mode_arg == mode_arg_annotation ||
        args_info.mode_arg == mode_arg_og) {

        // care about the OG file input
        if (!args_info.og_given) {
            fprintf(stderr, "%s: '--og' ('-o') option required in mode '%s'\n",
                PACKAGE, cmdline_parser_mode_values[args_info.mode_arg]);
            exit(EXIT_FAILURE);
        } else {
            G_filename = args_info.og_arg;
            og_yyin = fopen(G_filename, "r");
            if (og_yyin == NULL) {
                fprintf(stderr, "og '%s' not found\n", args_info.og_arg);
                exit(EXIT_FAILURE);
            }

            // call parser and copy read graph
            G_parsedGraph = Graph(G_filename);
            og_yyparse();
            fclose(og_yyin);
            B = G_parsedGraph;            
        }
    }
    

    // check if graphs are cyclic
    if (args_info.mode_arg == mode_arg_matching || args_info.mode_arg == mode_arg_simulation) {
        if (A.isCyclic()) {
            fprintf(stderr, "automaton '%s' is cyclic; aborting\n", args_info.automaton_arg);
            exit(EXIT_FAILURE);
        }
        if (B.isCyclic()) {
            fprintf(stderr, "OG '%s' is cyclic; aborting\n", args_info.og_arg);
            exit(EXIT_FAILURE);
        }        

        // statistical output
        fprintf(stderr, "calculating %s\n", cmdline_parser_mode_values[args_info.mode_arg]);
        fprintf(stderr, "source (SA): %s\t%u nodes\n",
                basename(args_info.automaton_arg), static_cast<unsigned int>(A.nodes.size()));
        fprintf(stderr, "target (OG): %s\t%u nodes\n",
                basename(args_info.og_arg), static_cast<unsigned int>(B.nodes.size()));
    }


    // more statistical output
    if (args_info.verbose_flag && (args_info.mode_arg == mode_arg_matching)) {
        fprintf(stderr, "OG characterizes %.3Le deterministic acyclic services (up to tree isomorphism)\n",
                B.countServices());
        fprintf(stderr, "OG has an average size of satisfying assignments of %f\n",
                B.averageSatSize());
    }

    if (args_info.mode_arg == mode_arg_simulation) {
        fprintf(stderr, "pairs to check: %lu\n", (A.nodes.size() * B.nodes.size()));
    }
    

    // do what you're told via "--mode" parameter
    switch (args_info.mode_arg) {
        case (mode_arg_annotation): {
            B.calculateCompactAnnotations();
            break;
        }

        case (mode_arg_bpmn): {
            A.bpmnOutput();
            break;
        }

        case (mode_arg_simulation): {
            fprintf(stderr, "similarity: %.2f\n", Simulation::simulation());
            break;
        }

        case (mode_arg_matching): {
            fprintf(stderr, "matching: %.2f\n", Matching::matching());
            break;
        }

        default: {}
    }

    
    // create dot if requested
    if (args_info.dot_given) {
        switch (args_info.mode_arg) {
            case(mode_arg_matching):
            case(mode_arg_simulation): {
                Graph C = outputEditScript(A,B);
                C.createDotFile();
                break;
            }
            
            case (mode_arg_og): {
                B.createDotFile();
                break;                
            }
            
            case (mode_arg_annotation): {
                B.createDotFile();
                B.createDotFile(true);
                break;
            }

            case (mode_arg_sa): {
                A.createDotFile();
                break;
            }
        }
    }


    return EXIT_SUCCESS;
}
