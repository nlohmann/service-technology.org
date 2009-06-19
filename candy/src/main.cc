// if you need assertions, always include these headers in _this_ order
#include "config.h"
#include <cassert>

// TODO remove?
// Merke: Header sind Case-Sensitiv
//#include <ctime>
//#include <sstream>
//#include <cstdio>
//#include "pnapi.h"

// C Standard General Utilities Library: EXIT_SUCCESS, atoi(), etc.
#include <cstdlib>
// standard header for declaring objects that control reading from and writing to the standard streams 
#include <iostream>
// C++ strings library
#include <string>
// using Map container
#include <map>

// header from gengetopt
#include "cmdline.h"
// header from configure
#include "Graph.h"


// used namespaces
using std::cerr;
using std::cout;
using std::endl;
using std::string;


/// og lexer and parser
extern int og_yyparse();
extern FILE* og_yyin;

/// cf lexer and parser
extern int cf_yyparse();
extern FILE* cf_yyin;

// global variables for above parsers
Graph* ogGraph;
map< string, unsigned int > ogEdges;



/// main method
int main(int argc, char** argv) {

	//time_t start_time, end_time;
	//time(&start_time);

	/*--------------------------------------.
	| 0. parse the command line parameters  |
	`--------------------------------------*/

    // the command line parameters filled with default values
    gengetopt_args_info args_info;
	cmdline_parser_init(&args_info);

    // call the command line parser from gengetopt
    // which will check for unused and unnamed paramters
    if (cmdline_parser(argc, argv, &args_info) != 0) {
        exit(EXIT_FAILURE);
    }

    // set input og source
    string inputPrefix = "";
    if ( args_info.input_given ) {
        inputPrefix = string(args_info.input_arg);
        if ( inputPrefix.find(".") != string::npos ) {
            inputPrefix = inputPrefix.erase( inputPrefix.find(".") );
        }
        assert(inputPrefix != "");
        cerr << "DEBUG: og file prefix is '" << inputPrefix << "'" << endl;

        og_yyin = fopen(args_info.input_arg, "r");
        if ( !og_yyin ) {
            cerr << PACKAGE << ": failed to open input file '"
                 << args_info.input_arg << "'" << endl;
            exit(EXIT_FAILURE);
        }
    }

    // set input cost source
    if ( args_info.costfile_given ) {
        if ( args_info.costfile_arg != NULL ) {
            cf_yyin = fopen( args_info.costfile_arg, "r");
        } else {
            cf_yyin = fopen( inputPrefix.c_str(), "r");
        }

        if ( !cf_yyin ) {
            cerr << PACKAGE << ": failed to open cost file '"
                 << args_info.costfile_arg << "'" << endl;
            exit(EXIT_FAILURE);
        }
    } else if ( args_info.netfile_given ) {
        // TODO read owfn
    } else {
        cerr << PACKAGE << ": a cost file or a net file must be given" << endl;
        exit(EXIT_FAILURE);
    }

    // set output source
    std::ostream* candyOut = &cout;
    if (args_info.output_given) {

        // TODO
        //if (args_info.output_arg != NULL) {
        //    candyOut.open(args_info.output_arg, std::ios_base::trunc);
        //} else {
        //    candyOut.open(args_info.input_arg + ".owfn", std::ios_base::trunc);
        //}
    }


	/*---------------------------------.
	| 1. parse the operating guideline |
	`---------------------------------*/

    if ( args_info.input_given ) {
        cerr << "DEBUG: parse og file '" << args_info.input_arg << "'" << endl;
    } else {
        cerr << "DEBUG: parse og from stdin" << endl;
    }

    ogGraph = new Graph();
    og_yyparse();
    fclose(og_yyin);
    
    cerr << "DEBUG: finished og parsing, parsed following data:\n\n" << endl;
    ogGraph->printToStdout();


	/*-------------------.
	| 2. parse cost file |
	`-------------------*/

    // TODO
    if ( args_info.costfile_given ) {
        cerr << "\nDEBUG: parse cf file '" << args_info.costfile_arg << "'" << endl;
        cf_yyparse();
        fclose(cf_yyin);
    } else {
        // TODO parse owfn
    }

    cerr << "DEBUG: finished cost parsing, parsed following data:\n\n" << endl;
    for ( map< string, unsigned int >::const_iterator iter = ogEdges.begin();
          iter != ogEdges.end(); ++iter ) {

        cerr << "'" << iter->first << "' with cost '" << iter->second << "'" << endl;
    }


	/*-----------------------------.
	| 3. compute cost-efficient OG |
	`-----------------------------*/

    //ogGraph->flagInefficientNodes();


	/*----------------------------------.
	| 4. compute cost-efficient partner |
	`----------------------------------*/


	/*-----------------------.
	| N. collect the garbage |
	`-----------------------*/

    // release from parser allocated memory
    cmdline_parser_free(&args_info);


    // release memory for parsed OG
    delete ogGraph;
    ogEdges.clear();

    return EXIT_SUCCESS;
}
