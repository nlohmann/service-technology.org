/*****************************************************************************
 * Copyright 2005, 2006 Niels Lohmann, Peter Massuthe, Daniela Weinberg,     *
 *                      Jan Bretschneider, Christian Gierds, Dennis Reinert  *
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
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 */

#ifndef OPTIONS_H
#define OPTIONS_H

#include "mynew.h"
#include <map>
#include <cstdio>
#include "getopt.h"

typedef enum
{
   	O_HELP,
	O_VERSION,
	O_DEBUG,
	O_OWFN_NAME,
	O_GRAPH_TYPE,	//!< OG or IG
	O_MESSAGES_MAX,
	O_EVENT_USE_MAX,
	O_CALC_REDUCED_IG,
	O_SHOW_NODES,	//!<
	O_CALC_ALL_STATES,
	O_BDD,
	O_EX,
	O_MATCH
} possibleOptions;

typedef enum
{
	P_IG,			//!< compute IG  
	P_OG,			//!< compute OG
	P_SHOW_ALL_NODES,
	P_SHOW_BLUE_NODES_ONLY,
	P_SHOW_NO_RED_NODES,
	P_SHOW_EMPTY_NODE,
	P_SHOW_STATES_PER_NODE
} possibleParameters;


// some file names and pointers (in options.cc)
// different modes controlled by command line (in options.cc)

extern std::map<possibleOptions,    bool> options;
extern std::map<possibleParameters, bool> parameters;

extern char * netfile;
extern std::string ogfile;
extern unsigned int messages_manual;
extern int events_manual;
extern int bdd_reordermethod;


/// pointer to input stream
//extern std::istream * input;
/// pointer to output stream
//extern std::ostream * output;
/// pointer to log stream
extern std::ostream * log_output;

void parse_command_line(int argc, char* argv[]);

#endif

