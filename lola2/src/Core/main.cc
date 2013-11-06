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

// a net parser
extern ParserPTNet *ParserPTNetLoLA();

// the input file for Flex
extern FILE *ptnetlola_in;

// a cleanup function from Flex
extern int ptnetlola_lex_destroy();

// the reporter
extern Reporter *rep;


/*!
\brief the command line parameters
\note variable is initialized by function evaluateParameters()
*/
gengetopt_args_info args_info;

/// symbol tables for the parsed net
ParserPTNet *symbolTables = NULL;

/// symbol table for a given Büchi automaton
SymbolTable *buechiStateTable = NULL;

/*!
\brief variable to manage multiple files
\todo This should not be global.

-1: stdin
otherwise: index in args_info.inputs
*/
int currentFile = -1;

/*!
\brief the current file to read a Petri net from
\note is changed by ptnetlola_wrap()
*/
Input *netFile = NULL;


/*!
\brief send the used configuration to a logging server
\note can be switched off with the command line option `--nolog`
*/
void callHome(int argc, char **argv)
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

    Socket et(5555, "nitpickertool.com", false);
    et.send(json.c_str());
}


/*!
\brief evaluate the command line parameters
\post global variable args_info is usable

\todo What does IO::setReporter(rep) do?
*/
void evaluateParameters(int argc, char **argv)
{
    // initialize the parameters structure
    struct cmdline_parser_params *params = cmdline_parser_params_create();

    // call the cmdline parser
    if (UNLIKELY(cmdline_parser(argc, argv, &args_info) != 0))
    {
        ReporterStream rep(true);
        rep.message("invalid command-line parameter(s)");
        rep.abort(ERROR_COMMANDLINE);
    }

    // select a report according to the --reporter option
    switch (args_info.reporter_arg)
    {
    case reporter_arg_stream:
        rep = new ReporterStream(args_info.verbose_given);
        break;

    case reporter_arg_socket:
        rep = new ReporterSocket((u_short)args_info.outputport_arg,
                                 args_info.address_arg, args_info.verbose_given);
        rep->message("pid = %d", getpid());
        break;

    case reporter__NULL:
        /* no reporter */
        break;
    }

    // register the reporter
    IO::setReporter(rep);

    free(params);
}


/*!
\brief the main workflow of LoLA

\todo use MARKUP_FILE for all concepts
\todo What does --testStateForeach do?
\todo Check if task.printDot() should have the same checks as task.printWitness()
\todo numMarkings and numEdges should be provided by the Task class
*/
int main(int argc, char **argv)
{
    //=================
    // (1) set up LoLA 
    //=================
    
    // install exit handler for ordered exit()
    Handlers::installExitHandler();

    // parse the command line parameters
    evaluateParameters(argc, argv);

    // install termination handler for ordered premature termination
    Handlers::installTerminationHandlers();

    // install new handler
    std::set_new_handler(Handlers::newHandler);

    // call home unless option --nolog is used
    if (not args_info.nolog_given)
    {
        callHome(argc, argv);
    }


    //===================
    // (2) process input 
    //===================

    if (args_info.buechi_given)
    {
        buechiStateTable = new SymbolTable();
    }

    // file input
    if (args_info.compressed_given)
    {
        rep->status("input: compressed net (%s)", rep->markup(MARKUP_PARAMETER, "--compressed").str());

        // read from stdin
        if (args_info.inputs_num == 0)
        {
            Input in("compressed net");
            ReadNetFile(in);
            rep->status("no name file given");
        }
        if (args_info.inputs_num == 1)
        {
            Input in("compressed net", args_info.inputs[0]);
            ReadNetFile(in);
            rep->status("no name file given");
        }
        if (args_info.inputs_num == 2)
        {
            Input netfile("compressed net", args_info.inputs[0]);
            ReadNetFile(netfile);
            Input namefile("names", args_info.inputs[1]);
            symbolTables = new ParserPTNet();
            // initializes a symbol table that is rudimentary filled such that mapping name->id is possible
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
        rep->status("%d symbol table entries, %d collisions",
                    symbolTables->PlaceTable->card + symbolTables->TransitionTable->card,
                    SymbolTable::collisions);
    }

    // create a task object to process
    Task task;

    delete symbolTables;
    rep->status("finished preprocessing");


    //===========================
    // (3) miscellaneous options
    //===========================

    if (args_info.randomWalk_given)
    {
        rep->status("debug function: randomly firing transitions (%s)", rep->markup(MARKUP_PARAMETER, "--randomWalk").str());
        randomWalk(*task.getNetState(), args_info.randomWalk_arg);
    }

    if (args_info.printNet_given)
    {
        rep->status("debug function: print net (%s)", rep->markup(MARKUP_PARAMETER, "--printNet").str());
        Net::print();
    }

    if (args_info.writeCompressed_given)
    {
        rep->status("output: compressed net (%s)", rep->markup(MARKUP_PARAMETER, "--writeCompressed").str());

        Output netfile("compressed net",
                       std::string(args_info.writeCompressed_arg) + ".net");
        WriteNetFile(netfile);

        Output namefile("compressed net",
                        std::string(args_info.writeCompressed_arg) + ".names");
        WriteNameFile(namefile);
    }


    //=====================
    // (4) excecute checks
    //=====================

    if (args_info.check_given and args_info.check_arg != check_arg_none)
    {
        // prepare task
        task.setStore();
        task.setProperty();

        /*if (args_info.symmetry_given)
        {
            // TODO: Move this to a more appropriate place.
            GeneratingSystem::create();
        }*/

        //======================
        // (5) the actual check
        //======================

        bool result = task.getResult();
        task.interpreteResult(& result);

        //============
        // (6) output
        //============

        // print current marking
        if (result and args_info.state_given)
        {
            rep->status("print witness state (%s)", rep->markup(MARKUP_PARAMETER, "--state").str());
            task.printMarking();
        }

        // print witness path
        if (args_info.path_given and task.hasWitness(result))
        {
            rep->status("print witness path (%s)", rep->markup(MARKUP_PARAMETER, "--path").str());
            task.printWitness();
        }

        if (result and args_info.run_given)
        {
            rep->status("print distributed run (%s)", rep->markup(MARKUP_PARAMETER, "--run").str());
            task.printDot();
        }

        //================
        // (7) statistics
        //================

        if (args_info.search_arg != search_arg_findpath)
        {
            uint64_t numMarkings = 0;
            if (task.store)
            {
                numMarkings = task.store->get_number_of_markings();
            }
            else if (task.ltlStore)
            {
                numMarkings = task.ltlStore->get_number_of_markings();
            }
            else if (task.ctlStore)
            {
                numMarkings = task.ctlStore->get_number_of_markings();
            }
            uint64_t numEdges = 0;
            if (task.store)
            {
                numEdges = task.store->get_number_of_calls();
            }
            else if (task.ltlStore)
            {
                numEdges = task.ltlStore->get_number_of_calls();
            }
            else if (task.ctlStore)
            {
                numEdges = task.ctlStore->get_number_of_calls();
            }
            if (numEdges)
            {
                numEdges--;
            }
            rep->message("%llu markings, %llu edges", numMarkings, numEdges);
        }

        // print statistics
        if (args_info.stats_flag)
        {
            rep->status("print statistics (%s)", rep->markup(MARKUP_PARAMETER, "--stats").str());
            Handlers::statistics();
        }

        // test popState()
        // TODO: remove this when actual use case is implemented
        if (args_info.testStateForeach_flag)
        {
            task.testPopState();
        }
    }

    return EXIT_SUCCESS;
}
