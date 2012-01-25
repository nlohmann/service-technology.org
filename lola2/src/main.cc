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
#include <cstdlib>
#include <libgen.h>
#include <sys/types.h>
#include <unistd.h>

#include "Reporter.h"
#include "cmdline.h"
#include "ParserPTNet.h"

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
}


/// evaluate the command line parameters
void evaluateParameters(int argc, char** argv)
{
    // overwrite invocation for consistent error messages
    //    argv[0] = basename(argv[0]);

    // initialize the parameters structure
    struct cmdline_parser_params* params = cmdline_parser_params_create();

    // call the cmdline parser
    if (cmdline_parser(argc, argv, &args_info) != 0)
    {
        fprintf(stderr, "invalid command-line parameter(s)\n");
        exit(EXIT_FAILURE);
    }

    switch (args_info.reporter_arg)
    {
        case (reporter_arg_stream):
            rep = new ReporterStream();
            break;
        case (reporter_arg_socket):
            rep = new ReporterSocket(args_info.port_arg, args_info.address_arg);
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
    rep->status("done");

    fclose(yyin);
    yylex_destroy();
    delete symbolTables;

    return EXIT_SUCCESS;
}
