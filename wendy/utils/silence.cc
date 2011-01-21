#include <cstdlib>
#include <cstdio>
#include <string>
#include <pnapi/pnapi.h>
#include "Graph.h"
#include "verbose.h"
#include "Output.h"
#include "cmdline.h"

using std::string;

extern int graph_parse();
extern int graph_lex_destroy();
extern FILE* graph_in;

Graph g;

/// the command line parameters
gengetopt_args_info args_info;


/// evaluate the command line parameters
void evaluateParameters(int argc, char** argv) {
    // set default values
    cmdline_parser_init(&args_info);

    // initialize the parameters structure
    struct cmdline_parser_params* params = cmdline_parser_params_create();

    // call the cmdline parser
    if (cmdline_parser(argc, argv, &args_info) != 0) {
        exit(EXIT_FAILURE);
    }

    free(params);
}


int main(int argc, char** argv) {
    evaluateParameters(argc, argv);

    /*----------------------.
    | 1. parse the open net |
    `----------------------*/
    status("reading from stdin...");
    std::cin >> pnapi::io::owfn >> Graph::net;

    g.initLabels();

    Output* temp = new Output();
    temp->stream() << pnapi::io::lola << Graph::net;

    std::string command_line = "lola-statespace " + temp->name() + " -M";

    // call LoLA
    status("calling %s: '%s'", _ctool_("LoLA"), command_line.c_str());
    graph_in = popen(command_line.c_str(), "r");
    graph_parse();
    pclose(graph_in);
    graph_lex_destroy();
    status("%s is done", _ctool_("LoLA"));
    delete temp;

    g.info();

    unsigned int i = 0;
    const unsigned int j = 50;

    if (not args_info.noReduction_flag) {
    while (g.rule63() or g.rule2() or g.rule62() ) {
        if (++i % j == 0) {
            g.info();
        }
    }
}

    g.info();

    if (args_info.dot_flag) {
        g.print();
    }

    return EXIT_SUCCESS;
}
