#include <cstdlib>
#include <string>
#include "cmdline.h"
#include "verbose.h"

/// the command line parameters
gengetopt_args_info args_info;

using std::string;

/// the parser
extern int chor_parse();

/// evaluate the command line parameters
void evaluateParameters(int argc, char** argv) {
    // set default values
    argv[0] = (char *)PACKAGE;

    cmdline_parser_init(&args_info);

    // initialize the parameters structure
    struct cmdline_parser_params *params = cmdline_parser_params_create();

    // call the cmdline parser
    cmdline_parser(argc, argv, &args_info);

    free(params);
}


int main(int argc, char** argv) {
    /*--------------------------------------.
    | 0. parse the command line parameters  |
    `--------------------------------------*/
    evaluateParameters(argc, argv);

    /*--------------------------------------.
    | 1. parse the chreography description  |
    `--------------------------------------*/
    // * open the file
    chor_parse();
    // * close the file

    /*----------------------------.
    | 2. prepare data structures  |
    `----------------------------*/
    // * check whether choreography is sane (each input has an output)
    // * create an array to quickly check whether two events are distant

    /*---------------------------------------------.
    | 3. main algorithm: resolution of dependency  |
    `---------------------------------------------*/

    /*--------------------------------.
    | 4. final step: peer projection  |
    `--------------------------------*/

    /*----------------.
    | 5. file output  |
    `----------------*/

    return EXIT_SUCCESS;
}