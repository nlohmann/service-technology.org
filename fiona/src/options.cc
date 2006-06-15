#include <string>
#include <iostream>
#include <map>

#include "options.h"
#include "debug.h"
#include "exception.h"
#include "fiona.h"

// some file names and pointers

char * netfile;
int commDepth_manual;
int events_manual;

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

/// Filename of log file
std::string log_filename = "";

bool createOutputFile = false;

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
  { "show",      	 optional_argument, 	NULL, 's' },
  { "calcallstates", no_argument, 			NULL, 'a' },
  { "reduceIG",   	 no_argument,	    	NULL, 'r' },
  { "commDepth",   	 required_argument,    	NULL, 'c' },
  { "eventsmaximum", required_argument,    	NULL, 'e' },
  NULL
};


const char * par_string = "hvd:n:t:s:arc:e:";

// --------------------- functions for command line evaluation ------------------------
// Prints an overview of all commandline arguments.
void print_help() {
  trace("\n");
  trace("\n");
  trace("Options: \n");
  trace("\n");
  trace(" -h | --help                  - print this information and exit\n");
  trace(" -v | --version               - print version information and exit\n");
  trace(" -d | --debug=<level>         - set debug <level>:\n");
  trace("                                   1 - show nodes and dfs information\n");
  trace("                                   2 - show considered events\n");
  trace("                                   3 - show states information\n");
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
  trace("\n");
  trace("For more information see:\n");
  trace("  http://www.informatik.hu-berlin.de/top/tools4bpel/fiona\n");
  trace("\n");
}

// Prints some version information
void print_version(std::string name) {
  trace(std::string(PACKAGE_STRING) + " -- ");
  trace("Functional Interaction Analysis of open Workflow Nets\n");
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
	options[O_CALC_ALL_STATES] = false;
	options[O_CALC_REDUCED_IG] = false;
	options[O_OWFN_NAME] = false;
	options[O_COMM_DEPTH] = false;
	options[O_EVENT_USE_MAX] = false;

	// initialize parameters
	parameters[P_IG] = true;
	parameters[P_OG] = false;
	parameters[P_SHOW_ALL_NODES] = false;
	parameters[P_SHOW_BLUE_NODES_ONLY] = true;
	parameters[P_SHOW_NO_RED_NODES] = true;
	parameters[P_SHOW_EMPTY_NODE] = false;
	parameters[P_SHOW_STATES_PER_NODE] = false;
	parameters[P_CALC_ALL_STATES] = false;
	parameters[P_CALC_REDUCED_IG] = false;
	
  	// evaluate options and set parameters
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
		      	options[O_DEBUG] = true;
	 			if ( string(optarg) == "1" ) {
					debug_level = TRACE_1;
		      	} else if ( string(optarg) == "2" ) {
					debug_level = TRACE_2;
		      	} else if ( string(optarg) == "3" ) {
					debug_level = TRACE_3;
		      	} else if ( string(optarg) == "5" ) {
					debug_level = TRACE_5;
		      	} else {
					throw Exception(OPTION_MISMATCH,
						"Unrecognised debug mode!\n",
						"Type " + progname + " -h for more information.\n");
			    }
		      	break;
	      	case 'n':
		      	options[O_OWFN_NAME] = true;
		      	if (optarg != NULL) {
		        	netfile = optarg;
		      	} else {
					throw Exception(OPTION_MISMATCH,
						"missing net file!\n",
						"Type " + progname + " -h for more information.\n");
			    }
	          	break;
	      	case 't':
			  	options[O_GRAPH_TYPE] = true;
			  	if (string(optarg) == "OG") {
			  		parameters[P_OG] = true;
			  		parameters[P_IG] = false;
			  	} else if (string(optarg) == "IG") {
			  		parameters[P_OG] = false;
			  		parameters[P_IG] = true;
			  	} else {
					throw Exception(OPTION_MISMATCH,
						"Unrecognised type parameter!\n",
						"Type " + progname + " -h for more information.\n");
			    }
			  	break;
	      	case 'c':
		      	if (optarg != NULL) {
		        	options[O_COMM_DEPTH] = true;
		        	commDepth_manual = atoi(optarg);
		      	} else {
					throw Exception(OPTION_MISMATCH,
						"Please enter valid communication depth\n",
						"Type " + progname + " -h for more information.\n");
			    }
	          	break;
	      	case 'e':
		      	if (optarg != NULL) {
		        	options[O_EVENT_USE_MAX] = true;
		        	events_manual = atoi(optarg);
		      	} else {
					throw Exception(OPTION_MISMATCH,
						"Please enter valid maximal number for using events\n",
						"Type " + progname + " -h for more information.\n");
			    }
	          	break;
	      	case 's':
	      		options[O_SHOW_NODES] = true;
	      		if (string(optarg) == "blue") {
	      			parameters[P_SHOW_ALL_NODES] = false;
	      			parameters[P_SHOW_NO_RED_NODES] = false;
	      			parameters[P_SHOW_BLUE_NODES_ONLY] = true;
	      		} else if (string(optarg) == "nored") {
	      			parameters[P_SHOW_ALL_NODES] = false;
	      			parameters[P_SHOW_NO_RED_NODES] = true;
	      			parameters[P_SHOW_BLUE_NODES_ONLY] = false;
	      		} else if (string(optarg) == "empty") {
	      			parameters[P_SHOW_EMPTY_NODE] = true;
	      		} else if (string(optarg) == "allstates") {
	      			parameters[P_SHOW_STATES_PER_NODE] = true;
	      		} else if (string(optarg) == "allnodes") {
	      			parameters[P_SHOW_ALL_NODES] = true;
	      			parameters[P_SHOW_EMPTY_NODE] = true;
	      			parameters[P_SHOW_NO_RED_NODES] = false;
	      			parameters[P_SHOW_BLUE_NODES_ONLY] = false;
	      		} else {
					throw Exception(OPTION_MISMATCH,
						"Unrecognised show parameter!\n",
						"Type " + progname + " -h for more information.\n");
			    }
			    break;
	      	case 'a':
			  	options[O_CALC_ALL_STATES] = true;
				parameters[P_CALC_ALL_STATES] = true;
			  	break;
	      	case 'r':
		      	options[O_CALC_REDUCED_IG] = true;
		      	parameters[P_CALC_REDUCED_IG] = true;
	          	break;
	      	default:
		     	throw Exception(OPTION_MISMATCH,
				     "Unknown option!\n",
				     "Type " + progname + " -h for more information.\n");
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
}


