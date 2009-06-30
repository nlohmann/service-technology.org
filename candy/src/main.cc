// if you need assertions, always include these headers in _this_ order
// header from configure
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
// header for graph structure
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
Graph* parsedOG;



/// little helper method for message output
void debug(string msg) {
	cerr << "DEBUG: " << msg << endl;
}

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
    	debug("error in commandline parser");
        exit(EXIT_FAILURE);
    }

    // set input og source
    string inputPrefix = "";
    if ( args_info.input_given ) {
    	// get og prefix in case of missing costfile/open net prefix
        inputPrefix = string(args_info.input_arg);
        if ( inputPrefix.find(".") != string::npos ) {
            inputPrefix = inputPrefix.erase( inputPrefix.find(".") );
        }
        assert(inputPrefix != "");
        debug("og file prefix is '" + inputPrefix + "'");

        og_yyin = fopen(args_info.input_arg, "r");
        if ( !og_yyin ) {
            cerr << PACKAGE << ": failed to open input file '" << args_info.input_arg << "'" << endl;
            exit(EXIT_FAILURE);
        }
    } else {
    	// og_yyin reads from stdin per default
    }

    // set input cost source
    if ( args_info.costfile_given ) {
        if ( args_info.costfile_arg != NULL ) {
            cf_yyin = fopen( args_info.costfile_arg, "r");
        } else {
        	string costfile = inputPrefix + ".cf";
            cf_yyin = fopen( costfile.c_str(), "r");
        }

        if ( !cf_yyin ) {
            cerr << PACKAGE << ": failed to open costfile '" << args_info.costfile_arg << "'" << endl;
            exit(EXIT_FAILURE);
        }
    } else if ( args_info.netfile_given ) {
        // TODO read open net
    } else {
        cerr << PACKAGE << ": a costfile or a netfile must be given" << endl;
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
    	debug("parse og file");
    } else {
        debug("parse og from stdin");
    }

    parsedOG = new Graph();
    og_yyparse();
    fclose(og_yyin);

    debug("finished og parsing, parsed following data:\n\n");
    parsedOG->printToStdout();


    /*---------------------.
	| 2. parse cost source |
	`---------------------*/

	// parse costfile or netfile and fill map ogEdges
	if ( args_info.costfile_given ) {
		debug("parse costfile");
		cf_yyparse();
		fclose(cf_yyin);
	} else {
		debug("parse netfile");
		// TODO parse netfile and close stream
	}

	debug("finished cost parsing, parsed following data:\n\n");
	parsedOG->printToStdout();

	debug("finished cost parsing, parsed following data (recursively):\n\n");
	(parsedOG->root)->printToStdoutRecursively();


	/*-----------------------------.
	| 3. compute cost-efficient OG |
	`-----------------------------*/

    //parsedOG->computeInefficientNodes();
	debug("compute cost efficient OG");
	(parsedOG->root)->computeEfficientSuccessors();


	/*----------------------------------.
	| 4. compute cost-efficient partner |
	`----------------------------------*/

	//parsedOG->printEfficientOG(stream);

	/*-----------------------.
	| N. collect the garbage |
	`-----------------------*/

    // release from parser allocated memory
    cmdline_parser_free(&args_info);


    // release memory for parsed OG
    delete parsedOG;

    return EXIT_SUCCESS;
}
