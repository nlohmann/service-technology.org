/*!
 * \file helpers.h
 *
 * \brief Helper functions (interface)
 *
 * This file defines several small helper functions that do not belong to any
 * other file.
 * 
 * \author  
 *          - Niels Lohmann <nlohmann@informatik.hu-berlin.de>
 *          
 * \date    2005-11-11
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \version
 *          - 2005-11-11 (nlohmann) Initial version.
 */





#ifndef HELPERS_H
#define HELPERS_H

#include <set>
#include <string>
#include "petrinet.h"

using namespace std;

// some file names and pointers (in main.cc)

/// Filename of input file.
extern std::string filename;
/// Filename of dot output file
extern std::string dot_filename;
/// Pointer to dot output file
extern std::ostream * dot_output;


// different modes controlled by command line (in main.cc)

/// print the Petri Net
extern bool mode_petri_net;
/// simplify Petri Net
extern bool mode_simplify_petri_net;
/// paint Petri Net with dot
extern bool mode_dot_petri_net;
/// paint Petri Net with dot and output to file
extern bool mode_dot_2_file;
/// pretty printer
extern bool mode_pretty_printer;
/// print AST
extern bool mode_ast;


/// debug level (in debug.cc)
extern int debug_level;


/* defined by Bison */
extern int yydebug;

/* defined by flex */
extern int yylineno;
extern char *yytext;
extern int yy_flex_debug;
extern FILE *yyin;


// to avoid compile errors
class Node;


/// Returns the union of two sets of Petri net nodes.
set<Node *> setUnion(set<Node *> a, set<Node *> b);

/// Converts int to string.
string intToString(int i);

/// Prints help for command line arguments
void print_help();

/// Evaluates command line arguments
void parse_command_line(int argc, char * argv[]);

#endif
