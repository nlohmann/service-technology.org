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
#include <cstdio>

#include "Reporter.h"
#include "cmdline.h"
#include "ParserPTNet.h"

#include "Net.h"
#include "Handlers.h"
#include "RandomWalk.h"
#include "CompressedIO.h"

#include "InputOutput.h"

#include "Store.h"
#include "BDDStore.h"

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
Input* netFile;


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

    IO::setReporter(rep);

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


    // file input
    if (args_info.compressed_given)
    {
        // read from stdin
        if (args_info.inputs_num == 0)
        {
            Input in("compressed net");
            ReadNetFile(in);
        }
        if (args_info.inputs_num == 1)
        {
            Input in("compressed net", args_info.inputs[0]);
            ReadNetFile(in);
        }
        if (args_info.inputs_num == 2)
        {
            Input netfile("compressed net", args_info.inputs[0]);
            ReadNetFile(netfile);
            Input namefile("compress net", args_info.inputs[1]);
            ReadNameFile(namefile);
        }
    }
    else
    {
        // handle input
        if (args_info.inputs_num == 0)
        {
            // read from stdin
            netFile = new Input("net");
        }
        else
        {
            currentFile = 0;
            netFile = new Input("net", args_info.inputs[currentFile]);
        }

        // pass the opened file pointer to flex via FILE *yyin
        yyin = *netFile;

        // read the input file(s)
        ParserPTNet* symbolTables = ParserPTNetLoLA();

        rep->status("finished parsing");

        // close net file
        delete netFile;

        yylex_destroy();

        // translate into general net structures
        symbolTables->symboltable2net();

        delete symbolTables;
    }


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

    if (args_info.writeCompressed_given)
    {
        rep->status("print compressed net");

        Output netfile("compressed net", std::string(args_info.writeCompressed_arg) + ".net");
        WriteNetFile(netfile);

        Output namefile("compressed net", std::string(args_info.writeCompressed_arg) + ".names");
        WriteNameFile(namefile);
    }

    /*
        Store *s = new BDDStore();
        s->searchAndInsert();
    */

    return EXIT_SUCCESS;
}
