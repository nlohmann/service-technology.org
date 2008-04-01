/*****************************************************************************
 * Copyright 2005, 2006, 2007 Peter Massuthe, Daniela Weinberg,              *
 *           Niels Lohmann, Jan Bretschneider, Christian Gierds,             *
 *           Martin Znamirowski                                              *
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
 * \file    options.h
 *
 * \brief   options management for fiona
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universitt zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 */

#ifndef OPTIONS_H
#define OPTIONS_H

#include "mynew.h"
#include "getopt.h"
#include <map>
#include <list>
#include <cstdio>


typedef enum {
    O_HELP,                     //!< -h
    O_VERSION,                  //!< -v
    O_DEBUG,                    //!< -d
    O_GRAPH_TYPE,               //!< -t
    O_MESSAGES_MAX,             //!< -m
    O_EVENT_USE_MAX,            //!< -e
    O_CALC_REDUCED_IG,          //!< -r
    O_SHOW_NODES,               //!< -s
    O_CALC_ALL_STATES,          //!< -R
    O_BDD,                      //!< -b,  BDD-representation of OG is computed
    O_OTF,                      //!< -B,  BDD-representation of OG is computed on the fly
    O_EX,                       //!< 
    O_EQ_R,                     //!< equivalence of two graphs (with -R and without -R), but the two graphs are computed automatically
    O_MATCH,                    //!< 
    O_PRODUCTOG,                //!< 
    O_SIMULATES,                //!<
    O_SIMULATES_WITH_COV,       //!<
    O_FILTER,                   //!< 
    O_OUTFILEPREFIX,            //!< 
    O_NOOUTPUTFILES,            //!< produce no output files
    O_SYNTHESIZE_PARTNER_OWFN,  //!< compute an oWFN from the IG/OG
    O_DISTRIBUTED,              //!< check for distributed controllability
    O_COUNT_SERVICES,           //!< 
    O_CHECK_ACYCLIC,            //!< 
    O_PNG,                      //!< graphical output of all given oWFNs
    O_REDUCE,                   //!< reduce given oWFNs 
    O_DIAGNOSIS,                //!< diagnosis information
    O_PV_MULTIPLE_DEADLOCKS,    //!< 
    O_ADAPTER_FILE,             //!< file with adapter rules
    O_AUTONOMOUS                //!< check for autonomous controllability
} possibleOptions;


typedef enum {
    P_IG,                       //!< compute IG
    P_OG,                       //!< compute OG
    P_MINIMIZE_OG,              //!< minimize a given OG
    P_PV,                       //!< compute Public View Service Automaton
    P_ADAPTER,                  //!< compute adapter for given services
    P_SHOW_BLUE_NODES,          //!< show blue nodes only (empty node not shown though)
    P_SHOW_EMPTY_NODE,          //!< show all blue nodes (including empty node)
    P_SHOW_RED_NODES,           //!< show blue and red nodes (empty node not shown)
    P_SHOW_ALL_NODES,           //!< show red, blue, and empty nodes
    P_SHOW_STATES_PER_NODE,     //!< show all states in a node (aka "knowledge")
    P_SHOW_DEADLOCKS_PER_NODE,  //!< show deadlock states only
    P_NOPNG,                    //!< do not create a PNG output
    P_TEX,                      //!< create a gastex tex file in some other mode
    P_READ_OG,                  //!< only read an OG from a file
    P_GASTEX,                   //!< read an annotated dot file and create a gastex tex file
    P_REPRESENTATIVE,	      	//!< state reduction in node using representatives
    P_SINGLE,                   //!< state reduction in node using single marking as representative
    P_USE_CRE,                  //!< Use "combine receiving events" for IG-reduction
    P_USE_RBS,                  //!< Use "receive before sending" for IG-reduction
    P_CHECK_FALSE_ANNOS,	    //!< check false annotations in og
    P_REMOVE_FALSE_ANNOS		//!< remove nodes with false annotations in og
} possibleParameters;


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

