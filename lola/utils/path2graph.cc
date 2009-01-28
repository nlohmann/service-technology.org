#include <cstdlib>
#include <map>
#include <string>
#include <list>
#include "cmdline-path2graph.h"

using std::string;
using std::list;
using std::map;

extern int net_parse();
extern FILE *net_in;

extern int path_parse();
extern FILE *path_in;

extern list<string> places;
extern map<string, int> marking;
extern map <string, map<string, int> > transitions;
extern list<string> sequence;

FILE *graph_out = stdout;

/// the command line parameters
gengetopt_args_info args_info;


void printMarking(unsigned int state) {
    fprintf(graph_out, "STATE %d\n", state);
    bool firstPlace = true;

    for (list<string>::iterator place = places.begin(); place != places.end(); place++) {
        if (marking[*place] != 0) {
            if (firstPlace) {
                fprintf(graph_out, "%s : %d", place->c_str(), marking[*place]);
                firstPlace = false;
            } else {
                fprintf(graph_out, ",\n%s : %d", place->c_str(), marking[*place]);
            }
        }
    }

    fprintf(graph_out, "\n");
}


void calculateMarking() {
    unsigned int currentState = 0;
    printMarking(currentState++);

    for (list<string>::iterator transition = sequence.begin(); transition != sequence.end(); transition++) {
        fprintf(graph_out, "%s -> %d\n\n", transition->c_str(), currentState);
        for (list<string>::iterator place = places.begin(); place != places.end(); place++) {
            marking[*place] += transitions[*transition][*place];
        }
        printMarking(currentState++);
    }
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
        
    free(params);
}


int main(int argc, char** argv) {
    evaluateParameters(argc, argv);

    // open net file
    net_in = fopen(args_info.net_arg, "r");    
    if (net_in == NULL) {
        fprintf(stderr, "%s: file `%s' could not be opened.\n", argv[0], args_info.net_arg);
        exit(EXIT_FAILURE);
    }

    // open graph file
    path_in = fopen(args_info.path_arg, "r");
    if (path_in == NULL) {
        fprintf(stderr, "%s: file `%s' could not be opened.\n", argv[0], args_info.path_arg);
        exit(EXIT_FAILURE);
    }

    // if no filename is given, write to stdout
    if (args_info.graph_given) {
        graph_out = fopen(args_info.graph_arg, "w");
        if (graph_out == NULL) {
            fprintf(stderr, "%s: file `%s' could not be opened to write.\n", argv[0], args_info.graph_arg);
            exit(EXIT_FAILURE);
        }
    }

    net_parse();
    path_parse();

    fclose(net_in);
    fclose(path_in);

    calculateMarking();

    return EXIT_SUCCESS;
}
