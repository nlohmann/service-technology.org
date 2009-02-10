/*****************************************************************************\
  UML2oWFN -- Translating UML2 Activity Diagrams to Petri nets
  Copyright (C) 2007, 2008  Dirk Fahland <dirk.fahland@service-technolog.org>,
                            Martin Znamirowski <znamirow@informatik.hu-berlin.de>

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

/*!
 * \file    options.h
 *
 * \brief   evaluation of command-line options
 *
 * \author  Martin Znamirowski <znamirow@informatik.hu-berlin.de>,
 *          last changes of: \$Author: znamirow $
 *
 * \since   2008/05/07
 *
 * \date    \$Date: 2008/05/09 12:38:16 $
 *
 * \note    This file is part of the tool UML2OWFN and was created at the
 *          Humboldt-Universität zu Berlin. See
 *
 * \version \$Revision: 1.00 $
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
#include <cstdlib>
#include <climits>

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
   O_HELP,		    ///< show help screen
   O_VERSION,		  ///< show version information
   O_INPUT,		    ///< read input file
   O_OUTPUT,		  ///< write output file
   O_FORMAT,		  ///< choose output file format (see #possibleFormats)
   O_PARAMETER,		///< set various parameters (see #possibleParameters)
   O_DEBUG,		    ///< set the debug level
   O_REDUCTION,   ///< set level of structural reduction
   O_ANALYSIS,      ///< set the analysis task
   O_ROLECUT,      	///< automatically cut away all roles without start nodes
   O_ROLEEXCLUSIVE,	///< cut away all roles matching exactly a given string
   O_ROLECONTAINS,  ///< cut away all roles containing a given string
   O_SKIP_BY_FILTER ///< filter processes according to processCharacteristics
} possibleOptions;


/*!
 * \brief enumeration of the possible output file formats
 */
typedef enum
{
  F_LOLA,   ///< LoLA Petri net
  F_OWFN,   ///< Fiona open workflow net
  F_DOT,    ///< Graphviz dot format
  F_PEP,    ///< low-level PEP notation
  F_TPN,    ///< Woflan TPN format
  F_APNN,   ///< Abstract Petri Net Notation
  F_INA,    ///< INA Petri net
  F_SPIN,   ///< SPIN Petri net
  F_PNML,   ///< Petri Net Markup Language
  F_INFO,   ///< BPEL2oWFN information file
  F_BOM_XML,  ///< BOM .xml-file
  F_NONE    ///< place holder for "no format"
} possibleFormats;



/*!
 * \brief enumeration of the possible parameters
 */
typedef enum
{
  P_LOG,                ///< write a log file about the translation
  P_KEEP_UNCONN_PINS,   ///< keep unconnected pins upon translation
  P_TASKFILE,   ///< insist on writing task files
  P_ANONYMIZE,  ///< anonymize the process output (re-enumerate places)
  P_CTL         ///< generate CTL properties for analysis (if applicable)
} possibleParameters;


/*!
 * \brief enumeration of the possible analysis switches
 */
typedef enum
{
  A_SOUNDNESS,    ///< analyze soundness
  A_STOP_NODES,   ///< distinguishing stop nodes from end nodes
  A_DEADLOCKS,    ///< check for absence of deadlocks
  A_SAFE,         ///< check for safeness
  A_TERM_IGNORE_DATA,  ///< remove the output pinsets when checking soundness
  A_TERM_WF_NET,       ///< translate into a workflow net for soundness analysis
  A_TERM_ORJOIN,       ///< analyze soundness by assuming an implicit OR-join
  A_MAX
} possibleAnalysis;

typedef map<possibleAnalysis, bool> analysis_t;


/*!
 * \brief enumeration of the process characteristics for filtering
 */
typedef enum
{
  PC_NORMAL,       ///< no specifics
  PC_OVERLAPPING,  ///< overlapping pinsets
  PC_PIN_MULTI,           ///< non-normal pin multiplicities
  PC_PIN_MULTI_NONMATCH,  ///< non-matching pin multiplicities
  PC_EMPTY,             ///< empty process
  PC_UNCONNECTED,       ///< unconnected process
  PC_TRIVIAL_INTERFACE  ///< process has a trivial interface
} processCharacteristics;

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
extern map<possibleOptions,    bool> options;
extern map<possibleParameters, bool> parameters;
extern map<possibleFormats,    bool> formats;
extern map<possibleFormats,  string> suffixes;

extern int frontend_debug;
extern int frontend__flex_debug;
extern FILE *frontend_in;





#endif
