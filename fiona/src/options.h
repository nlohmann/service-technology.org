#ifndef OPTIONS_H
#define OPTIONS_H

#include <map>
#include <cstdio>
#include "getopt.h"

typedef enum
{
   	O_HELP,
	O_VERSION,
	O_DEBUG,
	O_GRAPH_TYPE,	//!< OG or IG
	O_SHOW_NODES,	//!<
	O_CALC_ALL_STATES,
	O_CALC_REDUCED_IG,
	O_OWFN_NAME,
	O_COMM_DEPTH,
	O_EVENT_USE_MAX,
	O_BDD
} possibleOptions;

typedef enum
{
	P_IG,			//!< compute IG  
	P_OG,			//!< compute OG
	P_SHOW_ALL_NODES,
	P_SHOW_BLUE_NODES_ONLY,
	P_SHOW_NO_RED_NODES,
	P_SHOW_EMPTY_NODE,
	P_SHOW_STATES_PER_NODE,
	P_CALC_ALL_STATES,
	P_CALC_REDUCED_IG
} possibleParameters;


// some file names and pointers (in options.cc)


// different modes controlled by command line (in options.cc)

extern std::map<possibleOptions,    bool> options;
extern std::map<possibleParameters, bool> parameters;

extern char * netfile;
extern int commDepth_manual;
extern int events_manual;
extern int bdd_reordermethod;


/// pointer to input stream
extern std::istream * input;
/// pointer to output stream
extern std::ostream * output;
/// pointer to log stream
extern std::ostream * log_output;

void parse_command_line(int argc, char* argv[]);

#endif

