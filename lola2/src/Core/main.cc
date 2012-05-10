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
#include <cstdio>

#include "cmdline.h"

#include "Core/Handlers.h"

#include "Parser/ParserPTNet.h"
#include "Parser/ast-system-k.h"
#include "Parser/ast-system-rk.h"
#include "Parser/ast-system-unpk.h"

#include "Net/Net.h"
#include "Net/Marking.h"

#include "InputOutput/Reporter.h"
#include "InputOutput/CompressedIO.h"
#include "InputOutput/InputOutput.h"

#include "Exploration/RandomWalk.h"
#include "Exploration/Firelist.h"
#include "Exploration/SimpleProperty.h"
#include "Exploration/Deadlock.h"
#include "Exploration/FirelistStubbornDeadlock.h"
#include "Exploration/FirelistStubbornStatePredicate.h"
#include "Exploration/StatePredicateProperty.h"

#include "Formula/StatePredicate.h"

#include "Stores/Store.h"
#include "Stores/BinStore.h"
#include "Stores/BloomStore.h"
#include "Stores/BDDStore.h"
#include "Stores/SQLStore.h"
#include "Stores/STLStore.h"
#include "Stores/BitStore.h"


/// printer-function for output on stdout
void myprinter(const char* s, kc::uview v)
{
    printf("%s", s);
}



extern ParserPTNet* ParserPTNetLoLA();

/// the command line parameters
gengetopt_args_info args_info;

// input files
extern FILE* ptnetlola_in;
extern FILE* ptformula_in;

extern kc::tFormula TheFormula;

/// the reporter
Reporter* rep = NULL;

/// the top state predicate
StatePredicate *sp = NULL;

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
extern int ptformula_parse();
extern int ptformula_lex_destroy();


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




            if (args_info.formula_given)
            {
                ptformula_in = fopen(args_info.formula_arg, "r");
                ptformula_parse();
                rep->status("parsed formula file %s", rep->markup(MARKUP_FILE, basename((char*)args_info.formula_arg)).str());

                TheFormula = TheFormula->rewrite(kc::arrows);
                TheFormula = TheFormula->rewrite(kc::neg);
                TheFormula = TheFormula->rewrite(kc::sides);
                TheFormula = TheFormula->rewrite(kc::lists);

                TheFormula->print();
        //        TheFormula->unparse(myprinter, kc::out);

                TheFormula->unparse(myprinter, kc::internal);

                assert(sp);
                
                // tidy parser
                ptformula_lex_destroy();
                fclose(ptformula_in);
            }




        delete symbolTables;
    }

rep->status("%u", MAX_CAPACITY);
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


    if (args_info.check_given)
    {
        SimpleProperty* p = NULL;
        Store* s = NULL;
        Firelist* fl = NULL;

        // choose a store
        switch (args_info.store_arg)
        {
            case store_arg_bin:
                s = new BinStore();
                break;
            case store_arg_bdd:
                s = new BDDStore();
                break;
            case store_arg_bloom:
                s = new BloomStore(args_info.hashfunctions_arg);
                break;
            case store_arg_sql:
                s = new SQLStore();
                break;
            case store_arg_stl:
                s = new STLStore();
                break;
            case store_arg_bit:
                s = new BitStore();
                break;
        }

        // choose a simple property
        switch (args_info.check_arg)
        {
            case check_arg_none:
                return EXIT_SUCCESS;

            case check_arg_full:
                p = new SimpleProperty();
                break;

            case check_arg_deadlock:
                p = new Deadlock();
                fl = new FirelistStubbornDeadlock();
                break;
            
            case check_arg_statepredicate:
                p = new StatePredicateProperty(sp);
//                fl = new FirelistStubbornStatePredicate(sp);
                break;
        }

        if (fl == NULL)
        {
            fl = new Firelist();
        }

        assert(s);
        assert(p);
        assert(fl);
        const bool result = p->depth_first(*s, * fl);

        rep->message("result: %s", result ? rep->markup(MARKUP_GOOD, "yes").str() : rep->markup(MARKUP_BAD, "no").str());

        // print current marking
        if (result and args_info.state_given)
        {
            rep->message("%s", rep->markup(MARKUP_IMPORTANT, "witness state:").str());
            for (index_t p = 0; p < Net::Card[PL]; ++p)
            {
                if (Marking::Current[p] > 0)
                {
                    rep->message("%s : %d", Net::Name[PL][p], Marking::Current[p]);
                }
            }
        }

        // print witness path
        if (result and args_info.path_given)
        {
            rep->message("%s", rep->markup(MARKUP_IMPORTANT, "witness path:").str());
            index_t c;
            index_t* f;
            while (p->stack.StackPointer > 0)
            {
                index_t t = p->stack.topTransition();
                rep->message("%s", Net::Name[TR][t]);
                p->stack.pop(& c, & f);
            }
        }

        rep->message("%llu markings, %llu edges", s->markings, s->calls - 1);
        
        delete s;
        delete p;
        delete sp;
    }

    return EXIT_SUCCESS;
}
