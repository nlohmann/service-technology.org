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
 * \file check-symbols.h
 *
 * \brief Provides classes and functions in order to check the correct use of
 * 	  symbols within a BPEL process.
 *
 * \author  
 *          - last changes of: \$Author: nlohmann $
 *          
 * \date
 *          - last changed: \$Date: 2006/07/08 12:15:53 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \version \$Revision: 1.25 $
 */

#ifndef CHECK_SYMBOLS_H
#define CHECK_SYMBOLS_H

#include <list>
#include <algorithm>
#include <typeinfo>
#include <map>
#include <stack>

#include "bpel-kc-k.h" // phylum definitions
#include "bpel-kc-yystype.h" // data types for tokens and non-terminals

#include "debug.h"

using namespace std;

// forward declaration of classes
class SymbolManager;

/**
 * \class	SymbolManager
 *
 * \brief	Management class for handling scopes, variables, etc.
 *
 * The SymbolManager can be used to handle scopes, check the correct usage
 * of Variables, Links, PartnerLinks and Channels, and it provides basic
 * functions for Dead Path Elimination.
 * 
 */
class SymbolManager
{
  private:
    /// number of possible activities, where to start DPE from
    int dpePossibleStarts;
    /// number of possible activities, where to end DPE 
    int dpePossibleEnds;
    /// stack to save possible starts when entering while
    stack<int> dpeStartStack;
  public:

    /// constructor
    SymbolManager();

    /// destructor
    ~SymbolManager();
    
    /* --- Dead Path Elimination --- */
    /// add a possible start for DPE
    void addDPEstart();
    /// add a possible end for DPE
    void addDPEend();
    /// remove a possible start for DPE
    void remDPEstart();
    /// remove a possible end for DPE
    void remDPEend();
    /// set number of possible ends to 0
    void resetDPEend();
    /// links are not allowed to cross while borders, so special treatment
    void startDPEinWhile();
    /// links are not allowed to cross while borders, so special treatment
    void endDPEinWhile();
    /// checks if weed need a negLink under current conditions
    int needsDPE();
};

#endif

