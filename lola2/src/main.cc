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
#include <pthread.h>

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

/// the thread that listens for stop messages
pthread_t listener_thread;

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


/// a function collecting calls to organize termination (close files, ...)
void terminationHandler()
{
    cmdline_parser_free(&args_info);

    // shut down killer thread
    if (args_info.killerThread_given)
    {
        const int ret = pthread_cancel(listener_thread);
        if (ret == 0)
        {
            rep->status("killed listener thread");
        }
    }

    // should be the very last call
    delete rep;
}


/// helper function to terminate LoLA via sockets
__attribute__((noreturn)) void* start_listener_helper(void* ptr)
{
    Socket listener_socket(args_info.killerThread_arg);
    listener_socket.waitFor("foo");
    rep->message("received %s packet - shutting down",
        rep->markup(MARKUP_BAD, "KILL").str());
    exit(EXIT_SUCCESS);
}


/// evaluate the command line parameters
void evaluateParameters(int argc, char** argv)
{
    // initialize the parameters structure
    struct cmdline_parser_params* params = cmdline_parser_params_create();

    // call the cmdline parser
    if (UNLIKELY(cmdline_parser(argc, argv, &args_info) != 0))
    {
        fprintf(stderr, "%s: invalid command-line parameter(s)\n", PACKAGE);
        exit(EXIT_ERROR);
    }

    switch (args_info.reporter_arg)
    {
        case reporter_arg_stream:
            rep = new ReporterStream(args_info.verbose_given);
            break;

        case reporter_arg_socket:
            rep = new ReporterSocket((u_short)args_info.port_arg, args_info.address_arg, args_info.verbose_given);
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

    // start up listener thread
    if (args_info.killerThread_given)
    {
        rep->status("setting up listener socket at port %s",
            rep->markup(MARKUP_FILE, "%d", args_info.killerThread_arg).str());

        // start the listening thread
        int ret = pthread_create(&listener_thread, NULL, start_listener_helper, NULL);
    }

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
        if (!yyin)
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
    symboltable2net(symbolTables);

    delete symbolTables;

    if (args_info.randomWalk_given)
    {
        rep->status("random walk");
        randomWalk(args_info.randomWalk_arg);
    }

    if (args_info.printNet_given)
    {
        rep->status("print net");
        print_net();
    }

    Net::deleteNodes();
    Place::deletePlaces();
    Transition::deleteTransitions();
    Marking::deleteMarkings();

    return EXIT_SUCCESS;
}
