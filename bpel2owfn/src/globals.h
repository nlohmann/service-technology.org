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
 * \date    \$Date: 2007/03/07 14:35:34 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.2 $
 */





#ifndef GLOBALS_H
#define GLOBALS_H

#include <set>
#include <string>
#include <map>

#include "cfg.h"

using std::set;
using std::string;
using std::map;


/// namespace for global variables
namespace globals {
  /// prefix of the current scope (read by the stop pattern and used to direct
  /// failures to the correct fault_in place)
  extern string currentScope;

  /// \todo (gierds) comment me!
  extern map <string, CFGBlock *> cfgMap;


  extern string forEach_suffix;		///< used to generate parallel <forEach> scopes


  // the global mappings (though they are not global)
  extern set<string> ASTE_inputChannels;		///< \todo comment me
  extern set<string> ASTE_outputChannels;	///< \todo comment me
  extern set<string> ASTE_correlationSetNames;	///< a set holding all correlation set names (with prefix)
  extern set<string> ASTE_variableNames;		///< a set holding all variable names (with prefix)
  extern set<string> ASTE_partnerLinkNames;	///< a set holding all partnerLink names (with prefix)
  extern set<string> ASTE_linkNames;		///< a set holding all link names (with prefix)
  extern set<unsigned int> ASTE_startActivities;	///< a set of the ids of the start activities

  extern map<string, unsigned int> ASTE_linkIdMap;	///< maps link names to identifiers
  extern map<string, unsigned int> ASTE_scopeNames;	///< maps scope names to identifiers
  extern map<string, unsigned int> ASTE_partnerLinks;    ///< maps partnerlink names to identifiers

  extern map<string, unsigned int> joinCondition_links;		///< used to evaluate joinConditions: maps link names to a temporary link number
  extern map<unsigned int, bool> joinCondition_linkStatus;	///< used to evaluate joinConditions: maps temporary link numbers to Boolean assignment
  
  /// Used to evaluate joinConditions: maps an id of a join condition togehter
  /// with an assignment number to a Boolean result.
  extern map<unsigned int, map<unsigned int, bool > > joinCondition_result;

  extern map<unsigned int, unsigned int> PPhasCompensateMap;

  extern unsigned int PPcurrentScope;	///< the identifier of the current scope
}

#endif
