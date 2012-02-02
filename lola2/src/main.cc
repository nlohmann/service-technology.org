/*!
\file main.cc

\brief main entry point for LoLA.

\author Niels
\status approved 25.01.2012

\todo ParserPTNetLoLA() exportieren
\todo Parser/Lexer-Kram in h-Datei (yyin, ...)
\todo Exit-Codes klären
*/

#include <config.h>
#include <libgen.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstdlib>

#include "Reporter.h"
#include "cmdline.h"
#include "ParserPTNet.h"

#include "RandomWalk.h"
#include "Net.h"
#include "Place.h"
#include "Marking.h"
#include "Transition.h"
#include "SymbolTable2Net.h"

extern ParserPTNet* ParserPTNetLoLA();

/// the command line parameters
gengetopt_args_info args_info;

// input files
extern FILE* yyin;

/// the reporter
Reporter* rep = NULL;

// the parsers
extern int yyparse();
extern int yylex_destroy();


/// a function collecting calls to organize termination (close files, ...)
void terminationHandler()
{
    cmdline_parser_free(&args_info);

    // should be the very last call
    delete rep;
}


/// evaluate the command line parameters
void evaluateParameters(int argc, char** argv)
{
    // overwrite invocation for consistent error messages
    //    argv[0] = basename(argv[0]);

    // initialize the parameters structure
    struct cmdline_parser_params* params = cmdline_parser_params_create();

    // call the cmdline parser
    if UNLIKELY(cmdline_parser(argc, argv, &args_info) != 0)
    {
        fprintf(stderr, "invalid command-line parameter(s)\n");
        exit(EXIT_FAILURE);
    }

    switch (args_info.reporter_arg)
    {
        case reporter_arg_stream:
            rep = new ReporterStream();
            break;
        case reporter_arg_socket:
            rep = new ReporterSocket((u_short)args_info.port_arg, args_info.address_arg);
            rep->message("pid = %d", getpid());
            break;
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
    ParserPTNet* symbolTables = ParserPTNetLoLA();

    rep->status("finished parsing");


    fclose(yyin);
    yylex_destroy();

    // translate into general net structures
    symboltable2net(symbolTables);

    delete symbolTables;

    if (args_info.randomWalk_given)
    {
        randomWalk();
    }

    Net::deleteNodes();
    Place::deletePlaces();
    Transition::deleteTransitions();
    Marking::deleteMarkings();

    return EXIT_SUCCESS;
}
