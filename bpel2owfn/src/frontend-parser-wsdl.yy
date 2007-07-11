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
 *          last changes of: \$Author: znamirow $
 *
 * \since   2007/04/29
 *
 * \date    \$Date: 2007/07/11 11:35:51 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using GNU Bison reading file
 *          frontend-parser-chor.yy.
 *          See http://www.gnu.org/software/bison/bison.html for details
 *
 * \version \$Revision: 1.12 $
 *
 * \ingroup frontend
 */
%}


/******************************************************************************
 * Terminal symbols (tokens).
 *****************************************************************************/

%token APOSTROPHE EQUAL GREATER GREATEROREQUAL K_AND K_ASSIGN K_BRANCHES K_CASE
%token K_CATCH K_CATCHALL K_COMPENSATE K_COMPENSATESCOPE K_COMPENSATIONHANDLER
%token K_COMPLETIONCONDITION K_CONDITION K_COPY K_CORRELATION K_CORRELATIONS
%token K_CORRELATIONSET K_CORRELATIONSETS K_ELSE K_ELSEIF K_EMPTY
%token K_EVENTHANDLERS K_EXIT K_EXTENSION K_EXTENSIONACTIVITY
%token K_EXTENSIONASSIGNOPERATION K_EXTENSIONS K_FAULTHANDLERS
%token K_FINALCOUNTERVALUE K_FLOW K_FOR K_FOREACH K_FROM K_FROMPART K_FROMPARTS
%token K_GETLINKSTATUS K_IF K_IMPORT K_INVOKE K_JOINCONDITION K_LINK K_LINKS
%token K_LITERAL K_MESSAGEEXCHANGE K_MESSAGEEXCHANGES K_ONALARM K_ONEVENT
%token K_ONMESSAGE K_OPAQUEACTIVITY K_OPAQUEFROM K_OR K_OTHERWISE K_PARTNER
%token K_PARTNERLINK K_PARTNERLINKS K_PARTNERS K_PICK K_PROCESS K_QUERY K_RECEIVE
%token K_REPEATEVERY K_REPEATUNTIL K_REPLY K_RETHROW K_SCOPE K_SEQUENCE
%token K_SOURCE K_SOURCES K_STARTCOUNTERVALUE K_SWITCH K_TARGET K_TARGETS
%token K_TERMINATE K_TERMINATIONHANDLER K_THROW K_TO K_TOPART K_TOPARTS
%token K_TRANSITIONCONDITION K_UNTIL K_VALIDATE K_VARIABLE K_VARIABLES K_WAIT
%token K_WHILE LBRACKET LESS LESSOREQUAL NOTEQUAL RBRACKET X_CLOSE X_EQUALS
%token X_NEXT X_OPEN X_SLASH
%token K_TOPOLOGY K_PARTICIPANTTYPES K_PARTICIPANTTYPE K_PARTICIPANTS K_PARTICIPANT K_PARTICIPANTSET K_MESSAGELINKS K_MESSAGELINK
%token K_TYPES K_PORTTYPE K_FAULT K_OPERATION K_DEFINITIONS K_OUTPUT K_INPUT K_MESSAGE K_PART K_BINDING K_SERVICE K_PORT
%token K_PARTNERLINKTYPE K_ROLE K_SCHEMA K_PROPERTY K_PROPERTYALIAS
%token <yt_casestring> NUMBER
%token <yt_casestring> X_NAME
%token <yt_casestring> VARIABLENAME
%token <yt_casestring> X_STRING

// OR and AND are left-associative.
%left K_OR
%left K_AND

// The start symbol of the grammar.
%start tDefinions



/******************************************************************************
 * Bison options.
 *****************************************************************************/

// Bison generates a list of all used tokens in file "frontend-parser.h" (for
// Flex).
%token_table

%yacc

// We know what we are doing. Abort if any shift/reduce or reduce/reduce errors
// arise.
%expect 0

// All "yy"-prefixes are replaced with "frontend_wsdl_".
%name-prefix="frontend_wsdl_"





%{
#include <cassert>
#include <map>

#include "ast-config.h"
#include "helpers.h"
#include "debug.h"
#include "globals.h"
#include "extension-wsdl.h"


using std::cerr;
using std::endl;



/******************************************************************************
 * External variables
 *****************************************************************************/

extern int frontend_lex();	// from flex: the lexer funtion

// use the functions of the BPEL parser
#define frontend_wsdl_lex() frontend_lex()
#define frontend_wsdl_error(a) frontend_error(a)
#define frontend_wsdl_in frontend_in // needed?




/******************************************************************************
 * Global variables
 *****************************************************************************/

WSDL_Message *temp_message = NULL;
WSDL_PartnerLinkType *temp_partnerLinkType = NULL;
WSDL_PortType *temp_portType = NULL;
%}



%%



tDefinions:
  X_OPEN K_DEFINITIONS arbitraryAttributes X_NEXT
  tImport_list tTypes tMessage_list tPortType_list tBinding_list tService_list
  tPropertyPropertyAlias_list tPartnerLinkType_list
  X_SLASH K_DEFINITIONS X_CLOSE
    { globals::tempAttributes.clear(); }
;


/******************************************************************************
 * IMPORT
 *****************************************************************************/

tImport_list:
  /* empty */
| tImport X_NEXT tImport_list
;

tImport:
  K_IMPORT arbitraryAttributes X_SLASH
    { globals::wsdl_information.imports++; }
;


/******************************************************************************
 * TYPES
 *****************************************************************************/

tTypes:
  /* empty */
| K_TYPES X_NEXT tSchema X_SLASH K_TYPES X_NEXT
    { globals::wsdl_information.types++; }
;

tSchema:
  K_SCHEMA arbitraryAttributes X_NEXT error X_SLASH K_SCHEMA X_NEXT
    { genericError(124, globals::last_error_token, globals::last_error_line, ERRORLEVEL_NOTICE); }
| K_SCHEMA arbitraryAttributes X_NEXT tImport X_NEXT X_SLASH K_SCHEMA X_NEXT
;


/******************************************************************************
 * MESSAGE
 *****************************************************************************/

tMessage_list:
  /* empty */
| tMessage X_NEXT tMessage_list
;

tMessage:
  K_MESSAGE arbitraryAttributes
    { temp_message = new WSDL_Message(globals::tempAttributes["name"]); }
  X_NEXT tPart_list X_SLASH K_MESSAGE
    { globals::WSDLInfo.messages[temp_message->name] = temp_message; }
| K_MESSAGE arbitraryAttributes X_SLASH
    { 
      temp_message = new WSDL_Message(globals::tempAttributes["name"]); 
      globals::WSDLInfo.messages[temp_message->name] = temp_message;
    }
;

tPart_list:
  tPart X_NEXT
  { 
    temp_message->element.first = globals::tempAttributes["name"];
    temp_message->element.second = globals::tempAttributes["element"];
  }   
| tPart X_NEXT tPart_list
;

tPart:
  K_PART arbitraryAttributes X_SLASH
    { temp_message->parts[globals::tempAttributes["name"]] = globals::tempAttributes["type"];}
;


/******************************************************************************
 * PORT TYPE
 *****************************************************************************/

tPortType_list:
  /* empty */
| tPortType X_NEXT tPortType_list
;

tPortType:
  K_PORTTYPE arbitraryAttributes
    { temp_portType = new WSDL_PortType(globals::tempAttributes["name"]); }
  X_NEXT tOperation_list X_SLASH K_PORTTYPE
    { globals::WSDLInfo.portTypes[temp_portType->name] = temp_portType; }
;

tOperation_list:
  tOperation X_NEXT
| tOperation X_NEXT tOperation_list
;

tOperation:
  K_OPERATION arbitraryAttributes
    { temp_portType->addOperation(globals::tempAttributes["name"]); }
  X_NEXT tInputOutputFault_list X_SLASH K_OPERATION
| K_OPERATION arbitraryAttributes X_SLASH
    { temp_portType->addOperation(globals::tempAttributes["name"]); }
;

tInputOutputFault_list:
  tInputOutputFault X_NEXT
| tInputOutputFault X_NEXT tInputOutputFault_list
;

tInputOutputFault:
  tInput
| tOutput
| tFault
;

tInput:
  K_INPUT arbitraryAttributes X_SLASH
    { temp_portType->addOperationDetails("input", globals::tempAttributes["message"]); }
;

tOutput:
  K_OUTPUT arbitraryAttributes X_SLASH
    { temp_portType->addOperationDetails("output", globals::tempAttributes["message"]); }
;

tFault:
  K_FAULT arbitraryAttributes X_SLASH
    { temp_portType->addOperationDetails("fault", globals::tempAttributes["message"], globals::tempAttributes["name"]); }
;


/******************************************************************************
 * BINDING
 *****************************************************************************/

tBinding_list:
  /* empty */
| tBinding X_NEXT tBinding_list
;

tBinding:
  K_BINDING arbitraryAttributes X_NEXT tOperation_list X_SLASH K_BINDING
    { globals::wsdl_information.bindings++; }
;


/******************************************************************************
 * SERVICE
 *****************************************************************************/

tService_list:
  /* empty */
| tService X_NEXT tService_list
;

tService:
  K_SERVICE tPort_list X_SLASH K_SERVICE
    { globals::wsdl_information.services++; }
;

tPort_list:
  /* empty */
| tPort X_NEXT tPort_list
;

tPort:
  K_PORT arbitraryAttributes X_SLASH
;

/******************************************************************************
 * PROPERTY / PROPERTY ALIAS
 *****************************************************************************/

tPropertyPropertyAlias_list:
  /* empty */
| tPropertyPropertyAlias X_NEXT tPropertyPropertyAlias_list
    { globals::wsdl_information.properties++; }
;

tPropertyPropertyAlias:
  K_PROPERTY arbitraryAttributes X_SLASH
| K_PROPERTYALIAS arbitraryAttributes X_SLASH
| K_PROPERTYALIAS arbitraryAttributes X_NEXT error X_NEXT X_SLASH K_PROPERTYALIAS
    { genericError(125, globals::last_error_token, globals::last_error_line, ERRORLEVEL_NOTICE); }
;


/******************************************************************************
 * PARTNER LINK TYPE
 *****************************************************************************/

tPartnerLinkType_list:
  /* empty */
| tPartnerLinkType X_NEXT tPartnerLinkType_list
;

tPartnerLinkType:
  K_PARTNERLINKTYPE arbitraryAttributes
    { temp_partnerLinkType = new WSDL_PartnerLinkType(globals::tempAttributes["name"]); }
  X_NEXT tRoles X_SLASH K_PARTNERLINKTYPE
    { globals::WSDLInfo.partnerLinkTypes[temp_partnerLinkType->name] = temp_partnerLinkType; }
;

tRoles:
  tRole X_NEXT
| tRole X_NEXT tRole X_NEXT
;

tRole:
  K_ROLE arbitraryAttributes X_SLASH
    { temp_partnerLinkType->addRole(globals::tempAttributes["name"], globals::tempAttributes["portType"]); }
| K_ROLE arbitraryAttributes X_NEXT
    { globals::tempAttributes["RoleName"] = globals::tempAttributes["name"]; }
  K_PORTTYPE arbitraryAttributes X_SLASH X_NEXT X_SLASH K_ROLE
    { temp_partnerLinkType->addRole(globals::tempAttributes["RoleName"], globals::tempAttributes["name"]); }
;


/******************************************************************************
 * ATTRIBUTES
 *****************************************************************************/

arbitraryAttributes:
  /* empty */
| X_NAME X_EQUALS X_STRING arbitraryAttributes
    { globals::tempAttributes[strip_namespace($1->name)] = strip_namespace($3->name); }
;
