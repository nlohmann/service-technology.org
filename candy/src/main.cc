// TODO remove?
//#include <cstdlib>
//#include <ctime>
//#include <cassert>
//#include <sstream>
//#include <cstdio>
#include <cstdlib>
//#include "pnapi.h"

// standard header for declaring objects that control reading from and writing to the standard streams 
#include <iostream>
// c++ strings library
#include <string>

// header from gengetopt
#include "cmdline.h"
// header from configure
#include "config.h"
// header for graph class
#include "Graph.h"


// used namespaces
using std::cerr;
using std::cout;
using std::endl;
using std::string;


/// lexer and parser
extern int og_yyparse();
extern FILE* og_yyin;

Graph* og;



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
    string inputPrefix;
    if ( args_info.input_given ) {

        inputPrefix = string(args_info.input_arg);
        if ( inputPrefix.find(".") != string::npos ) {
            inputPrefix = inputPrefix.erase( inputPrefix.find(".") );
        }
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
        // TODO
    } else if ( args_info.netfile_given ) {
        // TODO
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

    if ( args_info.verbose_given ) {
        if ( args_info.input_given ) {
            cerr << "DEBUG: parse og file '" << args_info.input_arg << "'" << endl;
        } else {
            cerr << "DEBUG: parse og from stdin" << endl;
        }
    }

    og = new Graph();
    og_yyparse();
    fclose(og_yyin);
    
    cerr << "DEBUG: finished og parsing, parsed following data:\n\n" << endl;
    og->printToStdout();


	/*-------------------.
	| 2. parse cost file |
	`-------------------*/

    // TODO
    

	/*-----------------------------.
	| 3. compute cost-efficient OG |
	`-----------------------------*/

    //og->flagInefficientNodes();


	/*----------------------------------.
	| 4. compute cost-efficient partner |
	`----------------------------------*/


	/*-----------------------.
	| N. collect the garbage |
	`-----------------------*/

    // release from parser allocated memory
    cmdline_parser_free(&args_info);


    // release memory for parse OG
    delete og;
}
