#include <cstdlib>
#include <cstdio>
#include <map>
#include <string>
#include "cmdline-graph2dot.h"

using std::string;
using std::map;

extern int graph_parse();
map<string,bool> filter;

/// the command line parameters
gengetopt_args_info args_info;


/// evaluate the command line parameters
void evaluateParameters(int argc, char** argv) {
    // set default values
    cmdline_parser_init(&args_info);

    // initialize the parameters structure
    struct cmdline_parser_params *params = cmdline_parser_params_create();
    
    // call the cmdline parser
    if (cmdline_parser (argc, argv, &args_info) != 0)
        exit(EXIT_FAILURE);    
        
    free(params);
}


int main(int argc, char** argv) {
    evaluateParameters(argc, argv);

    // copy filtered places into mapping (used in parser)
    for (unsigned int i = 0; i < args_info.filter_given; ++i)
        filter[string(args_info.filter_arg[i])] = true;

    fprintf(stdout, "digraph G {\n");

    if (args_info.emptyStates_given) {
        fprintf(stdout, "node [shape=circle width=0.2 style=filled fillcolor=lightgray]\n");
    } else {
        fprintf(stdout, "node [shape=plaintext style=filled fillcolor=lightgray fontname=Helvetica]\n");
    }
        
    fprintf(stdout, "edge [fontname=Helvetica]\n");
    graph_parse();
    fprintf(stdout, "}\n");
}
