/*****************************************************************************\
 * Copyright 2005, 2006, 2007 Niels Lohmann, Christian Gierds                *
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
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          Christian Gierds <gierds@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nielslohmann $
 *
 * \since   2005/10/18
 *
 * \date    \$Date: 2007/06/07 09:17:43 $
 *
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.56 $
 */





#ifndef OPTIONS_H
#define OPTIONS_H





/******************************************************************************
 * Headers
 *****************************************************************************/

#include <map>
#include <set>
#include <list>
#include <string>
#include <iostream>

using std::map;
using std::set;
using std::list;
using std::istream;
using std::ostream;
using std::string;





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
   O_DEBUG,		///< set the debug level
   O_REDUCTION,		///< set level of structural reduction
   O_CHOR,		///< parse a choreography file (BPEL4Chor)
   O_WSDL		///< parse a WSDL file
} possibleOptions;





/*!
 * \brief enumeration of the possible modes
 */
typedef enum
{
  M_AST = 1,		///< abstract syntax tree
  M_PRETTY,			///< BPEL pretty-printer
  M_PETRINET,		///< Petri net
  M_CHOREOGRAPHY,	///< consistency (parse two input files)
  M_CFG,			///< control flow analysis
  M_VIS				///< BPEL process visualization
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
  F_INA,		///< INA Petri net
  F_SPIN,		///< SPIN Petri net
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
  P_VARIABLES,		///< removes all variables from the model
  P_STANDARDFAULTS,	///< only user-defined faults can occur
  P_FHFAULTS,		///< activities inside the FH throw no faults
  P_COMMUNICATIONONLY,	///< creates the smallest possible net
  P_XOR,		///< use XOR as transition condition
  P_LOOPCOUNT,		///< use loop counters
  P_LOOPCONTROL,	///< add communication to loops
  P_NOINTERFACE,	///< do not draw an interface
  P_DEADLOCKTEST,	///< removes the deadlock from the final place
  P_DATA		///< creates multiple nets (Diplomarbeit Thomas Heidinger)   
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
extern set<string> inputfiles;
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
