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
 * \file    options.h
 *
 * \brief   options management for fiona
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 */

#ifndef OPTIONS_H
#define OPTIONS_H

#include "mynew.h"
#include "getopt.h"
#include <map>
#include <list>
#include <cstdio>
#include <string>


typedef enum {
    O_HELP,                     //!< -h, show the help output
    O_VERSION,                  //!< -v, show the current fiona version
    O_DEBUG,                    //!< -d, show additional debug information
    O_GRAPH_TYPE,               //!< -t, set mode of operation
    O_MESSAGES_MAX,             //!< -m, set a message maximum
    O_EVENT_USE_MAX,            //!< -e, set an events maximum
    O_READ_EVENTS,              //!< -E, read the event occurrences from the oWFN
    O_CALC_REDUCED_IG,          //!< -r, reduce the ig
    O_CALC_ALL_STATES,          //!< -R, calculate all states
    O_SHOW_NODES,               //!< -s, show additional information in nodes
    O_BDD,                      //!< -b, BDD-representation of OG is computed
    O_OTF,                      //!< -B, BDD-representation of OG is computed on the fly
    O_OUTFILEPREFIX,            //!< -o, set an outputfile prefix
    O_NOOUTPUTFILES,            //!< -Q, produce no output files
    O_PV_MULTIPLE_DEADLOCKS,    //!< -M, use multiple deadlocks in pv generation
    O_PARAMETER,                //!< -p, use additional parameters
    O_ADAPTER_FILE,             //!< -a, file with adapter rules
} possibleOptions;


typedef enum {
    P_OG,                       //!< compute OG
    P_IG,                       //!< compute IG
    P_MATCH,                    //!< match an oWFN against an OG
    P_MINIMIZE_OG,              //!< minimize a given OG
    P_PV,                       //!< compute Public View Service Automaton
    P_SYNTHESIZE_PARTNER_OWFN,  //!< compute an oWFN from the IG/OG
    P_PRODUCTOG,                //!< compute the product of 2 OGs
    P_SIMULATES,                //!< Check whether an OG simulates another
    P_EX,                       //!< check equivalence (former exchangability) of two OGs 
    P_EQ_R,                     //!< equivalence of two graphs (with -R and without -R), but the two graphs are computed automatically
    P_ADAPTER,                  //!< compute adapter (with OG) for given services
    P_SMALLADAPTER,             //!< compute small adapter (with IG) for given services
    P_PNG,                      //!< graphical output of all given oWFNs
    P_GASTEX,                   //!< read an annotated dot file and create a gastex tex file
    P_CHECK_FALSE_NODES,        //!< check false annotations in og
    P_REMOVE_FALSE_NODES,        //!< remove nodes with false annotations in og
    P_CHECK_ACYCLIC,            //!< Check whether an OG is acyclic 
    P_COUNT_SERVICES,           //!< Compute the number of services characterized by an OG
    P_READ_OG,                  //!< only read an OG from a file
    P_REDUCE,                   //!< reduce given oWFNs 
    P_NORMALIZE,                //!< normalize given oWFNs
    P_PARTNER_TEST,             //!< first synthesize a partner, then match it with the og. 
    P_RESPONSIVE,               //!< generate responsive partners (IG/OG) using TSCCs

    P_SHOW_BLUE_NODES,          //!< show blue nodes only (empty node not shown though)
    P_SHOW_EMPTY_NODE,          //!< show all blue nodes (including empty node)
    P_SHOW_RED_NODES,           //!< show blue and red nodes (empty node not shown)
    P_SHOW_ALL_NODES,           //!< show red, blue, and empty nodes
    P_SHOW_STATES_PER_NODE,     //!< show all states in a node (aka "knowledge")
    P_SHOW_DEADLOCKS_PER_NODE,  //!< show deadlock states only

    P_NOPNG,                    //!< do not create a PNG output
    P_NODOT,                    //!< do not create any dot related output
    P_TEX,                      //!< create a gastex tex file in some other mode
    P_DIAGNOSIS,                //!< diagnosis information
    P_DISTRIBUTED,              //!< check for distributed controllability
    P_AUTONOMOUS,               //!< check for autonomous controllability
    P_REDUCE_LEVEL,             //!< set reduction level of mode -t reduce
    P_USE_CRE,                  //!< Use "combine receiving events" for IG-reduction
    P_USE_RBS,                  //!< Use "receive before sending" for IG-reduction
    P_USE_EAD,                  //!< use "early detection" rule for IG-reduction
    P_REPRESENTATIVE,           //!< state reduction in node using representatives
    P_SINGLE                    //!< state reduction in node using single marking as representative
} possibleParameters;

// EXIT CODE CONSTANTS
const unsigned short int EC_FILE_ERROR = 4;                 //!< An error occured with a file stream.
const unsigned short int EC_NO_RULES_FILE = 4;              //!< Adapter: No rules file found.
const unsigned short int EC_NO_CUDD_FILE = 4;               //!< No cudd file found.
const unsigned short int EC_CAPACITY_EXCEEDED = 4;          //!< oWFN: Place capacity exceeded.
const unsigned short int EC_BDD_ERROR = 1;                  //!< BDD: An error occured.
const unsigned short int EC_MEMORY_EXHAUSTED = 5;           //!< "new" was not possible -> memory is exhausted.
const unsigned short int EC_UNREACHABLE_LINE = 1;           //!< An unreachable line in code was reached. 
const unsigned short int EC_DOT_ERROR = 1;                  //!< Dot refused to work properly.
const unsigned short int EC_BAD_CALL = 2;                   //!< Fiona was called in an inappropriate way: Wrong mode/parameters.


// some file names and pointers (in options.cc)
// different modes controlled by command line (in options.cc)

extern std::map<possibleOptions, bool> options;
extern std::map<possibleParameters, bool> parameters;

extern std::string outfilePrefix;
extern std::string adapterRulesFile;

extern unsigned int messages_manual;
extern int events_manual;

extern bool OGfirst;

extern int bdd_reordermethod;

extern std::ostream* log_output;

void parse_command_line(int argc, char* argv[]);

enum FileType {FILETYPE_OWFN, FILETYPE_OG, FILETYPE_DOT, FILETYPE_UNKNOWN};
extern FileType getFileType(const std::string& fileName);

extern bool contains(const std::string& hostString, const std::string& subString);

#endif

