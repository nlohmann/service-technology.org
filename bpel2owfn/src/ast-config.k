/*****************************************************************************\
 * Copyright 2006, 2007 Niels Lohmann                                        *
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
 *          last changes of: \$Author: znamirow $
 * 
 * \since   2006-24-03
 *
 * \date    \$Date: 2007/05/11 10:42:02 $
 * 
 * \note    This file is part of the tool GNU BPEL2oWFN and was created during
 *          the project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file bpel-abstract.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.51 $
 *
 * \todo    Annotate the global mappings which function clear them.
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

%uview petrinetsmall;
%uview petrinetnew;
%uview xml;
%uview cfg;
%uview postprocessing;
%uview evaluation;
%uview visualization;


// the header file

%{ KC_UNPARSE
#include <iostream>
#include <cmath>
#include <cassert>
#include <map>
#include <set>
#include <climits>
#include <string>

#include "petrinet.h"
#include "petrinet-patterns-basic.h"
#include "petrinet-patterns-structured.h"
#include "petrinet-patterns-scopes.h"
#include "petrinet-patterns-handlers.h"
#include "petrinet-patterns-links.h"
#include "extension-wsdl.h"
#include "options.h"
#include "cfg.h"
#include "helpers.h"
#include "ast-details.h"
#include "ast-tools.h"
#include "globals.h"
#include "extension-chor.h"

using std::map;
using std::string;
using std::set;
using std::endl;
using std::cerr;

extern PetriNet PN;				// introduced in main.c
extern CFGBlock * CFG;				// introduced in cfg.cc
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
%rview invoke;
%rview newNames;


// the header file

%{ KC_REWRITE
#include <map>

#include "ast-details.h"
#include "options.h"
#include "globals.h"
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

// a preprocessor directive for the quite generic constructors
#define CTOR(token) { \
  ENTER("[AST]"); \
  assert(integer_1 != NULL); \
  if (integer_1->value == 0) \
  { \
    id = globals::ASTEid; \
    integer_1 = mkinteger(globals::ASTEid++); \
  } \
  else { \
    id = integer_1->value; \
  } \
  if (globals::ASTEmap[id] == NULL) \
  { \
    globals::ASTEmap[id] = new ASTE(id, token); \
  } \
  assert(globals::ASTEmap[id] != NULL); \
  globals::ASTEmap[id]->phylum = this; \
  globals::ASTEmap[id]->isUserDefined = globals::parsing; \
  LEAVE("[AST]"); \
}


// use to get ANSI C
#undef kc_strcasecmp
#define kc_strcasecmp strcmp

%}
