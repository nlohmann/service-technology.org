/*****************************************************************************\
 BSD -- generating BSD automata

 Copyright (c) 2013 Simon Heiden

 BSD is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 BSD is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with BSD.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/


#include <config.h>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <libgen.h>
#include <fstream>
#include <sstream>
#include <string>
#include "config-log.h"
#include "InnerMarking.h"
#include "openNet.h"
#include "BSD.h"
#include "CSD.h"
#include "partnercheck.h"
#include "confcheck.h"
#include "parser.h"
#include "Label.h"
#include "cmdline.h"
#include "Output.h"
#include "verbose.h"
#include "util.h"
#include <pnapi/pnapi.h>


// input files
extern FILE* graph_in;

// the parsers
extern int graph_parse();
extern int graph_lex_destroy();

/// the command line parameters
gengetopt_args_info args_info;

/// the invocation string
std::string invocation;

/// a file to store a mapping from marking ids to actual Petri net markings
Output* markingoutput = NULL;

/// a variable holding the time of the call
clock_t start_clock = clock();

/// check if a file exists and can be opened for reading
inline bool fileExists(const std::string& filename) {
    std::ifstream tmp(filename.c_str(), std::ios_base::in);
    return tmp.good();
}


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
        abort(16, "invalid command-line parameter(s)");
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

    // read a configuration file if necessary
    if (args_info.config_given) {
        // initialize the config file parser
        params->initialize = 0;
        params->override = 0;

        // call the config file parser
        if (cmdline_parser_config_file(args_info.config_arg, &args_info, params) != 0) {
            abort(15, "error reading configuration file '%s'", args_info.config_arg);
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
                abort(15, "error reading configuration file '%s'", conf_filename.c_str());
            } else {
                status("using configuration file '%s'", _cfilename_(conf_filename));
            }
        } else {
            status("not using a configuration file");
        }
    }

    // set LoLA if it is present in the environment and not set otherwise
    if (getenv("LOLA") != NULL and not args_info.lola_given) {
        status("using environment variable '%s' set to '%s'",
            _cfilename_("LOLA"), _cfilename_(getenv("LOLA")));
        free(args_info.lola_arg);
        args_info.lola_arg = strdup(getenv("LOLA"));
    }

    // only one option is allowed at one time
    if (args_info.partnerCheck_flag + args_info.confCheck_flag + args_info.matching_flag
    		+ args_info.max_flag + args_info.mp_flag + args_info.BSD_flag + args_info.CSD_flag > 1) {
        abort(4, "only one basic option is allowed");
    }

    if (args_info.bound_arg < 1) {
    	abort(5, "bound has to be a positive integer");
    }

    // check whether (at least) two files are given in the case of matching
    if (args_info.inputs_num < 2 && (args_info.matching_flag)) {
    	abort(6, "two input files must be given");
    }

    // check whether two files are given in the case of partner or conformance check
    if (args_info.inputs_num != 2 && (args_info.partnerCheck_flag || args_info.confCheck_flag)) {
    	abort(6, "two input files must be given");
    }

    // check whether at most one file given
    if (args_info.inputs_num > 1 && !(args_info.partnerCheck_flag || args_info.confCheck_flag || args_info.matching_flag)) {
    	abort(7, "at most one input file must be given");
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


int main(int argc, char** argv) {
    time_t start_time, end_time, program_start;
    int returnvalue = EXIT_SUCCESS;

    // set the function to call on normal termination
    atexit(terminationHandler);

    /*--------------------------------------.
    | 1. parse the command line parameters  |
    `--------------------------------------*/
    evaluateParameters(argc, argv);
    Output::setTempfileTemplate(args_info.tmpfile_arg);
    Output::setKeepTempfiles(args_info.noClean_flag);

    time(&program_start);

    // in case of BSD or CSD computation
    if (args_info.BSD_flag || args_info.CSD_flag) {

        // set standard filenames
        std::string filename;
        std::string filepath;
        filename = std::string(PACKAGE) + "_output";
        filepath = "./";

        BSDgraph _BSDgraph;

    	openNet::initialize();

    	/*------------------------.
    	| 1.0. parse the open net |
    	`------------------------*/
    	try {
    		// parse either from standard input or from a given file
    		if (args_info.inputs_num == 0) {
    			status("reading from stdin...");
    			std::cin >> pnapi::io::owfn >> *openNet::net;
    		} else {
    			// strip suffix from input filename
    			filename = std::string(args_info.inputs[0]).substr(0, std::string(args_info.inputs[0]).find_last_of("."));
    			filepath = std::string(args_info.inputs[0]).substr(0, filename.find_last_of("/")+1);
    			filename = filename.substr(filename.find_last_of("/")+1, filename.length());

    			std::ifstream inputStream(args_info.inputs[0]);
    			if (!inputStream) {
    				abort(1, "could not open file '%s'", args_info.inputs[0]);
    			}

    			inputStream >> meta(pnapi::io::INPUTFILE, args_info.inputs[0])
    						>> pnapi::io::owfn >> *openNet::net;
    		}
    		if (args_info.verbose_flag) {
    			std::ostringstream s;
    			s << pnapi::io::stat << *openNet::net;
    			status("read net: %s", s.str().c_str());
    		}
    	} catch (const pnapi::exception::InputError& error) {
    		std::ostringstream s;
    		s << error;
    		abort(2, "\b%s", s.str().c_str());
    	}


    	// "fix" the net in order to avoid parse errors from LoLA (see bug #14166)
    	if (openNet::net->getTransitions().empty()) {
    		status("net has no transitions -- adding dead dummy transition");
    		openNet::net->createArc(openNet::net->createPlace(), openNet::net->createTransition());
    	}

    	// only normal nets are supported so far
    	if (not openNet::net->isNormal()) {
    		abort(3, "the input open net must be normal");
    	}



    	/*===================================..
    	|| OWFN -> BSD automaton             ||
    	``===================================*/

    	openNet::changeView(openNet::net, args_info.bound_arg);

    	/*----------------------------------------------.
    	| 1.1. initialize labels and interface markings |
    	`----------------------------------------------*/
    	Label::initialize();


    	/*----------------------------------------------.
    	| 1.2. write inner of the open net to LoLA file |
    	`----------------------------------------------*/
    	Output* temp = new Output();
    	std::stringstream ss;
    	ss << pnapi::io::lola << *openNet::net;
    	std::string lola_net = ss.str();

    	temp->stream() << lola_net << std::endl;

    	status("%s", lola_net.c_str());


    	/*--------------------------------------------.
    	| 1.3. call LoLA and parse reachability graph |
    	`--------------------------------------------*/
    	// select LoLA binary and build LoLA command
#if defined(__MINGW32__)
    	//    // MinGW does not understand pathnames with "/", so we use the basename
    	const std::string command_line = "\"" + std::string(args_info.lola_arg) + "\" " + temp->name() + " -M" + (args_info.verbose_flag ? "" : " 2> nul");
#else
    	const std::string command_line = std::string(args_info.lola_arg) + " " + temp->name() + " -M" + (args_info.verbose_flag ? "" : " 2> /dev/null");
#endif

    	// call LoLA
    	status("calling %s: '%s'", _ctool_("LoLA"), command_line.c_str());
    	time(&start_time);
    	graph_in = popen(command_line.c_str(), "r");

    	graph_parse();
    	pclose(graph_in);
    	graph_lex_destroy();
    	time(&end_time);
    	status("%s is done [%.0f sec]", _ctool_("LoLA"), difftime(end_time, start_time));
    	delete temp;


    	/*---------------------------------.
    	| 1.4. organize reachability graph |
    	`---------------------------------*/

    	InnerMarking::initialize();


    	/*---------------------------------.
       	| 1.5. compute the BSD automaton   |
    	`---------------------------------*/

    	status("starting BSD automaton computation...");

    	BSD::initialize();

    	BSD::computeBSD();

    	/*------------------------------------------------------.
       	| 1.6. save the BSD automaton and some important values |
 	   	`------------------------------------------------------*/

    	_BSDgraph.graph = BSD::graph;

    	_BSDgraph.id2name = Label::id2name;
    	_BSDgraph.U = BSD::U;
    	_BSDgraph.emptyset = BSD::emptyset;
    	_BSDgraph.events = Label::events;

    	_BSDgraph.first_receive = Label::first_receive;
    	_BSDgraph.last_receive = Label::last_receive;
    	_BSDgraph.first_send = Label::first_send;
    	_BSDgraph.last_send = Label::last_send;

    	_BSDgraph.receive_events = Label::receive_events;
    	_BSDgraph.send_events = Label::send_events;


    	/*---------------.
 	   	| 1.7. tidy up	 |
       	`---------------*/

        delete openNet::net;
        Label::finalize();
        InnerMarking::finalize();

        /*-------------------------------.
        | 1.8. create CSD from BSD		 |
        `-------------------------------*/

        if (args_info.CSD_flag) {
        	status("starting CSD automaton computation...");
        	CSD::computeCSD(_BSDgraph);
        }

        time(&end_time);
        _BSDgraph.comp_time = difftime(end_time, program_start);
        status("computation is done [%.0f sec]", _BSDgraph.comp_time);

        std::string call = std::string("ps -o rss -o comm | ") + TOOL_GREP + " " + PACKAGE + " | " + TOOL_AWK + " '{ if ($1 > max) max = $1 } END { print max \" KB\" }'";
        FILE* ps = popen(call.c_str(), "r");
        unsigned int memory;
        int res = fscanf(ps, "%u", &memory);
        assert(res != EOF);
        pclose(ps);
        _BSDgraph.memory_consumption = memory;
        status("memory consumption: %s%u KB %s", _bold_, memory, _c_);


        /*---------------------.
       	| 1.9. DOT output	   |
        `---------------------*/

        if (args_info.BSD_flag) {
        	std::stringstream temp (std::stringstream::in | std::stringstream::out);
        	temp << "BSD_" << args_info.bound_arg << "(";

        	std::string dot_filename = args_info.outFile_arg ? args_info.outFile_arg : filepath + temp.str() + filename + ").dot";

        	Output output(dot_filename, "BSD automaton");
        	output.stream() << pnapi::io::sa;
        	Output::dotoutput(output.stream(), _BSDgraph, filename, false, args_info.bound_arg);
        }

        if (args_info.CSD_flag) {
        	std::stringstream temp (std::stringstream::in | std::stringstream::out);
        	temp << "CSD_" << args_info.bound_arg << "(";

        	std::string dot_filename = args_info.outFile_arg ? args_info.outFile_arg : filepath + temp.str() + filename + ").dot";

        	Output output(dot_filename, "CSD automaton");
        	output.stream() << pnapi::io::sa;
        	Output::dotoutput(output.stream(), _BSDgraph, filename, true, args_info.bound_arg);
        }


        /*---------------.
 	   	| 1.10. tidy up	 |
        `---------------*/

        // delete the graph
        for (std::list<BSDNode *>::const_iterator it = _BSDgraph.graph->begin(); it != _BSDgraph.graph->end(); ++it) {
        	delete[] (*it)->pointer;
        	delete *it;
        }
        delete _BSDgraph.graph;

        BSD::finalize();
    }

    // in case of partner or conformance check
    if (args_info.partnerCheck_flag || args_info.confCheck_flag) {

        parsedGraph * _parsedGraph[2];

    	/*------------------------------------------.
    	| 2.1. parse the BSD or CSD automata (DOT)  |
    	`------------------------------------------*/
        for (int i = 0; i < 2; ++i) {
        	try {
        		// parse from given files
        		std::ifstream inputStream(args_info.inputs[i]);
        		if (!inputStream) {
        			abort(1, "could not open file '%s'", args_info.inputs[i]);
        		}

        		// parse into _parsedGraph[i]
        		_parsedGraph[i] = parser::dot2graph_parse(inputStream);

        		if (args_info.verbose_flag) {
        			status("parsed graph %i:", i+1);
        			parser::printParsedGraph(*_parsedGraph[i]);
        		}
        	} catch (const pnapi::exception::InputError& error) {
        		std::ostringstream s;
        		s << error;
        		abort(2, "\b%s", s.str().c_str());
        	}
        }

    	/*---------------------------.
   		| 2.2. check for b-partners	 |
   		`---------------------------*/

    	if (args_info.partnerCheck_flag) {
    		bool valid = partnercheck::check_b_partner(*_parsedGraph[0], *_parsedGraph[1]);

    		if (valid) {
    			message("net 1 is a %i-partner of net 2!", _parsedGraph[0]->bound);
    		} else {
    			message("net 1 is NOT a %i-partner of net 2!", _parsedGraph[0]->bound);
    			returnvalue = EXIT_FAILURE;
    		}
    	}

    	/*-------------------------------.
    	| 2.3. check for b-conformance	 |
   		`-------------------------------*/

    	if (args_info.confCheck_flag) {
    		bool valid = confcheck::check_b_conformance(*_parsedGraph[0], *_parsedGraph[1]);

    		if (valid) {
    			message("net 1 %i-conforms to net 2!", _parsedGraph[0]->bound);
    		} else {
    			message("net 1 does NOT %i-conform to net 2!", _parsedGraph[0]->bound);
    			returnvalue = EXIT_FAILURE;
    		}
    	}

    	/*---------------.
    	| 2.4. tidy up	 |
    	`---------------*/

    	// delete the graphs
    	for (int i = 0; i < 2; ++i) {
    		delete[] _parsedGraph[i]->names;
    		delete[] _parsedGraph[i]->lambdas;
    		for (unsigned int j = 0; j < _parsedGraph[i]->nodes; ++j)
    			delete[] _parsedGraph[i]->pointer[j];
    		delete[] _parsedGraph[i]->pointer;

    		delete _parsedGraph[i]->id2name;
    		delete _parsedGraph[i]->is_sending_label;
    		delete _parsedGraph[i]->name2id;

    		delete _parsedGraph[i];
    	}
    }


    // in case of maximal partner computation
    if (args_info.max_flag || args_info.mp_flag) {

    	parsedGraph * _parsedGraph;

       	// set standard filenames
    	std::string filename;
    	std::string filepath;
    	filename = std::string(PACKAGE) + "_output";
    	filepath = "./";


    	/*------------------------------------.
        | 2.1. parse the CSD automaton (DOT)  |
        `------------------------------------*/

    	try {
			// strip suffix from input filename
			filename = std::string(args_info.inputs[0]).substr(0, std::string(args_info.inputs[0]).find_last_of("."));
			filepath = std::string(args_info.inputs[0]).substr(0, filename.find_last_of("/")+1);
			filename = filename.substr(filename.find_last_of("/")+1, filename.length());

    		// parse from given files
    		std::ifstream inputStream(args_info.inputs[0]);
    		if (!inputStream) {
    			abort(1, "could not open file '%s'", args_info.inputs[0]);
    		}

    		// parse into _parsedGraph
    		_parsedGraph = parser::dot2graph_parse(inputStream);

    		if (args_info.verbose_flag) {
    			status("parsed graph:");
    			parser::printParsedGraph(*_parsedGraph);
    		}
    	} catch (const pnapi::exception::InputError& error) {
    		std::ostringstream s;
    		s << error;
    		abort(2, "\b%s", s.str().c_str());
    	}

    	openNet::initialize();

    	/*-------------------------------------------.
        | 2.2. compute maximal b-partner or mp_b	 |
       	`-------------------------------------------*/

    	if (args_info.max_flag)
    		CSD::computeMaxPartner(*_parsedGraph, true);
    	else
    		CSD::computeMaxPartner(*_parsedGraph, false);

    	/*---------------------.
    	| 1.9. OWFN output	   |
        `---------------------*/

    	std::stringstream temp (std::stringstream::in | std::stringstream::out);
    	temp << "max_" << _parsedGraph->bound << "(";

    	std::string owfn_filename = args_info.outFile_arg ? args_info.outFile_arg : filepath + temp.str() + filename + ").owfn";

    	Output output(owfn_filename, "OWFN");
    	output.stream() << pnapi::io::owfn << *openNet::net;

    	/*---------------.
        | 2.4. tidy up	 |
       	`---------------*/

    	delete openNet::net;

    	// delete the graph
    	delete[] _parsedGraph->names;
    	delete[] _parsedGraph->lambdas;
    	for (unsigned int j = 0; j < _parsedGraph->nodes; ++j)
    		delete[] _parsedGraph->pointer[j];
    	delete[] _parsedGraph->pointer;

    	delete _parsedGraph->id2name;
    	delete _parsedGraph->is_sending_label;
    	delete _parsedGraph->name2id;

    	delete _parsedGraph;
    }


    // in case of matching
    if (args_info.matching_flag) {

    	if (args_info.inputs_num < 2)
    		abort(1337, "this shit is stupid, dawg!");

    	// set standard filenames
    	std::string filename;
    	std::string filepath;
    	filename = std::string(PACKAGE) + "_output";
    	filepath = "./";

    	BSDgraph _BSDgraph;

    	openNet::initialize();

    	/*------------------------.
        | 1.0. parse the open net |
        `------------------------*/
    	try {
			// strip suffix from input filename
			filename = std::string(args_info.inputs[0]).substr(0, std::string(args_info.inputs[0]).find_last_of("."));
			filepath = std::string(args_info.inputs[0]).substr(0, filename.find_last_of("/")+1);
			filename = filename.substr(filename.find_last_of("/")+1, filename.length());

    		// parse the first input file
    		std::ifstream inputStream(args_info.inputs[0]);
    		if (!inputStream) {
    			abort(1, "could not open file '%s'", args_info.inputs[0]);
    		}

    		inputStream >> meta(pnapi::io::INPUTFILE, args_info.inputs[0])
        				>> pnapi::io::owfn >> *openNet::net;

    		if (args_info.verbose_flag) {
    			std::ostringstream s;
    			s << pnapi::io::stat << *openNet::net;
    			status("read net: %s", s.str().c_str());
    		}
    	} catch (const pnapi::exception::InputError& error) {
    		std::ostringstream s;
    		s << error;
    		abort(2, "\b%s", s.str().c_str());
    	}


    	// "fix" the net in order to avoid parse errors from LoLA (see bug #14166)
    	if (openNet::net->getTransitions().empty()) {
    		status("net has no transitions -- adding dead dummy transition");
    		openNet::net->createArc(openNet::net->createPlace(), openNet::net->createTransition());
    	}

    	// only normal nets are supported so far
    	if (not openNet::net->isNormal()) {
    		abort(3, "the input open net must be normal");
    	}



    	/*===================================..
        || OWFN -> BSD automaton             ||
        ``===================================*/

    	openNet::changeView(openNet::net, args_info.bound_arg);

    	/*----------------------------------------------.
        | 1.1. initialize labels and interface markings |
        `----------------------------------------------*/
    	Label::initialize();


    	/*----------------------------------------------.
        | 1.2. write inner of the open net to LoLA file |
        `----------------------------------------------*/
    	Output* temp = new Output();
    	std::stringstream ss;
    	ss << pnapi::io::lola << *openNet::net;
    	std::string lola_net = ss.str();

    	temp->stream() << lola_net << std::endl;

    	status("%s", lola_net.c_str());


    	/*--------------------------------------------.
        | 1.3. call LoLA and parse reachability graph |
        `--------------------------------------------*/
    	// select LoLA binary and build LoLA command
#if defined(__MINGW32__)
    	//    // MinGW does not understand pathnames with "/", so we use the basename
    	const std::string command_line = "\"" + std::string(args_info.lola_arg) + "\" " + temp->name() + " -M" + (args_info.verbose_flag ? "" : " 2> nul");
#else
    	const std::string command_line = std::string(args_info.lola_arg) + " " + temp->name() + " -M" + (args_info.verbose_flag ? "" : " 2> /dev/null");
#endif

    	// call LoLA
    	status("calling %s: '%s'", _ctool_("LoLA"), command_line.c_str());
    	time(&start_time);
    	graph_in = popen(command_line.c_str(), "r");

    	graph_parse();
    	pclose(graph_in);
    	graph_lex_destroy();
    	time(&end_time);
    	status("%s is done [%.0f sec]", _ctool_("LoLA"), difftime(end_time, start_time));
    	delete temp;


    	/*---------------------------------.
        | 1.4. organize reachability graph |
        `---------------------------------*/

    	InnerMarking::initialize();


    	/*---------------------------------.
        | 1.5. compute the BSD automaton   |
        `---------------------------------*/

    	status("starting BSD automaton computation...");
    	time(&start_time);

    	BSD::initialize();

    	BSD::computeBSD();

    	/*------------------------------------------------------.
        | 1.6. save the BSD automaton and some important values |
     	`------------------------------------------------------*/

    	_BSDgraph.graph = BSD::graph;

    	_BSDgraph.id2name = Label::id2name;
    	_BSDgraph.U = BSD::U;
    	_BSDgraph.emptyset = BSD::emptyset;
    	_BSDgraph.events = Label::events;

    	_BSDgraph.first_receive = Label::first_receive;
    	_BSDgraph.last_receive = Label::last_receive;
    	_BSDgraph.first_send = Label::first_send;
    	_BSDgraph.last_send = Label::last_send;

    	_BSDgraph.receive_events = Label::receive_events;
    	_BSDgraph.send_events = Label::send_events;


    	/*---------------.
     	| 1.7. tidy up	 |
        `---------------*/

    	delete openNet::net;
    	Label::finalize();
    	InnerMarking::finalize();


    	/*-------------------------------.
        | 1.8. create CSD from BSD		 |
        `-------------------------------*/

    	status("starting CSD automaton computation...");
    	CSD::computeCSD(_BSDgraph);

    	time(&end_time);
    	_BSDgraph.comp_time = difftime(end_time, program_start);
    	status("computation is done [%.0f sec]", _BSDgraph.comp_time);

    	std::string call = std::string("ps -o rss -o comm | ") + TOOL_GREP + " " + PACKAGE + " | " + TOOL_AWK + " '{ if ($1 > max) max = $1 } END { print max \" KB\" }'";
    	FILE* ps = popen(call.c_str(), "r");
    	unsigned int memory;
    	int res = fscanf(ps, "%u", &memory);
    	assert(res != EOF);
    	pclose(ps);
    	_BSDgraph.memory_consumption = memory;
    	status("memory consumption: %s%u KB %s", _bold_, memory, _c_);


    	//\todo: got the first CSD automaton. now do stuff here i guess...
    	// parse all input nets and match the reachability graphs to the CSD/MP automaton of net 1
    	for (int i = 1; i < args_info.inputs_num; ++i) {
    		// set standard filenames
    		std::string filenamePartner;
    		std::string filepathPartner;
    		filenamePartner = std::string(PACKAGE) + "_output";
    		filepathPartner = "./";

    		openNet::initialize();

    		/*------------------------.
            | 1.0. parse the open net |
            `------------------------*/
    		try {
    			// strip suffix from input filename
    			filenamePartner = std::string(args_info.inputs[i]).substr(0, std::string(args_info.inputs[i]).find_last_of("."));
    			filepathPartner = std::string(args_info.inputs[i]).substr(0, filenamePartner.find_last_of("/")+1);
    			filenamePartner = filename.substr(filenamePartner.find_last_of("/")+1, filenamePartner.length());

    			// parse the first input file
    			std::ifstream inputStream(args_info.inputs[i]);
    			if (!inputStream) {
    				abort(1, "could not open file '%s'", args_info.inputs[i]);
    			}

    			inputStream >> meta(pnapi::io::INPUTFILE, args_info.inputs[i])
            				>> pnapi::io::owfn >> *openNet::net;

    			if (args_info.verbose_flag) {
    				std::ostringstream s;
    				s << pnapi::io::stat << *openNet::net;
    				status("read net: %s", s.str().c_str());
    			}
    		} catch (const pnapi::exception::InputError& error) {
    			std::ostringstream s;
    			s << error;
    			abort(2, "\b%s", s.str().c_str());
    		}


    		// "fix" the net in order to avoid parse errors from LoLA (see bug #14166)
    		if (openNet::net->getTransitions().empty()) {
    			status("net has no transitions -- adding dead dummy transition");
    			openNet::net->createArc(openNet::net->createPlace(), openNet::net->createTransition());
    		}

    		// only normal nets are supported so far
    		if (not openNet::net->isNormal()) {
    			abort(3, "the input open net must be normal");
    		}


    		/*----------------------------------------------.
            | 1.1. initialize labels and interface markings |
            `----------------------------------------------*/
    		Label::initialize();


    		/*----------------------------------------------.
            | 1.2. write inner of the open net to LoLA file |
            `----------------------------------------------*/
    		Output* temp = new Output();
    		std::stringstream ss;
    		ss << pnapi::io::lola << *openNet::net;
    		std::string lola_net = ss.str();

    		temp->stream() << lola_net << std::endl;

    		status("%s", lola_net.c_str());


    		/*--------------------------------------------.
            | 1.3. call LoLA and parse reachability graph |
            `--------------------------------------------*/
    		// select LoLA binary and build LoLA command
#if defined(__MINGW32__)
    		//    // MinGW does not understand pathnames with "/", so we use the basename
    		const std::string command_line = "\"" + std::string(args_info.lola_arg) + "\" " + temp->name() + " -M" + (args_info.verbose_flag ? "" : " 2> nul");
#else
    		const std::string command_line = std::string(args_info.lola_arg) + " " + temp->name() + " -M" + (args_info.verbose_flag ? "" : " 2> /dev/null");
#endif

    		// call LoLA
    		status("calling %s: '%s'", _ctool_("LoLA"), command_line.c_str());
    		time(&start_time);
    		graph_in = popen(command_line.c_str(), "r");

    		graph_parse();
    		pclose(graph_in);
    		graph_lex_destroy();
    		time(&end_time);
    		status("%s is done [%.0f sec]", _ctool_("LoLA"), difftime(end_time, start_time));
    		delete temp;


    		/*---------------------------------.
            | 1.4. organize reachability graph |
            `---------------------------------*/

    		InnerMarking::initialize();

    		// \todo: do stuff here with the CSD automaton and the RG

    		partnercheck::match(_BSDgraph);

        	/*---------------.
         	| 1.7. tidy up	 |
            `---------------*/

        	delete openNet::net;
        	Label::finalize();
        	InnerMarking::finalize();
    	}

    	/*---------------.
     	| 1.10. tidy up	 |
        `---------------*/

    	// delete the graph
    	for (std::list<BSDNode *>::const_iterator it = _BSDgraph.graph->begin(); it != _BSDgraph.graph->end(); ++it) {
    		delete[] (*it)->pointer;
    		delete *it;
    	}
    	delete _BSDgraph.graph;

    	BSD::finalize();
    }

    return returnvalue;
}

