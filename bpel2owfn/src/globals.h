/*****************************************************************************\
 * Copyright 2007 Niels Lohmann                                              *
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
 * \file    globals.h
 *
 * \brief   global varialbes
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nielslohmann $
 * 
 * \since   2007/03/05
 *
 * \date    \$Date: 2007/05/02 15:22:57 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.20 $
 */





#ifndef GLOBALS_H
#define GLOBALS_H

#include <set>
#include <string>
#include <map>

#include "ast-config.h"
#include "options.h"

using std::set;
using std::string;
using std::map;

class WSDL;
class ASTE;
class CFGBlock;
class Choreography;




/*!
 * \brief process information
 */
struct s_process_information
{
  unsigned int basic_activities;
  unsigned int structured_activities;
  unsigned int scopes;
  unsigned int event_handlers;
  unsigned int fault_handlers;
  unsigned int termination_handlers;
  unsigned int compensation_handlers;
  unsigned int variables;
  unsigned int links;
  unsigned int implicit_activities;
  unsigned int implicit_handlers;
};





/*!
 * \brief WSDL information
 */
struct s_wsdl_information
{
  unsigned int imports;
  unsigned int types;
  unsigned int messages;
  unsigned int portTypes;
  unsigned int operations;
  unsigned int bindings;
  unsigned int services;
  unsigned int properties;
  unsigned int partnerLinkTypes;
};





/*!
 * \brief namespace of all global variables
 */
namespace globals
{
  /// prefix of the current scope (read by the stop pattern and used to direct
  /// failures to the correct fault_in place)
  extern string currentScope;

  /// maps the name of a node in the CFG to an actual block pointer
  extern map <string, CFGBlock *> cfgMap;

  /// used to generate parallel <forEach> scopes
  extern string forEach_suffix;

  /// mapping of AST phylum identifiers to ASTE objects
  extern map<unsigned int, ASTE*> ASTEmap;

  /// set of input channel names
  extern set<string> ASTE_inputChannels;

  /// set of output channel names
  extern set<string> ASTE_outputChannels;

  /// set of (prefixed) correlation set names
  extern set<string> ASTE_correlationSetNames;

  /// set of (prefixed) variable names
  extern set<string> ASTE_variableNames;

  /// set of (prefixed) partner link names
  extern set<string> ASTE_partnerLinkNames;

  /// set of (prefixed) link names
  extern set<string> ASTE_linkNames;

  /// set of ASTE identifiers of the start activities
  extern set<unsigned int> ASTE_startActivities;

  /// mapping of link names to ASTE identifiers
  extern map<string, unsigned int> ASTE_linkIdMap;

  /// mapping of scope names to ASTE identifiers
  extern map<string, unsigned int> ASTE_scopeNames;

  /// mapping of partner links to ASTE identifiers
  extern map<string, unsigned int> ASTE_partnerLinks;


  /*!
   * variables used to evaluate join conditions
   */
  /// mapping of link names to temporary link numbers
  extern map<string, unsigned int> joinCondition_links;
  
  /// mapping of temporary link numbers to Boolean value
  extern map<unsigned int, bool> joinCondition_linkStatus;
  
  /// mapping of join conditions and an assignment number to a Boolean result.
  extern map<unsigned int, map<unsigned int, bool > > joinCondition_result;

  /// \todo comment me
  extern map<unsigned int, unsigned int> PPhasCompensateMap;

  /// the identifier of the current scope
  extern unsigned int PPcurrentScope;

  /// statistics about the activities and handlers of the process
  extern s_process_information process_information;

  /// statistics about the optional WSDL file
  extern s_wsdl_information wsdl_information;

  /// A temporary mapping of attributes. This mapping is filled during parsing
  /// and is copied during post-processing to the AST annotation.
  extern map<unsigned int, map<string, string> > temporaryAttributeMap;

  /// Identifier of the next AST element. The process's id is 1.
  extern int ASTEid;  

  /// The root of the abstract syntax tree.
  extern kc::tProcess AST;

  /// String to gather arc information for visualization
  extern string visArcs;

  /// string holding the invocation of BPEL2oWFN
  extern string invocation;

  /// string holding the called program name of BPEL2oWFN
  extern string program_name;

  /// filename of input file
  extern string filename;

  /// filename of output file
  extern string output_filename;

  /// filename of the optional BPEL4Chor choreography file
  extern string choreography_filename;  

  /// filename of the optional WSDL file
  extern string wsdl_filename;  

  /// parameters (set by #parse_command_line)
  extern map<possibleParameters, bool> parameters;

  /// the last token that was displayed in an error message
  extern string last_error_token;

  /// the line of the last syntax error
  extern string last_error_line;

  /// number of detect static analysis errors
  extern unsigned int static_analysis_errors;

  /// number of other detected warnings and errors
  extern unsigned int other_errors;

  /// true while parsing, false while postprocessing
  extern bool parsing;

  /// all elements that caused parse errors (either misplaced or nonstandard)
  extern set<string> unknown_elements;

  /// true if process is abstract BPEL
  extern bool abstract_process;

  /// the level of structural reduction
  extern unsigned int reduction_level;

  /// an object holding information from the optional WSDL file
  extern WSDL WSDLInfo;

  /// an object holding information from the optional BPEL4Chor file
  extern Choreography ChorInfo;

  /// a temporary attribute mapping for the WSDL and BPEL4Chor parser
  extern map<string, string> tempAttributes;
}

#endif
