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

%{
/*!
 * \file bpel-syntax.cc
 *
 * \brief BPEL grammar (implementation)
 *
 * This file defines and implements the grammar of BPEL using standard 
 * BNF-rules to describe the originally XML-based syntax as it is specified in
 * the BPEL4WS 1.1 specification. All terminals are passed from the lexer
 * (implemented in \ref bpel-lexic.cc). Besides simple syntax-checking the
 * grammar is used to build the abstract syntax tree as it is defined in
 * bpel-abstract.k and implemented in \ref bpel-abstract.cc and
 * \ref bpel-abstract.h.
 * 
 * \author  
 *          - responsible: Niels Lohmann <nlohmann@informatik.hu-berlin.de>
 *          - last changes of: \$Author: nlohmann $
 *          
 * \date 
 *          - created: 2005/11/10
 *          - last changed: \$Date: 2006/10/04 08:26:36 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universit�t zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using GNU Bison reading file bpel-syntax.yy.
 *          See http://www.gnu.org/software/bison/bison.html for details
 *
 * \version \$Revision: 1.220 $
 * 
 */
%}


%{	
/*!
 * \file bpel-syntax.h
 * \brief BPEL grammar (interface)
 *
 * See \ref bpel-syntax.cc for more information.
 */
%}



// the terminal symbols (tokens)
%token K_ASSIGN K_CASE K_CATCH K_CATCHALL K_COMPENSATE K_COMPENSATIONHANDLER
%token K_COPY K_CORRELATION K_CORRELATIONS K_CORRELATIONSET K_CORRELATIONSETS
%token K_EMPTY K_EVENTHANDLERS K_FAULTHANDLERS K_FLOW K_FROM K_IMPORT K_INVOKE
%token K_LINK K_LINKS K_ONALARM K_ONMESSAGE K_OTHERWISE K_PARTNER K_PARTNERLINK
%token K_PARTNERLINKS K_PARTNERS K_PICK K_PROCESS K_RECEIVE K_REPLY K_SCOPE
%token K_SEQUENCE K_SOURCE K_SWITCH K_TARGET K_TERMINATE K_THROW K_TO
%token K_VARIABLE K_VARIABLES K_WAIT K_WHILE
%token X_OPEN X_SLASH X_CLOSE X_NEXT X_EQUALS QUOTE
%token K_JOINCONDITION K_GETLINKSTATUS RBRACKET LBRACKET APOSTROPHE K_AND K_OR
%token <yt_casestring> X_NAME
%token <yt_casestring> X_STRING


// the start symbol of the grammar
%start tProcess


// Bison generates a list of all used tokens in file "bpel-syntax.h" (for flex)
%token_table

%defines
%yacc



%{



// options for Bison
#define YYDEBUG 1
#define YYERROR_VERBOSE 1  // for verbose error messages


// to avoid the message "parser stack overflow"
#define YYMAXDEPTH 1000000
#define YYINITDEPTH 10000


#include <assert.h>
#include <map>
#include <iostream> // for cerr

#include "bpel-kc-k.h" // phylum definitions
#include "bpel-kc-yystype.h" // data types for tokens and non-terminals
#include "helpers.h"
//#include "symbol-table.h"
#include "ast-details.h"


using namespace kc;
using namespace std;


/******************************************************************************
 * External variables
 *****************************************************************************/

// from flex
extern char* yytext;
extern int yylex();
extern int yylineno;

// defined in "debug.h"
extern int yyerror(const char *);



/// the root of the abstract syntax tree
tProcess TheProcess;

/// a pointer to the current join condition
impl_joinCondition* currentJoinCondition = standardJoinCondition();

// Niels' stuff
map<unsigned int, ASTE*> ASTEmap; ///< the map of all AST elements
map<unsigned int, map<string, string> > temporaryAttributeMap; ///< a temporary mapping of attributs
unsigned int ASTEid = 1;

%}



/* the types of the non-terminal symbols */
%type <yt_activity_list> activity_list
%type <yt_activity> activity
%type <yt_integer> arbitraryAttributes
%type <yt_tAssign> tAssign
%type <yt_tCase_list> tCase_list
%type <yt_tCase> tCase
%type <yt_tCatch_list> tCatch_list
%type <yt_tCatch> tCatch
%type <yt_tCatchAll> tCatchAll
%type <yt_tCompensate> tCompensate
%type <yt_tCompensationHandler> tCompensationHandler
%type <yt_tCopy_list> tCopy_list
%type <yt_tCopy> tCopy
%type <yt_tCorrelation_list> tCorrelation_list
%type <yt_tCorrelation_list> tCorrelations
%type <yt_tCorrelation> tCorrelation
%type <yt_tCorrelationSet_list> tCorrelationSet_list
%type <yt_tCorrelationSet_list> tCorrelationSets
%type <yt_tCorrelationSet> tCorrelationSet
%type <yt_tEmpty> tEmpty
%type <yt_tEventHandlers> tEventHandlers
%type <yt_tFaultHandlers> tFaultHandlers
%type <yt_tFlow> tFlow
%type <yt_tFrom> tFrom
%type <yt_tInvoke> tInvoke
%type <yt_tLink_list> tLink_list
%type <yt_tLink_list> tLinks
%type <yt_tLink> tLink
%type <yt_tOnAlarm_list> tOnAlarm_list
%type <yt_tOnAlarm> tOnAlarm
%type <yt_tOnMessage_list> tOnMessage_list
%type <yt_tOnMessage> tOnMessage
%type <yt_tOtherwise> tOtherwise
%type <yt_tPartner_list> tPartner_list
%type <yt_tPartner_list> tPartners
%type <yt_tPartner> tPartner
%type <yt_tPartnerLink_list> tPartnerLink_list
%type <yt_tPartnerLink_list> tPartnerLinks
%type <yt_tPartnerLink> tPartnerLink
%type <yt_tPick> tPick
%type <yt_tProcess> tProcess
%type <yt_tReceive> tReceive
%type <yt_tReply> tReply
%type <yt_tScope> tScope
%type <yt_tSequence> tSequence
%type <yt_tSource_list> tSource_list
%type <yt_tSource> tSource
%type <yt_standardElements> standardElements
%type <yt_tSwitch> tSwitch
%type <yt_tTarget_list> tTarget_list
%type <yt_tTarget> tTarget
%type <yt_tTerminate> tTerminate
%type <yt_tThrow> tThrow
%type <yt_tTo> tTo
%type <yt_tVariable_list> tVariable_list
%type <yt_tVariable_list> tVariables
%type <yt_tVariable> tVariable
%type <yt_tWait> tWait
%type <yt_tWhile> tWhile
%type <yt_expression> booleanLinkCondition



%%



/******************************************************************************
  PROCESS
******************************************************************************/

tProcess:
    {
      // initialisation (for multiple input files)
      yylineno = 0;
      ASTEid = 1;
      currentJoinCondition = standardJoinCondition();
      temporaryAttributeMap.clear();
    }
  X_OPEN K_PROCESS arbitraryAttributes X_NEXT imports
  tPartnerLinks tPartners tVariables tCorrelationSets tFaultHandlers tCompensationHandler tEventHandlers activity
  X_NEXT X_SLASH K_PROCESS X_CLOSE
    {
      TheProcess = $$ = Process($7, $8, $9, $10, $11, $12, $13, StopInProcess(), $14);
      $$->id = $4->value;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_PROCESS);
    }
;

/* import other namespaces */
imports:
  /* empty */
| K_IMPORT arbitraryAttributes X_SLASH X_NEXT imports
;

/*---------------------------------------------------------------------------*/


activity:
  tEmpty	{ $$ = activityEmpty($1);	$$->id = $1->id; }
| tInvoke	{ $$ = activityInvoke($1);	$$->id = $1->id; }
| tReceive	{ $$ = activityReceive($1);	$$->id = $1->id; }
| tReply	{ $$ = activityReply($1);	$$->id = $1->id; }
| tAssign	{ $$ = activityAssign($1);	$$->id = $1->id; }
| tWait		{ $$ = activityWait($1);	$$->id = $1->id; }
| tThrow	{ $$ = activityThrow($1);	$$->id = $1->id; }
| tTerminate	{ $$ = activityTerminate($1);	$$->id = $1->id; }
| tFlow		{ $$ = activityFlow($1);	$$->id = $1->id; }
| tSwitch	{ $$ = activitySwitch($1);	$$->id = $1->id; }
| tWhile	{ $$ = activityWhile($1);	$$->id = $1->id; }
| tSequence	{ $$ = activitySequence($1);	$$->id = $1->id; }
| tPick		{ $$ = activityPick($1);	$$->id = $1->id; }
| tScope	{ $$ = activityScope($1);	$$->id = $1->id; }
| tCompensate	{ $$ = activityCompensate($1);	$$->id = $1->id; }
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
    { $$ = PartnerLink();
      $$->id = $2->value;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_PARTNERLINK); }
| K_PARTNERLINK arbitraryAttributes X_SLASH
    { $$ = PartnerLink();
      $$->id = $2->value;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_PARTNERLINK); }
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
    { $$ = Partner($4);
      $$->id = $2->value; }
| K_PARTNER arbitraryAttributes X_SLASH
    { $$ = Partner(NiltPartnerLink_list());
      $$->id = $2->value; }
;


/******************************************************************************
  FAULT HANDLERS
******************************************************************************/

tFaultHandlers:
  /* empty */
    { $$ = implicitFaultHandler();
      $$->id = ASTEid++;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_FAULTHANDLERS); }
| K_FAULTHANDLERS X_NEXT tCatch_list tCatchAll X_SLASH K_FAULTHANDLERS X_NEXT
    { $$ = userDefinedFaultHandler($3, $4);
      $$->id = ASTEid++;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_FAULTHANDLERS); }
;

tCatch_list:
  /* empty */
    { $$ = NiltCatch_list(); }
| tCatch X_NEXT tCatch_list
    { $$ = ConstCatch_list($1, $3); }
;

tCatch:
  K_CATCH arbitraryAttributes X_NEXT activity X_NEXT X_SLASH K_CATCH
    { $$ = Catch($4);
      $$->id = $2->value;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_CATCH); }
;

tCatchAll:
  /* empty */
    { $$ = NoCatchAll(); }
| K_CATCHALL arbitraryAttributes X_NEXT activity X_NEXT X_SLASH K_CATCHALL X_NEXT
    { $$ = CatchAll($4);
      $$->id = $2->value;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_CATCHALL); }
;


/******************************************************************************
  COMPENSATION HANDLERS
******************************************************************************/

tCompensationHandler:
  /* empty */
    { $$ = implicitCompensationHandler();
      $$->id = ASTEid++;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_COMPENSATIONHANDLER); }
| K_COMPENSATIONHANDLER X_NEXT activity X_NEXT X_SLASH K_COMPENSATIONHANDLER X_NEXT
    { $$ = userDefinedCompensationHandler($3);
      $$->id = ASTEid++;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_COMPENSATIONHANDLER); }
;


/******************************************************************************
  EVENT HANDLERS
******************************************************************************/

tEventHandlers:
  /* empty */
    { $$ = implicitEventHandler();
      $$->id = ASTEid++;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_EVENTHANDLERS); }
| K_EVENTHANDLERS X_NEXT tOnMessage_list tOnAlarm_list X_SLASH K_EVENTHANDLERS X_NEXT
    { $$ = userDefinedEventHandler($3, $4);
      $$->id = ASTEid++;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_EVENTHANDLERS); }
;

tOnMessage_list:
  /* empty */
    { $$ = NiltOnMessage_list(); }
| tOnMessage X_NEXT tOnMessage_list
    { $$ = ConstOnMessage_list($1, $3); }
;

tOnAlarm_list:
  /* empty */
    { $$ = NiltOnAlarm_list(); }
| tOnAlarm X_NEXT tOnAlarm_list
    { $$ = ConstOnAlarm_list($1, $3); }
;

tOnMessage:
  K_ONMESSAGE arbitraryAttributes X_NEXT tCorrelations activity X_NEXT X_SLASH K_ONMESSAGE
    { $$ = OnMessage($5);
      $$->id = $2->value;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_ONMESSAGE); }
;

tOnAlarm:
  K_ONALARM arbitraryAttributes X_NEXT activity X_NEXT X_SLASH K_ONALARM 
    { $$ = OnAlarm($4);
      $$->id = $2->value;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_ONALARM); }
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
    { $$ = Variable();
      $$->id = $2->value;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_VARIABLE); }
| K_VARIABLE arbitraryAttributes X_SLASH
    { $$ = Variable();
      $$->id = $2->value;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_VARIABLE); }
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
    { $$ = CorrelationSet();
      $$->id = $2->value;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_CORRELATIONSET); }
| K_CORRELATIONSET arbitraryAttributes X_SLASH
    { $$ = CorrelationSet();
      $$->id = $2->value;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_CORRELATIONSET); }
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
    { $$ = Correlation();
      $$->id = $2->value;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_CORRELATION); }
| K_CORRELATION arbitraryAttributes X_SLASH
    { $$ = Correlation();
      $$->id = $2->value;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_CORRELATION); }
;


/******************************************************************************
  EMPTY
******************************************************************************/

tEmpty:
  K_EMPTY arbitraryAttributes X_NEXT standardElements X_SLASH K_EMPTY
    { $$ = Empty($4);
      $$->id = $2->value;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_EMPTY); }
| K_EMPTY arbitraryAttributes X_SLASH
    { impl_standardElements_StandardElements *noLinks = StandardElements(NiltTarget_list(), NiltSource_list(), standardJoinCondition());
      $$ = Empty(noLinks);
      $$->id = $2->value;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_EMPTY); }
;


/******************************************************************************
  INVOKE
******************************************************************************/

tInvoke:
  K_INVOKE arbitraryAttributes X_NEXT
  standardElements tCorrelations //tCatch_list tCatchAll tCompensationHandler
  X_SLASH K_INVOKE
    { $$ = Invoke($4, $5);
      $$->id = $2->value;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_INVOKE); }
| K_INVOKE arbitraryAttributes X_SLASH
    { impl_standardElements_StandardElements *noLinks = StandardElements(NiltTarget_list(), NiltSource_list(), standardJoinCondition());
      $$ = Invoke(noLinks, NiltCorrelation_list());
      $$->id = $2->value;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_INVOKE); }
;


/******************************************************************************
  RECEIVE
******************************************************************************/

tReceive:
  K_RECEIVE arbitraryAttributes X_NEXT standardElements tCorrelations X_SLASH K_RECEIVE
    { $$ = Receive($4, $5);
      $$->id = $2->value;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_RECEIVE); }
| K_RECEIVE arbitraryAttributes X_SLASH
    { impl_standardElements_StandardElements *noLinks = StandardElements(NiltTarget_list(), NiltSource_list(), standardJoinCondition());
      $$ = Receive(noLinks, NiltCorrelation_list());
      $$->id = $2->value;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_RECEIVE); }
;


/******************************************************************************
  REPLY
******************************************************************************/

tReply:
  K_REPLY arbitraryAttributes X_NEXT standardElements tCorrelations X_SLASH K_REPLY
    { $$ = Reply($4, $5);
      $$->id = $2->value;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_REPLY); }
| K_REPLY arbitraryAttributes X_SLASH
    { impl_standardElements_StandardElements *noLinks = StandardElements(NiltTarget_list(), NiltSource_list(), standardJoinCondition());
      $$ = Reply(noLinks, NiltCorrelation_list());
      $$->id = $2->value;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_REPLY); }
;


/******************************************************************************
  ASSIGN
******************************************************************************/

tAssign:
  K_ASSIGN arbitraryAttributes X_NEXT standardElements tCopy_list X_SLASH K_ASSIGN
    { $$ = Assign($4, $5);
      $$->id = $2->value;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_ASSIGN); }
;

tCopy_list:
  tCopy X_NEXT
    { $$ = ConstCopy_list($1, NiltCopy_list()); }
| tCopy X_NEXT tCopy_list
    { $$ = ConstCopy_list($1, $3); }
;

tCopy:
  K_COPY X_NEXT tFrom X_NEXT tTo X_NEXT X_SLASH K_COPY
    { $$ = Copy($3, $5); }
; 

tFrom:
  K_FROM arbitraryAttributes X_NEXT X_SLASH K_FROM
    { $$ = From();
      $$->id = $2->value;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_FROM); }
| K_FROM arbitraryAttributes X_CLOSE X_NAME X_OPEN X_SLASH K_FROM
    { $$ = From();
      $$->id = $2->value;
      $$->literal = $4->name;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_FROM); }
| K_FROM arbitraryAttributes X_SLASH
    { $$ = From();
      $$->id = $2->value;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_FROM); }
;

tTo:
  K_TO arbitraryAttributes X_NEXT X_SLASH K_TO
    { $$ = To();
      $$->id = $2->value;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_TO); }
| K_TO arbitraryAttributes X_SLASH
    { $$ = To();
      $$->id = $2->value;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_TO); }
;


/******************************************************************************
  WAIT
******************************************************************************/

tWait:
  K_WAIT arbitraryAttributes X_NEXT standardElements X_SLASH K_WAIT
    { $$ = Wait($4);
      $$->id = $2->value;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_WAIT); }
| K_WAIT arbitraryAttributes X_SLASH
    { impl_standardElements_StandardElements *noLinks = StandardElements(NiltTarget_list(), NiltSource_list(), standardJoinCondition());
      $$ = Wait(noLinks);
      $$->id = $2->value;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_WAIT); }
;


/******************************************************************************
  THROW
******************************************************************************/

tThrow:
  K_THROW arbitraryAttributes X_NEXT standardElements X_SLASH K_THROW
    { $$ = Throw($4);
      $$->id = $2->value;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_THROW); }
| K_THROW arbitraryAttributes X_SLASH
    { impl_standardElements_StandardElements *noLinks = StandardElements(NiltTarget_list(), NiltSource_list(), standardJoinCondition());
      $$ = Throw(noLinks);
      $$->id = $2->value;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_THROW); }
;


/******************************************************************************
  COMPENSATE
******************************************************************************/

tCompensate:
  K_COMPENSATE arbitraryAttributes X_NEXT standardElements X_SLASH K_COMPENSATE
    { $$ = Compensate($4);
      $$->id = $2->value;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_COMPENSATE); }
| K_COMPENSATE arbitraryAttributes X_SLASH
    { impl_standardElements_StandardElements *noLinks = StandardElements(NiltTarget_list(), NiltSource_list(), standardJoinCondition());
      $$ = Compensate(noLinks);
      $$->id = $2->value;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_COMPENSATE); }
;


/******************************************************************************
  TERMINATE
******************************************************************************/

tTerminate:
  K_TERMINATE arbitraryAttributes X_NEXT standardElements X_SLASH K_TERMINATE
    { $$ = Terminate($4);
      $$->id = $2->value;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_TERMINATE); }
| K_TERMINATE arbitraryAttributes X_SLASH
    { impl_standardElements_StandardElements *noLinks = StandardElements(NiltTarget_list(), NiltSource_list(), standardJoinCondition());
      $$ = Terminate(noLinks);
      $$->id = $2->value;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_TERMINATE); }
;


/******************************************************************************
  FLOW
******************************************************************************/

tFlow:
  K_FLOW arbitraryAttributes X_NEXT standardElements tLinks activity_list X_SLASH K_FLOW
    { $$ = Flow($4, $5, $6);
      $$->id = $2->value;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_FLOW); }
;

activity_list:
  activity X_NEXT
    { $$ = Consactivity_list($1, Nilactivity_list()); }
| activity X_NEXT activity_list
    { $$ = Consactivity_list($1, $3); }
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
    { $$ = Link();
      $$->id = $2->value;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_LINK); }
| K_LINK arbitraryAttributes X_SLASH
    { $$ = Link();
      $$->id = $2->value;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_LINK); }
;


/******************************************************************************
  SWITCH
******************************************************************************/

tSwitch:
  K_SWITCH arbitraryAttributes X_NEXT standardElements tCase_list tOtherwise X_SLASH K_SWITCH
    { $$ = Switch($4, $5, $6);
      $$->id = $2->value;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_SWITCH); }
;


tCase_list:
  tCase X_NEXT
    { $$ = ConstCase_list($1, NiltCase_list()); }
| tCase X_NEXT tCase_list
    { $$ = ConstCase_list($1, $3); }
;

tCase:
  K_CASE arbitraryAttributes X_NEXT activity X_NEXT X_SLASH K_CASE
    { $$ = Case($4);
      $$->id = $2->value;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_CASE); }
;


tOtherwise:
  /* If the otherwise branch is not explicitly specified, then an otherwise
     branch with an empty activity is deemed to be present. */
    {
      cerr << "You did not specify an otherwise branch." << endl;
      // creaty empty activity with id, without links etc.
      // TODO FIX THIS BUG
      impl_standardElements_StandardElements* noLinks = StandardElements(NiltTarget_list(),NiltSource_list(), standardJoinCondition());
      impl_tEmpty_Empty* implicitEmpty = Empty(noLinks);
      implicitEmpty->id = ASTEid++;
      impl_activity *otherwiseActivity = activityEmpty(implicitEmpty);
      otherwiseActivity->id = ASTEid++;

      $$ = Otherwise(otherwiseActivity);
      $$->id = otherwiseActivity->id;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_OTHERWISE);
    }
| K_OTHERWISE X_NEXT activity X_NEXT X_SLASH K_OTHERWISE X_NEXT
    { $$ = Otherwise($3);
      $$->id = ASTEid++;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_OTHERWISE); }
;


/******************************************************************************
  WHILE
******************************************************************************/

tWhile:
  K_WHILE arbitraryAttributes X_NEXT standardElements activity X_NEXT X_SLASH K_WHILE
    { $$ = While($4, $5);
      $$->id = $2->value;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_WHILE); }
;


/******************************************************************************
  SEQUENCE
******************************************************************************/

tSequence:
  K_SEQUENCE arbitraryAttributes X_NEXT standardElements activity_list X_SLASH K_SEQUENCE
    { $$ = Sequence($4, $5);
      $$->id = $2->value;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_SEQUENCE); }
;


/******************************************************************************
  PICK
******************************************************************************/

tPick:
  K_PICK arbitraryAttributes X_NEXT standardElements tOnMessage X_NEXT tOnMessage_list tOnAlarm_list X_SLASH K_PICK
    { $$ = Pick($4, ConstOnMessage_list($5, $7), $8);
      $$->id = $2->value;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_PICK); }
;


/******************************************************************************
  SCOPE
******************************************************************************/

tScope:
  K_SCOPE arbitraryAttributes X_NEXT standardElements tVariables
  tCorrelationSets tFaultHandlers tCompensationHandler tEventHandlers activity 
  X_NEXT X_SLASH K_SCOPE
    { $$ = Scope($4, $5, $7, $8, $9, StopInScope(), $10);
      $$->id = $2->value;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_SCOPE); }
;


/******************************************************************************
  STANDARD ELEMENTS
******************************************************************************/

standardElements:
  tTarget_list tSource_list
    { $$ = StandardElements($1, $2, currentJoinCondition);
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
    { $$ = Target();
      $$->id = $2->value;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_TARGET); }
| K_TARGET arbitraryAttributes X_SLASH
    { $$ = Target();
      $$->id = $2->value;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_TARGET); }
;

tSource_list:
  /* empty */
    { $$ = NiltSource_list(); }
| tSource X_NEXT tSource_list
    { $$ = ConstSource_list($1, $3); }
;

tSource:
  K_SOURCE arbitraryAttributes X_NEXT X_SLASH K_SOURCE
    { $$ = Source();
      $$->id = $2->value;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_SOURCE); }
| K_SOURCE arbitraryAttributes X_SLASH
    { $$ = Source();
      $$->id = $2->value;
      assert(ASTEmap[$$->id] == NULL);
      ASTEmap[$$->id] = new ASTE((kc::impl_activity*)$$, K_SOURCE); }
;


/*---------------------------------------------------------------------------*/

arbitraryAttributes:
  /* empty */
     { $$ = mkinteger(ASTEid++); // generate a new id
       temporaryAttributeMap[ASTEid-1]["referenceLine"] = toString(yylineno); // remember the file position
     }
| joinCondition arbitraryAttributes
     { $$ = $2; }
| X_NAME X_EQUALS X_STRING arbitraryAttributes
     { temporaryAttributeMap[$4->value][$1->name] = $3->name;
       $$ = $4; }
;


joinCondition:
  K_JOINCONDITION X_EQUALS booleanLinkCondition
    { currentJoinCondition = userDefinedJoinCondition($3);
      currentJoinCondition->print(); }
| K_JOINCONDITION X_EQUALS X_STRING
    { cerr << "ignoring given join condition: \"" << $3->name << "\"" << endl; }
;


booleanLinkCondition:
  K_GETLINKSTATUS LBRACKET APOSTROPHE X_NAME APOSTROPHE RBRACKET
    { $$ = Term($4); }
| LBRACKET booleanLinkCondition K_AND booleanLinkCondition RBRACKET
    { $$ = Conjunction($2, $4); }
| LBRACKET booleanLinkCondition K_OR booleanLinkCondition RBRACKET
    { $$ = Disjunction($2, $4); }
;
