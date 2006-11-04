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
\*****************************************************************************/



/******************************************************************************
 * THE HEADER FOR ALL KIMWITU++ USERS (ast-config.h)
 *****************************************************************************/

%{ HEADER
#include "ast-system-k.h"	// phylum definitions
#include "ast-system-rk.h"	// ?
#include "ast-system-unpk.h"	// ?
#include "ast-system-yystype.h" // data types for tokens and non-terminals
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

#include "petrinet.h"
#include "options.h"
#include "cfg.h"
#include "helpers.h"
#include "ast-details.h"
#include "ast-tools.h"

using namespace std;

extern PetriNet *TheNet;			// introduced in main.c
extern CFGBlock * TheCFG;			// introduced in cfg.cc
extern map<unsigned int, ASTE*> ASTEmap;	// introduced in ast-details.cc


/// prefix of the current scope (read by the stop pattern and used to direct
/// failures to the correct fault_in place)
string currentScope;

/// \todo (gierds) comment me!
map <std::string, CFGBlock *> cfgMap;


set<string> ASTE_inputChannels;
set<string> ASTE_outputChannels;
set<string> ASTE_variables;
set<string> ASTE_links;

map<string, unsigned int> scopeNames;

set<string> catches;			///< a set to detect multiple catches
set<unsigned int> startActivities;	///< a set of the ids of the start activities

map<string, int> linkIdMap;
map<int, int> PPhasCompensateMap;

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

using std::string;
using std::cerr;
using std::endl;
using std::map;

typedef std::map<string, string> stringMap;




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
