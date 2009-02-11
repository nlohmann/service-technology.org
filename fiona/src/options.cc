/*****************************************************************************\
 Copyright (c) 2008  Robert Danitz, Christian Gierds, Niels Lohmann,
                     Peter Massuthe, Richard Mu"ller, Janine Ott,
                     Jan Su"rmeli, Daniela Weinberg, Karsten Wolf,
                     Martin Znamirowski

 Copyright (c) 2007  Jan Bretschneider, Robert Danitz, Christian Gierds,
                     Kathrin Kaschner, Leonard Kern, Niels Lohmann,
                     Peter Massuthe, Daniela Weinberg, Karsten Wolf

 Copyright (c) 2006  Kathrin Kaschner, Peter Massuthe, Daniela Weinberg,
                     Karsten Wolf

 This file is part of Fiona, see <http://service-technology.org/fiona>.

 Fiona is free software; you can redistribute it and/or modify it under the
 terms of the GNU General Public License as published by the Free Software
 Foundation; either version 3 of the License, or (at your option) any later
 version.

 Fiona is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with
 Fiona (see file COPYING). If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

/*!
 * \file    options.cc
 *
 * \brief   options management for fiona
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
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
#include "config.h"
#include "pnapi.cc"

using namespace std;

// some file names and pointers
std::list<string> netfiles;
std::list<string> ogfiles;
std::list<string> dotfiles;

// boolean value do save which file was mentioned first for simulation
bool OGfirst;

std::string ogfileToMatch;
std::string outfilePrefix;
std::string adapterRulesFile;

int events_manual;
unsigned int messages_manual;
int bdd_reordermethod;

// const int GETOPT_BLA = 256;

/// pointer to log stream
std::ostream* log_output = &std::cout; // &std::clog;

// different modes controlled by command line
std::map<possibleOptions, bool> options;
std::map<possibleParameters, bool> parameters;


// long options
static struct option longopts[] = {
//  { "bla",           no_argument,       NULL, GETOPT_BLA },
    { "help",            no_argument,       NULL, 'h' },
    { "version",         no_argument,       NULL, 'v' },
    { "debug",           required_argument, NULL, 'd' },
    { "type",            required_argument, NULL, 't' },
    { "messagebound",    required_argument, NULL, 'm' },
    { "eventsmaximum",   required_argument, NULL, 'e' },
    { "readevents",      no_argument,       NULL, 'E' },
    { "reduceIG",        no_argument,       NULL, 'r' },
    { "reduce-nodes",    no_argument,       NULL, 'R' },
    { "show",            required_argument, NULL, 's' },
    { "BDD",             required_argument, NULL, 'b' },
    { "OnTheFly",        required_argument, NULL, 'B' },
    { "output",          required_argument, NULL, 'o' },
    { "no-output",       no_argument,       NULL, 'Q' },
    { "multipledeadlocks", no_argument,     NULL, 'M' },
    { "parameter",       required_argument, NULL, 'p' },
    { "adapterrules",    required_argument, NULL, 'a' },
    { NULL,              0,                 NULL, 0   }
};

const char* par_string = "hvd:t:m:e:ErRs:b:B:o:QMp:a:";


// --------------------- functions for command line evaluation ------------------------
// Prints an overview of all commandline arguments.
void print_help() {
  trace("Usage: fiona [OPTION]... [FILE]...\n");
  trace("\n");
  trace("[FILE]:  can be one ore more oWFN(s) and/or OG(s)\n");
  trace("\n");
  trace("Options: can be any of the follwing\n");
  trace("         (if an option is skipped, the default settings are denoted)\n");
  trace("\n");
  trace(" -h | --help ................... print this information and exit\n");
  trace(" -v | --version ................ print version information and exit\n");
#ifndef NDEBUG
  // show this part of the help only, if debug output is possible
  trace(" -d | --debug=<level> .......... set debug <level>:\n");
  trace("                                   1 - show nodes and dfs information\n");
  trace("                                   2 - show analyse information (i.e. colors)\n");
  trace("                                   3 - show information on events and states\n");
  trace("                                   4 - yet to be defined ;)\n");
  trace("                                   5 - show detailed information on everything\n");
#endif
  trace("\n");
  trace(" -t | --type=<type> ............ select the modus operandi of fiona <type>:\n");
  trace("                                 (only one type is allowed; default is IG)\n");
  trace("\n");
  trace("                                   IG          - compute interaction graph\n");
  trace("                                   OG          - compute operating guideline\n");
  trace("                                   match       - check whether a given oWFN\n");
  trace("                                                 matches with a given OG\n");
  trace("                                   productog   - calculate the product OG\n");
  trace("                                                 of all given OGs\n");
  trace("                                   minimize    - minimizes a given OG\n");
  trace("\n");
  trace("                                   reduce      - structurally reduce all\n");
  trace("                                                 given oWFNs\n");
  trace("                                   normalize   - normalize all given oWFNs\n");
  trace("\n");
  trace("                                   pv          - calculate the public view of a\n");
  trace("                                                 given OG\n");
  trace("                                   simulation  - check whether the first OG\n");
  trace("                                                 characterizes more strategies\n");
  trace("                                                 than the second one\n");
  trace("                                                 (if an oWFN is given, its OG\n");
  trace("                                                 is computed automatically)\n");
  trace("                                   equivalence - check whether two OGs\n");
  trace("                                                 characterize the same\n");
  trace("                                                 strategies (if an oWFN is\n");
  trace("                                                 given, its OG is computed\n");
  trace("                                                 automatically)\n");
  trace("                                                 (option -b1 can be used to\n");
  trace("                                                 check the equivalence of \n");
  trace("                                                 (already present!) BDDs)\n");
  trace("                         mostpermissivepartner - generates the most permissive\n");
  trace("                                                 partner for a given oWFN \n");
  trace("                                                 or all given OGs\n");
  trace("                                  smallpartner - generates a (small) partner for\n");
  trace("                                                 a given oWFN or IG\n");
  trace("                                   adapter     - calculate an adapter for a\n");
  trace("                                                 given set of services\n");
  trace("                                  smalladapter - calculate a small adapter for\n");
  trace("                                                 a given set of services\n");
  trace("\n");
  trace("                                   png         - generate png files for all\n");
  trace("                                                 given oWFNs or OGs\n");
//  trace("                                   tex         - generate gastex files for a\n");
//  trace("                                                 given annotated dot file\n");
  trace("                               checkfalsenodes - checks for nodes that\n");
  trace("                                                 violate their own annotation.\n");
  trace("                              removefalsenodes - removes nodes that\n");
  trace("                                                 violate their own annotation \n");
  trace("                                                 and stores the result to a\n");
  trace("                                                 specified output file (-o)\n");
  trace("                                   isacyclic   - check a given OG for cycles\n");
  trace("                                   count       - count the number of strategies\n");
  trace("                                                 that are characterized by a\n");
  trace("                                                 given OG\n");
  trace("                                   readOG      - only reads a given OG File\n");
  trace(" -m | --messagebound=<level> ... set maximum number of same messages per\n");
  trace("                                 state to <level>  (default is 1)\n");
//  trace(" -e | --eventsmaximum=<level> .. set event to occur at most <level> times\n");
//  trace("                                 (default is 1)\n");
//  trace("                                 (-1 means disabling -e option, but is only\n");
//  trace("                                 possible if -m option is set)\n");
//  trace("                                 (only relevant for OG)\n");
//  trace(" -E | --readevents ............. set the maximum occurrences of events to\n");
//  trace("                                 values taken from the oWFN file. If none\n");
//  trace("                                 are given, eventsmaximum is used. \n");
//  trace("                                 (default is no maximum)\n");
  trace(" -r | --reduceIG ............... use reduction rules for IG. If no reduction \n");
  trace("                                 rules are given (see -p), all rules are used. \n");
  trace(" -R | --reduce-nodes ........... use node reduction (IG or OG) which stores\n");
  trace("                                 less states per IG/OG node\n");
  trace("                                 (reduces memory, but increases time)\n");
  trace(" -s | --show=<parameter> ....... different display options <parameter>:\n");
  trace("                                   blue      - show blue nodes only (default)\n");
  trace("                                               (empty node not shown)\n");
  trace("                                   empty     - show empty node\n");
  trace("                                   rednodes  - show blue and red nodes\n");
  trace("                                               (only empty node not shown\n");
  trace("                                   allnodes  - show all nodes\n");
  trace("                                   allstates - show all calculated states per\n");
  trace("                                               node\n");
  trace("                                   deadlocks - show all but transient states\n");
  trace("                                   coverall  - show all covered oWFN nodes per\n");
  trace("                                               node\n");
  trace(" -b | --BDD=<reordering> ....... enable BDD construction (only relevant for OG)\n");
  trace("                                 <reordering> specifies reordering method:\n");
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
  trace(" -B | --OnTheFly=<reordering> .. BDD construction on the fly (only for OG)\n");
  trace("                                 <reordering> as above\n");
  trace(" -o | --output=<filename> ...... prefix of the output files\n");
  trace(" -Q | --no-output .............. runs quietly, i.e., produces no output files\n");
  trace(" -M | --multipledeadlocks ...... create multiple deadlocks of public view\n");
  trace("                                 (only relevant for PV mode -t PV)\n");
  trace(" -p | --parameter=<param> ...... additional parameter <param>\n");
  trace("                                   no-png     - does not create a PNG file\n");
  trace("                                   no-dot     - does not create dot-related output\n");
  trace("                                   responsive - create responsive partner(s)\n");
  trace("                                                (for IG/OG computation)\n");
//  trace("                                   diagnosis  - disables optimizations\n");
//  trace("                                  distributed - checks a given oWFN for\n");
//  trace("                                                distributed controllability\n");
//  trace("                                   autonomous - autonomous controllability\n");
  trace("                                   r1 - r5    - set reduction level in mode\n");
  trace("                                                \"-t reduce\" to to the \n");
  trace("                                                specified value \n");
  trace("                                   cre        - use \"combine receiving events\"\n");
  trace("                                                rule for IG reduction (see -r)\n");
  trace("                                   rbs        - use \"receive before sending\"\n");
  trace("                                                rule for IG reduction (see -r)\n");
  trace("                                   cover      - requires \"-t og\" or \"-t match\"\n");
  trace("                                                and is used for OGs with constraint\n");
  trace(" -a | --adapterrules=<filename>  read adapter rules from <filename>\n");
  trace("\n");
  trace("\n");
  trace("The most common calls of fiona are:\n");
  trace("  fiona example.owfn                           (for controllability)\n");
  trace("  fiona -t og example.owfn                     (for OG construction)\n");
  trace("  fiona -t match client.owfn service.owfn.og   (for matching client.owfn\n");
  trace("                                               with the OG of service.owfn)\n");
  trace("\n");
  trace("For more information see:\n");
  trace("  http://www.service-technology.org/fiona\n");
  trace("\n");
}


// Prints some version information
void print_version() {
    trace(std::string(PACKAGE_STRING) + " -- ");
    trace("Functional InteractiON Analysis of open nets\n\n");

    trace("Copyright (c) 2008  Robert Danitz, Christian Gierds, Niels Lohmann,\n");
    trace("                    Peter Massuthe, Richard Müller, Janine Ott,\n");
    trace("                    Jan Sürmeli, Daniela Weinberg, Karsten Wolf,\n");
    trace("                    Martin Znamirowski\n\n");

    trace("Copyright (c) 2007  Jan Bretschneider, Robert Danitz, Christian Gierds,\n");
    trace("                    Kathrin Kaschner, Leonard Kern, Niels Lohmann,\n");
    trace("                    Peter Massuthe, Daniela Weinberg, Karsten Wolf\n\n");

    trace("Copyright (c) 2006  Kathrin Kaschner, Peter Massuthe, Daniela Weinberg,\n");
    trace("                    Karsten Wolf\n\n");

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

    exit(EC_BAD_CALL);
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
    std::string progname(argv[0]);

    // initialize options
    options[O_HELP] = false;
    options[O_VERSION] = false;
    options[O_DEBUG] = false;
    options[O_GRAPH_TYPE] = false;
    options[O_MESSAGES_MAX] = true;
    options[O_EVENT_USE_MAX] = false;
    options[O_READ_EVENTS] = false;
    options[O_CALC_REDUCED_IG] = false;
//    options[O_CALC_ALL_STATES] = false; // standard: man muss -a angeben, um voll
    options[O_CALC_ALL_STATES] = true; // so lange Reduktion im Teststadium
    options[O_SHOW_NODES] = false;
    options[O_BDD] = false;
    options[O_OTF] = false;
    options[O_OUTFILEPREFIX] = false;
    options[O_NOOUTPUTFILES] = false;
    options[O_PV_MULTIPLE_DEADLOCKS] = false;
    options[O_PARAMETER] = false;
    options[O_ADAPTER_FILE] = false;

    // initialize parameters

    // -t parameters are the "scenarios" in which Fiona runs
    // (descriptions are found in options.h)
    parameters[P_OG] = false;
    parameters[P_IG] = true;
    parameters[P_MATCH] = false;
    parameters[P_MINIMIZE_OG] = false;
    parameters[P_PV] = false;
    parameters[P_SYNTHESIZE_PARTNER_OWFN] = false;
    parameters[P_PRODUCTOG] = false;
    parameters[P_SIMULATES] = false;
    parameters[P_EX] = false;
    parameters[P_EQ_R] = false;
    parameters[P_ADAPTER] = false;
    parameters[P_SMALLADAPTER] = false;
    parameters[P_PNG] = false;
    parameters[P_GASTEX] = false;
    parameters[P_CHECK_FALSE_NODES] = false;
    parameters[P_REMOVE_FALSE_NODES] = false;
    parameters[P_CHECK_ACYCLIC] = false;
    parameters[P_COUNT_SERVICES] = false;
    parameters[P_DISTRIBUTED] = false;
    parameters[P_READ_OG] = false;
    parameters[P_REDUCE] = false;
    parameters[P_NORMALIZE] = false;
    parameters[P_COVERALL] = false;
    parameters[P_RESPONSIVE] = false;

    // -s parameters
    parameters[P_SHOW_BLUE_NODES] = true;
    parameters[P_SHOW_EMPTY_NODE] = false;
    parameters[P_SHOW_RED_NODES] = false;
    parameters[P_SHOW_ALL_NODES] = false;
    parameters[P_SHOW_STATES_PER_NODE] = false;
    parameters[P_SHOW_DEADLOCKS_PER_NODE] = false;

    // -p parameters
    parameters[P_NOPNG] = false;
    parameters[P_NODOT] = false;
    parameters[P_TEX] = false;
    parameters[P_DIAGNOSIS] = false;
    parameters[P_AUTONOMOUS] = false;
    parameters[P_REDUCE_LEVEL] = false;
    parameters[P_USE_CRE] = false;
    parameters[P_USE_RBS] = false;
    parameters[P_USE_EAD] = false;
    parameters[P_REPRESENTATIVE] = false;
    parameters[P_SINGLE] = true;
    parameters[P_COVER] = false;




    bdd_reordermethod = 0;

    messages_manual = 1;
    events_manual = -1;
    globals::reduction_level = 0;

    // evaluate options and set parameters
    trace( "\n");
    int optc = 0;
    while ((optc = getopt_long (argc, argv, par_string, longopts, (int *) 0))
           != EOF) {

        switch (optc) {
            // case GETOPT_REDUCENODES:
            case 'h':
                options[O_HELP] = true;
                print_help();
                exit(0);
            case 'v':
                options[O_VERSION] = true;
                print_version();
                exit(0);
            case 'd':
                options[O_DEBUG] = true;
                if (string(optarg) == "1") {
                    debug_level = TRACE_1;
                } else if (string(optarg) == "2") {
                    debug_level = TRACE_2;
                } else if (string(optarg) == "3") {
                    debug_level = TRACE_3;
                } else if (string(optarg) == "4") {
                    debug_level = TRACE_4;
                } else if (string(optarg) == "5") {
                    debug_level = TRACE_5;
                } else {
                    cerr << "Error:\twrong debug mode" << endl
                         << "\tEnter \"fiona --help\" for more information.\n"
                         << endl;
                    exit(EC_BAD_CALL);
                }
                break;
            case 't': {
                options[O_GRAPH_TYPE] = true;
                string lc_optarg = toLower(optarg);
                if (lc_optarg == "og") {
                    parameters[P_OG] = true;
                    parameters[P_IG] = false;
                } else if (lc_optarg == "ig") {
                    parameters[P_OG] = false;
                    parameters[P_IG] = true;
                } else if (lc_optarg == "match") {
                    parameters[P_MATCH] = true;
                    parameters[P_IG] = false;
                } else if (lc_optarg == "minimize") {
                    parameters[P_OG] = false;
                    parameters[P_IG] = false;
                    // if original OG has empty node, then empty node must be kept
                    parameters[P_SHOW_EMPTY_NODE] = true;
                    parameters[P_MINIMIZE_OG] = true;
                } else if ((lc_optarg == "pv") || (lc_optarg == "publicview")) {
                    parameters[P_PV] = true;
                    parameters[P_IG] = false;
                } else if (lc_optarg == "smallpartner") {
                    parameters[P_OG] = false;
                    parameters[P_IG] = true;
                    parameters[P_SHOW_EMPTY_NODE] = false;
                    parameters[P_SYNTHESIZE_PARTNER_OWFN] = true;
                } else if (lc_optarg == "mostpermissivepartner") {
                    parameters[P_OG] = true;
                    parameters[P_IG] = false;
                    parameters[P_SHOW_EMPTY_NODE] = false;
                    parameters[P_SYNTHESIZE_PARTNER_OWFN] = true;
                } else if (lc_optarg == "productog") {
                    parameters[P_IG] = false;
                    parameters[P_PRODUCTOG] = true;
                    // if original OG has empty node, then empty node must be kept
                    parameters[P_SHOW_EMPTY_NODE] = true;
                } else if (lc_optarg == "simulation") {
                    parameters[P_SIMULATES] = true;
                    parameters[P_IG] = false;
                } else if (lc_optarg == "equivalence") {
                    // using BDDs or OGs/oWFNs
                    parameters[P_EX] = true;
                    parameters[P_IG] = false;
                    parameters[P_OG] = false;
                } else if (lc_optarg == "eqr") {
                    parameters[P_EQ_R] = true;
                    // make sure -s empty is set by setting it ;-)
                    options[O_SHOW_NODES] = true;
                    parameters[P_SHOW_BLUE_NODES] = true;
                    parameters[P_SHOW_EMPTY_NODE] = true;
                    parameters[P_SHOW_RED_NODES] = false;
                    parameters[P_SHOW_ALL_NODES] = false;
                } else if (lc_optarg == "adapter") {
                    parameters[P_OG] = true;
                    parameters[P_IG] = false;
                    parameters[P_ADAPTER] = true;
                    parameters[P_SHOW_EMPTY_NODE] = true;
                    parameters[P_SYNTHESIZE_PARTNER_OWFN] = true;
                } else if (lc_optarg == "smalladapter") {
                    parameters[P_OG] = false;
                    parameters[P_IG] = true;
                    parameters[P_SMALLADAPTER] = true;
                    parameters[P_SHOW_EMPTY_NODE] = true;
                    parameters[P_SYNTHESIZE_PARTNER_OWFN] = true;
                } else if (lc_optarg == "png") {
                    parameters[P_IG] = false;
                    parameters[P_OG] = false;
                    parameters[P_PNG] = true;
                } else if (lc_optarg == "tex") {
                    parameters[P_IG] = false;
                    parameters[P_OG] = false;
                    parameters[P_GASTEX] = true;
                } else if (lc_optarg == "checkfalsenodes") {
                    parameters[P_OG] = false;
                    parameters[P_IG] = false;
                    parameters[P_CHECK_FALSE_NODES] = true;
                } else if (lc_optarg == "removefalsenodes") {
                    parameters[P_OG] = false;
                    parameters[P_IG] = false;
                    parameters[P_REMOVE_FALSE_NODES] = true;
                    // if original OG has empty node, then empty node must be kept
                    parameters[P_SHOW_EMPTY_NODE] = true;
                } else if (lc_optarg == "isacyclic") {
                    parameters[P_IG] = false;
                    parameters[P_CHECK_ACYCLIC] = true;
                } else if (lc_optarg == "count") {
                    parameters[P_IG] = false;
                    parameters[P_COUNT_SERVICES] = true;
                } else if (lc_optarg == "readog") {
                    parameters[P_IG] = false;
                    parameters[P_OG] = false;
                    parameters[P_READ_OG] = true;
                } else if (lc_optarg == "reduce") {
                    if (globals::reduction_level == 0) {
                        globals::reduction_level = 5;
                    }
                    parameters[P_IG] = false;
                    parameters[P_OG] = false;
                    parameters[P_REDUCE] = true;
                } else if (lc_optarg == "normalize") {
                    parameters[P_IG] = false;
                    parameters[P_OG] = false;
                    parameters[P_NORMALIZE] = true;
                } else {
                    cerr << "Error:\t\"" << optarg << "\" is a wrong modus operandi (option -t)" << endl
                         << "\tEnter \"fiona --help\" for more information.\n"
                         << endl;
                    exit(EC_BAD_CALL);
                }
                break;
            }
            case 'm':
                options[O_MESSAGES_MAX] = true;
                testForInvalidArgumentNumberAndPrintErrorAndExitIfNecessary("-m", optarg);
                messages_manual = atoi(optarg);
                break;
            case 'e':
                options[O_EVENT_USE_MAX] = true;
                events_manual = atoi(optarg);
                break;
            case 'E':
                options[O_READ_EVENTS] = true;
                break;
            case 'r':
                options[O_CALC_REDUCED_IG] = true;
                break;
            case 'R':
                options[O_CALC_ALL_STATES] = false;
                break;
            case 's':
                options[O_SHOW_NODES] = true;
                if (string(optarg) == "blue") {
                    parameters[P_SHOW_BLUE_NODES] = true;
                } else if (string(optarg) == "empty") {
                    parameters[P_SHOW_EMPTY_NODE] = true;
                } else if (string(optarg) == "rednodes") {
                    parameters[P_SHOW_RED_NODES] = true;
                } else if (string(optarg) == "allnodes") {
                    parameters[P_SHOW_BLUE_NODES] = true;
                    parameters[P_SHOW_EMPTY_NODE] = true;
                    parameters[P_SHOW_RED_NODES] = true;
                    parameters[P_SHOW_ALL_NODES] = true;
                } else if (string(optarg) == "allstates") {
                    parameters[P_SHOW_STATES_PER_NODE] = true;
                } else if (string(optarg) == "deadlocks") {
                    parameters[P_SHOW_DEADLOCKS_PER_NODE] = true;
                } else if (string(optarg) == "coverall") {
                    parameters[P_COVERALL] = true;
                } else {
                    cerr << "Error:\twrong show option" << endl
                         << "\tEnter \"fiona --help\" for more information.\n"
                         << endl;
                    exit(EC_BAD_CALL);
                }
                break;
            case 'b':
                options[O_BDD] = true;
                testForInvalidArgumentNumberAndPrintErrorAndExitIfNecessary("-b", optarg);
                i = atoi(optarg);
                if (i >= 0 && i <= 21){
                    bdd_reordermethod = i;
                } else {
                    cerr << "Error:\twrong BDD reorder method" << endl
                         << "\tEnter \"fiona --help\" for more information.\n"
                         << endl;
                    exit(EC_BAD_CALL);
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
                    exit(EC_BAD_CALL);
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
                    exit(EC_BAD_CALL);
                }
                break;
            case 'Q':
                options[O_NOOUTPUTFILES] = true;
                break;
            case 'M':
                options[O_PV_MULTIPLE_DEADLOCKS] = true;
                break;
            case 'p': {
                string lc_optarg = toLower(optarg);
                options[O_PARAMETER] = true;
                if (lc_optarg == "no-png") {
                    parameters[P_NOPNG] = true;
                } else if (lc_optarg == "no-dot") {
                    parameters[P_NODOT] = true;
                } else if (lc_optarg == "tex") {
                    parameters[P_TEX] = true;
                } else if (lc_optarg == "diagnosis") {
                    parameters[P_DIAGNOSIS] = true;
                    options[O_SHOW_NODES] = true;
                    parameters[P_SHOW_BLUE_NODES] = true;
                    parameters[P_SHOW_EMPTY_NODE] = true;
                    parameters[P_SHOW_RED_NODES] = true;
                    parameters[P_SHOW_ALL_NODES] = true;
                    parameters[P_SHOW_DEADLOCKS_PER_NODE] = true;
                } else if (lc_optarg == "distributed") {
                    parameters[P_OG] = true;
                    parameters[P_IG] = false;
                    options[O_SHOW_NODES] = true;
                    parameters[P_SHOW_EMPTY_NODE] = true; // essential for theory!
                    parameters[P_DISTRIBUTED] = true;
                } else if (lc_optarg == "autonomous") {
                    parameters[P_AUTONOMOUS] = true;
                } else if (string(optarg) == "r1") {
                    parameters[P_REDUCE_LEVEL] = true;
                    globals::reduction_level = 1;
                } else if (string(optarg) == "r2") {
                    parameters[P_REDUCE_LEVEL] = true;
                    globals::reduction_level = 2;
                } else if (string(optarg) == "r3") {
                    parameters[P_REDUCE_LEVEL] = true;
                    globals::reduction_level = 3;
                } else if (string(optarg) == "r4") {
                    parameters[P_REDUCE_LEVEL] = true;
                    globals::reduction_level = 4;
                } else if (string(optarg) == "r5") {
                    parameters[P_REDUCE_LEVEL] = true;
                    globals::reduction_level = 5;
                } else if (string(optarg) == "cre") {
                    parameters[P_USE_CRE] = true;
                } else if (string(optarg) == "rbs") {
                    parameters[P_USE_RBS] = true;
                } else if (string(optarg) == "ead") {
                    parameters[P_USE_EAD] = true;
                } else if (lc_optarg == "representative") {
                    parameters[P_REPRESENTATIVE] = true;
                    parameters[P_SINGLE] = false;
                } else if (lc_optarg == "single") {
                    parameters[P_SINGLE] = true;
                    parameters[P_REPRESENTATIVE] = false;
                } else if (lc_optarg == "responsive") {
                    parameters[P_RESPONSIVE] = true;
                } else if (lc_optarg == "cover") {
                    parameters[P_COVER] = true;
                } else {
                    cerr << "Error:\twrong parameter (option -p)" << endl
                    << "\tEnter \"fiona --help\" for more information.\n" << endl;
                    exit(EC_BAD_CALL);
                }
                break;
            }
            case 'a':
                if (optarg) {
                    options[O_ADAPTER_FILE] = true;
                    adapterRulesFile = optarg;
                } else {
                    cerr << "Error:\toutput filename prefix missing" << endl
                         << "\tEnter \"fiona --help\" for more information.\n"
                         << endl;
                    exit(EC_BAD_CALL);
                }
                break;
            case '?':
                cerr << "Error:\toption error" << endl
                     << "\tEnter \"fiona --help\" for more information.\n"
                     << endl;
                exit(EC_BAD_CALL);
            default:
                cerr << "Warning:\tUnknown option ignored" << endl
                     << "\tEnter \"fiona --help\" for more information.\n"
                     << endl;
                break;
        }
    }

    // if all kinds of nodes have been chosen to be shown, set all-nodes to true
    if (parameters[P_SHOW_BLUE_NODES] == true && parameters[P_SHOW_EMPTY_NODE] == true && parameters[P_SHOW_RED_NODES] == true) {
        parameters[P_SHOW_ALL_NODES] = true;
    }

    // IG Reduction - correcting parameters if needed
    if (options[O_CALC_REDUCED_IG]) {
        if (!parameters[P_USE_CRE] && !parameters[P_USE_RBS]) {
            parameters[P_USE_CRE] = true;
            parameters[P_USE_RBS] = true;
        }
    }

    // responsive partners can only be calculated if there is no state reduction
    // in case, the graphs are to be calculated using state reduction, we set
    // responsive to false --> this is not possible yet
    if (!options[O_CALC_ALL_STATES] && parameters[P_RESPONSIVE]) {
    	parameters[P_RESPONSIVE] = false;
    }

    // Max Occurrences - correcting parameters if needed
    if (!options[O_READ_EVENTS] && !options[O_EVENT_USE_MAX]) {
        options[O_READ_EVENTS] = true;
    }

    bool firstfile = true;
    // reading all oWFNs and OGs
    for ( ; optind < argc; ++optind) {
        TRACE(TRACE_1, "trying to get file type of given file " + (string)(argv[optind]));
        switch (getFileType(argv[optind])) {
            case FILETYPE_OWFN:
                if (firstfile) {
                    OGfirst = false;
                    firstfile = false;
                }
                netfiles.push_back(argv[optind]);
                break;
            case FILETYPE_OG:
                if (firstfile) {
                    OGfirst = true;
                    firstfile = false;
                }
                ogfiles.push_back(argv[optind]);
                break;
            case FILETYPE_DOT:
                if (firstfile) {
                    OGfirst = false;
                    firstfile = false;
                }
                options[O_OUTFILEPREFIX] = true;
                dotfiles.push_back(argv[optind]);
                outfilePrefix = string(argv[optind]);
                break;
            case FILETYPE_UNKNOWN:
                cerr << "Error:\t Cannot determine file type of '"
                     << argv[optind] << "'" << endl
                     << "\tEnter \"fiona --help\" for more information.\n"
                     << endl;
        }
        TRACE(TRACE_1, " ... done\n");
    }
    TRACE(TRACE_1, "\n");

    // read net from stdin
    if (ogfiles.size() == 0 && netfiles.size() == 0 &&
        (parameters[P_IG] || parameters[P_OG] ||
         parameters[P_PNG] || parameters[P_REDUCE])) {

        netfiles.push_back("<stdin>");
    }

    // check if the numbers of inputfiles corresponds to the chosen mode
    if (ogfiles.size() == 0 && netfiles.size() == 0 && dotfiles.size() == 0) {
        cerr << "Error: \t No oWFNs or OGs are given." << endl;
        cerr << "       \t Enter \"fiona --help\" for more information.\n" << endl;
        exit(EC_BAD_CALL);
    }

    if (parameters[P_MINIMIZE_OG] && ogfiles.size() == 0) {
        cerr << "Error: \t If option '-t minimize' is used, at least one OG" << endl;
        cerr << "       \t file must be given." << endl;
        cerr << "       \t Enter \"fiona --help\" for more information.\n" << endl;
        exit(EC_BAD_CALL);
    }

    if (parameters[P_GASTEX] && dotfiles.size() != 1) {
        cerr << "Error: \t If option '-t tex' is used, exactly one annotated dot " << endl;
        cerr << "       \t file must be given." << endl;
        cerr << "       \t Enter \"fiona --help\" for more information.\n" << endl;
        exit(EC_BAD_CALL);
    }

    if (parameters[P_EX] && (((netfiles.size() + ogfiles.size()) != 2)
                          || (options[O_BDD] && netfiles.size() != 2))) {
        cerr << "Error: \t If option '-t equivalence' is used, either two OG-/oWFN-files must be" << endl;
        cerr << "       \t given or (in case of BDD-equivalence) exactly two oWFNs have to be" << endl;
        cerr << "       \t entered and their BDDs must have been computed before." << endl;
        cerr << "       \t Enter \"fiona --help\" for more information.\n" << endl;
        exit(EC_BAD_CALL);
    }

    if ((parameters[P_REDUCE] || parameters[P_IG] || parameters[P_OG]) &&
        !parameters[P_SYNTHESIZE_PARTNER_OWFN] && netfiles.size() == 0) {
        cerr << "Error: \t No oWFNs are given." << endl;
        cerr << "       \t Enter \"fiona --help\" for more information.\n" << endl;
        exit(EC_BAD_CALL);
    }

    if ( parameters[P_SYNTHESIZE_PARTNER_OWFN] && (parameters[P_IG] || parameters[P_OG]) &&
         ogfiles.size() == 0 && netfiles.size() == 0) {
        cerr << "Error: \t No oWFNs or OGs are given." << endl;
        cerr << "       \t Enter \"fiona --help\" for more information.\n" << endl;
        exit(EC_BAD_CALL);
    }

    if ((parameters[P_ADAPTER] || parameters[P_SMALLADAPTER]) && netfiles.size() < 2) {
        cerr << "Error: \t No oWFNs are given." << endl;
        cerr << "       \t Enter \"fiona --help\" for more information.\n" << endl;
        exit(EC_BAD_CALL);
    }

    if (parameters[P_PNG] && netfiles.size() == 0 && ogfiles.size() == 0) {
        cerr << "Error: \t No oWFNs or OGs are given." << endl;
        cerr << "       \t Enter \"fiona --help\" for more information.\n" << endl;
        exit(EC_BAD_CALL);
    }

    if (parameters[P_MATCH] && (netfiles.size() < 1 || ogfiles.size() < 1)) {
        cerr << "Error: \t At least one oWFN and one OG must be given for matching!\n" << endl;
        cerr << "       \t Enter \"fiona --help\" for more information.\n" << endl;
        exit(EC_BAD_CALL);
    }

    if (parameters[P_PRODUCTOG] && ogfiles.size() < 2) {
        cerr << "Error: \t Give at least two OGs to build their product!\n" << endl;
        cerr << "       \t Enter \"fiona --help\" for more information.\n" << endl;
        exit(EC_BAD_CALL);
    }

    if (parameters[P_SIMULATES] && ((netfiles.size() + ogfiles.size()) != 2)) {
        cerr << "Error: \t If option -t simulation is used, exactly two OG/oWFN files must be entered\n" << endl;
        cerr << "       \t Enter \"fiona --help\" for more information.\n" << endl;
        exit(EC_BAD_CALL);
    }

    if (parameters[P_READ_OG] && ogfiles.size() < 1) {
        cerr << "Error: \t If option -t readOG is used, at least one OG file must be entered\n" << endl;
        cerr << "       \t Enter \"fiona --help\" for more information.\n" << endl;
        exit(EC_BAD_CALL);
    }

    if (options[O_EVENT_USE_MAX] == false && options[O_MESSAGES_MAX] == false) {
        cerr << "Error: \t if no limit for using events is given, you must specify a message bound via option -m " << endl;
        cerr << "       \t Enter \"fiona --help\" for more information.\n" << endl;
        exit(EC_BAD_CALL);
    }

    if (options[O_BDD] && parameters[P_OG] == false && parameters[P_EX] == false) {
        cerr << "Warning: \t computing IG -- BDD option ignored\n" << endl;
        options[O_BDD] = false;
    }

    if (ogfiles.size() == 0 && netfiles.size() == 0 && parameters[P_PV]) {
        cerr << "Error:\tNo OGs or oWFNs given. Public View Service Automaton cannot be generated." << endl
             << "\tEnter \"fiona --help\" for more information.\n" << endl;
        exit(EC_BAD_CALL);
    }

    if (!parameters[P_PV] && options[O_PV_MULTIPLE_DEADLOCKS]) {
        cerr << "not computing a public view - multiple deadlocks option ignored\n" << endl;
        options[O_PV_MULTIPLE_DEADLOCKS] = false;
    }

    if (!parameters[P_PRODUCTOG] && !parameters[P_SIMULATES] &&
        !parameters[P_EX] && options[O_OUTFILEPREFIX] && (ogfiles.size() > 1 || netfiles.size() > 1)) {
        cerr << "Error: \t The output option cannot be used if multiple output files are to be created!\n" << endl;
        exit(EC_BAD_CALL);
    }

    if (parameters[P_COVER] &&

        // possible options
        (!parameters[P_OG] && !parameters[P_MATCH]) &&

        // not possible options due to lack of definition
        (parameters[P_IG] || parameters[P_MATCH] || parameters[P_MINIMIZE_OG] ||
         parameters[P_PV] || parameters[P_SYNTHESIZE_PARTNER_OWFN] ||
         parameters[P_PRODUCTOG] || parameters[P_SIMULATES] || parameters[P_EX] ||
         parameters[P_EQ_R] || parameters[P_ADAPTER] || parameters[P_SMALLADAPTER] ||
         parameters[P_GASTEX] || parameters[P_COUNT_SERVICES] || parameters[P_REDUCE] ||
         parameters[P_NORMALIZE] || parameters[P_PARTNER_TEST] ||
         parameters[P_RESPONSIVE] || parameters[P_TEX] || parameters[P_DISTRIBUTED] ||
         parameters[P_AUTONOMOUS] || parameters[P_REDUCE_LEVEL] ||
         parameters[P_USE_CRE] || parameters[P_USE_RBS] || parameters[P_USE_EAD] ||
         parameters[P_REPRESENTATIVE] || parameters[P_SINGLE])) {

        cerr << "Error: \t Check for coverability is only available for computing operating guidelines";
        cerr << " and for matching!\n" << endl;

        exit(EC_BAD_CALL);
    }
}


FileType getFileType(const std::string& fileName) {
    unsigned int owfnHits = 0;
    unsigned int ogHits   = 0;
    unsigned int dotHits  = 0;

    ifstream fileStream(fileName.c_str());
    if (!fileStream) {
        cerr << "Error:\tCould not open '" << fileName << "' for reading.\n"
             << endl;
        exit(EC_FILE_ERROR);
    }

    string line = "";
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
        if (contains(line, ": blue")) ++ogHits;
        if (contains(line, "->")) ++ogHits;

        if (contains(line, "digraph")) ++dotHits;
        if (contains(line, "label=")) ++dotHits;
        if (contains(line, "graph")) ++dotHits;
        if (contains(line, "color=")) ++dotHits;
        if (contains(line, "pos=")) ++dotHits;
        if (contains(line, "fontcolor=")) ++dotHits;
    }

    if ((owfnHits > ogHits) && (owfnHits > dotHits)) {
        return FILETYPE_OWFN;
    }

    else if ((ogHits > owfnHits) && (ogHits > dotHits)) {
        return FILETYPE_OG;
    }

    else if ((dotHits > owfnHits) && (dotHits > ogHits)) {
        return FILETYPE_DOT;
    }

    return FILETYPE_UNKNOWN;
}

bool contains(const std::string& hostString, const std::string& subString) {
    return hostString.find(subString, 0) != std::string::npos;
}
