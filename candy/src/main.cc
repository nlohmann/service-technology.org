// global headers and DEBUG INFO END macros
#include "settings.h"
// header for graph structure
#include "Graph.h"



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
// global variable for command line parameters
gengetopt_args_info args_info;



/// main method
int main(int argc, char** argv) {

	/*--------------------------------------.
	| 0. parse the command line parameters  |
	`--------------------------------------*/

    // the command line parameters filled with default values
	cmdline_parser_init(&args_info);

    // call the command line parser from gengetopt
    // which will check for unused and unnamed paramters
    // this will set the correct flags for the macros from settings.h
    if (cmdline_parser(argc, argv, &args_info) != 0) {
    	DEBUG "error in commandline parser" END
        exit(EXIT_FAILURE);
    }

    // store invocation in a string for meta information in file output
    for (int i = 0; i < argc; ++i) {
        invocation += string(argv[i]) + " ";
    }



    // set input og source
    string inputPrefix = "";
    if ( args_info.input_given ) {

        INFO "og will be read from file" END

    	// get ogfile prefix
        inputPrefix = string(args_info.input_arg);
        if ( inputPrefix.find(".") != string::npos ) {
            inputPrefix = inputPrefix.erase( inputPrefix.find(".") );
        }
        DEBUG "og file prefix is '" + inputPrefix + "'" END

        // open filestream
        og_yyin = fopen(args_info.input_arg, "r");
        if ( !og_yyin ) {
            cerr << PACKAGE << ": failed to open input file '" << args_info.input_arg << "'" << endl;
            exit(EXIT_FAILURE);
        }
    } else {
    	// og_yyin reads from stdin per default
        INFO "og will be read from stdin" END
    }



    // set input cost source
    if ( args_info.costfile_given ) {

        INFO "cost will be read from costfile" END

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

        INFO "cost will be read from netfile" END

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

        INFO "efficient og will be written to file" END

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
        INFO "efficient og will be written to stdout" END
    }



	/*---------------------------------.
	| 1. parse the operating guideline |
	`---------------------------------*/

    INFO "reading og ..." END
    parsedOG = new Graph();
    og_yyparse();
    fclose(og_yyin);

    //debug("finished og parsing, parsed following data:\n\n");
    //parsedOG->outputDebug(stdout);



    /*---------------------.
	| 2. parse cost source |
	`---------------------*/

    INFO "reading cost ..." END
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

	INFO "computing cost efficient og ..." END
	(parsedOG->root)->computeEfficientSuccessors();

	//debug("finished computing cost efficient og, current og:\n\n");
	//parsedOG->outputDebug();
	//debug("finished computing cost efficient og, current og (recursively):\n\n");
	//(parsedOG->root)->printToStdoutRecursively();



	/*----------------------------------------.
	| 4. output cost efficient og as og or sa |
	`----------------------------------------*/

    INFO "writing cost efficient og ..." END
	parsedOG->output( *outputStream );



	/*-----------------------.
	| N. collect the garbage |
	`-----------------------*/

    INFO "closing down program ..." END

    // release from parser allocated memory
    cmdline_parser_free(&args_info);

    // release memory for parsed og
    delete parsedOG;



    return EXIT_SUCCESS;
}
