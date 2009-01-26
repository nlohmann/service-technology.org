/*****************************************************************************\
  GNU BPEL2oWFN -- Translating BPEL Processes into Petri Net Models
  Copyright (C) 2006, 2007  Niels Lohmann,
                            Christian Gierds, and
                            Martin Znamirowski
  Copyright (C) 2005        Niels Lohmann and
			    Christian Gierds
  GNU BPEL2oWFN is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the Free
  Software Foundation; either version 3 of the License, or (at your option) any
  later version.
  GNU BPEL2oWFN is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
  details.
  You should have received a copy of the GNU General Public License along with
  GNU BPEL2oWFN (see file COPYING); if not, see http://www.gnu.org/licenses
  or write to the Free Software Foundation,Inc., 51 Franklin Street, Fifth
  Floor, Boston, MA 02110-1301  USA.
\*****************************************************************************/
%{
/*!
 * \file    frontend-parser-wsdl.cc
 *
 * \brief   WSDL 1.1 parser
 *
 *          WSDL is parsed as defined in
 *          http://schemas.xmlsoap.org/wsdl/2003-02-11.xsd
 *
 *          Partner Link Types are parsed according to the "Schema for OASIS
 *          Business Process Execution Language (WS-BPEL) 2.0 - Schema for
 *          Partner Link Type" (http://docs.oasis-open.org/wsbpel/2.0/plnktype)
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nielslohmann $
 *
 * \since   2007/04/29
 *
 * \date    \$Date: 2007/06/28 07:38:16 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using GNU Bison reading file
 *          frontend-parser-chor.yy.
 *          See http://www.gnu.org/software/bison/bison.html for details
 *
 * \version \$Revision: 1.10 $
 *
 * \ingroup frontend
 */
%}
// options for the bison flex interplay 
//
// Bison generates a list of all used tokens in file "frontend-parser.h" (for
// Flex).
%token_table
%defines
%yacc
/******************************************************************************
 * Terminal symbols (tokens).
 *****************************************************************************/
%token APOSTROPHE EQUAL GREATER GREATEROREQUAL K_AND
%token LBRACKET LESS LESSOREQUAL NOTEQUAL RBRACKET X_CLOSE X_EQUALS
%token X_OPEN X_END_CLOSE X_END_OPEN X_NAME K_TAG
%token X_LITERAL
%token X_STRING
// OR and AND are left-associative.
%left K_OR
%left K_AND
// The start symbol of the grammar.
%start tInit
/******************************************************************************
 * Bison options.
 *****************************************************************************/
// We know what we are doing. Abort if any shift/reduce or reduce/reduce errors
// arise.
%expect 0
// All "yy"-prefixes are replaced with "frontend_wsdl_".
%name-prefix="frontend_"
%{
// Options for Bison (1): Enable debug mode for verbose messages during
// parsing. The messages can be enabled by using command-line parameter
// "-d bison".
#define YYDEBUG 1
#define YYERROR_VERBOSE 1
// Options for Bison (2): To avoid the message "parser stack overflow". or
// "memory exhausted". These values are just guesses. Increase them if
// necessary.
#define YYMAXDEPTH 1000000
#define YYINITDEPTH 10000
#include "helpers.h"
#include "debug.h"
#include "globals.h"
#include "AST.h"
#include "yystype.h"

#include <string>
#include <list>
#include <map>

using std::cerr;
using std::endl;
using std::string;
using std::list;
using std::map;

/******************************************************************************
 * External variables
 *****************************************************************************/
extern char *frontend_text;	// from flex: the current token
extern int frontend_lex();	// from flex: the lexer funtion
extern int frontend_lineno;	// from flex: the current line number
/******************************************************************************
 * Global variables
 *****************************************************************************/

ASTNode* currentNode;

%}
%%

tInit:
  {
    currentNode = globals::rootNode;
  }
  tElementList
;

tElementList:
  tElement tElementList
| {/*skip*/}
;

tElement:
  X_OPEN K_TAG
  {
    ASTNode* currentParent = currentNode;
    currentNode = new ASTNode($2, currentParent);
    currentParent->addChild(currentNode);
  }
  tNodeRest
| tLiteral
;

tNodeRest:
  arbitraryAttributes X_END_CLOSE
  {
    currentNode = currentNode->getParent();
  }
| arbitraryAttributes X_CLOSE tElementList X_END_OPEN K_TAG X_CLOSE
  {
    currentNode = currentNode->getParent();
  }
;
tLiteral:
  X_LITERAL
  {
    currentNode->addChild(new ASTLiteral($1, currentNode));
  }
;

/******************************************************************************
 * ATTRIBUTES
 *****************************************************************************/
arbitraryAttributes:
  /* empty */
| X_NAME X_EQUALS X_STRING
  {
    currentNode->addAttribute($1,$3);
  }
  arbitraryAttributes
;
