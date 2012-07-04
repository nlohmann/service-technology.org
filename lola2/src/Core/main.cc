/*!
\file main.cc
\author Niels
\status approved 25.01.2012

\brief Main entry point for LoLA.

\todo ParserPTNetLoLA() exportieren
\todo Parser/Lexer-Kram in h-Datei (yyin, ...)
*/

#include <config.h>
#include <libgen.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <new>
#include <string>

#include <cmdline.h>

#include <Core/Handlers.h>

#include <Parser/ParserPTNet.h>
#include <Parser/ast-system-k.h>

#include <Net/Net.h>
#include <Net/Marking.h>

#include <InputOutput/Reporter.h>
#include <InputOutput/CompressedIO.h>
#include <InputOutput/InputOutput.h>

#include <Exploration/RandomWalk.h>
#include <Exploration/SimpleProperty.h>

#include <Planning/Task.h>

#include <Stores/Store.h>


extern ParserPTNet* ParserPTNetLoLA();

/// the command line parameters
gengetopt_args_info args_info;

// input files
extern FILE* ptnetlola_in;

/// the reporter
Reporter* rep = NULL;

ParserPTNet* symbolTables;

/*!
\brief variable to manage multiple files
\todo This should not be global.

-1: stdin
otherwise: index in args_info.inputs
*/
int currentFile = -1;
Input* netFile;


// the parsers
extern int ptnetlola_parse();
extern int ptnetlola_lex_destroy();

void callHome(int argc, char** argv)
{
    std::string json = "";

    json += "{";
    json += "\"package_version\":\"";
    json += PACKAGE_VERSION;
    json += "\",";
    json += "\"svn_version\":\"";
    json += VERSION_SVN;
    json += "\",";
    json += "\"build_system\":\"";
    json += CONFIG_BUILDSYSTEM;
    json += "\",";
    json += "\"hostname\":\"";
    json += CONFIG_HOSTNAME;
    json += "\",";
    json += "\"architecture\":\"";
    json += (SIZEOF_VOIDP * 8 == 32) ? "32" : "64";
    json += "\",";
    json += "\"parameters\":[";
    for (int i = 1; i < argc; ++i)
    {
        if (i != 1)
        {
            json += ",";
        }
        json += "\"";
        json += argv[i];
        json += "\"";
    }
    json += "]";
    json += "}";

    Socket et(5555, "nitpickertool.com");
    et.send(json.c_str());
}

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

    // install new handler
    std::set_new_handler(Handlers::newHandler);

    if (not args_info.nolog_given)
    {
        callHome(argc, argv);
    }

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
            symbolTables = new ParserPTNet(); // initializes a symbol table that is rudimentary filled such that mapping name->id is possible
            ReadNameFile(namefile, symbolTables);
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
        ptnetlola_in = *netFile;

        // read the input file(s)
        symbolTables = ParserPTNetLoLA();

        rep->status("finished parsing");

        // close net file
        delete netFile;

        // tidy parser
        ptnetlola_lex_destroy();

        rep->status("preprocessing net");

        // translate into general net structures
        symbolTables->symboltable2net();

        // report hash table usage (size would be SIZEOF_SYMBOLTABLE)
        rep->status("%d symbol table entries, %d collisions", symbolTables->PlaceTable->card + symbolTables->TransitionTable->card, SymbolTable::collisions);
    }

    Task task;

    delete symbolTables;

    if (args_info.randomWalk_given)
    {
        rep->status("random walk");
        randomWalk(*task.getNetState(), args_info.randomWalk_arg);
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

    if (args_info.check_given and args_info.check_arg != check_arg_none)
    {
        task.setStore();
        task.setProperty();

        bool result = task.getResult();
        task.interpreteResult(result);

        // print current marking
        if (result and args_info.state_given)
        {
            task.printMarking();
        }

        // print witness path
        if (result and args_info.path_given)
        {
            task.printWitness();
        }

        if (result and args_info.run_given)
        {
            task.printDot();
        }

        if (args_info.search_arg != search_arg_findpath)
        {
            rep->message("%llu markings, %llu edges", task.s->get_number_of_markings(), (task.s->get_number_of_calls() > 0) ? task.s->get_number_of_calls() - 1 : 0);
        }

        // print statistics
        if (args_info.stats_flag)
        {
            Handlers::statistics();
        }
    }

    return EXIT_SUCCESS;
}
