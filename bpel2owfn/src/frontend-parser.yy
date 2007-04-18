/*****************************************************************************\
 * Copyright 2007       Niels Lohmann, Martin Znamirowski                    *
 * Copyright 2005, 2006 Niels Lohmann, Christian Gierds                      *
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
 * \file    frontend-parser.cc
 *
 * \brief   BPEL parser
 *
 *          This file defines and implements the grammar of BPEL using standard
 *          BNF-rules to describe the originally XML-based syntax as it is
 *          specified in the WS-BPEL 2.0 and the BPEL4WS 1.1 specification. All
 *          terminals are passed from the lexer (implemented in
 *          \ref frontend-lexer.cc). Besides simple syntax-checking the grammar
 *          is used to build the abstract syntax tree as it is defined in
 *          ast-grammar.k.
 * 
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          Christian Gierds <gierds@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nielslohmann $
 *
 * \since   2005/11/10
 *
 * \date    \$Date: 2007/04/18 16:18:31 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using GNU Bison reading file
 *          frontend-parser.yy.
 *          See http://www.gnu.org/software/bison/bison.html for details
 *
 * \version \$Revision: 1.306 $
 *
 * \ingroup frontend
 *
 * \todo    Check whether the out-commented code lines in the <invoke> and
            <scope> section can be removed.
 */
%}


%{	
/*!
 * \file frontend-parser.h
 * \brief BPEL parser
 *
 * See \ref frontend-parser.cc for more information.
 * \ingroup frontend
 */

/*!
 * \fn frontend_parse
 * \brief parse the input file
 *
 * \note C LALR(1) parser skeleton written by Richard Stallman, by simplifying
 *       the original so-called "semantic" parser.
 *
 * \return 0 if no parse error occurs
 * \return 1 if any error occurs
 * \ingroup frontend
 */

/*!
 * \enum yytokentype
 * \brief list of possible tokens
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
%token K_ONMESSAGE K_OR K_OTHERWISE K_PARTNER K_PARTNERLINK K_PARTNERLINKS
%token K_PARTNERS K_PICK K_PROCESS K_QUERY K_RECEIVE K_REPEATEVERY
%token K_REPEATUNTIL K_REPLY K_RETHROW K_SCOPE K_SEQUENCE K_SOURCE K_SOURCES
%token K_STARTCOUNTERVALUE K_SWITCH K_TARGET K_TARGETS K_TERMINATE
%token K_TERMINATIONHANDLER K_THROW K_TO K_TOPART K_TOPARTS
%token K_TRANSITIONCONDITION K_UNTIL K_VALIDATE K_VARIABLE K_VARIABLES K_WAIT
%token K_WHILE LBRACKET LESS LESSOREQUAL NOTEQUAL RBRACKET X_CLOSE X_EQUALS
%token X_NEXT X_OPEN X_SLASH
%token <yt_casestring> NUMBER
%token <yt_casestring> X_NAME
%token <yt_casestring> VARIABLENAME
%token <yt_casestring> X_STRING

// OR and AND are left-associative.
%left K_OR
%left K_AND

// The start symbol of the grammar.
%start tProcess



/******************************************************************************
 * Bison options.
 *****************************************************************************/

// Bison generates a list of all used tokens in file "frontend-parser.h" (for
// Flex).
%token_table

%defines
%yacc

// We know what we are doing. Abort if any shift/reduce or reduce/reduce errors
// arise.
%expect 0

// All "yy"-prefixes are replaced with "frontend_".
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


#include <cassert>
#include <map>

#include "ast-config.h"
#include "helpers.h"
#include "debug.h"
#include "globals.h"
#include "ast-details.h"


using namespace kc;
using std::cerr;
using std::endl;



/******************************************************************************
 * External variables
 *****************************************************************************/

extern char *frontend_text;	// from flex: the current token
extern int frontend_lex();	// from flex: the lexer funtion
extern int frontend_lineno;	// from flex: the current line number



/******************************************************************************
 * Global variables
 *****************************************************************************/

/// A pointer to the current join condition.
impl_joinCondition* currentJoinCondition = standardJoinCondition();
%}



/******************************************************************************
 * Non-terminal symbols.
 *****************************************************************************/

/*
 * The types of the non-terminal symbols: Bison symbols are mapped to Kimwitu++
 * phyla.
 */
%type <yt_activity_list> activity_list
%type <yt_activity> activity
%type <yt_activity> activity2
%type <yt_casestring> constant
%type <yt_casestring> tBranches
%type <yt_casestring> tCompletionCondition
%type <yt_casestring> tCondition
%type <yt_casestring> tFinalCounterValue
%type <yt_casestring> tLiteral
%type <yt_casestring> tStartCounterValue
%type <yt_expression> booleanLinkCondition
%type <yt_integer> arbitraryAttributes
%type <yt_joinCondition> tJoinCondition
%type <yt_standardElements> standardElements
%type <yt_tAssign> tAssign
%type <yt_tCatch_list> tCatch_list
%type <yt_tCatch> tCatch
%type <yt_tCatchAll> tCatchAll
%type <yt_tCompensate> tCompensate
%type <yt_tCompensate> tCompensateScope
%type <yt_tCompensationHandler> tCompensationHandler
%type <yt_tCompensationHandler> truetCompensationHandler
%type <yt_tCopy_list> tCopy_list
%type <yt_tCopy> tCopy
%type <yt_tCorrelation_list> tCorrelation_list
%type <yt_tCorrelation_list> tCorrelations
%type <yt_tCorrelation> tCorrelation
%type <yt_tCorrelationSet_list> tCorrelationSet_list
%type <yt_tCorrelationSet_list> tCorrelationSets
%type <yt_tCorrelationSet> tCorrelationSet
%type <yt_tElse> tElse
%type <yt_tElse> tOtherwise
%type <yt_tElseIf_list> tCase_list
%type <yt_tElseIf_list> tElseIf_list
%type <yt_tElseIf> tCase
%type <yt_tElseIf> tElseIf
%type <yt_tEmpty> tEmpty
%type <yt_tEventHandlers> tEventHandlers
%type <yt_tEventHandlers> truetEventHandlers
%type <yt_tExit> tExit
%type <yt_tFaultHandlers> tFaultHandlers
%type <yt_tFaultHandlers> truetFaultHandlers
%type <yt_tFlow> tFlow
%type <yt_tForEach> tForEach
%type <yt_tFrom> tFrom
%type <yt_tFromPart_list> tFromPart_list
%type <yt_tFromPart_list> tFromParts
%type <yt_tFromPart> tFromPart
%type <yt_tIf> tIf
%type <yt_tInvoke> tInvoke
%type <yt_tLink_list> tLink_list
%type <yt_tLink_list> tLinks
%type <yt_tLink> tLink
%type <yt_tOnAlarm_list> tOnAlarm_list
%type <yt_tOnAlarm> tOnAlarm
%type <yt_tOnMessage_list> tOnMessage_list
%type <yt_tOnMessage> tOnEvent
%type <yt_tOnMessage> tOnMessage
%type <yt_tPartner_list> tPartner_list
%type <yt_tPartner_list> tPartners
%type <yt_tPartner> tPartner
%type <yt_tPartnerLink_list> tPartnerLink_list
%type <yt_tPartnerLink_list> tPartnerLinks
%type <yt_tPartnerLink_list> truetPartnerLinks
%type <yt_tPartnerLink> tPartnerLink
%type <yt_tPick> tPick
%type <yt_tProcess> tProcess
%type <yt_tReceive> tReceive
%type <yt_tRepeatEvery> tRepeatEvery
%type <yt_tRepeatUntil> tRepeatUntil
%type <yt_tReply> tReply
%type <yt_tRethrow> tRethrow
%type <yt_tScope> tScope
%type <yt_tSequence> tSequence
%type <yt_tSource_list> tSource_list
%type <yt_tSource> tSource
%type <yt_tTarget_list> tTarget_list
%type <yt_tTarget> tTarget
%type <yt_tTerminationHandler> truetTerminationHandler
%type <yt_tTerminationHandler> tTerminationHandler
%type <yt_tThrow> tThrow
%type <yt_tTo> tTo
%type <yt_tToPart_list> tToPart_list
%type <yt_tToPart_list> tToParts
%type <yt_tToPart> tToPart
%type <yt_tValidate> tValidate
%type <yt_tVariable_list> tVariable_list
%type <yt_tVariable_list> tVariables
%type <yt_tVariable> tVariable
%type <yt_tWait> tWait
%type <yt_tWhile> tWhile




%%



/******************************************************************************
  PROCESS
******************************************************************************/

tProcess:
    {
      // initialisation (for multiple input files, i.e. `consistency' mode)
      frontend_lineno = 1;
      currentJoinCondition = standardJoinCondition();
      globals::temporaryAttributeMap.clear();
      globals::ASTEmap.clear();
      globals::ASTEid = 1;
      globals::ASTE_inputChannels.clear();
      globals::ASTE_outputChannels.clear();
      globals::ASTE_correlationSetNames.clear();
      globals::ASTE_variableNames.clear();
      globals::ASTE_partnerLinkNames.clear();
      globals::ASTE_linkNames.clear();
      globals::ASTE_startActivities.clear();
      globals::ASTE_linkIdMap.clear();
      globals::ASTE_scopeNames.clear();
      globals::ASTE_partnerLinks.clear();
      globals::joinCondition_links.clear();
      globals::joinCondition_linkStatus.clear();
      globals::joinCondition_result.clear();
      globals::parsing = true;
      globals::static_analysis_errors = 0;
      globals::other_errors = 0;
    }
  X_OPEN K_PROCESS arbitraryAttributes X_NEXT tExtensions imports tPartnerLinks
  tPartners tMessageExchanges tVariables tCorrelationSets tFaultHandlers
  tCompensationHandler tEventHandlers activity X_NEXT X_SLASH K_PROCESS X_CLOSE
    { globals::AST = $$ = Process($8, $9, $11, $12, $13, $15, $16, $4); }
;

/*---------------------------------------------------------------------------*/

activity:
  activity2
    { $$ = $1; }
| error activity2
    { $$ = $2;
      genericError("skipped <" + globals::last_error_token + ">: either non-standard element or misplaced", globals::last_error_line); }
;

activity2:
  tReceive		{ $$ = activityReceive($1);	}
| tReply		{ $$ = activityReply($1);	}
| tInvoke		{ $$ = activityInvoke($1);	}
| tAssign		{ $$ = activityAssign($1);	}
| tValidate		{ $$ = activityValidate($1);	}
| tEmpty		{ $$ = activityEmpty($1);	}
| tWait			{ $$ = activityWait($1);	}
| tExit			{ $$ = activityExit($1);	}
| tThrow		{ $$ = activityThrow($1);	}
| tRethrow		{ $$ = activityRethrow($1);	}
| tCompensate		{ $$ = activityCompensate($1);	}
| tCompensateScope	{ $$ = activityCompensate($1);	}
| tSequence		{ $$ = activitySequence($1);	}
| tIf			{ $$ = activityIf($1);		}
| tWhile		{ $$ = activityWhile($1);	}
| tRepeatUntil		{ $$ = activityRepeatUntil($1);	}
| tForEach		{ $$ = activityForEach($1);	}
| tFlow			{ $$ = activityFlow($1);	}
| tPick			{ $$ = activityPick($1);	}
| tScope		{ $$ = activityScope($1);	}
;

activity_list:
  activity X_NEXT
    { $$ = Consactivity_list($1, Nilactivity_list()); }
| activity X_NEXT activity_list
    { $$ = Consactivity_list($1, $3); }
;


/******************************************************************************
  EXTENSIONS AND IMPORTS
******************************************************************************/

imports:
  /* empty */
| K_IMPORT arbitraryAttributes X_SLASH X_NEXT imports
;

tExtensions:
  /* empty */
| K_EXTENSIONS X_NEXT tExtension_list X_SLASH K_EXTENSIONS X_NEXT
;

tExtension_list:
  tExtension X_NEXT
| tExtension X_NEXT tExtension_list
;

tExtension:
  K_EXTENSION arbitraryAttributes X_SLASH X_NEXT
| K_EXTENSION arbitraryAttributes X_NEXT X_SLASH K_EXTENSION
;


/******************************************************************************
  PARTNER LINKS
******************************************************************************/

tPartnerLinks:
  /* empty */
    { $$ = NiltPartnerLink_list(); }
| K_PARTNERLINKS arbitraryAttributes X_NEXT tPartnerLink_list X_SLASH K_PARTNERLINKS X_NEXT
    { $$ = $4; } 
;

tPartnerLink_list:
  tPartnerLink X_NEXT
    { $$ = ConstPartnerLink_list($1, NiltPartnerLink_list()); }
| tPartnerLink X_NEXT tPartnerLink_list
    { $$ = ConstPartnerLink_list($1, $3); }
;

tPartnerLink:
  K_PARTNERLINK arbitraryAttributes X_NEXT X_SLASH K_PARTNERLINK
    { $$ = PartnerLink($2); }
| K_PARTNERLINK arbitraryAttributes X_SLASH
    { $$ = PartnerLink($2); }
;


/******************************************************************************
  PARTNERS
******************************************************************************/

tPartners:
  /* empty */
    { $$ = NiltPartner_list(); }
| K_PARTNERS X_NEXT tPartner_list X_SLASH K_PARTNERS X_NEXT
    { $$ = $3; }
;

tPartner_list:
  tPartner X_NEXT
    { $$ = ConstPartner_list($1, NiltPartner_list()); }
| tPartner X_NEXT tPartner_list
    { $$ = ConstPartner_list($1, $3); }
;

tPartner:
  K_PARTNER arbitraryAttributes X_NEXT tPartnerLink_list X_SLASH K_PARTNER
    { $$ = Partner($4, $2); }
;


/******************************************************************************
  MESSAGE EXCHANGES
******************************************************************************/

tMessageExchanges:
  /* empty */
| K_MESSAGEEXCHANGES X_NEXT tMessageExchange_list X_SLASH K_MESSAGEEXCHANGES X_NEXT
;

tMessageExchange_list:
  tMessageExchange X_NEXT
| tMessageExchange X_NEXT tMessageExchange_list
;

tMessageExchange:
  K_MESSAGEEXCHANGE arbitraryAttributes X_NEXT X_SLASH K_MESSAGEEXCHANGE
| K_MESSAGEEXCHANGE arbitraryAttributes X_SLASH
;		





/******************************************************************************
  FAULT HANDLERS
******************************************************************************/

tFaultHandlers:
  /* empty */
    { $$ = volatile_standardFaultHandlers(mkinteger(0)); }
| K_FAULTHANDLERS X_NEXT tCatch_list tCatchAll X_SLASH K_FAULTHANDLERS X_NEXT
    { $$ = FaultHandlers($3, $4, mkinteger(0)); }
;

tCatch_list:
  /* empty */
    { $$ = NiltCatch_list(); }
| tCatch tCatch_list
    { $$ = ConstCatch_list($1, $2); }
;

tCatch:
  K_CATCH arbitraryAttributes X_NEXT activity X_NEXT X_SLASH K_CATCH X_NEXT
    { $$ = Catch($4, $2); }
;

tCatchAll:
  /* empty */
    { $$ = NoCatchAll(); }
| K_CATCHALL arbitraryAttributes X_NEXT activity X_NEXT X_SLASH K_CATCHALL X_NEXT
    { $$ = CatchAll($4, $2); }
;


/******************************************************************************
  COMPENSATION HANDLERS
******************************************************************************/

tCompensationHandler:
  /* empty */
    { $$ = volatile_standardCompensationHandler(mkinteger(0)); }
| K_COMPENSATIONHANDLER X_NEXT activity X_NEXT X_SLASH K_COMPENSATIONHANDLER X_NEXT
    { $$ = CompensationHandler($3, mkinteger(0)); }
;


/******************************************************************************
  TERMINATION HANDLER
******************************************************************************/

tTerminationHandler:
  /* empty */
    { $$ = volatile_standardTerminationHandler(mkinteger(0)); }
| K_TERMINATIONHANDLER X_NEXT activity X_NEXT X_SLASH K_TERMINATIONHANDLER X_NEXT
    { $$ = TerminationHandler($3, mkinteger(0)); }
;


/******************************************************************************
  EVENT HANDLERS
******************************************************************************/

tEventHandlers:
  /* empty */
    { $$ = emptyEventHandlers(mkinteger(0)); }
| K_EVENTHANDLERS X_NEXT tOnMessage_list tOnAlarm_list X_SLASH K_EVENTHANDLERS X_NEXT
    { $$ = EventHandlers($3, $4, mkinteger(0)); }
;

tOnMessage_list:
  /* empty */
    { $$ = NiltOnMessage_list(); }
| tOnMessage X_NEXT tOnMessage_list
    { $$ = ConstOnMessage_list($1, $3); }
| tOnEvent X_NEXT tOnMessage_list
    { $$ = ConstOnMessage_list($1, $3); }
;

tOnAlarm_list:
  /* empty */
    { $$ = NiltOnAlarm_list(); }
| tOnAlarm X_NEXT tOnAlarm_list
    { $$ = ConstOnAlarm_list($1, $3); }
;

tOnMessage:
  K_ONMESSAGE arbitraryAttributes X_NEXT tCorrelations tFromParts activity X_NEXT X_SLASH K_ONMESSAGE
    { $$ = OnMessage($4, $5, $6, $2); }
;

tOnEvent:
  K_ONEVENT arbitraryAttributes X_NEXT tCorrelations tFromParts activity X_NEXT X_SLASH K_ONEVENT
    { $$ = OnMessage($4, $5, $6, $2); }
;

tOnAlarm:
  K_ONALARM arbitraryAttributes X_NEXT tRepeatEvery activity X_NEXT X_SLASH K_ONALARM 
    { $$ = OnAlarm($4, $5, $2); }
| K_ONALARM arbitraryAttributes X_NEXT tFor tRepeatEvery activity X_NEXT X_SLASH K_ONALARM 
    { $$ = OnAlarm($5, $6, $2); }
| K_ONALARM arbitraryAttributes X_NEXT tUntil tRepeatEvery activity X_NEXT X_SLASH K_ONALARM 
    { $$ = OnAlarm($5, $6, $2); }
;

tRepeatEvery:
  /* empty */ 
  {
  $$ = emptyRepeatEvery(mkinteger(globals::ASTEid++));
  }
| K_REPEATEVERY arbitraryAttributes X_CLOSE constant X_OPEN X_SLASH K_REPEATEVERY X_NEXT
  {
  $$ = RepeatEvery(mkinteger(globals::ASTEid++));
  }
;





/******************************************************************************
  VARIABLES
******************************************************************************/

tVariables:
  /* empty */
    { $$ = NiltVariable_list(); }
| K_VARIABLES X_NEXT tVariable_list X_SLASH K_VARIABLES X_NEXT
    { $$ = $3; }
;

tVariable_list:
  tVariable X_NEXT
    { $$ = ConstVariable_list($1, NiltVariable_list()); }
| tVariable X_NEXT tVariable_list
    { $$ = ConstVariable_list($1, $3); }
;

tVariable:
  K_VARIABLE arbitraryAttributes X_NEXT X_SLASH K_VARIABLE
    { $$ = Variable($2); }
| K_VARIABLE arbitraryAttributes X_NEXT tFrom X_NEXT X_SLASH K_VARIABLE
    { $$ = Variable($2); }
| K_VARIABLE arbitraryAttributes X_SLASH
    { $$ = Variable($2); }
;


/******************************************************************************
  CORRELATION SETS
******************************************************************************/

tCorrelationSets:
  /* empty */
    { $$ = NiltCorrelationSet_list(); }
| K_CORRELATIONSETS X_NEXT tCorrelationSet_list X_SLASH K_CORRELATIONSETS X_NEXT
    { $$ = $3; }
;

tCorrelationSet_list:
  tCorrelationSet X_NEXT
    { $$ = ConstCorrelationSet_list($1, NiltCorrelationSet_list()); }
| tCorrelationSet X_NEXT tCorrelationSet_list
    { $$ = ConstCorrelationSet_list($1, $3); }
;

tCorrelationSet:
  K_CORRELATIONSET arbitraryAttributes X_NEXT X_SLASH K_CORRELATIONSET
    { $$ = CorrelationSet($2); }
| K_CORRELATIONSET arbitraryAttributes X_SLASH
    { $$ = CorrelationSet($2); }
;


/******************************************************************************
  CORRELATIONS
******************************************************************************/

tCorrelations:
  /* empty */
    { $$ = NiltCorrelation_list(); }
| K_CORRELATIONS X_NEXT tCorrelation_list X_SLASH K_CORRELATIONS X_NEXT
    { $$ = $3; }
;

tCorrelation_list:
  tCorrelation X_NEXT
    { $$ = ConstCorrelation_list($1, NiltCorrelation_list()); }
| tCorrelation X_NEXT tCorrelation_list
    { $$ = ConstCorrelation_list($1, $3); }
;

tCorrelation:
  K_CORRELATION arbitraryAttributes X_NEXT X_SLASH K_CORRELATION
    { $$ = Correlation($2); }
| K_CORRELATION arbitraryAttributes X_SLASH
    { $$ = Correlation($2); }
;


/******************************************************************************
  FROM & TO PARTS
******************************************************************************/

tToParts:
  /* empty */
    { $$ = NiltToPart_list(); }
| K_TOPARTS X_NEXT tToPart_list X_SLASH K_TOPARTS X_NEXT
    { $$ = $3; }
;

tToPart_list:
  tToPart X_NEXT
    { $$ = ConstToPart_list($1, NiltToPart_list()); }
| tToPart X_NEXT tToPart_list
    { $$ = ConstToPart_list($1, $3); }
;

tToPart:
  K_TOPART arbitraryAttributes X_NEXT X_SLASH K_TOPART
    { $$ = ToPart($2); }
| K_TOPART arbitraryAttributes X_SLASH
    { $$ = ToPart($2); }
;

tFromParts:
  /* empty */
    { $$ = NiltFromPart_list(); }
| K_FROMPARTS X_NEXT tFromPart_list X_SLASH K_FROMPARTS X_NEXT
    { $$ = $3; }
;

tFromPart_list:
  tFromPart X_NEXT
    { $$ = ConstFromPart_list($1, NiltFromPart_list()); }
| tFromPart X_NEXT tFromPart_list
    { $$ = ConstFromPart_list($1, $3); }
;

tFromPart:
  K_FROMPART arbitraryAttributes X_NEXT X_SLASH K_FROMPART
    { $$ = FromPart($2); }
| K_FROMPART arbitraryAttributes X_SLASH
    { $$ = FromPart($2); }
;





/******************************************************************************
  RECEIVE
******************************************************************************/

tReceive:
  K_RECEIVE arbitraryAttributes X_NEXT standardElements tCorrelations tFromParts X_SLASH K_RECEIVE
    { $$ = Receive($4, $5, $6, $2); }
| K_RECEIVE arbitraryAttributes X_SLASH
    { $$ = Receive(NoStandardElements(), NiltCorrelation_list(), NiltFromPart_list(), $2); }
;


/******************************************************************************
  REPLY
******************************************************************************/

tReply:
  K_REPLY arbitraryAttributes X_NEXT standardElements tCorrelations tToParts X_SLASH K_REPLY
    { $$ = Reply($4, $5, $6, $2); }
| K_REPLY arbitraryAttributes X_SLASH
    { $$ = Reply(NoStandardElements(), NiltCorrelation_list(), NiltToPart_list(), $2); }
;


/******************************************************************************
  INVOKE
******************************************************************************/

tInvoke:
  K_INVOKE arbitraryAttributes X_SLASH
    { $$ = volatile_Invoke(NoStandardElements(), NiltCorrelation_list(), NiltToPart_list(), NiltFromPart_list(), $2); }
| K_INVOKE arbitraryAttributes X_NEXT standardElements tCorrelations tCatch_list tCatchAll tCompensationHandler tToParts tFromParts X_SLASH K_INVOKE
    { $$ = volatile_annotatedInvoke($4, $5, $6, $7, $8, $9, $10, $2); }
;


/******************************************************************************
  ASSIGN
******************************************************************************/

tAssign:
  K_ASSIGN arbitraryAttributes X_NEXT standardElements tCopy_list X_SLASH K_ASSIGN
    { $$ = Assign($4, $5, $2); }
;

tCopy_list:
  tCopy X_NEXT
    { $$ = ConstCopy_list($1, NiltCopy_list()); }
| tCopy X_NEXT tCopy_list
    { $$ = ConstCopy_list($1, $3); }
;

tCopy:
  K_COPY arbitraryAttributes X_NEXT tFrom X_NEXT tTo X_NEXT X_SLASH K_COPY
    { $$ = Copy($4, $6, $2); }
; 

tFrom:
  K_FROM arbitraryAttributes X_NEXT X_SLASH K_FROM
    { $$ = From($2); }
| K_FROM arbitraryAttributes X_CLOSE constant X_OPEN X_SLASH K_FROM
    { $$ = From($2); }
| K_FROM arbitraryAttributes X_CLOSE VARIABLENAME X_OPEN X_SLASH K_FROM
    { $$ = From($2); }
| K_FROM arbitraryAttributes X_NEXT tLiteral X_NEXT X_SLASH K_FROM
    { $$ = From($2); }
| K_FROM arbitraryAttributes X_NEXT tQuery X_NEXT X_SLASH K_FROM
    { $$ = From($2); }
| K_FROM arbitraryAttributes X_SLASH
    { $$ = From($2); }
| K_FROM arbitraryAttributes error K_FROM
    { genericError("skipped <from> due to syntax error", toString(frontend_lineno-1));
      $$ = From($2); }
;

tLiteral:
  K_LITERAL X_CLOSE X_NAME X_OPEN X_SLASH K_LITERAL
    { $$ = $3; }
;

tQuery:
  K_QUERY arbitraryAttributes X_CLOSE X_NAME X_OPEN X_SLASH K_QUERY
;

tTo:
  K_TO arbitraryAttributes X_NEXT X_SLASH K_TO
    { $$ = To($2); }
| K_TO arbitraryAttributes X_CLOSE constant X_OPEN X_SLASH K_TO
    { $$ = To($2); }
| K_TO arbitraryAttributes X_CLOSE VARIABLENAME X_OPEN X_SLASH K_TO
    { $$ = To($2); }
| K_TO arbitraryAttributes X_NEXT tQuery X_NEXT X_SLASH K_TO
    { $$ = To($2); }
| K_TO arbitraryAttributes X_SLASH
    { $$ = To($2); }
;


/******************************************************************************
  VALIDATE
******************************************************************************/

tValidate:
  K_VALIDATE arbitraryAttributes X_NEXT standardElements X_SLASH K_VALIDATE
    { $$ = Validate($4, $2); }
| K_VALIDATE arbitraryAttributes X_SLASH
    { $$ = Validate(NoStandardElements(), $2); }
;


/******************************************************************************
  EMPTY
******************************************************************************/

tEmpty:
  K_EMPTY arbitraryAttributes X_NEXT standardElements X_SLASH K_EMPTY
    { $$ = Empty($4, $2); }
| K_EMPTY arbitraryAttributes X_SLASH
    { $$ = Empty(NoStandardElements(), $2); }
;


/******************************************************************************
  WAIT
******************************************************************************/

tWait:
  K_WAIT arbitraryAttributes X_NEXT standardElements X_SLASH K_WAIT
    { $$ = Wait($4, $2); }
| K_WAIT arbitraryAttributes X_NEXT tFor standardElements X_SLASH K_WAIT
    { $$ = Wait($5, $2); }
| K_WAIT arbitraryAttributes X_NEXT tUntil standardElements X_SLASH K_WAIT
    { $$ = Wait($5, $2); }
| K_WAIT arbitraryAttributes X_SLASH
    { $$ = Wait(NoStandardElements(), $2); }
;

tFor:
  K_FOR arbitraryAttributes X_CLOSE constant X_OPEN X_SLASH K_FOR X_NEXT
;

tUntil:
  K_UNTIL arbitraryAttributes X_CLOSE constant X_OPEN X_SLASH K_UNTIL X_NEXT
;


/******************************************************************************
  EXIT

  Both the <exit> activity (WS-BPEL 2.0) and the <terminate> (BPEL4WS 1.1)
  activity is parsed to phylum Exit().
******************************************************************************/

tExit:
  K_EXIT arbitraryAttributes X_NEXT standardElements X_SLASH K_EXIT
    { $$ = Exit($4, $2); }
| K_EXIT arbitraryAttributes X_SLASH
    { $$ = Exit(NoStandardElements(), $2); }
| K_TERMINATE arbitraryAttributes X_NEXT standardElements X_SLASH K_TERMINATE
    { $$ = Exit($4, $2); }
| K_TERMINATE arbitraryAttributes X_SLASH
    { $$ = Exit(NoStandardElements(), $2); }
;


/******************************************************************************
  THROW
******************************************************************************/

tThrow:
  K_THROW arbitraryAttributes X_NEXT standardElements X_SLASH K_THROW
    { $$ = Throw($4, $2); }
| K_THROW arbitraryAttributes X_SLASH
    { $$ = Throw(NoStandardElements(), $2); }
;


/******************************************************************************
  RETHROW
******************************************************************************/

tRethrow:
  K_RETHROW arbitraryAttributes X_NEXT standardElements X_SLASH K_RETHROW
    { $$ = Rethrow($4, $2); }
| K_RETHROW arbitraryAttributes X_SLASH
    { $$ = Rethrow(NoStandardElements(), $2); }
;


/******************************************************************************
  COMPENSATE AND COMPENSATESCOPE

  The <compensate> and <compensateScope> activities share the same phylum
  type as BPEL4WS 1.1 and WS-BPEL 2.0 have different syntactical definitions:
  A <compensate scope="a"> activity in BPEL4WS 1.1 is equivalent to an activity 
  <compensateScope target="a"> in WS-BPEL 2.0.
******************************************************************************/

tCompensate:
  K_COMPENSATE arbitraryAttributes X_NEXT standardElements X_SLASH K_COMPENSATE
    { if(globals::temporaryAttributeMap[$2->value]["scope"] == "")
        $$ = Compensate($4, $2);
      else
        $$ = CompensateScope($4, mkcasestring(globals::temporaryAttributeMap[$2->value]["scope"].c_str()), $2); }
| K_COMPENSATE arbitraryAttributes X_SLASH
    { if(globals::temporaryAttributeMap[$2->value]["scope"] == "")
        $$ = Compensate(NoStandardElements(), $2);
      else
        $$ = CompensateScope(NoStandardElements(), mkcasestring(globals::temporaryAttributeMap[$2->value]["scope"].c_str()), $2); }
;


tCompensateScope:
  K_COMPENSATESCOPE arbitraryAttributes X_NEXT standardElements X_SLASH K_COMPENSATESCOPE
    { $$ = CompensateScope($4, mkcasestring(globals::temporaryAttributeMap[$2->value]["target"].c_str()), $2); }
| K_COMPENSATESCOPE arbitraryAttributes X_SLASH
    { $$ = CompensateScope(NoStandardElements(), mkcasestring(globals::temporaryAttributeMap[$2->value]["target"].c_str()), $2); }
;


/******************************************************************************
  SEQUENCE
******************************************************************************/

tSequence:
  K_SEQUENCE arbitraryAttributes X_NEXT standardElements activity_list X_SLASH K_SEQUENCE
    { $$ = Sequence($4, $5, $2); }
;


/******************************************************************************
  IF / SWITCH
******************************************************************************/

tIf:
  K_IF arbitraryAttributes X_NEXT standardElements tCondition activity X_NEXT tElseIf_list tElse X_SLASH K_IF
    { $$ = If($4, ConstElseIf_list(ElseIf($6, mkinteger(0)), $8), $9, $2); }
| K_SWITCH arbitraryAttributes X_NEXT standardElements tCase_list tOtherwise X_SLASH K_SWITCH
    { $$ = If($4, $5, $6, $2); }
;

tCondition:
  K_CONDITION arbitraryAttributes error K_CONDITION X_NEXT
    { genericError("skipped <condition> due to syntax error", toString(frontend_lineno-1));
      $$ = mkcasestring(""); }
| K_CONDITION arbitraryAttributes X_CLOSE X_NAME X_OPEN X_SLASH K_CONDITION X_NEXT
    { $$ = $4; }
;	  

tElseIf_list:
  /* empty */
    { $$ = NiltElseIf_list(); }
| tElseIf tElseIf_list
    { $$ = ConstElseIf_list($1, $2); }
;

tElseIf:
  K_ELSEIF arbitraryAttributes X_NEXT tCondition activity X_NEXT X_SLASH K_ELSEIF X_NEXT
    { $$ = ElseIf($5, $2); }
;

tElse:
  /* empty */
    { $$ = NoElse(mkinteger(0)); }
| K_ELSE X_NEXT activity X_NEXT X_SLASH K_ELSE X_NEXT
    { $$ = Else($3, mkinteger(0)); }
;


tCase_list:
  tCase X_NEXT
    { $$ = ConstElseIf_list($1, NiltElseIf_list()); }
| tCase X_NEXT tCase_list
    { $$ = ConstElseIf_list($1, $3); }
;

tCase:
  K_CASE arbitraryAttributes X_NEXT activity X_NEXT X_SLASH K_CASE
    { $$ = ElseIf($4, $2); }
;


tOtherwise:
  /* empty */
    { $$ = Else(activityEmpty(Empty(NoStandardElements(), mkinteger(0))), mkinteger(0)); }
| K_OTHERWISE arbitraryAttributes X_SLASH // wrong BPEL, yet widely-used :-( ...
    { $$ = Else(activityEmpty(Empty(NoStandardElements(), mkinteger(0))), mkinteger(0)); }
| K_OTHERWISE arbitraryAttributes X_NEXT activity X_NEXT X_SLASH K_OTHERWISE X_NEXT
    { $$ = Else($4, $2); }
;


/******************************************************************************
  WHILE
******************************************************************************/

tWhile:
  K_WHILE arbitraryAttributes X_NEXT standardElements activity X_NEXT X_SLASH K_WHILE
    { $$ = While($4, $5, $2); }
| K_WHILE arbitraryAttributes X_NEXT standardElements tCondition activity X_NEXT X_SLASH K_WHILE
    {
      $$ = While($4, $6, $2);
      // "copy" condition to attributes      
      assert(globals::ASTEmap[$2->value] != NULL);
      globals::ASTEmap[$2->value]->attributes["condition"] = $5->name;
    }
;


/******************************************************************************
  REPEATUNTIL
******************************************************************************/

tRepeatUntil:
  K_REPEATUNTIL arbitraryAttributes X_NEXT standardElements activity X_NEXT tCondition X_SLASH K_REPEATUNTIL
    {
      $$ = RepeatUntil($4, $5, $2);
      // "copy" condition to attributes      
      assert(globals::ASTEmap[$2->value] != NULL);
      globals::ASTEmap[$2->value]->attributes["condition"] = $7->name;
    }
;


/******************************************************************************
  FOREACH
******************************************************************************/

tForEach:
  K_FOREACH arbitraryAttributes X_NEXT standardElements tStartCounterValue tFinalCounterValue tCompletionCondition tScope X_NEXT X_SLASH K_FOREACH
    { $$ = ForEach($4, $5, $6, $7, $8, $2); }
;

tStartCounterValue:
  K_STARTCOUNTERVALUE arbitraryAttributes X_CLOSE constant X_OPEN X_SLASH K_STARTCOUNTERVALUE X_NEXT
    { $$ = $4; }
;

tFinalCounterValue:
  K_FINALCOUNTERVALUE arbitraryAttributes X_CLOSE constant X_OPEN X_SLASH K_FINALCOUNTERVALUE X_NEXT
    { $$ = $4; }
;

tCompletionCondition:
  /* empty */
    { $$ = mkcasestring(""); }
| K_COMPLETIONCONDITION X_NEXT tBranches X_SLASH K_COMPLETIONCONDITION X_NEXT
    { $$ = $3; }
;

tBranches:
  K_BRANCHES arbitraryAttributes X_CLOSE constant X_OPEN X_SLASH K_BRANCHES X_NEXT
    { $$ = $4; }
;


/******************************************************************************
  PICK
******************************************************************************/

tPick:
  K_PICK arbitraryAttributes X_NEXT standardElements tOnMessage X_NEXT tOnMessage_list tOnAlarm_list X_SLASH K_PICK
    { $$ = Pick($4, ConstOnMessage_list($5, $7), $8, $2); }
;


/******************************************************************************
  FLOW
******************************************************************************/

tFlow:
  K_FLOW arbitraryAttributes X_NEXT standardElements tLinks activity_list X_SLASH K_FLOW
    { $$ = Flow($4, $5, $6, $2); }
;

tLinks:
  /* empty */
    { $$ = NiltLink_list(); }
| K_LINKS arbitraryAttributes X_NEXT tLink_list X_SLASH K_LINKS X_NEXT
    { $$ = $4; }
;

tLink_list:
  tLink X_NEXT
    { $$ = ConstLink_list($1, NiltLink_list()); }
| tLink X_NEXT tLink_list
    { $$ = ConstLink_list($1, $3); }
;

tLink:
  K_LINK arbitraryAttributes X_NEXT X_SLASH K_LINK
    { $$ = Link($2); }
| K_LINK arbitraryAttributes X_SLASH
    { $$ = Link($2); }
;


/******************************************************************************
  SCOPE
******************************************************************************/

/* HELPERS */

truetTerminationHandler:
  K_TERMINATIONHANDLER X_NEXT activity X_NEXT X_SLASH K_TERMINATIONHANDLER X_NEXT
    { $$ = TerminationHandler($3, mkinteger(0)); }
;

truetCompensationHandler:
  K_COMPENSATIONHANDLER X_NEXT activity X_NEXT X_SLASH K_COMPENSATIONHANDLER X_NEXT
    { $$ = CompensationHandler($3, mkinteger(0)); }
;

truetFaultHandlers:
  K_FAULTHANDLERS X_NEXT tCatch_list tCatchAll X_SLASH K_FAULTHANDLERS X_NEXT
    { $$ = FaultHandlers($3, $4, mkinteger(0)); }
;

truetEventHandlers:
  K_EVENTHANDLERS X_NEXT tOnMessage_list tOnAlarm_list X_SLASH K_EVENTHANDLERS X_NEXT
    { $$ = EventHandlers($3, $4, mkinteger(0)); }
;

truetPartnerLinks:
  K_PARTNERLINKS arbitraryAttributes X_NEXT tPartnerLink_list X_SLASH K_PARTNERLINKS X_NEXT
    { $$ = $4; } 
;

truetMessageExchanges:
  K_MESSAGEEXCHANGES X_NEXT tMessageExchange_list X_SLASH K_MESSAGEEXCHANGES X_NEXT
;

/* ALTERNATIVE */

tScope:
  K_SCOPE arbitraryAttributes X_NEXT standardElements tVariables
  tCorrelationSets activity 
  X_NEXT X_SLASH K_SCOPE
    { $$ = Scope($4, $5, volatile_standardFaultHandlers(mkinteger(0)), volatile_standardCompensationHandler(mkinteger(0)), volatile_standardTerminationHandler(mkinteger(0)), emptyEventHandlers(mkinteger(0)), $6, $7, NiltPartnerLink_list(), $2); }
| K_SCOPE arbitraryAttributes X_NEXT standardElements tVariables
  tCorrelationSets truetFaultHandlers activity 
  X_NEXT X_SLASH K_SCOPE
    { $$ = Scope($4, $5, $7, volatile_standardCompensationHandler(mkinteger(0)), volatile_standardTerminationHandler(mkinteger(0)), emptyEventHandlers(mkinteger(0)), $6, $8, NiltPartnerLink_list(), $2); }
| K_SCOPE arbitraryAttributes X_NEXT standardElements tVariables
  tCorrelationSets truetCompensationHandler activity 
  X_NEXT X_SLASH K_SCOPE
    { $$ = Scope($4, $5, volatile_standardFaultHandlers(mkinteger(0)), $7, volatile_standardTerminationHandler(mkinteger(0)), emptyEventHandlers(mkinteger(0)), $6, $8, NiltPartnerLink_list(), $2); }
| K_SCOPE arbitraryAttributes X_NEXT standardElements tVariables
  tCorrelationSets truetEventHandlers activity 
  X_NEXT X_SLASH K_SCOPE
    { $$ = Scope($4, $5, volatile_standardFaultHandlers(mkinteger(0)), volatile_standardCompensationHandler(mkinteger(0)), volatile_standardTerminationHandler(mkinteger(0)), $7, $6, $8, NiltPartnerLink_list(), $2); }
| K_SCOPE arbitraryAttributes X_NEXT standardElements tVariables
  tCorrelationSets truetFaultHandlers 
  truetEventHandlers activity 
  X_NEXT X_SLASH K_SCOPE
    { $$ = Scope($4, $5, $7, volatile_standardCompensationHandler(mkinteger(0)), volatile_standardTerminationHandler(mkinteger(0)), $8, $6, $9, NiltPartnerLink_list(), $2); }
| K_SCOPE arbitraryAttributes X_NEXT standardElements tVariables
  tCorrelationSets truetCompensationHandler 
  truetEventHandlers activity 
  X_NEXT X_SLASH K_SCOPE
    { $$ = Scope($4, $5, volatile_standardFaultHandlers(mkinteger(0)), $7, volatile_standardTerminationHandler(mkinteger(0)), $8, $6, $9, NiltPartnerLink_list(), $2); }
| K_SCOPE arbitraryAttributes X_NEXT standardElements tVariables
  tCorrelationSets truetFaultHandlers truetCompensationHandler
  truetEventHandlers activity 
  X_NEXT X_SLASH K_SCOPE
    { $$ = Scope($4, $5, $7, $8, volatile_standardTerminationHandler(mkinteger(0)), $9, $6, $10, NiltPartnerLink_list(), $2); }
| K_SCOPE arbitraryAttributes X_NEXT standardElements tVariables
  tCorrelationSets truetFaultHandlers truetCompensationHandler
  activity 
  X_NEXT X_SLASH K_SCOPE
    { $$ = Scope($4, $5, $7, $8, volatile_standardTerminationHandler(mkinteger(0)), emptyEventHandlers(mkinteger(0)), $6, $9, NiltPartnerLink_list(), $2); }
| K_SCOPE arbitraryAttributes X_NEXT standardElements tVariables truetPartnerLinks 
  tMessageExchanges tCorrelationSets tEventHandlers tFaultHandlers
  tCompensationHandler tTerminationHandler activity 
  X_NEXT X_SLASH K_SCOPE
    { $$ = Scope($4, $5, $10, $11, $12, $9, $8, $13, $6, $2); }
| K_SCOPE arbitraryAttributes X_NEXT standardElements tVariables 
  truetMessageExchanges tCorrelationSets tEventHandlers tFaultHandlers
  tCompensationHandler tTerminationHandler activity 
  X_NEXT X_SLASH K_SCOPE
    { $$ = Scope($4, $5, $9, $10, $11, $8, $7, $12, NiltPartnerLink_list(), $2); }
| K_SCOPE arbitraryAttributes X_NEXT standardElements tVariables 
  tCorrelationSets truetEventHandlers truetFaultHandlers
  tCompensationHandler tTerminationHandler activity 
  X_NEXT X_SLASH K_SCOPE
    { $$ = Scope($4, $5, $8, $9, $10, $7, $6, $11, NiltPartnerLink_list(), $2); }
| K_SCOPE arbitraryAttributes X_NEXT standardElements tVariables 
  tCorrelationSets truetEventHandlers truetCompensationHandler tTerminationHandler activity 
  X_NEXT X_SLASH K_SCOPE
    { $$ = Scope($4, $5, volatile_standardFaultHandlers(mkinteger(0)), $8, $9, $7, $6, $10, NiltPartnerLink_list(), $2); }
| K_SCOPE arbitraryAttributes X_NEXT standardElements tVariables 
  tCorrelationSets truetFaultHandlers
  truetCompensationHandler truetTerminationHandler activity 
  X_NEXT X_SLASH K_SCOPE
    { $$ = Scope($4, $5, $7, $8, $9, emptyEventHandlers(mkinteger(0)), $6, $10, NiltPartnerLink_list(), $2); }
| K_SCOPE arbitraryAttributes X_NEXT standardElements tVariables 
  tCorrelationSets truetEventHandlers truetTerminationHandler activity 
  X_NEXT X_SLASH K_SCOPE
    { $$ = Scope($4, $5, volatile_standardFaultHandlers(mkinteger(0)), volatile_standardCompensationHandler(mkinteger(0)), $8, $7, $6, $9, NiltPartnerLink_list(), $2); }
| K_SCOPE arbitraryAttributes X_NEXT standardElements tVariables 
  tCorrelationSets truetFaultHandlers truetTerminationHandler activity 
  X_NEXT X_SLASH K_SCOPE
    { $$ = Scope($4, $5, $7, volatile_standardCompensationHandler(mkinteger(0)), $8, emptyEventHandlers(mkinteger(0)), $6, $9, NiltPartnerLink_list(), $2); }
| K_SCOPE arbitraryAttributes X_NEXT standardElements tVariables 
  tCorrelationSets truetCompensationHandler truetTerminationHandler activity 
  X_NEXT X_SLASH K_SCOPE
    { $$ = Scope($4, $5, volatile_standardFaultHandlers(mkinteger(0)), $7, $8, emptyEventHandlers(mkinteger(0)), $6, $9, NiltPartnerLink_list(), $2); }
;





/******************************************************************************
  ATTRIBUTES

  Most BPEL activities can have attributes which (due to the extensibility of
  the language) can be pretty arbitrary. As the attributes are parsed before
  an AST phylum is generated they can be not "pinned" to a phylum. Therefore,
  attributes are stored in the mapping `temporaryAttributeMap' and are
  indexed by an unsigned integer `ASTEid'. This identifier is passed to the
  activity and used to identify the generated phylum. In the postrocessing
  steps, the attributes are copied from the `temporaryAttributeMap' to the
  phylum.
******************************************************************************/

arbitraryAttributes:
  /* empty */
     { $$ = mkinteger(globals::ASTEid++); // generate a new id
     }
| X_NAME
     { if (globals::temporaryAttributeMap[globals::ASTEid]["referenceLine"] == "")
         globals::temporaryAttributeMap[globals::ASTEid]["referenceLine"] = toString(frontend_lineno); // remember the file position       
     }
  X_EQUALS X_STRING arbitraryAttributes
     { globals::temporaryAttributeMap[$5->value][$1->name] = $4->name;
       $$ = $5; }
| joinCondition arbitraryAttributes
     { $$ = $2; }
;





/******************************************************************************
  STANDARD ELEMENTS
******************************************************************************/

standardElements:
  tTarget_list tSource_list
    { $$ = StandardElements($1, $2, currentJoinCondition);
      currentJoinCondition = standardJoinCondition(); }
| K_TARGETS X_NEXT tJoinCondition tTarget X_NEXT tTarget_list X_SLASH K_TARGETS X_NEXT
    { $$ = StandardElements(ConstTarget_list($4, $6), NiltSource_list(), currentJoinCondition);
      currentJoinCondition = standardJoinCondition(); }
| K_SOURCES X_NEXT tJoinCondition tSource X_NEXT tSource_list X_SLASH K_SOURCES X_NEXT
    { $$ = StandardElements(NiltTarget_list(), ConstSource_list($4, $6), currentJoinCondition);
      currentJoinCondition = standardJoinCondition(); }
| K_TARGETS X_NEXT tJoinCondition tTarget X_NEXT tTarget_list X_SLASH K_TARGETS X_NEXT K_SOURCES X_NEXT tSource X_NEXT tSource_list X_SLASH K_SOURCES X_NEXT
    { $$ = StandardElements(ConstTarget_list($4, $6), ConstSource_list($12, $14), currentJoinCondition);
      currentJoinCondition = standardJoinCondition(); }
;

tTarget_list:
  /* empty */
    { $$ = NiltTarget_list(); }
| tTarget X_NEXT tTarget_list
    { $$ = ConstTarget_list($1, $3); }
;

tTarget:
  K_TARGET arbitraryAttributes X_NEXT X_SLASH K_TARGET
    { $$ = Target($2); }
| K_TARGET arbitraryAttributes X_SLASH
    { $$ = Target($2); }
;

tSource_list:
  /* empty */
    { $$ = NiltSource_list(); }
| tSource X_NEXT tSource_list
    { $$ = ConstSource_list($1, $3); }
;

tSource:
  K_SOURCE arbitraryAttributes X_NEXT tTransitionCondition X_SLASH K_SOURCE
    { $$ = Source($2); }
| K_SOURCE arbitraryAttributes X_SLASH
    { $$ = Source($2); }
;

tTransitionCondition:
  /* empty */
| K_TRANSITIONCONDITION X_CLOSE transitionCondition X_OPEN X_SLASH K_TRANSITIONCONDITION X_NEXT
;

tJoinCondition:
  /* empty */
    { $$ = standardJoinCondition(); }
| K_JOINCONDITION X_CLOSE booleanLinkCondition X_OPEN X_SLASH K_JOINCONDITION X_NEXT
    { $$ = currentJoinCondition = userDefinedJoinCondition($3); }
;

joinCondition: /* a join condition as attribute */
  K_JOINCONDITION X_EQUALS booleanLinkCondition
    { currentJoinCondition = userDefinedJoinCondition($3);
      currentJoinCondition->print(); }
| K_JOINCONDITION X_EQUALS X_STRING
    { cerr << "ignoring given join condition: \"" << $3->name << "\"" << endl; }
;

booleanLinkCondition:
  K_GETLINKSTATUS LBRACKET APOSTROPHE X_NAME APOSTROPHE RBRACKET
    { $$ = Term($4); }
| VARIABLENAME
    { $$ = Term($1); }
| LBRACKET booleanLinkCondition K_AND booleanLinkCondition RBRACKET
    { $$ = Conjunction($2, $4); }
| LBRACKET booleanLinkCondition K_OR booleanLinkCondition RBRACKET
    { $$ = Disjunction($2, $4); }
| booleanLinkCondition K_AND booleanLinkCondition
    { $$ = Conjunction($1, $3); }
| booleanLinkCondition K_OR booleanLinkCondition
    { $$ = Disjunction($1, $3); }
;

transitionCondition:
  VARIABLENAME comparison constant
;

comparison:
  GREATEROREQUAL
| GREATER
| LESS
| LESSOREQUAL
| EQUAL
| NOTEQUAL
;

constant:
  X_NAME
    { $$ = $1; }
| APOSTROPHE X_NAME APOSTROPHE
    { $$ = $2; }
| NUMBER
    { $$ = $1; }
;
