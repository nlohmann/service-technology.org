// global headers and DEBUG INFO END macros
#include "settings.h"
#include "Graph.h"
#include "Output.h"
#include "verbose.h"


/// og lexer and parser
extern int og_yyparse();
extern FILE* og_yyin;

/// cf lexer and parser
extern int cf_yyparse();
extern FILE* cf_yyin;

/// nf lexer and parser
extern int nf_yyparse();
extern FILE* nf_yyin;



/// global variable for above parsers
Graph* parsedOG;
/// global variable for program invocation
std::string invocation;
/// global variable for command line parameters
gengetopt_args_info args_info;



/// main method
int main(int argc, char** argv) {

	/*--------------------------------------.
	| 0. parse the command line parameters  |
	`--------------------------------------*/

    // the command line parameters filled with default values
	//cmdline_parser_init(&args_info);

    // call the command line parser from gengetopt
    // which will check for unused and unnamed paramters
    // this will set the correct flags for the macros from settings.h
    if (cmdline_parser(argc, argv, &args_info) != 0) {
    	abort( 1, "invalid command-line parameter(s)" );
    }

    // store invocation in a string for meta information in file output
    for (int i = 0; i < argc; ++i) {
        invocation += string(argv[i]) + " ";
    }



    // set input og source
    string inputPrefix = "";
    if ( args_info.input_given ) {

        status("og will be read from file");

    	// get ogfile prefix
        inputPrefix = string(args_info.input_arg);
        if ( inputPrefix.find(".") != string::npos ) {
            inputPrefix = inputPrefix.erase( inputPrefix.find(".") );
        }
        debug("og file prefix is %s", inputPrefix.c_str());

        // open filestream
        og_yyin = fopen(args_info.input_arg, "r");
        if ( !og_yyin ) {
            abort( 2, "failed to open input file %s", args_info.input_arg);
        }
    } else {
    	// og_yyin reads from stdin per default
        status("og will be read from stdin");
    }



    // set input cost source
    if ( args_info.costfile_given ) {

    	status("cost will be read from costfile");

        // get costfile name
        string costfile = ( args_info.costfile_arg != NULL ? args_info.costfile_arg : inputPrefix + ".cf" );
        assert(costfile != "");

        // open filestream
        cf_yyin = fopen( costfile.c_str(), "r");
        if ( !cf_yyin ) {
            abort( 3, "failed to open costfile %s", costfile.c_str());
        }
    //} else if ( args_info.netfile_given ) {

    //    INFO "cost will be read from netfile" END

    //    // get netfile name
    //    string netfile = ( args_info.netfile_arg != NULL ? args_info.netfile_arg : inputPrefix + ".owfn" );
    //    assert(netfile != "");

    //    // open filestream
    //    nf_yyin = fopen( netfile.c_str(), "r");
    //    if ( !nf_yyin ) {
    //        ERROR "failed to open netfile '" << netfile << "'" END
    //        exit(EXIT_FAILURE);
    //    }
    } else {
    	abort( 4, "a costfile must be given");
    }



	/*---------------------------------.
	| 1. parse the operating guideline |
	`---------------------------------*/

    status("reading og...");
    parsedOG = new Graph();
    og_yyparse();
    fclose(og_yyin);

    debug("finished og parsing, parsed following data:\n\n");
    if (args_info.debug_flag) parsedOG->outputDebug( cout);



    /*---------------------.
	| 2. parse cost source |
	`---------------------*/

    status("reading cost...");
	if ( args_info.costfile_given ) {
		cf_yyparse();
		fclose(cf_yyin);
	} else {
        //nf_yyparse();
        //fclose(nf_yyin);
        assert(false);
	}

	debug("finished cost parsing, parsed following data:\n\n");
    if (args_info.debug_flag) parsedOG->outputDebug( cout );
	debug("finished cost parsing, parsed following data (recursively):\n\n");
    if (args_info.debug_flag) {
        map<Node*, bool> printed;
        (parsedOG->root)->outputDebugRecursively( cout, printed );
        printed.clear();
    }



	/*-----------------------------.
	| 3. compute cost efficient og |
	`-----------------------------*/

	status("computing cost efficient og...");
	(parsedOG->root)->computeCost();
    parsedOG->removeInefficientNodesAndEvents();

	debug("finished computing cost efficient og, current og:\n\n");
    if (args_info.debug_flag) parsedOG->outputDebug( cout );
	debug("finished computing cost efficient og, current og (recursively):\n\n");
    if (args_info.debug_flag) {
        map<Node*, bool> printed;
        (parsedOG->root)->outputDebugRecursively( cout, printed );
        printed.clear();
    }



	/*----------------------------------------.
	| 4. output cost efficient og as og or sa |
	`----------------------------------------*/

    {
    	// write to stdout per default or to file given with output parameter
    	std::string outputFilename = "-";
    	if ( args_info.output_given ) {
    		outputFilename = ( args_info.output_arg != NULL ? args_info.output_arg : inputPrefix + "_efficient.og" );
    	}

    	// output sa or og
    	if ( args_info.automata_given ) {
    		status("writing cost efficient service automata...");
    		Output out( outputFilename, "service automata" );
			parsedOG->outputOG( out );
    	} else {
    		status("writing cost efficient operating guideline...");
    		Output out( outputFilename, "operating guideline" );
			parsedOG->outputOG( out );
    	}
    }



	/*-----------------------.
	| N. collect the garbage |
	`-----------------------*/

    status("closing down program...");

    // release from parser allocated memory
    cmdline_parser_free(&args_info);

    // release memory for parsed og
    delete parsedOG;



    return EXIT_SUCCESS;
}
