/*****************************************************************************\
  GNU BPEL2oWFN -- Translating BPEL Processes into Petri Net Models

  Copyright (C) 2009        Niels Lohmann
  Copyright (C) 2006, 2007  Niels Lohmann,
                            Christian Gierds, and
                            Martin Znamirowski
  Copyright (C) 2005        Niels Lohmann and
                            Christian Gierds

  GNU BPEL2oWFN is free software: you can redistribute it and/or modify it
  under the terms of the GNU Affero General Public License as published by the
  Free Software Foundation, either version 3 of the License, or (at your
  option) any later version.

  GNU BPEL2oWFN is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more
  details.

  You should have received a copy of the GNU Affero General Public License
  along with GNU BPEL2oWFN. If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

/*!
 * \file    globals.cc
 *
 * \brief   global varialbes
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nielslohmann $
 * 
 * \since   2007/03/05
 *
 * \date    \$Date: 2007/07/30 19:56:40 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.35 $
 */





/******************************************************************************
 * Headers
 *****************************************************************************/

#include "globals.h"
#include "extension-wsdl.h"
#include "extension-chor.h"
#include "ast-details.h"






namespace globals {
  string currentScope;

  map <string, CFGBlock *> cfgMap;

  string forEach_suffix = "";

  map<string, unsigned int> ASTE_inputChannels;
  map<string, unsigned int> ASTE_outputChannels;
  map<string, string> ASTE_channel_to_port;
  set<string> ASTE_correlationSetNames;
  set<string> ASTE_variableNames;
  set<string> ASTE_messageExchangeNames;
  set<string> ASTE_partnerLinkNames;
  set<string> ASTE_linkNames;
  set<unsigned int> ASTE_IMAs;
  set<unsigned int> ASTE_startActivities;

  map<string, string> ASTE_variableMap;
  map<string, unsigned int> ASTE_linkIdMap;
  map<string, unsigned int> ASTE_scopeNames;
  map<string, unsigned int> ASTE_partnerLinks;

  map<string, unsigned int> joinCondition_links;
  map<unsigned int, bool> joinCondition_linkStatus;
  map<unsigned int, map<unsigned int, bool > > joinCondition_result;

  map<unsigned int, unsigned int> PPhasCompensateMap;

  unsigned int PPcurrentScope = 0;

  s_process_information process_information = {0,0,0,0,0,0,0,0,0,0,0};
  s_wsdl_information wsdl_information = {0,0,0,0,0,0,0,0,0};

  map<unsigned int, ASTE*> ASTEmap;

  map<unsigned int, map<string, string> > temporaryAttributeMap;

  int ASTEid = 1;

  kc::tProcess AST;

/// string holding the arcs of the visualization
  string visArcs="";

  string invocation="";
  
  string filename = "<STDIN>";
  string output_filename = "";

  map<possibleParameters, bool> parameters;

  string last_error_token = "";
  string last_error_line = "";

  unsigned int static_analysis_errors = 0;
  unsigned int other_errors = 0;

  bool parsing = true;

  bool hideStandardHandlers = false;

  set<string> unknown_elements;

  bool abstract_process = false;

  unsigned int reduction_level = 0;

  string choreography_filename = "";

  string wsdl_filename = "";

  string net_filename = "";

  WSDL WSDLInfo = WSDL();

  BPEL4Chor BPEL4ChorInfo = BPEL4Chor();

  map<string, string> tempAttributes;

  int instances_of_current_process = 0;
  
  map<unsigned int, unsigned int> if_branches;

  map<string, string> owfn_commands;
  
  string owfn_statepredicate;
  
  net_modes net_mode;
}
