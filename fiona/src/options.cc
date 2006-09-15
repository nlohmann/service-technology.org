#include "mynew.h"
#include <string>
#include <iostream>
#include <map>
#include <list>
#include <stdlib.h>

#include "options.h"
#include "debug.h"
#include "exception.h"
#include "fiona.h"

// some file names and pointers

char * netfile;
std::list<char*> netfiles; 


int commDepth_manual;
int events_manual;
int messages_manual;
int bdd_reordermethod;

/// Filename of input file
std::string filename = "<STDIN>";

/// Filename of input file
std::string output_filename = "";

/// pointer to input stream
std::istream * input = &std::cin;
/// pointer to output stream
std::ostream * output = &std::cout;
/// pointer to log stream
std::ostream * log_output = &std::cout;   // &std::clog;

///// Filename of log file
//std::string log_filename = "";
//
//bool createOutputFile = false;

// different modes controlled by command line

map<possibleOptions,    bool> options;
map<possibleParameters, bool> parameters;

// long options
static struct option longopts[] =
{
  { "help",       	 no_argument,       	NULL, 'h' },
  { "version",    	 no_argument,       	NULL, 'v' },
  { "debug",      	 required_argument, 	NULL, 'd' },
  { "net",       	 required_argument, 	NULL, 'n' },
  { "graphtype", 	 required_argument, 	NULL, 't' },
  { "show",      	 required_argument, 	NULL, 's' },
  { "calcallstates", no_argument, 			NULL, 'a' },
  { "reduceIG",   	 no_argument,	    	NULL, 'r' },
  { "commDepth",   	 required_argument,    	NULL, 'c' },
  { "messagemaximum",required_argument,    	NULL, 'm' },
  { "eventsmaximum", required_argument,    	NULL, 'e' },
  { "BDD",			 required_argument,    	NULL, 'b' },
  NULL
};


const char * par_string = "hvd:n:t:s:arc:m:e:b:";

// --------------------- functions for command line evaluation ------------------------
// Prints an overview of all commandline arguments.
void print_help() {
  trace("\n");
  trace("\n");
  trace("Options: (if an option is skipped, the default settings are denoted)\n");
  trace("\n");
  trace(" -h | --help                  - print this information and exit\n");
  trace(" -v | --version               - print version information and exit\n");
  trace(" -d | --debug=<level>         - set debug <level>:\n");
  trace("                                   1 - show nodes and dfs information\n");
  trace("                                   2 - show analyse information (i.e. colors)\n");
  trace("                                   3 - show information on events and states\n");
  trace("                                   4 - yet to be defined ;)\n");
  trace("                                   5 - show detailed information on everything\n");
  trace("\n");
  trace(" -n | --net=<filename>        - read input owfn from <filename>\n");
  trace("\n");
  trace(" -t | --graphtype=<type>      - select the graph <type> to be calculated:\n");
  trace("                                   OG - operating guideline\n");
  trace("                                   IG - interaction graph (default)\n");
  trace("\n");
  trace(" -c | --commDepth=<level>     - set communication depth to <level>\n");
  trace("                                   (only relevant for OG)\n");
  trace("\n");
  trace(" -m | --messagemaximum=<level> - set maximum number of same messages per state to <level>\n");
  trace("\n");
  trace(" -e | --eventsmaximum=<level> - set event to occur at most <level> times\n");
  trace("                                   (only relevant for OG)\n");
  trace("\n");
  trace(" -r | --reduceIG              - use reduction rules for IG\n");
  trace("\n");
  trace(" -s | --show=<parameter>      - different display options <parameter>:\n");
  trace("                                   allnodes  - show nodes of all colors\n");
  trace("                                   blue      - show blue nodes only (default)\n");
  trace("                                   nored     - show blue and black nodes\n");
  trace("                                   empty     - show empty node\n");
  trace("                                   allstates - show all calculated states per node\n");
  trace("\n");
  trace(" -b | --BDD=<reordering>      - enable BDD construction (only relevant for OG)\n");
  trace("                                optional argument <reordering> specifies reodering method:\n");
  trace("                                    0 - CUDD_REORDER_SAME\n");
  trace("                                    1 - CUDD_REORDER_NONE\n");
  trace("                                    2 - CUDD_REORDER_RANDOM\n");
  trace("                                    3 - CUDD_REORDER_RANDOM_PIVOT\n");
  trace("                                    4 - CUDD_REORDER_SIFT\n");
  trace("                                    5 - CUDD_REORDER_SIFT_CONVERGE\n");
  trace("                                    6 - CUDD_REORDER_SYMM_SIFT\n");
  trace("                                    7 - CUDD_REORDER_SYMM_SIFT_CONV\n");
  trace("                                    8 - CUDD_REORDER_WINDOW2\n");
  trace("                                    9 - CUDD_REORDER_WINDOW3\n");
  trace("                                   10 - CUDD_REORDER_WINDOW4\n");
  trace("                                   11 - CUDD_REORDER_WINDOW2_CONV\n");
  trace("                                   12 - CUDD_REORDER_WINDOW3_CONV\n");
  trace("                                   13 - CUDD_REORDER_WINDOW4_CONV\n");
  trace("                                   14 - CUDD_REORDER_GROUP_SIFT\n");
  trace("                                   15 - CUDD_REORDER_GROUP_SIFT_CONV\n");
  trace("                                   16 - CUDD_REORDER_ANNEALING\n");
  trace("                                   17 - CUDD_REORDER_GENETIC\n");
  trace("                                   18 - CUDD_REORDER_LINEAR\n");
  trace("                                   19 - CUDD_REORDER_LINEAR_CONVERGE\n");
  trace("                                   20 - CUDD_REORDER_LAZY_SIFT\n");
  trace("                                   21 - CUDD_REORDER_EXACT\n");
  trace("\n");
  trace("\n");
  trace("For more information see:\n");
  trace("  http://www.informatik.hu-berlin.de/top/tools4bpel/fiona\n");
  trace("\n");
}

// Prints some version information
void print_version(std::string name) {
  trace(std::string(PACKAGE_STRING) + " -- ");
  trace("Functional InteractiON Analysis of open workflow nets\n");
  trace("\n");
  trace("Copyright (C) 2005, 2006 Peter Massuthe and Daniela Weinberg\n");
  trace("This is free software; see the source for copying conditions. There is NO\n");
  trace("warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");
}


// get parameters from options
void parse_command_line(int argc, char* argv[]) {
	
  	// the program name on the commandline
  	string progname = string(argv[0]);

	// initialize options
	options[O_HELP] = false;
	options[O_VERSION] = false;
	options[O_DEBUG] = false;
	options[O_GRAPH_TYPE] = false;
	options[O_SHOW_NODES] = false;
//	options[O_CALC_ALL_STATES] = false;		// standard: man muss -a angeben, um voll
	options[O_CALC_ALL_STATES] = true;		// so lange Reduktion im Teststadium
	options[O_CALC_REDUCED_IG] = false;
	options[O_OWFN_NAME] = false;
	options[O_BDD] = false;

	options[O_COMM_DEPTH] = false;
	options[O_MESSAGES_MAX] = false;
	options[O_EVENT_USE_MAX] = false;


	// initialize parameters
	parameters[P_IG] = true;
	parameters[P_OG] = false;
	parameters[P_SHOW_ALL_NODES] = false;
	parameters[P_SHOW_BLUE_NODES_ONLY] = true;
	parameters[P_SHOW_NO_RED_NODES] = true;
	parameters[P_SHOW_EMPTY_NODE] = false;
	parameters[P_SHOW_STATES_PER_NODE] = false;

	bdd_reordermethod = 0;

	messages_manual = 1;
	events_manual = 1;
	commDepth_manual = 1;
	
  	// evaluate options and set parameters
  	trace(TRACE_0, "\n");
  	int optc = 0;
  	while ((optc = getopt_long (argc, argv, par_string, longopts, (int *) 0)) != EOF) {
	    switch (optc) {
			case 'h':
		      	options[O_HELP] = true;
	  	      	break;
	      	case 'v':
		      	options[O_VERSION] = true;
				break;
	      	case 'd':
	 			if ( string(optarg) == "1" ) {
		      		options[O_DEBUG] = true;
					debug_level = TRACE_1;
		      	} else if ( string(optarg) == "2" ) {
		    	  	options[O_DEBUG] = true;
					debug_level = TRACE_2;
		      	} else if ( string(optarg) == "3" ) {
			      	options[O_DEBUG] = true;
					debug_level = TRACE_3;
		      	} else if ( string(optarg) == "4" ) {
			      	options[O_DEBUG] = true;
					debug_level = TRACE_4;
		      	} else if ( string(optarg) == "5" ) {
			      	options[O_DEBUG] = true;
					debug_level = TRACE_5;
		      	} else {
					cerr << "Option error: wrong debug mode \t ...ignored" << endl;
			    }
		      	break;
	      	case 'n':
		      	options[O_OWFN_NAME] = true;
	        	// netfile = optarg;
				netfiles.push_back(optarg);
	          	break;
	      	case 't':
			  	if (string(optarg) == "OG") {
				  	options[O_GRAPH_TYPE] = true;
			  		parameters[P_OG] = true;
			  		parameters[P_IG] = false;
			  	} else if (string(optarg) == "IG") {
				  	options[O_GRAPH_TYPE] = true;
			  		parameters[P_OG] = false;
			  		parameters[P_IG] = true;
			  	} else {
					cerr << "Option error: wrong graph type \t ...IG computed" << endl;
			    }
			  	break;
	      	case 'c':
	        	options[O_COMM_DEPTH] = true;
	        	commDepth_manual = atoi(optarg);
	          	break;
	      	case 'm':
	        	options[O_MESSAGES_MAX] = true;
	        	messages_manual = atoi(optarg);
	          	break;
	      	case 'e':
	        	options[O_EVENT_USE_MAX] = true;
	        	events_manual = atoi(optarg);
	          	break;
	      	case 's':
	      		if (string(optarg) == "blue") {
	      			options[O_SHOW_NODES] = true;
	      			parameters[P_SHOW_ALL_NODES] = false;
	      			parameters[P_SHOW_NO_RED_NODES] = false;
	      			parameters[P_SHOW_BLUE_NODES_ONLY] = true;
	      		} else if (string(optarg) == "nored") {
	      			options[O_SHOW_NODES] = true;
	      			parameters[P_SHOW_ALL_NODES] = false;
	      			parameters[P_SHOW_NO_RED_NODES] = true;
	      			parameters[P_SHOW_BLUE_NODES_ONLY] = false;
	      		} else if (string(optarg) == "empty") {
	    	  		options[O_SHOW_NODES] = true;
	      			parameters[P_SHOW_EMPTY_NODE] = true;
	      		} else if (string(optarg) == "allstates") {
		      		options[O_SHOW_NODES] = true;
	      			parameters[P_SHOW_STATES_PER_NODE] = true;
	      		} else if (string(optarg) == "allnodes") {
		      		options[O_SHOW_NODES] = true;
	      			parameters[P_SHOW_ALL_NODES] = true;
	      			parameters[P_SHOW_EMPTY_NODE] = true;
	      			parameters[P_SHOW_NO_RED_NODES] = false;
	      			parameters[P_SHOW_BLUE_NODES_ONLY] = false;
	      		} else {
					cerr << "Option error: wrong show option \t ...ignored" << endl;
			    }
			    break;
	      	case 'a':
			  	options[O_CALC_ALL_STATES] = true;
			  	break;
	      	case 'r':
		      	options[O_CALC_REDUCED_IG] = true;
	          	break;
	      	case 'b':
		      	if (optarg) {
			      	options[O_BDD] = true;
		      		int i = atoi(optarg);
		      		if (i >= 0 && i <= 21){
		      			bdd_reordermethod = i;
		      		} else {
						cerr << "Option error: wrong parameter for -b option \t ...method 0 used" << endl;
			      		bdd_reordermethod = 0;
		      		}
		      	} else {
					cerr << "Option error: missing parameter for -b option \t ...method 0 used" << endl;
		      		bdd_reordermethod = 0;
		      	}
		      	break;
	      	default:
				cerr << "Option error: unknown option or missing argument" << endl;
				break;
		}
	}

  	// print help and exit
  	if (options[O_HELP]) {
		print_help();
    	exit(0);
  	}
  	
  	// print version and exit
  	if (options[O_VERSION]) {
    	print_version("");
    	exit(0);
  	}

  	if (options[O_OWFN_NAME] == false) {
  		trace(TRACE_0, "missing parameter -n\n");
  		exit(1);
  	}

	if (options[O_BDD] == true && parameters[P_OG] == false) {
		cerr << "computing IG -- BDD option ignored" << endl;
		options[O_BDD] = false;
	}
}


