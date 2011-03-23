/*****************************************************************************\
 Candy -- Synthesizing cost efficient partners for services

 Copyright (c) 2010 Richard MŸller

 Candy is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Candy is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Candy.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/


// global headers
#include "settings.h" // global settings
#include <libgen.h>  // for basename()
#include "config-log.h" //for CONFIG_LOG
#include "Graph.h"  // Graph class
#include "Output.h"  // handles output
#include "verbose.h"  // status, debug, abort messages
#include <ctime> // clock_t


// og, cf and nf lexer and parser
extern int og_yyparse();
extern FILE* og_yyin;
extern int cf_yyparse();
extern FILE* cf_yyin;
extern int nf_yyparse();
extern FILE* nf_yyin;


// global variable for above parsers
Graph* parsedOG;
// global variable for program invocation
std::string invocation;
// global variable for filename
std::string filename;
// global variable holding the time of the call
clock_t start_clock = clock();
// global variable for command line parameters
gengetopt_args_info args_info;



/// evaluate the command line parameters
void evaluateParameters(int argc, char** argv) {

	// overwrite invocation for consistent error messages
	// TODO: basename seems to allocated memory which is not freed afterwards,
	// i.e., valgrind 3.6.0.SVN reports a memory leak
	// a fix like "free(argv[0]);" after the call of the cmdline parser works an Mac OS 10.6.4
	// but unfortunately seems to fail on linux
	argv[0] = basename(argv[0]);

	// store invocation in a std::string for meta information in file output
	for (int i = 0; i < argc; ++i) {
		invocation += (i == 0 ? "" : " ") + std::string(argv[i]);
	}

	// initialize the parameters structure
	struct cmdline_parser_params* params = cmdline_parser_params_create();

	// call the cmdline parser
	if (cmdline_parser(argc, argv, &args_info) != 0) {
		abort(1, "invalid command-line parameter(s)");
	}

	// debug option
	if (args_info.bug_flag) {
		{
			Output debug_output("bug.log", "configuration information");
			debug_output.stream() << CONFIG_LOG;
		}
		message("Please send file '%s' to %s!", _cfilename_("bug.log"), _coutput_(PACKAGE_BUGREPORT));
		exit(EXIT_SUCCESS);
	}

	/*
	 // read a configuration file if necessary
	if (args_info.config_given) {
		// initialize the config file parser
		params->initialize = 0;
		params->override = 0;

		// call the config file parser
		if (cmdline_parser_config_file(args_info.config_arg, &args_info, params) != 0) {
			abort(14, "error reading configuration file '%s'", args_info.config_arg);
		} else {
			status("using configuration file '%s'", _cfilename_(args_info.config_arg));
		}
	} else {
		// check for configuration files
		std::string conf_generic_filename = std::string(PACKAGE) + ".conf";
		std::string conf_filename = fileExists(conf_generic_filename) ? conf_generic_filename :
									(fileExists(std::string(SYSCONFDIR) + "/" + conf_generic_filename) ?
									 (std::string(SYSCONFDIR) + "/" + conf_generic_filename) : "");

		if (conf_filename != "") {
			// initialize the config file parser
			params->initialize = 0;
			params->override = 0;
			if (cmdline_parser_config_file(const_cast<char*>(conf_filename.c_str()), &args_info, params) != 0) {
				abort(14, "error reading configuration file '%s'", conf_filename.c_str());
			} else {
				status("using configuration file '%s'", _cfilename_(conf_filename));
			}
		} else {
			status("not using a configuration file");
		}
	}
	*/

	// set input og source
	filename = "";
	if ( args_info.input_given ) {

		// get ogfile prefix
		status("og will be read from file");
		filename = string(args_info.input_arg);
		if ( filename.find(".") != string::npos ) {
			filename = filename.erase( filename.find(".") );
		}
		debug("og file prefix is %s", filename.c_str());

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

		// get costfile name
		status("cost will be read from costfile");
		string costfile = ( args_info.costfile_arg != NULL ? args_info.costfile_arg : filename + ".cf" );
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

    free(params);
}


/// a function collecting calls to organize termination (close files, ...)
void terminationHandler() {
    // release memory (used to detect memory leaks)
    if (args_info.finalize_flag) {
        time_t start_time, end_time;
        time(&start_time);

        cmdline_parser_free(&args_info);
        delete parsedOG;

        time(&end_time);
        status("released memory [%.0f sec]", difftime(end_time, start_time));
    }

    // print statistics
    if (args_info.stats_flag) {
        message("runtime: %s%.2f sec%s", _bold_, (static_cast<double>(clock()) - static_cast<double>(start_clock)) / CLOCKS_PER_SEC, _c_);
        std::string call = std::string("ps -o rss -o comm | ") + TOOL_GREP + " " + PACKAGE + " | " + TOOL_AWK + " '{ if ($1 > max) max = $1 } END { print max \" KB\" }'";
        FILE* ps = popen(call.c_str(), "r");
        unsigned int memory;
        int res = fscanf(ps, "%u", &memory);
        assert(res != EOF);
        pclose(ps);
        message("memory consumption: %s%u KB %s", _bold_, memory, _c_);
    }
}


/// main method
int main(int argc, char** argv) {

	// set the function to call on normal termination
	atexit(terminationHandler);

	/*--------------------------------------.
	| 0. parse the command line parameters  |
	`--------------------------------------*/

	evaluateParameters(argc, argv);
    Output::setTempfileTemplate(args_info.tmpfile_arg);
    Output::setKeepTempfiles(args_info.noClean_flag);



	/*---------------------------------.
	| 1. parse the operating guideline |
	`---------------------------------*/

    status("reading og...");
    parsedOG = new Graph();
    og_yyparse();
    fclose(og_yyin);

    debug("finished og parsing, parsed following data:\n\n");
    if (args_info.debug_flag) parsedOG->outputDebug( cout );



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
    		outputFilename = ( args_info.output_arg != NULL ? args_info.output_arg : filename + "_efficient.og" );
    	}

    	// output sa or og
    	if ( args_info.automata_given ) {
    		status("writing cost efficient service automata...");
    		Output out( outputFilename, "service automata" );
			parsedOG->outputSA( out );
    	} else {
    		status("writing cost efficient operating guideline...");
    		Output out( outputFilename, "operating guideline" );
			parsedOG->outputOG( out );
    	}
    }


    status("closing down program...");
    return EXIT_SUCCESS;
}
