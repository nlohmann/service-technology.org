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
 * \file check-symbols.cc
 *
 * \brief Provides classes and functions in order to check the correct use of
 * 	  symbols within a BPEL process.
 *
 * \author  
 *          - last changes of: \$Author: nlohmann $
 *          
 * \date
 *          - last changed: \$Date: 2006/06/19 14:51:42 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universit&auml;t zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 */

#include "check-symbols.h"

#include <stack>

extern std::string intToString(int); // little helper function (helpers.cc)
extern int yylineno;		     // line number from flex/bison
extern bool inWhile;		     // flag if in while activity (bpel-syntax.y)

/// constructor for class SymbolManager
SymbolManager::SymbolManager()
{
  dpePossibleStarts = 0;
  dpePossibleEnds = 0;
}

/// destructor for class SymbolManager
SymbolManager::~SymbolManager()
{
}  


/* --- Dead Path Elimination --- */
/// add a possible start for DPE
void SymbolManager::addDPEstart()
{
  trace(TRACE_VERY_DEBUG, "[CS] DPE: Adding possible start\n");
  dpePossibleStarts++;
}

/// add a possible end for DPE
void SymbolManager::addDPEend()
{
  trace(TRACE_VERY_DEBUG, "[CS] DPE: Adding possible end\n");
  dpePossibleEnds++;
}

/// remove a possible start for DPE
void SymbolManager::remDPEstart()
{
  trace(TRACE_VERY_DEBUG, "[CS] DPE: Removing possible start\n");
  dpePossibleStarts--;
}

/// remove a possible end for DPE
void SymbolManager::remDPEend()
{
  trace(TRACE_VERY_DEBUG, "[CS] DPE: Removing possible end(s)\n");
  dpePossibleEnds--;
}

/// set number of possible DPE ends to 0
void SymbolManager::resetDPEend()
{
  trace(TRACE_VERY_DEBUG, "[CS] DPE: Resetting number of possible ends\n");
  dpePossibleEnds = 0;
}

/// links are not allowed to cross the borders of whiles, so special treatment
void SymbolManager::startDPEinWhile()
{
  trace(TRACE_VERY_DEBUG, "[CS] DPE: Entering while\n");
  dpeStartStack.push(dpePossibleStarts);
  dpePossibleStarts = 0;
  dpePossibleEnds = 0;
}

/// links are not allowed to cross the borders of whiles, so special treatment
void SymbolManager::endDPEinWhile()
{
  trace(TRACE_VERY_DEBUG, "[CS] DPE: Leaving while\n");
  dpePossibleStarts = dpeStartStack.top();
  dpeStartStack.pop();
  dpePossibleEnds = 0;
}

/// checks if weed need a negLink under current conditions
kc::integer SymbolManager::needsDPE()
{
  // if we have possible starts and ends, we need a negLink and return 1
  if ( (dpePossibleEnds > 0) && (dpePossibleStarts > 0))
  {
    trace(TRACE_VERY_DEBUG, "[CS] DPE: negLink needed\n");
    trace(TRACE_VERY_DEBUG, "[CS] DPE: possible starts: " 
		           + intToString(dpePossibleStarts) + "\n"
		           + "          possible ends: " 
			   + intToString(dpePossibleEnds)
			   + "\n");
    return kc::mkinteger(1);
  }
  // no need for a negLink, so return 0
  trace(TRACE_VERY_DEBUG, "[CS] DPE: no negLink needed\n");
  trace(TRACE_VERY_DEBUG, "[CS] DPE: possible starts: " 
		         + intToString(dpePossibleStarts) + "\n"
		         + "          possible ends: " 
			 + intToString(dpePossibleEnds) + "\n");
  return kc::mkinteger(0);
}

