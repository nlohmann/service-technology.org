/*****************************************************************************\
 * Copyright 2005, 2006 Niels Lohmann, Christian Gierds                      *
 *                                                                           *
 * This file is part of GNU BPEL2oWFN.                                       *
 *                                                                           *
 * GNU BPEL2oWFN is free software; you can redistribute it and/or modify it  *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * GNU BPEL2oWFN is distributed in the hope that it will be useful, but      *
 * WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General  *
 * Public License for more details.                                          *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with GNU BPEL2oWFN; see file COPYING. if not, write to the Free Software  *
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA. *
\*****************************************************************************/

/*!
 * \file    options.h
 *
 * \brief   evaluation of command-line options
 *
 * \author  responsible: Christian Gierds <gierds@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nielslohmann $
 *
 * \since   2005/10/18
 *
 * \date    \$Date: 2006/12/20 11:50:18 $
 *
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.42 $
 */





#ifndef OPTIONS_H
#define OPTIONS_H





/******************************************************************************
 * Headers
 *****************************************************************************/

#include <map>
#include <list>

using namespace std;





/******************************************************************************
 * Data structures
 *****************************************************************************/

/*!
 * \brief enumeration of the possible options
 */
typedef enum
{
   O_HELP,		///< show help screen
   O_VERSION,		///< show version information
   O_INPUT,		///< read input file
   O_OUTPUT,		///< write output file
   O_MODE,		///< choose mode (see #possibleModi)
   O_LOG,		///< write a log-file
   O_FORMAT,		///< choose output file format (see #possibleFormats)
   O_PARAMETER,		///< set various parameters (see #possibleParameters)
   O_DEBUG		///< set the debug level
} possibleOptions;





/*!
 * \brief enumeration of the possible modes
 */
typedef enum
{
  M_AST = 1,		///< abstract syntax tree
  M_PRETTY,		///< BPEL pretty-printer
  M_PETRINET,		///< Petri net
  M_CONSISTENCY,	///< consistency (parse two input files)
  M_CFG			///< control flow analysis
} possibleModi;





/*!
 * \brief enumeration of the possible output file formats
 */
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





/*!
 * \brief enumeration of the possible parameters
 */
typedef enum
{
  P_REDUCE,		///< structural reduction rules
  P_VARIABLES,		///< removes all variables from the model
  P_STANDARDFAULTS,	///< only user-defined faults can occur
  P_FHFAULTS,		///< activities inside the FH throw no faults
  P_COMMUNICATIONONLY,	///< creates the smallest possible net
//  P_TRED,		///< use transitive reduction
  P_XOR,		///< use XOR as transition condition
  P_WSBPEL,		///< use new (WS-BPEL) Petri net patterns
  P_LOOPCOUNT,		///< use loop counters
  P_LOOPCONTROL		///< add communication to loops
} possibleParameters;





/******************************************************************************
 * External functions
 *****************************************************************************/

// from options.cc

extern void closeOutput(ostream *file);
extern void parse_command_line(int argc, char* argv[]);
extern ostream *openOutput(string name);





/******************************************************************************
 * External variables
 *****************************************************************************/

// from options.cc

extern bool createOutputFile;
extern string filename;
extern list<string> inputfiles;
extern string output_filename;
extern string log_filename;
extern istream *input;
extern ostream *output;
extern ostream *log_output;
extern possibleModi modus;
extern map<possibleOptions,    bool> options;
extern map<possibleParameters, bool> parameters;
extern map<possibleFormats,    bool> formats;
extern map<possibleFormats,  string> suffixes;

extern int frontend_debug;
extern int frontend__flex_debug;
extern FILE *frontend_in;





#endif
