#include <iostream>
#include <fstream>
#include <cstdlib>
#include "pnapi.h"
#include "cmdline.h"

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
    
    // collect input files
    if (args_info.inputs_num == 0) {
        // read from stdin
        fprintf(stderr, "You gave no input files.\n");
    } else {
        // read from files
        fprintf(stderr, "You gave %d input files:\n", args_info.inputs_num);
        for (unsigned int i = 0; i < args_info.inputs_num; i++) {
            fprintf(stderr, "file #%d: %s\n", i, args_info.inputs[i]);
        }
    }
    
    
    return EXIT_SUCCESS;
}
