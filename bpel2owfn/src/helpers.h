/*****************************************************************************\
 * Copyright 2005, 2006 Niels Lohmann, Christian Gierds, Dennis Reinert      *
 *                                                                           *
 * This file is part of BPEL2oWFN.                                           *
 *                                                                           *
 * BPEL2oWFN is free software; you can redistribute it and/or modify it      *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * BPEL2oWFN is distributed in the hope that it will be useful, but WITHOUT  *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
 * more details.                                                             *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with BPEL2oWFN; if not, write to the Free Software Foundation, Inc., 51   *
 * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.                      *
\****************************************************************************/

/*!
 * \file helpers.h
 *
 * \brief Helper functions (interface)
 *
 * This file defines several small helper functions that do not belong to any
 * other file.
 * 
 * \author  
 *          - last changes by: \$Author: nlohmann $
 *          
 * \date    
 *          - last changed: \$Date: 2006/01/26 09:11:04 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 */

#ifndef HELPERS_H
#define HELPERS_H

#include <set>
#include <string>
#include "petrinet.h"
#include "exception.h"	// exception handling
#include "check-symbols.h"
#include "bpel2owfn.h"

using namespace std;

// some file names and pointers (in main.cc)

/// Filename of input file.
extern std::string filename;

/// Filename of dot output file
extern std::string dot_filename;
/// Pointer to dot output file
extern std::ostream * dot_output;

/// Filename of APPN output file
extern std::string appn_filename;
/// Pointer to APPN output file
extern std::ostream * appn_output;

/// Filename of LoLA output file
extern std::string lola_filename;
/// Pointer to LoLA output file
extern std::ostream * lola_output;

/// Filename of oWFN output file
extern std::string owfn_filename;
/// Pointer to oWFN output file
extern std::ostream * owfn_output;

/// Filename of PN info file
extern std::string info_filename;
/// Pointer to PN info file
extern std::ostream * info_output;


// different modes controlled by command line (in main.cc)

/// read from file ?
extern bool mode_file;
/// print the Petri Net
extern bool mode_petri_net;
/// simplify Petri Net
extern bool mode_simplify_petri_net;
/// output Petri Net for APPN
extern bool mode_appn_petri_net;
/// output Petri Net for APPN to file
extern bool mode_appn_2_file;
/// output Petri Net for LoLA
extern bool mode_lola_petri_net;
/// output Petri Net for LoLA to file
extern bool mode_lola_2_file;
/// output Petri Net for oWFN
extern bool mode_owfn_petri_net;
/// output Petri Net for oWFN to file
extern bool mode_owfn_2_file;
/// paint Petri Net with dot
extern bool mode_dot_petri_net;
/// paint Petri Net with dot and output to file
extern bool mode_dot_2_file;
/// pretty printer
extern bool mode_pretty_printer;
/// print AST
extern bool mode_ast;
/// print the "low level" Petri Net
extern bool mode_low_level_petri_net;

/// debug level (in debug.cc)
extern int debug_level;


/* defined by Bison */
extern int yydebug;

/* defined by flex */
extern int yy_flex_debug;
extern FILE *yyin;


// to avoid compile errors
class Node;

/// Returns the union of two sets of Petri net nodes.
set<Node *> setUnion(set<Node *> a, set<Node *> b);

/// Converts int to string.
string intToString(int i);

/// Formats a debug information.
string pos(const char *file, int line, const char *function);

/// Prints help for command line arguments
void print_help();

/// Prints version information
void print_version(std::string name);

/// Evaluates command line arguments
void parse_command_line(int argc, char * argv[]);

/// Calls #cleanup(), then exits
void error();

/// Prints information about #Exception e, calls #cleanup() and exits 
void error(Exception e);

/// Close all open files, delete all pointers.
void cleanup();

#endif
