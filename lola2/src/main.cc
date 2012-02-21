/*!
\file main.cc

\brief main entry point for LoLA.

\author Niels
\status approved 25.01.2012

\todo ParserPTNetLoLA() exportieren
\todo Parser/Lexer-Kram in h-Datei (yyin, ...)
*/

#include <config.h>
#include <libgen.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstdlib>

#include "Reporter.h"
#include "cmdline.h"
#include "ParserPTNet.h"

#include "Handlers.h"
#include "RandomWalk.h"
#include "Net.h"
#include "Place.h"
#include "Marking.h"
#include "Transition.h"
#include "CompressedIO.h"
#include <cstdio>

extern ParserPTNet* ParserPTNetLoLA();

/// the command line parameters
gengetopt_args_info args_info;

// input files
extern FILE* yyin;

/// the reporter
Reporter* rep = NULL;


/*!
\brief variable to manage multiple files
\todo This should not be global.

-1: stdin
otherwise: index in args_info.inputs
*/
int currentFile = -1;

// the parsers
extern int yyparse();
extern int yylex_destroy();


/// evaluate the command line parameters
void evaluateParameters(int argc, char** argv)
{
    // initialize the parameters structure
    struct cmdline_parser_params* params = cmdline_parser_params_create();

    // call the cmdline parser
    if (UNLIKELY(cmdline_parser(argc, argv, &args_info) != 0))
    {
        ReporterStream rep(true);
        rep.message("invalid command-line parameter(s)");
        rep.abort(ERROR_COMMANDLINE);
    }

    switch (args_info.reporter_arg)
    {
        case reporter_arg_stream:
            rep = new ReporterStream(args_info.verbose_given);
            break;

        case reporter_arg_socket:
            rep = new ReporterSocket((u_short)args_info.outputport_arg, args_info.address_arg, args_info.verbose_given);
            rep->message("pid = %d", getpid());
            break;
    }

    free(params);
}


int main(int argc, char** argv)
{
    // install exit handler for ordered exit()
    Handlers::installExitHandler();

    // parse the command line parameters
    evaluateParameters(argc, argv);

    // install termination handler for ordered premature termination
    Handlers::installTerminationHandlers();

    // handle input
    if (args_info.inputs_num == 0)
    {
        rep->status("reading from %s", rep->markup(MARKUP_FILE, "stdin").str());
        /* nothing to do - yyin is pointing to stdin */
    }
    else
    {
        currentFile = 0;
        yyin = fopen(args_info.inputs[currentFile], "r");
        if (UNLIKELY(!yyin))
        {
            rep->status("could not open file %s", rep->markup(MARKUP_FILE, args_info.inputs[currentFile]).str());
            rep->abort(ERROR_FILE);
        }
        rep->status("reading from file %s", rep->markup(MARKUP_FILE, args_info.inputs[currentFile]).str());
    }


    // read the input file(s)
    ParserPTNet* symbolTables = ParserPTNetLoLA();

    rep->status("finished parsing");

    fclose(yyin);
    yylex_destroy();

    // translate into general net structures
    symbolTables->symboltable2net();

    delete symbolTables;


    if (args_info.randomWalk_given)
    {
        rep->status("random walk");
        randomWalk(args_info.randomWalk_arg);
    }

    if (args_info.printNet_given)
    {
        rep->status("print net");
        Net::print();
    }

    Net::deleteNodes();
    Place::deletePlaces();
    Transition::deleteTransitions();
    Marking::deleteMarkings();

    return EXIT_SUCCESS;
}
