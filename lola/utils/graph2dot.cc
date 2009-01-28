#include <cstdlib>
#include <cstdio>
#include <map>
#include <string>
#include "cmdline-graph2dot.h"

using std::string;
using std::map;

extern int graph_parse();
extern FILE *graph_in;

FILE *dot_out = stdout;
map<string,bool> filter;
map<string,bool> exclude;

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

    // if filename no is given, read from it instead of stdin
    if (args_info.graph_given) {
        graph_in = fopen(args_info.graph_arg, "r");
        if (graph_in == NULL) {
            fprintf(stderr, "%s: file `%s' could not be opened.\n", argv[0], args_info.graph_arg);
            exit(EXIT_FAILURE);
        }
    }

    // if filename no is given, write to stdout
    if (args_info.dot_given) {
        dot_out = fopen(args_info.dot_arg, "w");
        if (dot_out == NULL) {
            fprintf(stderr, "%s: file `%s' could not be opened to write.\n", argv[0], args_info.dot_arg);
            exit(EXIT_FAILURE);
        }
    }

    // copy names of filtered and excluded places into mappings (used in parser)
    for (unsigned int i = 0; i < args_info.filter_given; ++i)
        filter[string(args_info.filter_arg[i])] = true;
    for (unsigned int i = 0; i < args_info.exclude_given; ++i)
        exclude[string(args_info.exclude_arg[i])] = true;

    fprintf(dot_out, "digraph G {\n");

    if (args_info.emptyStates_given) {
        fprintf(dot_out, "node [shape=circle width=0.2 style=filled fillcolor=lightgray]\n");
    } else {
        fprintf(dot_out, "node [shape=plaintext style=filled fillcolor=lightgray fontname=Helvetica]\n");
    }

    fprintf(dot_out, "edge [fontname=Helvetica]\n");
    graph_parse();
    fprintf(dot_out, "}\n");
    
    fclose(graph_in);
    
    return EXIT_SUCCESS;
}
