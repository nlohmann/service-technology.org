/*****************************************************************************\
 * Copyright 2006 Niels Lohmann                                              *
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

%{
/*!
 * \file    ast-config.cc
 *
 * \brief   Configuration of the AST
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nielslohmann $ 
 * 
 * \since   2006-24-03
 *
 * \date    \$Date: 2006/12/17 19:05:00 $
 * 
 * \note    This file is part of the tool GNU BPEL2oWFN and was created during
 *          the project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file bpel-abstract.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.33 $
 */
%}


/******************************************************************************
 * THE HEADER FOR ALL KIMWITU++ USERS (ast-config.h)
 *****************************************************************************/

%{ HEADER

/*!
 * \file ast-config.h
 * \brief Configuration of the AST (interface)
 *
 * See \ref ast-config.cc for more information.
 */

#include <cassert>

#include "ast-system-k.h"	// phylum definitions
#include "ast-system-rk.h"	// rewrite rules
#include "ast-system-unpk.h"	// unparse rules
#include "ast-system-yystype.h" // data types for tokens and non-terminals

extern string forEach_suffix;
%}





/******************************************************************************
  UNPARSING FEATURES
******************************************************************************/

// the unparse views

%uview petrinet;
%uview petrinetsmall;
%uview petrinetnew;
%uview xml;
%uview cfg;
%uview postprocessing;


// the header file

%{ KC_UNPARSE
#include <iostream>
#include <cmath>
#include <cassert>
#include <map>
#include <set>
#include <climits>

#include "petrinet.h"
#include "petrinet-patterns-basic.h"
#include "petrinet-patterns-structured.h"
#include "petrinet-patterns-scopes.h"
#include "options.h"
#include "cfg.h"
#include "helpers.h"
#include "ast-details.h"
#include "ast-tools.h"

using namespace std;

extern PetriNet PN;				// introduced in main.c
extern CFGBlock * CFG;				// introduced in cfg.cc
extern map<unsigned int, ASTE*> ASTEmap;	// introduced in ast-details.cc


/// prefix of the current scope (read by the stop pattern and used to direct
/// failures to the correct fault_in place)
string currentScope;

/// \todo (gierds) comment me!
map <string, CFGBlock *> cfgMap;


string forEach_suffix = "";		///< used to generate parallel <forEach> scopes


set<string> ASTE_inputChannels;
set<string> ASTE_outputChannels;

set<string> correlationSetNames;	///< a set holding all correlation set names (with prefix)
set<string> variableNames;		///< a set holding all variable names (with prefix)
set<string> partnerLinkNames;		///< a set holding all partnerLink names (with prefix)
set<string> linkNames;			///< a set holding all link names (with prefix)

set<unsigned int> startActivities;	///< a set of the ids of the start activities

map<string, unsigned int> linkIdMap;	///< maps link names to identifiers
map<string, unsigned int> scopeNames;	///< maps scope names to identifiers

map<unsigned int, unsigned int> PPhasCompensateMap;

unsigned int PPcurrentScope = 0;	///< the identifier of the current scope
%}


// All the includes, variables and structs defined here can be used in the
// printers below.
%{
#include <iostream>
#include "options.h" // for #output
%}


// the printer functions

/// "fake" printer-function
void pseudoPrinter(const char *s, uview v) { }


/// printer-function for output on stdout
void printer(const char *s, uview v)
{
  *output << s;
}





/******************************************************************************
  REWRITING FEATURES
******************************************************************************/

// the rewrite views

%rview implicit;
%rview newNames;


// the header file

%{ KC_REWRITE
#include <map>

#include "ast-details.h"
#include "options.h"

using namespace std;

extern map<unsigned int, ASTE*> ASTEmap;
%}





/******************************************************************************
  PHYLUM DECLARATIONS
******************************************************************************/

// all class declarations generated from phylum definitions; included by all
// implicitly generated files

%{ KC_TYPES_HEADER
#include <string>
#include <iostream>
#include <map>

#include "debug.h"
#include "helpers.h"

using namespace std;





// a preprocessor directive for the quite generic constructors
#define CTOR(token) { \
  ENTER("[AST]"); \
  assert(integer_1 != NULL); \
  if (integer_1->value == 0) \
  { \
    id = ASTEid; \
    integer_1 = mkinteger(ASTEid++); \
  } \
  else { \
    id = integer_1->value; \
  } \
  if (ASTEmap[id] == NULL) \
  { \
    ASTEmap[id] = new ASTE(id, token); \
  } \
  assert(ASTEmap[id] != NULL); \
  ASTEmap[id]->phylum = this; \
  LEAVE("[AST]"); \
}

%}
