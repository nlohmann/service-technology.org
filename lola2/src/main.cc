/*!
\file main.cc

\brief main entry point for LoLA.

\author Niels
\status new
*/

#include <config.h>
#include <cstdlib>
#include <libgen.h>

#include "Reporter.h"
#include "cmdline.h"

/// the command line parameters
gengetopt_args_info args_info;

// input files
extern FILE* yyin;

/// the reporter
Reporter* rep = new ReporterStream();
//Reporter *rep = new ReporterSocket(1234, "127.0.0.1");

// the parsers
extern int yyparse();
extern int yylex_destroy();


/// a function collecting calls to organize termination (close files, ...)
void terminationHandler()
{
    cmdline_parser_free(&args_info);
}


/// evaluate the command line parameters
void evaluateParameters(int argc, char** argv)
{
    // overwrite invocation for consistent error messages
    argv[0] = basename(argv[0]);

    // initialize the parameters structure
    struct cmdline_parser_params* params = cmdline_parser_params_create();

    // call the cmdline parser
    if (cmdline_parser(argc, argv, &args_info) != 0)
    {
        rep->abort(1, "invalid command-line parameter(s)");
        exit(EXIT_FAILURE);
    }

    free(params);
}


int main(int argc, char** argv)
{
    // set the function to call on normal termination
    atexit(terminationHandler);

    // parse the command line parameters
    evaluateParameters(argc, argv);

    // read the input file(s)
    yyparse();
    fclose(yyin);
    yylex_destroy();

    return EXIT_SUCCESS;
}
