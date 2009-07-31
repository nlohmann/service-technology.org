// if you need assertions, always include these headers in _this_ order
// header from configure
#include "config.h"
#include <cassert>

// C Standard General Utilities Library: EXIT_SUCCESS, atoi(), etc.
#include <cstdlib>
// standard header for declaring objects that control reading from and writing to the standard streams
#include <iostream>
#include <fstream>
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

/// nf lexer and parser
// TODO



// global variable for above parsers
Graph* parsedOG;
// global variable for program invocation
string invocation;
// command line parameters
gengetopt_args_info args_info;


/// little helper methods for message output
void debug(string msg) {
    if ( args_info.debug_flag ) {
        cerr << "DEBUG: " << msg << endl;
    }
}

void info(string msg) {
    if ( args_info.verbose_flag ) {
        cout << msg << endl;
    }
}

/// main method
int main(int argc, char** argv) {

	/*--------------------------------------.
	| 0. parse the command line parameters  |
	`--------------------------------------*/

    // the command line parameters filled with default values
	cmdline_parser_init(&args_info);

    // call the command line parser from gengetopt
    // which will check for unused and unnamed paramters
    if (cmdline_parser(argc, argv, &args_info) != 0) {
    	debug("error in commandline parser");
        exit(EXIT_FAILURE);
    }

    // store invocation in a string for meta information in file output
    for (int i = 0; i < argc; ++i) {
        invocation += string(argv[i]) + " ";
    }



    // set input og source
    string inputPrefix = "";
    if ( args_info.input_given ) {

        info("og will be read from file");

    	// get ogfile prefix
        inputPrefix = string(args_info.input_arg);
        if ( inputPrefix.find(".") != string::npos ) {
            inputPrefix = inputPrefix.erase( inputPrefix.find(".") );
        }
        debug("og file prefix is '" + inputPrefix + "'");

        // open filestream
        og_yyin = fopen(args_info.input_arg, "r");
        if ( !og_yyin ) {
            cerr << PACKAGE << ": failed to open input file '" << args_info.input_arg << "'" << endl;
            exit(EXIT_FAILURE);
        }
    } else {
    	// og_yyin reads from stdin per default
        info("og will be read from stdin");
    }



    // set input cost source
    if ( args_info.costfile_given ) {

        info("cost will be read from costfile");

        // get costfile name
        string costfile = ( args_info.costfile_arg != NULL ? args_info.costfile_arg : inputPrefix + ".cf" );
        assert(costfile != "");

        // open filestream
        cf_yyin = fopen( costfile.c_str(), "r");
        if ( !cf_yyin ) {
            cerr << PACKAGE << ": failed to open costfile '" << costfile << "'" << endl;
            exit(EXIT_FAILURE);
        }
    } else if ( args_info.netfile_given ) {

        info("cost will be read from netfile");
        // TODO read open net
        cerr << PACKAGE << ": reading a netfile is not implemented" << endl;
        exit(EXIT_FAILURE);

    } else {

        cerr << PACKAGE << ": a costfile or a netfile must be given" << endl;
        exit(EXIT_FAILURE);
    }



    // set output og source
    std::ostream* outputStream = &cout;
    std::ofstream ofs;
    if (args_info.output_given) {

        info("efficient og will be written to file");

        // get ouput filename
        string outfile = ( args_info.output_arg != NULL ? args_info.output_arg : inputPrefix + "_efficient.og" );
        assert(outfile != "");

        // open outputstream
        ofs.open( outfile.c_str(), std::ios_base::trunc);
        if( !ofs ) {
            cerr << PACKAGE << ": failed to open output file '" << outfile << "'" << endl;
            exit(EXIT_FAILURE);
        }
        outputStream = &ofs;
    } else {
        // efficient og will be written to stdout
        info("efficient og will be written to stdout");
    }



	/*---------------------------------.
	| 1. parse the operating guideline |
	`---------------------------------*/

    info("reading og ...");
    parsedOG = new Graph();
    og_yyparse();
    fclose(og_yyin);

    //debug("finished og parsing, parsed following data:\n\n");
    //parsedOG->outputDebug(stdout);



    /*---------------------.
	| 2. parse cost source |
	`---------------------*/

    info("reading cost ...");
	if ( args_info.costfile_given ) {
		cf_yyparse();
		fclose(cf_yyin);
	} else {
		// TODO parse netfile and close stream
        //nf_yyparse();
        //fclose(nf_yyin);
	}

	//debug("finished cost parsing, parsed following data:\n\n");
	//parsedOG->outputDebug();
	//debug("finished cost parsing, parsed following data (recursively):\n\n");
	//(parsedOG->root)->printToStdoutRecursively();



	/*-----------------------------.
	| 3. compute cost efficient og |
	`-----------------------------*/

	info("computing cost efficient og ...");
	(parsedOG->root)->computeEfficientSuccessors();

	//debug("finished computing cost efficient og, current og:\n\n");
	//parsedOG->outputDebug();
	//debug("finished computing cost efficient og, current og (recursively):\n\n");
	//(parsedOG->root)->printToStdoutRecursively();



	/*----------------------------------------.
	| 4. output cost efficient og as og or sa |
	`----------------------------------------*/

    info("writing cost efficient og ...");
	parsedOG->output( *outputStream );



	/*-----------------------.
	| N. collect the garbage |
	`-----------------------*/

    info("closing down program ...");

    // release from parser allocated memory
    cmdline_parser_free(&args_info);

    // release memory for parsed og
    delete parsedOG;



    return EXIT_SUCCESS;
}
