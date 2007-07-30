/*****************************************************************************
 * Copyright 2005, 2006, 2007                 Niels Lohmann, Peter Massuthe, *
 *                                      Daniela Weinberg, Jan Bretschneider, *
 *                                           Christian Gierds, Leonard Kern  *
 *                                                                           *
 * This file is part of Fiona.                                               *
 *                                                                           *
 * Fiona is free software; you can redistribute it and/or modify it          *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * Fiona is distributed in the hope that it will be useful, but WITHOUT      *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
 * more details.                                                             *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with Fiona; if not, write to the Free Software Foundation, Inc., 51       *
 * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.                      *
 *****************************************************************************/

/*!
 * \file    options.cc
 *
 * \brief   options management for fiona
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 */

#include "mynew.h"
#include <string>
#include <iostream>
#include <map>
#include <list>
#include <stdlib.h>
#include <fstream>

#include "options.h"
#include "debug.h"
#include "fiona.h"

using namespace std;

// some file names and pointers
char * netfile;
std::list<char*> netfiles;
OGFromFile::ogfiles_t ogfiles;
std::string ogfileToMatch;
std::string ogfileToParse;
std::string outfilePrefix;

int events_manual;
unsigned int messages_manual;
int bdd_reordermethod;

// const int GETOPT_BLA = 256;

/// pointer to log stream
std::ostream * log_output = &std::cout;   // &std::clog;

// different modes controlled by command line
std::map<possibleOptions,    bool> options;
std::map<possibleParameters, bool> parameters;


// long options
static struct option longopts[] = {
//  { "bla",           no_argument,       NULL, GETOPT_BLA },
  { "help",            no_argument,       NULL, 'h' },
  { "version",         no_argument,       NULL, 'v' },
  { "debug",           required_argument, NULL, 'd' },
  { "type",            required_argument, NULL, 't' },
  { "show",            required_argument, NULL, 's' },
  { "reduce-nodes",    no_argument,       NULL, 'R' },
  { "reduceIG",        no_argument,       NULL, 'r' },
  { "messagemaximum",  required_argument, NULL, 'm' },
  { "eventsmaximum",   required_argument, NULL, 'e' },
  { "BDD",             required_argument, NULL, 'b' },
  { "OnTheFly",        required_argument, NULL, 'B' },
  { "output",          required_argument, NULL, 'o' },
  { NULL,              0,                 NULL, 0   }
};

const char * par_string = "hvd:t:s:Rrm:e:b:B:o:";


// --------------------- functions for command line evaluation ------------------------
// Prints an overview of all commandline arguments.
void print_help() {
  trace("Usage: fiona [OPTION]... [FILE]...\n");
  trace("\n");
  trace("Options: (if an option is skipped, the default settings are denoted)\n");
  trace("\n");
  trace(" -h | --help ..................... print this information and exit\n");
  trace(" -v | --version .................. print version information and exit\n");
  trace(" -d | --debug=<level> ............ set debug <level>:\n");
  trace("                                     1 - show nodes and dfs information\n");
  trace("                                     2 - show analyse information (i.e. colors)\n");
  trace("                                     3 - show information on events and states\n");
  trace("                                     4 - yet to be defined ;)\n");
  trace("                                     5 - show detailed information on everything\n");
  trace(" -t | --type=<type> .............. select the modus operandi of fiona <type>:\n");
  trace("                                     OG - compute operating guideline\n");
  trace("                                     IG - compute interaction graph (default)\n");
  trace("                                     match - check if given oWFN matches\n");
  trace("                                             with the operating guideline given\n");
  trace("                                             in [FILE]\n");
  trace("                                     simulation  - check whether the first OG\n");
  trace("                                                   characterizes more strategies\n");
  trace("                                                   than the second one\n");
  trace("                                     equality    - check whether two OGs\n");
  trace("                                                   characterize the same\n");
  trace("                                                   strategies\n");
  trace("                                     equivalence - check whether two OGs (given\n");
  trace("                                                   as BDDs) are equivalent\n");
  trace("                                     productog   - calculate the product OG of\n");
  trace("                                                   all given OGs\n");
  trace("                                     countservices - compute the number of\n");
  trace("                                                   services determined by\n");  
  trace("                                                   every singe given OG\n");  
  trace(" -m | --messagemaximum=<level> ... set maximum number of same messages per\n");
  trace("                                   state to <level>\n");
  trace("                                   (default is 1)\n");
//  trace(" -e | --eventsmaximum=<level> .... set event to occur at most <level> times\n");
//  trace("                                   (default is 1)\n");
//  trace("                                   (-1 means disabling -e option, but is only\n");
//  trace("                                   possible if -m option is set)\n");
//  trace("                                   (only relevant for OG)\n");
  trace(" -r | --reduceIG ................. use reduction rules for IG\n");
  trace(" -R | --reduce-nodes ............. use node reduction (IG or OG) which\n");
  trace("                                   stores less states per node of IG/OG\n");
  trace("                                   and reduces memory, but increases time\n");
  trace(" -s | --show=<parameter> ......... different display options <parameter>:\n");
  trace("                                     allnodes  - show nodes of all colors\n");
  trace("                                     blue      - show blue nodes only (default)\n");
  trace("                                     rednodes  - show blue and red nodes (no\n");
  trace("                                                 empty node and no black nodes)\n");
  trace("                                     empty     - show empty node\n");
  trace("                                     allstates - show all calculated states per\n");
  trace("                                                 node\n");
  trace("                                     deadlocks - show all but transient states\n");
  trace(" -b | --BDD=<reordering> ......... enable BDD construction\n");
  trace("                                   (only relevant for OG)\n");
  trace("                                   argument <reordering> specifies reodering\n");
  trace("                                   method:\n");
  trace("                                      0 - CUDD_REORDER_SAME\n");
  trace("                                      1 - CUDD_REORDER_NONE\n");
  trace("                                      2 - CUDD_REORDER_RANDOM\n");
  trace("                                      3 - CUDD_REORDER_RANDOM_PIVOT\n");
  trace("                                      4 - CUDD_REORDER_SIFT\n");
  trace("                                      5 - CUDD_REORDER_SIFT_CONVERGE\n");
  trace("                                      6 - CUDD_REORDER_SYMM_SIFT\n");
  trace("                                      7 - CUDD_REORDER_SYMM_SIFT_CONV\n");
  trace("                                      8 - CUDD_REORDER_WINDOW2\n");
  trace("                                      9 - CUDD_REORDER_WINDOW3\n");
  trace("                                     10 - CUDD_REORDER_WINDOW4\n");
  trace("                                     11 - CUDD_REORDER_WINDOW2_CONV\n");
  trace("                                     12 - CUDD_REORDER_WINDOW3_CONV\n");
  trace("                                     13 - CUDD_REORDER_WINDOW4_CONV\n");
  trace("                                     14 - CUDD_REORDER_GROUP_SIFT\n");
  trace("                                     15 - CUDD_REORDER_GROUP_SIFT_CONV\n");
  trace("                                     16 - CUDD_REORDER_ANNEALING\n");
  trace("                                     17 - CUDD_REORDER_GENETIC\n");
  trace("                                     18 - CUDD_REORDER_LINEAR\n");
  trace("                                     19 - CUDD_REORDER_LINEAR_CONVERGE\n");
  trace("                                     20 - CUDD_REORDER_LAZY_SIFT\n");
  trace("                                     21 - CUDD_REORDER_EXACT\n");
  trace(" -B | --OnTheFly=<reordering> .... enable BDD construction on the fly\n");
  trace("                                   (only relevant for OG)\n");
  trace("                                   argument <reordering> specifies reodering\n");
  trace("                                   method (see option -b)\n");
  trace(" -o | --output=<filename prefix> . prefix of the output files\n");
  trace("\n");
  trace("\n");
  trace("For more information see:\n");
  trace("  http://www.informatik.hu-berlin.de/top/tools4bpel/fiona\n");
  trace("\n");
}


// Prints some version information
void print_version() {
  trace(std::string(PACKAGE_STRING) + " -- ");
  trace("Functional InteractiON Analysis of open workflow nets\n");
  trace("\n");
  trace("Copyright (C) 2005, 2006, 2007 Peter Massuthe and Daniela Weinberg\n");
  trace("This is free software; see the source for copying conditions. There is NO\n");
  trace("warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\n");
}


void printErrorInvalidNumberForOptionAndExit(const std::string& option,
    const std::string& givenArgument) {
    cerr << "Error:\tArgument for option " << option << " has to be a "
            "non-negative number." << endl
         << "\tInstead you gave '" << givenArgument
         << "' as the argument for " << option << '.' << endl
         << "\tEnter \"fiona --help\" for more information.\n"
         << endl;

    exit(1);
}


void testForInvalidArgumentNumberAndPrintErrorAndExitIfNecessary(
    const std::string& option, const std::string& givenArgument) {
    if (!isNonNegativeInteger(givenArgument)) {
        printErrorInvalidNumberForOptionAndExit(option, optarg);
    }
}


// get parameters from options
void parse_command_line(int argc, char* argv[]) {

    int i;

    // the program name on the commandline
    std::string progname = std::string(argv[0]);

    // initialize options
    options[O_HELP] = false;
    options[O_VERSION] = false;
    options[O_DEBUG] = false;
    options[O_GRAPH_TYPE] = false;
    options[O_SHOW_NODES] = false;
//    options[O_CALC_ALL_STATES] = false; // standard: man muss -a angeben, um voll
    options[O_CALC_ALL_STATES] = true; // so lange Reduktion im Teststadium
    options[O_CALC_REDUCED_IG] = false;
    options[O_BDD] = false;
    options[O_OTF] = false;
    options[O_EX] = false;
    options[O_MATCH] = false;
    options[O_PRODUCTOG] = false;
    options[O_SIMULATES] = false;
	options[O_EQUALS] = false;
    options[O_OUTFILEPREFIX] = false;
    options[O_COUNT_SERVICES] = false;

    options[O_MESSAGES_MAX] = true;
    options[O_EVENT_USE_MAX] = false;

    // initialize parameters
    parameters[P_IG] = true;
    parameters[P_OG] = false;
    parameters[P_SHOW_ALL_NODES] = false;
    parameters[P_SHOW_BLUE_NODES_ONLY] = true;
    parameters[P_SHOW_NO_RED_NODES] = true;
    parameters[P_SHOW_EMPTY_NODE] = false;
    parameters[P_SHOW_STATES_PER_NODE] = false;
    parameters[P_SHOW_DEADLOCKS_PER_NODE] = false;

    bdd_reordermethod = 0;

    messages_manual = 1;
    events_manual = -1;

    // evaluate options and set parameters
    trace(TRACE_0, "\n");
    int optc = 0;
    while ((optc = getopt_long (argc, argv, par_string, longopts, (int *) 0))
           != EOF) {

        switch (optc) {
            case 'h': print_help();      exit(0);
            case 'v': print_version();   exit(0);
            case 'd':
                if (string(optarg) == "1") {
                    options[O_DEBUG] = true;
                    debug_level = TRACE_1;
                } else if (string(optarg) == "2") {
                    options[O_DEBUG] = true;
                    debug_level = TRACE_2;
                } else if (string(optarg) == "3") {
                    options[O_DEBUG] = true;
                    debug_level = TRACE_3;
                } else if (string(optarg) == "4") {
                    options[O_DEBUG] = true;
                    debug_level = TRACE_4;
                } else if (string(optarg) == "5") {
                    options[O_DEBUG] = true;
                    debug_level = TRACE_5;
                } else {
                    cerr << "Error:\twrong debug mode" << endl
                         << "\tEnter \"fiona --help\" for more information.\n"
                         << endl;
                    exit(1);
                }
                break;
            case 't': {
                string lc_optarg = toLower(optarg);
                if (lc_optarg == "og") {
                    options[O_GRAPH_TYPE] = true;
                    parameters[P_OG] = true;
                    parameters[P_IG] = false;
                } else if (lc_optarg == "ig") {
                    options[O_GRAPH_TYPE] = true;
                    parameters[P_OG] = false;
                    parameters[P_IG] = true;
                } else if (lc_optarg == "smallpartner") {
                    options[O_GRAPH_TYPE] = true;
                    parameters[P_OG] = false;
                    parameters[P_IG] = true;
                    options[O_SYNTHESIZE_PARTNER_OWFN] = true;
                } else if (lc_optarg == "mostpermissivepartner") {
                    options[O_GRAPH_TYPE] = true;
                    parameters[P_OG] = true;
                    parameters[P_IG] = false;
                    options[O_SYNTHESIZE_PARTNER_OWFN] = true;
                } else if (lc_optarg == "match") {
                    options[O_MATCH] = true;
                    parameters[P_IG] = false;
                } else if (lc_optarg == "simulation") {
                    options[O_SIMULATES] = true;
                    parameters[P_IG] = false;
                } else if (lc_optarg == "equality") {
                    // using simulation in both directions
                    options[O_EQUALS] = true;
                    parameters[P_IG] = false;
                } else if (lc_optarg == "equivalence") {
                    // using BDDs
                    options[O_EX] = true;
                    options[O_GRAPH_TYPE] = false;
                    parameters[P_IG] = false;
                    parameters[P_OG] = false;
                } else if (lc_optarg == "productog") {
                    parameters[P_IG] = false;
                    options[O_PRODUCTOG] = true;
                } else if (lc_optarg == "countservices") {
                    options[O_COUNT_SERVICES] = true;
                    parameters[P_IG] = false;
                } else {
                    cerr << "Error:\twrong modus operandi (option -t)" << endl
                         << "\tEnter \"fiona --help\" for more information.\n"
                         << endl;
                    exit(1);
                }
                break;
            }
            case 'm':
                options[O_MESSAGES_MAX] = true;
                testForInvalidArgumentNumberAndPrintErrorAndExitIfNecessary(
                    "-m", optarg);
                messages_manual = atoi(optarg);
                break;
            case 'e':
                options[O_EVENT_USE_MAX] = true;
                events_manual = atoi(optarg);
                if (events_manual < 0) {
                	options[O_EVENT_USE_MAX] = false;
                }
                break;
            case 's':
                if (string(optarg) == "blue") {
                    options[O_SHOW_NODES] = true;
                    parameters[P_SHOW_ALL_NODES] = false;
                    parameters[P_SHOW_NO_RED_NODES] = false;
                    parameters[P_SHOW_BLUE_NODES_ONLY] = true;
                } else if (string(optarg) == "rednodes") {
                    options[O_SHOW_NODES] = true;
                    parameters[P_SHOW_ALL_NODES] = false;
                    parameters[P_SHOW_NO_RED_NODES] = false;
                    parameters[P_SHOW_BLUE_NODES_ONLY] = false;
                } else if (string(optarg) == "empty") {
                    options[O_SHOW_NODES] = true;
                    parameters[P_SHOW_EMPTY_NODE] = true;
                } else if (string(optarg) == "allnodes") {
                    options[O_SHOW_NODES] = true;
                    parameters[P_SHOW_ALL_NODES] = true;
                    parameters[P_SHOW_EMPTY_NODE] = true;
                    parameters[P_SHOW_NO_RED_NODES] = false;
                    parameters[P_SHOW_BLUE_NODES_ONLY] = false;
                } else if (string(optarg) == "allstates") {
                    options[O_SHOW_NODES] = true;
                    parameters[P_SHOW_STATES_PER_NODE] = true;
                } else if (string(optarg) == "deadlocks") {
                    options[O_SHOW_NODES] = true;
                    parameters[P_SHOW_DEADLOCKS_PER_NODE] = true;
                } else {
                    cerr << "Error:\twrong show option" << endl
                         << "\tEnter \"fiona --help\" for more information.\n"
                         << endl;
                    exit(1);
                }
                break;
//            case GETOPT_REDUCENODES:
            case 'R':
                options[O_CALC_ALL_STATES] = false;
                break;
            case 'r':
                options[O_CALC_REDUCED_IG] = true;
                break;
            case 'b':
                options[O_BDD] = true;
                testForInvalidArgumentNumberAndPrintErrorAndExitIfNecessary(
                    "-b", optarg);
                i = atoi(optarg);
                if (i >= 0 && i <= 21){
                    bdd_reordermethod = i;
                } else {
                    cerr << "Error:\twrong BDD reorder method" << endl
                         << "\tEnter \"fiona --help\" for more information.\n"
                         << endl;
                    exit(1);
                }
                break;
            case 'B':
                options[O_OTF] = true;
                options[O_BDD] = false;
                testForInvalidArgumentNumberAndPrintErrorAndExitIfNecessary(
                    "-B", optarg);
                i = atoi(optarg);
                if (i >= 0 && i <= 21){
                    bdd_reordermethod = i;
                } else {
                    cerr << "Error:\twrong BDD reorder method" << endl
                         << "\tEnter \"fiona --help\" for more information.\n"
                         << endl;
                    exit(1);
                }
                break;
            case 'o':
                if (optarg) {
                    options[O_OUTFILEPREFIX] = true;
                    outfilePrefix = optarg;
                } else {
                    cerr << "Error:\toutput filename prefix missing" << endl
                         << "\tEnter \"fiona --help\" for more information.\n"
                         << endl;
                    exit(1);
                }
                break;
            case '?':
                cerr << "Error:\toption error" << endl
                     << "\tEnter \"fiona --help\" for more information.\n"
                     << endl;
                exit(1);
            default:
                cerr << "Warning:\tUnknown option ignored" << endl
                     << "\tEnter \"fiona --help\" for more information.\n"
                     << endl;
                break;
        }
    }

    for ( ; optind < argc; ++optind) {
        switch (getFileType(argv[optind])) {
            case FILETYPE_OWFN:
                netfiles.push_back(argv[optind]);
                break;
            case FILETYPE_OG:
                ogfiles.push_back(argv[optind]);
                break;
            case FILETYPE_UNKNOWN:
                cerr << "Error:\tCannot determine file type of '"
                     << argv[optind] << "'" << endl
                     << "\tEnter \"fiona --help\" for more information.\n"
                     << endl;
        }
    }

    if (options[O_EX] == true && netfiles.size() == 0) {
        cerr << "To check equivalence of two OGs, please give the"
        	 << "corresponding oWFNs on the command line."
        	 << "The OGs as BDDs must have been computed before."
             << "\tEnter \"fiona --help\" for more information.\n" << endl;
        exit(1);
    }

    if (ogfiles.size() == 0 && netfiles.size() == 0) {
        cerr << "Error:\tNo oWFNs or OGs given." << endl
             << "\tEnter \"fiona --help\" for more information.\n" << endl;
        exit(1);
    }

    if (options[O_EVENT_USE_MAX] == false && options[O_MESSAGES_MAX] == false) {
        cerr << "if no limit for using events is given, you must specify a message bound via option -m " << endl
             << "\tEnter \"fiona --help\" for more information.\n" << endl;
        exit(1);
    }

    if (options[O_BDD] && parameters[P_OG] == false && options[O_EX] == false) {
        cerr << "computing IG -- BDD option ignored\n" << endl;
        options[O_BDD] = false;
    }
}

FileType getFileType(const std::string& fileName) {
    unsigned int owfnHits = 0;
    unsigned int ogHits   = 0;

    ifstream fileStream(fileName.c_str());
    if (!fileStream) {
        cerr << "Error:\tCould not open '" << fileName << "' for reading.\n"
             << endl;
        exit(1);
    }

    string line;
    while (getline(fileStream, line)) {
        if (contains(line, "PLACE")) ++owfnHits;
        if (contains(line, "INTERNAL")) ++owfnHits;
        if (contains(line, "INPUT")) ++owfnHits;
        if (contains(line, "OUTPUT")) ++owfnHits;
        if (contains(line, "INITIALMARKING")) ++owfnHits;
        if (contains(line, "FINALMARKING")) ++owfnHits;
        if (contains(line, "FINALCONDITION")) ++owfnHits;
        if (contains(line, "CONSUME")) ++owfnHits;
        if (contains(line, "PRODUCE")) ++owfnHits;
        if (contains(line, "ALL_OTHER_PLACES_EMPTY")) ++owfnHits;
        if (contains(line, "ALL_OTHER_INTERNAL_PLACES_EMPTY")) ++owfnHits;
        if (contains(line, "ALL_OTHER_EXTERNAL_PLACES_EMPTY")) ++owfnHits;
        if (contains(line, "MAX_UNIQUE_EVENTS")) ++owfnHits;
        if (contains(line, "ON_LOOP")) ++owfnHits;
        if (contains(line, "MAX_OCCURRENCES")) ++owfnHits;

        if (contains(line, "NODES")) ++ogHits;
        if (contains(line, "INITIALNODE")) ++ogHits;
        if (contains(line, "TRANSITIONS")) ++ogHits;
    }

    if (owfnHits > ogHits) {
        return FILETYPE_OWFN;
    }

    if (owfnHits < ogHits) {
        return FILETYPE_OG;
    }

    return FILETYPE_UNKNOWN;
}

bool contains(const std::string& hostString, const std::string& subString) {
    return hostString.find(subString, 0) != std::string::npos;
}

