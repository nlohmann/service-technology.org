/*****************************************************************************\
 * Copyright 2005, 2006 Niels Lohmann, Christian Gierds, Dennis Reinert      *
 *                                                                           *
 * This file is part of BPEL2oWFN.                                           *
 *                                                                           *
 * BPEL2oWFN is free software; you can redistribute it and/or modify it      *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or(at your     *
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
\*****************************************************************************/

/*!
 * \file options.h
 *
 * \brief 
 *
 * \author
 *          - responsible: Christian Gierds <gierds@informatik.hu-berlin.de>
 *          - last changes of: \$Author: nlohmann $
 *
 * \date
 *          - created: 2005/10/18
 *          - last changed: \$Date: 2006/09/28 15:30:51 $
 *
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.30 $
 */





#ifndef OPTIONS_H
#define OPTIONS_H





/******************************************************************************
 * Headers
 *****************************************************************************/

#include <map>
#include <list>

#include "getopt.h"

using namespace std;





/******************************************************************************
 * Data structures
 *****************************************************************************/

/// Enumeration of the possible options
typedef enum
{
   O_HELP,		///< show help screen
   O_VERSION,		///< show version information
   O_INPUT,		///< read input file
   O_OUTPUT,		///< write output file
   O_MODE,		///< choose mode (see #possibleModi)
   O_BPEL2PN,		///< emulate the tool BPEL2PN
   O_LOG,		///< write a log-file
   O_FORMAT,		///< choose output file format (see #possibleFormats)
   O_PARAMETER,		///< set various parameters (see #possibleParameters)
   O_DEBUG		///< set the debug level
} possibleOptions;





/// Enumeration of the possible modes
typedef enum
{
  M_AST = 1,		///< abstract syntax tree
  M_PRETTY,		///< BPEL pretty-printer
  M_PETRINET,		///< Petri net
  M_CONSISTENCY,	///< consistency (parse two input files)
  M_CFG			///< control flow analysis
} possibleModi;





/// Enumeration of the possible output file formats
typedef enum
{
  F_LOLA,		///< LoLA Petri net
  F_OWFN,		///< Fiona open workflow net
  F_DOT,		///< Graphviz dot format
  F_PEP,		///< low-level PEP notation
  F_APNN,		///< Abstract Petri Net Notation
  F_INFO,		///< BPEL2oWFN information file
  F_PNML,		///< Petri Net Markup Language
  F_TXT,		///< ASCII output (for the abstract syntax tree)
  F_XML			///< XML output (for pretty-printed BPEL)
} possibleFormats;





/// Enumeration of the possible parameters (mostly Petri net-related)
typedef enum
{
  P_SIMPLIFY,		///< structural reduction rules
  P_FINALLOOP,		///< live-lock "well-formed" systems
  P_NOVARIABLES,	///< removes all variables from the model
  P_NOSTANDARDFAULTS,	///< only user-defined faults can occur
  P_NOFHFAULTS,		///< activities inside the FH throw no faults
  P_COMMUNICATIONONLY,	///< creates the smallest possible net
  P_CYCLICWHILE,	///< use the original <while> pattern
  P_CYCLICEH,		///< use the original <eventHandlers> pattern
  P_TRED,		///< use transitive reduction
  P_XOR			///< use XOR as transition condition
} possibleParameters;





/******************************************************************************
 * External variables
 *****************************************************************************/

// some file names and pointers (in options.cc)
extern void parse_command_line(int argc, char* argv[]);


/// Filename of input file
extern string filename;
/// Filename of first input file
extern list <string> inputfiles;
/// Filename of input file
extern string output_filename;
/// Filename of log file
extern string log_filename;

/// pointer to input stream
extern istream * input;
/// pointer to output stream
extern ostream * output;
/// pointer to log stream
extern ostream * log_output;

extern bool createOutputFile;

// different modes controlled by command line (in options.cc)

extern possibleModi modus;

extern map<possibleOptions,    bool> options;
extern map<possibleParameters, bool> parameters;
extern map<possibleFormats,    bool> formats;
// suffixes are defined in parse_command_line();
extern map<possibleFormats,  string> suffixes;


/* defined by Bison */
extern int yydebug;

/* defined by flex */
extern int yy_flex_debug;
extern FILE *yyin;

// returns an open file pointer
extern ostream * openOutput(string name);
extern void closeOutput(ostream * file);

#endif
