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
 *          - last changed: \$Date: 2006/07/06 14:19:59 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universit�t zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using GNU Bison reading file bpel-syntax.yy.
 *          See http://www.gnu.org/software/bison/bison.html for details
 *
 * \version \$Revision: 1.195 $
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


%defines
%yacc

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



%{



// options for Bison
#define YYDEBUG 1
#define YYERROR_VERBOSE 1  // for verbose error messages


// to avoid the message "parser stack overflow"
#define YYMAXDEPTH 1000000
#define YYINITDEPTH 10000


#include <assert.h>
#include <map>

#include "bpel-kc-k.h" // phylum definitions
#include "bpel-kc-yystype.h" // data types for tokens and non-terminals
#include "helpers.h"
#include "symbol-table.h"
#include "ast-details.h"
#include "bpel-attributes.h"
#include "check-symbols.h"


using namespace kc;
using namespace std;


// from flex
extern char* yytext;
extern int yylex();
extern int yylineno;


// defined in "debug.h"
extern int yyerror(const char *);
extern map<string, string> channelShortNames;


/// an instance of the symbol table
SymbolTable symTab = SymbolTable();
SymbolTableEntry *currentSymTabEntry;
unsigned int currentSymTabEntryKey = 0;


/// needed to tag handlers
integer currentScopeId;
/// pointer to the current surrounding Scope symbol table entry
STScope * currentSTScope = NULL;
/// pointer to the current surrounding Flow symbol table entry
STFlow * currentSTFlow = NULL;
/// pointer to the Process symbol table entry
STProcess * stProcess = NULL;
///
//STPartner * currentPartner = NULL;


/// an instance of the attribute manager
attributeManager att = attributeManager();


/// from check-symbols
SymbolManager symMan = SymbolManager();


/// needed to check occurrence of links within whiles
bool inWhile = false;

/// needed to tag scopes
map <integer, integer> parent;

/// the root of the abstract syntax tree
tProcess TheProcess;

/// stack for checking for FaultHandler
stack<bool> isInFH;
/// stack for checking for CompensationHandler
stack< pair<bool,int> > isInCH;
int hasCompensate;


/// a pointer to the current join condition
impl_joinCondition* currentJoinCondition = standardJoinCondition();


// Niels' stuff
map<unsigned int, ASTE*> ASTEmap; ///< the map of all AST elements
map<unsigned int, map<string, string> > temporaryAttributeMap; ///< a temporary mapping of attributs

%}



/* the types of the non-terminal symbols */
%type <yt_activity_list> activity_list
%type <yt_activity> activity
%type <yt_integer> arbitraryAttributes
%type <yt_standardElements> standardElements
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
%type <yt_activity> tInvoke
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

%type <yt_integer> genSymTabEntry_Process
%type <yt_integer> genSymTabEntry_PartnerLink
%type <yt_integer> genSymTabEntry_Partner
%type <yt_integer> genSymTabEntry_FaultHandlers
%type <yt_integer> genSymTabEntry_Catch
%type <yt_integer> genSymTabEntry_CatchAll
%type <yt_integer> genSymTabEntry_CompensationHandler
%type <yt_integer> genSymTabEntry_EventHandlers
%type <yt_integer> genSymTabEntry_OnMessage
%type <yt_integer> genSymTabEntry_OnAlarm
%type <yt_integer> genSymTabEntry_Variable
%type <yt_integer> genSymTabEntry_CorrelationSet
%type <yt_integer> genSymTabEntry_Correlation
%type <yt_integer> genSymTabEntry_Empty
%type <yt_integer> genSymTabEntry_Invoke
%type <yt_integer> genSymTabEntry_Receive
%type <yt_integer> genSymTabEntry_Reply
%type <yt_integer> genSymTabEntry_Assign
%type <yt_integer> genSymTabEntry_From
%type <yt_integer> genSymTabEntry_To
%type <yt_integer> genSymTabEntry_Wait
%type <yt_integer> genSymTabEntry_Throw
%type <yt_integer> genSymTabEntry_Compensate
%type <yt_integer> genSymTabEntry_Terminate
%type <yt_integer> genSymTabEntry_Flow
%type <yt_integer> genSymTabEntry_Link
%type <yt_integer> genSymTabEntry_Switch
%type <yt_integer> genSymTabEntry_Case
%type <yt_integer> genSymTabEntry_While
%type <yt_integer> genSymTabEntry_Sequence
%type <yt_integer> genSymTabEntry_Scope
%type <yt_integer> genSymTabEntry_Pick
%type <yt_integer> genSymTabEntry_Target
%type <yt_integer> genSymTabEntry_Source

%type <yt_integer> descent_activity_list
%type <yt_integer> descent_case_list


%%

/******************************************************************************
  PROCESS
******************************************************************************/

tProcess:
    {
      // initialisation
      symTab = SymbolTable();
      currentSymTabEntryKey = 0;
      currentSTScope = NULL;
      currentSTFlow = NULL;
      stProcess = NULL;
//      currentPartner = NULL;
      att = attributeManager();
      symMan = SymbolManager();
      inWhile = false;
      parent = map<integer, integer>();
      isInFH = stack<bool>();
      currentJoinCondition = standardJoinCondition();
      yylineno = 1;
      channelShortNames = map< string, string >();
    }
  X_OPEN K_PROCESS genSymTabEntry_Process
  arbitraryAttributes
    {
      stProcess = dynamic_cast<STProcess*> (symTab.lookup($4));
      assert (stProcess != NULL);

      currentScopeId = $4;
      currentSTScope = dynamic_cast<STScope *> (symTab.lookup(currentScopeId->value));
      assert (currentSTScope != NULL);

      isInFH.push(false);
      isInCH.push(pair<bool,int>(false,0));
      hasCompensate = 0;
   }
  X_NEXT imports tPartnerLinks tPartners tVariables tCorrelationSets tFaultHandlers tCompensationHandler tEventHandlers
  activity
  X_NEXT X_SLASH K_PROCESS X_CLOSE
    {
      TheProcess = $$ = Process($9, $10, $11, $12, $13, $14, $15, StopInProcess(), $16);
      $$->id = $4;

      isInFH.pop();
      isInCH.pop();
      ((STProcess*)symTab.lookup($4))->hasEventHandler = (string($15->op_name()) == "userDefinedEventHandler");

      assert(ASTEmap[$$->id->value] == NULL);
      ASTEmap[$$->id->value] = new ASTE((kc::impl_activity*)$$, K_PROCESS);
    }
;

genSymTabEntry_Process:
    { currentSymTabEntryKey = symTab.insert(K_PROCESS);
      $$ = mkinteger(currentSymTabEntryKey); }
;

/* import other namespaces */
imports:
  /* empty */
| K_IMPORT
  { currentSymTabEntryKey = 0; /* no entry in SymbolTable */ } arbitraryAttributes
  X_SLASH X_NEXT imports
;

/*---------------------------------------------------------------------------*/


activity:
  tEmpty
    { $$ = activityEmpty($1); $$->id = $1->id; 

      // collect source links for new DPE
      STElement* branch = dynamic_cast<STElement *> (symTab.lookup($$->id->value));
      branch->processLinks($$->id->value, currentSymTabEntryKey); }
| tInvoke
    { $$ = $1; }
| tReceive
    { $$ = activityReceive($1); $$->id = $1->id; 

      // collect source links for new DPE
      STElement* branch = dynamic_cast<STElement *> (symTab.lookup($$->id->value));
      branch->processLinks($$->id->value, currentSymTabEntryKey); }
| tReply
    { $$ = activityReply($1); $$->id = $1->id; 

      // collect source links for new DPE
      STElement* branch = dynamic_cast<STElement *> (symTab.lookup($$->id->value));
      branch->processLinks($$->id->value, currentSymTabEntryKey); }
| tAssign
    { $$ = activityAssign($1); $$->id = $1->id; 

      // collect source links for new DPE
      STElement* branch = dynamic_cast<STElement *> (symTab.lookup($$->id->value));
      branch->processLinks($$->id->value, currentSymTabEntryKey); }
| tWait
    { $$ = activityWait($1); $$->id = $1->id; 

      // collect source links for new DPE
      STElement* branch = dynamic_cast<STElement *> (symTab.lookup($$->id->value));
      branch->processLinks($$->id->value, currentSymTabEntryKey); }
| tThrow
    { $$ = activityThrow($1); $$->id = $1->id; 

      // collect source links for new DPE
      STElement* branch = dynamic_cast<STElement *> (symTab.lookup($$->id->value));
      branch->processLinks($$->id->value, currentSymTabEntryKey); }
| tTerminate
    { $$ = activityTerminate($1); $$->id = $1->id; 

      // collect source links for new DPE
      STElement* branch = dynamic_cast<STElement *> (symTab.lookup($$->id->value));
      branch->processLinks($$->id->value, currentSymTabEntryKey); }
| tFlow
    { $$ = activityFlow($1); $$->id = $1->id; 
      $$->dpe = $1->dpe ;

      // collect source links for new DPE
      STElement* branch = dynamic_cast<STElement *> (symTab.lookup($$->id->value));
      branch->processLinks($$->id->value, currentSymTabEntryKey); }
| tSwitch
    { $$ = activitySwitch($1); $$->id = $1->id; 
      $$->dpe = $1->dpe;

      // collect source links for new DPE
      STElement* branch = dynamic_cast<STElement *> (symTab.lookup($$->id->value));
      branch->processLinks($$->id->value, currentSymTabEntryKey); }
| tWhile
    { $$ = activityWhile($1); $$->id = $1->id; 

      // collect source links for new DPE
      STElement* branch = dynamic_cast<STElement *> (symTab.lookup($$->id->value));
      branch->processLinks($$->id->value, currentSymTabEntryKey); }
| tSequence
    { $$ = activitySequence($1); $$->id = $1->id; 
      $$->dpe = $1->dpe;

      // collect source links for new DPE
      STElement* branch = dynamic_cast<STElement *> (symTab.lookup($$->id->value));
      branch->processLinks($$->id->value, currentSymTabEntryKey); }
| tPick
    { $$ = activityPick($1); $$->id = $1->id; 
      $$->dpe = $1->dpe;

      // collect source links for new DPE
      STElement* branch = dynamic_cast<STElement *> (symTab.lookup($$->id->value));
      branch->processLinks($$->id->value, currentSymTabEntryKey); }
| tScope
    { $$ = activityScope($1); $$->id = $1->id; 
      $$->dpe = $1->dpe;

      // collect source links for new DPE
      STElement* branch = dynamic_cast<STElement *> (symTab.lookup($$->id->value));
      branch->processLinks($$->id->value, currentSymTabEntryKey); 
    }
| tCompensate
    { $$ = activityCompensate($1); $$->id = $1->id; 

      // collect source links for new DPE
      STElement* branch = dynamic_cast<STElement *> (symTab.lookup($$->id->value));
      branch->processLinks($$->id->value, currentSymTabEntryKey); }
;


/******************************************************************************
  PARTNER LINKS
******************************************************************************/

tPartnerLinks:
  /* empty */
    { $$ = NiltPartnerLink_list(); }
| K_PARTNERLINKS X_NEXT tPartnerLink_list X_SLASH K_PARTNERLINKS X_NEXT
    { $$ = $3; } 
;

tPartnerLink_list:
  tPartnerLink X_NEXT
    { $$ = ConstPartnerLink_list($1, NiltPartnerLink_list()); }
| tPartnerLink X_NEXT tPartnerLink_list
    { $$ = ConstPartnerLink_list($1, $3); }
;

tPartnerLink:
  K_PARTNERLINK genSymTabEntry_PartnerLink
    arbitraryAttributes X_NEXT X_SLASH K_PARTNERLINK
    { 
      STPartnerLink *stPartnerLink = dynamic_cast<STPartnerLink*> (symTab.lookup($2));
      assert(stPartnerLink != NULL);
/*
      if(inPartners) {
        stPartnerLink->isInPartners = true;
      }
      else {
        stPartnerLink->isInPartners = false;      
      }
*/
      // symTab.checkAttributes($2);
      $$ = PartnerLink();
      $$->id = $2;
/*
      if (inPartners) {
	STPartnerLink * definedPL = stProcess->checkPartnerLink(symTab.readAttributeValue($2, "name"));
	definedPL->partners.insert(currentPartner);
	if (definedPL->partners.size() > 1)
	{
	  trace("WARNING: Overlapping Partners\n");
	}
      }
      else
      {
	// cast to STProcess should always happen, since PartnerLinks can only be declared there
	(dynamic_cast<STProcess*>(currentSTScope))->addPartnerLink(stPartnerLink);
      }
*/

      (dynamic_cast<STProcess*>(currentSTScope))->addPartnerLink(stPartnerLink);

      assert(ASTEmap[$$->id->value] == NULL);
      ASTEmap[$$->id->value] = new ASTE((kc::impl_activity*)$$, K_PARTNERLINK);
    }
| K_PARTNERLINK genSymTabEntry_PartnerLink
    arbitraryAttributes X_SLASH
    { 
      STPartnerLink *stPartnerLink = dynamic_cast<STPartnerLink*> (symTab.lookup($2));
      assert(stPartnerLink != NULL);
/*
      if(inPartners) {
        stPartnerLink->isInPartners = true;
      }
      else {
        stPartnerLink->isInPartners = false;      
      }
*/
      // symTab.checkAttributes($2);
      $$ = PartnerLink();
      $$->id = $2;

/*
      if (inPartners) {
	STPartnerLink * definedPL = stProcess->checkPartnerLink(symTab.readAttributeValue($2, "name"));
	definedPL->partners.insert(currentPartner);
	if (definedPL->partners.size() > 1)
	{
	  trace("WARNING: Overlapping Partner definitions!\n");
	  trace("         PartnerLink \"" + symTab.readAttributeValue($2, "name")+ "\" ");
	  trace("(line " + intToString(stPartnerLink->line) + ")\n");
	  trace("         was used multiple times in a Partner definition: \n");
	  for(set<STPartner *>::iterator iter = definedPL->partners.begin(); iter != definedPL->partners.end(); iter++)
	  {
	    trace("          * Partner \"" + symTab.readAttributeValue((*iter)->entryKey, "name") + "\"");
	    trace(" (line " + intToString(dynamic_cast<STElement*>(symTab.lookup((*iter)->entryKey))->line) + ")\n");
	  }
	}
      }
      else
      {
	// cast to STProcess should always happen, since PartnerLinks can only be declared there
	(dynamic_cast<STProcess*>(currentSTScope))->addPartnerLink(stPartnerLink);
      }
*/

      (dynamic_cast<STProcess*>(currentSTScope))->addPartnerLink(stPartnerLink);

      assert(ASTEmap[$$->id->value] == NULL);
      ASTEmap[$$->id->value] = new ASTE((kc::impl_activity*)$$, K_PARTNERLINK);
    }
;

genSymTabEntry_PartnerLink:
  { currentSymTabEntryKey = symTab.insert(K_PARTNERLINK);
    $$ = mkinteger(currentSymTabEntryKey);
  }
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
  K_PARTNER genSymTabEntry_Partner arbitraryAttributes X_NEXT 
    {
//      STPartner *stPartner = dynamic_cast<STPartner*> (symTab.lookup($2));
//      assert (stPartner != NULL);
//      currentPartner = stPartner;
    }
  tPartnerLink_list X_SLASH K_PARTNER
    {
      $$ = Partner($6);
      $$->id = $2;
    }
| K_PARTNER genSymTabEntry_Partner arbitraryAttributes X_SLASH
    {
      $$ = Partner(NiltPartnerLink_list());
      $$->id = $2;      
    }
;

genSymTabEntry_Partner:
  { currentSymTabEntryKey = symTab.insert(K_PARTNER);
    $$ = mkinteger(currentSymTabEntryKey);
  }
;

/******************************************************************************
  FAULT HANDLERS
******************************************************************************/

tFaultHandlers:
  /* empty */
    { currentSymTabEntryKey = symTab.insert(K_FAULTHANDLERS);
      $$ = implicitFaultHandler();
      $$->id = mkinteger(currentSymTabEntryKey);
      $$->inProcess = (currentScopeId->value == 1);
      $$->parentScopeId = currentScopeId; 

      assert(ASTEmap[$$->id->value] == NULL);
      ASTEmap[$$->id->value] = new ASTE((kc::impl_activity*)$$, K_FAULTHANDLERS);
    }
| K_FAULTHANDLERS genSymTabEntry_FaultHandlers X_NEXT 
    {
      symMan.startDPEinWhile();
      isInFH.push(true);
      hasCompensate = 0;
    }
  tCatch_list  
  tCatchAll 
  X_SLASH K_FAULTHANDLERS X_NEXT
    { $$ = userDefinedFaultHandler($5, $6);
      $$->id = $2;
      $$->inProcess = (currentScopeId->value == 1);
      $$->parentScopeId = currentScopeId;

      isInFH.pop();
      hasCompensate = 0;
      symMan.endDPEinWhile();

      assert(ASTEmap[$$->id->value] == NULL);
      ASTEmap[$$->id->value] = new ASTE((kc::impl_activity*)$$, K_FAULTHANDLERS);
    }
;

genSymTabEntry_FaultHandlers:
  { currentSymTabEntryKey = symTab.insert(K_FAULTHANDLERS);
    $$ = mkinteger(currentSymTabEntryKey);
  }
;

tCatch_list:
  /* empty */
    { $$ = NiltCatch_list(); }
| tCatch X_NEXT tCatch_list
    { $$ = ConstCatch_list($1, $3); }
;

tCatch:
  K_CATCH genSymTabEntry_Catch
  arbitraryAttributes X_NEXT 
    {
      // symTab.checkAttributes($2);
      STCatch *stCatch = dynamic_cast<STCatch*> (symTab.lookup($2));
      assert (stCatch != NULL);

      stCatch->faultVariable = currentSTScope->checkVariable(symTab.readAttributeValue($2, "faultVariable"), symTab.readAttribute($2, "faultVariable")->line, currentSTScope, true);
    }
  activity X_NEXT X_SLASH K_CATCH
    { 

      $$ = Catch($6);
      $$->id = $2;

      assert(ASTEmap[$$->id->value] == NULL);
      ASTEmap[$$->id->value] = new ASTE((kc::impl_activity*)$$, K_CATCH);
    }
;

genSymTabEntry_Catch:
  { currentSymTabEntryKey = symTab.insert(K_CATCH);
    $$ = mkinteger(currentSymTabEntryKey);
  }
;

tCatchAll:
  /* empty */
    { $$ = NoCatchAll(); }
| K_CATCHALL genSymTabEntry_CatchAll
  arbitraryAttributes X_NEXT activity X_NEXT X_SLASH K_CATCHALL X_NEXT
    {
      $$ = CatchAll($5);
      $$->id = $2;      

      assert(ASTEmap[$$->id->value] == NULL);
      ASTEmap[$$->id->value] = new ASTE((kc::impl_activity*)$$, K_CATCHALL);
    }
;

genSymTabEntry_CatchAll:
  { currentSymTabEntryKey = symTab.insert(K_CATCHALL);
    $$ = mkinteger(currentSymTabEntryKey);
  }
;

/******************************************************************************
  COMPENSATION HANDLERS
******************************************************************************/

tCompensationHandler:
  /* empty */
    { currentSymTabEntryKey = symTab.insert(K_COMPENSATIONHANDLER);
      if (currentScopeId->value == 1)
        $$ = processCompensationHandler();
      else
        $$ = implicitCompensationHandler();
      $$->id = mkinteger(currentSymTabEntryKey);              
      $$->parentScopeId = currentScopeId;

      assert(ASTEmap[$$->id->value] == NULL);
      ASTEmap[$$->id->value] = new ASTE((kc::impl_activity*)$$, K_COMPENSATIONHANDLER);
    }
| K_COMPENSATIONHANDLER genSymTabEntry_CompensationHandler X_NEXT 
    {
      symMan.startDPEinWhile();
      isInCH.push(pair<bool,int>(true,hasCompensate));
      hasCompensate = 0;
    }
  activity 
  X_NEXT X_SLASH K_COMPENSATIONHANDLER X_NEXT
    { if (currentScopeId->value == 1)
        $$ = processCompensationHandler();
      else
        $$ = userDefinedCompensationHandler($5);

      switch ( hasCompensate ) {
        case 1 : $$->compensateWithoutScope = true;
                 $$->compensateWithScope = false;
                 break;
        case 2 : $$->compensateWithoutScope = false;
                 $$->compensateWithScope = true;
                 break;
        default: $$->compensateWithoutScope = false;
                 $$->compensateWithScope = false;
                 break;
      }
      hasCompensate = isInCH.top().second;
      isInCH.pop();
      
      $$->id = $2;
      $$->parentScopeId = currentScopeId; 
      symMan.endDPEinWhile();

      assert(ASTEmap[$$->id->value] == NULL);
      ASTEmap[$$->id->value] = new ASTE((kc::impl_activity*)$$, K_COMPENSATIONHANDLER);
    }
;

genSymTabEntry_CompensationHandler:
  { currentSymTabEntryKey = symTab.insert(K_COMPENSATIONHANDLER);
    $$ = mkinteger(currentSymTabEntryKey);
  }
;

/******************************************************************************
  EVENT HANDLERS
******************************************************************************/

tEventHandlers:
  /* empty */
    { currentSymTabEntryKey = symTab.insert(K_EVENTHANDLERS);
      $$ = implicitEventHandler();
      $$->id = mkinteger(currentSymTabEntryKey);      
      $$->parentScopeId = currentScopeId;

      assert(ASTEmap[$$->id->value] == NULL);
      ASTEmap[$$->id->value] = new ASTE((kc::impl_activity*)$$, K_EVENTHANDLERS);
}
| K_EVENTHANDLERS genSymTabEntry_EventHandlers X_NEXT 
    {
      symMan.startDPEinWhile();
    }
  tOnMessage_list 
  tOnAlarm_list 
  X_SLASH K_EVENTHANDLERS X_NEXT
    { $$ = userDefinedEventHandler($5, $6);
      $$->id = $2;    
      $$->parentScopeId = currentScopeId; 
      symMan.endDPEinWhile();

      assert(ASTEmap[$$->id->value] == NULL);
      ASTEmap[$$->id->value] = new ASTE((kc::impl_activity*)$$, K_EVENTHANDLERS);
    }
;

genSymTabEntry_EventHandlers:
  { currentSymTabEntryKey = symTab.insert(K_EVENTHANDLERS);
    $$ = mkinteger(currentSymTabEntryKey);
  }
;

tOnMessage_list:
  /* empty */
    { $$ = NiltOnMessage_list(); 
      $$->dpe = kc::mkinteger(0);
    }
| tOnMessage X_NEXT tOnMessage_list
    { $$ = ConstOnMessage_list($1, $3); 
      $$->dpe = kc::mkinteger($1->dpe->value + $3->dpe->value);
    }
;

tOnAlarm_list:
  /* empty */
    { $$ = NiltOnAlarm_list(); 
      $$->dpe = kc::mkinteger(0);
    }
| tOnAlarm X_NEXT tOnAlarm_list
    { $$ = ConstOnAlarm_list($1, $3); 
      $$->dpe = kc::mkinteger($1->dpe->value + $3->dpe->value);
    }
;

tOnMessage:
  K_ONMESSAGE genSymTabEntry_OnMessage
  arbitraryAttributes X_NEXT
    { // symTab.checkAttributes($2);
      symMan.resetDPEend(); }
  tCorrelations activity X_NEXT X_SLASH K_ONMESSAGE
    { $$ = OnMessage($7);
      $$->id = $2;    

      STOnMessage *stOnMessage = dynamic_cast<STOnMessage *> (symTab.lookup($2));
      assert(stOnMessage != NULL);

      stOnMessage->variable = currentSTScope->checkVariable(symTab.readAttributeValue($2, "variable"), symTab.readAttribute($2, "variable")->line, currentSTScope);
      stOnMessage->partnerLink = stProcess->checkPartnerLink(symTab.readAttributeValue($2, "partnerLink"));
      stOnMessage->channelId = stProcess->addChannel(channelName(symTab.readAttributeValue($2, "portType"), 
								 symTab.readAttributeValue($2, "operation"), 
								 symTab.readAttributeValue($2, "partnerLink")),
								 true);
      $$->dpe = symMan.needsDPE();

      // collect source links for new DPE
      stOnMessage->processLinks($7->id->value, currentSymTabEntryKey);
    }
;

genSymTabEntry_OnMessage:
  { currentSymTabEntryKey = symTab.insert(K_ONMESSAGE);
    $$ = mkinteger(currentSymTabEntryKey);
  }
;


tOnAlarm:
  K_ONALARM genSymTabEntry_OnAlarm
  arbitraryAttributes X_NEXT 
    { // symTab.checkAttributes($2); //att.check($3, K_ONALARM);
      
      symMan.resetDPEend();
    }
  activity X_NEXT X_SLASH K_ONALARM 
    { $$ = OnAlarm($6);
      $$->id = $2;
      $$->dpe = symMan.needsDPE();

      // collect source links for new DPE
      STElement* branch = dynamic_cast<STElement *> (symTab.lookup($$->id->value));
      branch->processLinks($6->id->value, currentSymTabEntryKey);
    }
;

genSymTabEntry_OnAlarm:
  { currentSymTabEntryKey = symTab.insert(K_ONALARM);
    $$ = mkinteger(currentSymTabEntryKey);
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
  K_VARIABLE genSymTabEntry_Variable
  arbitraryAttributes X_NEXT X_SLASH K_VARIABLE
    {
      STVariable *stVar = dynamic_cast<STVariable *> (symTab.lookup(currentSymTabEntryKey));
      assert(stVar != NULL);

      $$ = Variable();
      $$->id = $2;
      stVar->name = currentSTScope->addVariable(stVar);

      assert(ASTEmap[$$->id->value] == NULL);
      ASTEmap[$$->id->value] = new ASTE((kc::impl_activity*)$$, K_VARIABLE);
    }
| K_VARIABLE genSymTabEntry_Variable
  arbitraryAttributes X_SLASH
    {
      STVariable *stVar = dynamic_cast<STVariable *> (symTab.lookup(currentSymTabEntryKey));
      assert (stVar != NULL);

      $$ = Variable();
      $$->id = $2;
      stVar->name = currentSTScope->addVariable(stVar);

      assert(ASTEmap[$$->id->value] == NULL);
      ASTEmap[$$->id->value] = new ASTE((kc::impl_activity*)$$, K_VARIABLE);
    }
;

genSymTabEntry_Variable:
  { currentSymTabEntryKey = symTab.insert(K_VARIABLE);
    $$ = mkinteger(currentSymTabEntryKey);
  }
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
  K_CORRELATIONSET genSymTabEntry_CorrelationSet
  arbitraryAttributes X_NEXT X_SLASH K_CORRELATIONSET
    { // symTab.checkAttributes($2); //att.check($3, K_CORRELATIONSET);
      $$ = CorrelationSet();
      $$->id = $2;

      assert(ASTEmap[$$->id->value] == NULL);
      ASTEmap[$$->id->value] = new ASTE((kc::impl_activity*)$$, K_CORRELATIONSET);
    }
| K_CORRELATIONSET genSymTabEntry_CorrelationSet
  arbitraryAttributes X_SLASH
    { // symTab.checkAttributes($2); //att.check($3, K_CORRELATIONSET);
      $$ = CorrelationSet();
      $$->id = $2;

      assert(ASTEmap[$$->id->value] == NULL);
      ASTEmap[$$->id->value] = new ASTE((kc::impl_activity*)$$, K_CORRELATIONSET);
    }
;

genSymTabEntry_CorrelationSet:
  { currentSymTabEntryKey = symTab.insert(K_CORRELATIONSET);
    $$ = mkinteger(currentSymTabEntryKey);
  }
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
  K_CORRELATION genSymTabEntry_Correlation
  arbitraryAttributes X_NEXT X_SLASH K_CORRELATION
    { // symTab.checkAttributes($2);
      $$ = Correlation();
      $$->id = $2;      

      assert(ASTEmap[$$->id->value] == NULL);
      ASTEmap[$$->id->value] = new ASTE((kc::impl_activity*)$$, K_CORRELATION);
    }
| K_CORRELATION genSymTabEntry_Correlation
  arbitraryAttributes X_SLASH
    { // symTab.checkAttributes($2);
      $$ = Correlation();
      $$->id = $2;      

      assert(ASTEmap[$$->id->value] == NULL);
      ASTEmap[$$->id->value] = new ASTE((kc::impl_activity*)$$, K_CORRELATION);
    }
;

genSymTabEntry_Correlation:
    { currentSymTabEntryKey = symTab.insert(K_CORRELATION);
      $$ = mkinteger(currentSymTabEntryKey); }
;


/******************************************************************************
  EMPTY
******************************************************************************/

tEmpty:
  K_EMPTY genSymTabEntry_Empty arbitraryAttributes
  X_NEXT standardElements X_SLASH K_EMPTY
    {
      $$ = Empty($5);
      $$->id = $5->parentId = $2; 

      assert(ASTEmap[$$->id->value] == NULL);
      ASTEmap[$$->id->value] = new ASTE((kc::impl_activity*)$$, K_EMPTY);

      if ($5->hasTarget)
	symMan.remDPEstart();
      if ($5->dpe->value > 0)
        symMan.addDPEend();
    }
| K_EMPTY genSymTabEntry_Empty arbitraryAttributes X_SLASH
    {
      impl_standardElements_StandardElements *noLinks = StandardElements(NiltTarget_list(), NiltSource_list(), standardJoinCondition());
      noLinks->parentId = $2;

      $$ = Empty(noLinks);
      $$->id = $2; 

      assert(ASTEmap[$$->id->value] == NULL);
      ASTEmap[$$->id->value] = new ASTE((kc::impl_activity*)$$, K_EMPTY);
    }
;

genSymTabEntry_Empty:
    { currentSymTabEntryKey = symTab.insert(K_EMPTY);
      $$ = mkinteger(currentSymTabEntryKey); }
;


/******************************************************************************
  INVOKE
******************************************************************************/

tInvoke:
  K_INVOKE genSymTabEntry_Invoke 
  arbitraryAttributes 
    { //symTab.checkAttributes($2); //att.check($3, K_INVOKE);
//NL      if(att.isAttributeValueEmpty($3, "suppressJoinFailure"))
//NL      {
//NL      	/// parent BPEL-element attribute value
//NL      	att.pushSJFStack($3, (att.topSJFStack()).getSJFValue());
//NL      }
//NL      else
//NL      {
//NL        /// current BPEL-element attribute value
//NL      	att.pushSJFStack($3, att.read($3, "suppressJoinFailure"));      
//NL      }
    }   
  X_NEXT
    {
      // automatically create scope?
      isInFH.push(false);
      isInCH.push(pair<bool,int>(false,hasCompensate));
      parent[$2] = currentScopeId;
      kc::integer qad_id = symTab.nextId();
      parent[qad_id] = currentScopeId;
      currentScopeId = qad_id; 
    }
  standardElements 
  tCorrelations tCatch_list  tCatchAll tCompensationHandler X_SLASH K_INVOKE
    { 
      isInFH.pop();
      hasCompensate = isInCH.top().second;
      isInCH.pop();
      if ($9->length() > 0 
           || (string($10->op_name()) != "NoCatchAll") 
           || string($11->op_name()) != "implicitCompensationHandler")
      {
        //cerr << "embed in scope" << endl;
        currentSymTabEntryKey = symTab.insert(K_SCOPE);
        currentSymTabEntry = symTab.lookup(currentSymTabEntryKey); 
	parent[mkinteger(currentSymTabEntryKey)] = parent[currentScopeId];
	currentScopeId = mkinteger(currentSymTabEntryKey);
	
	currentSTScope = dynamic_cast<STScope *> (symTab.lookup(currentSymTabEntryKey));
	currentSTScope->parentScopeId = parent[currentScopeId]->value;
	(dynamic_cast<STScope *> (symTab.lookup(currentSTScope->parentScopeId)))->childScopes.push_back(currentSTScope);

	STInvoke *stInvoke = dynamic_cast<STInvoke *> (symTab.lookup($2));
        assert (stInvoke != NULL);

	stInvoke->inputVariable = currentSTScope->checkVariable(symTab.readAttributeValue($2, "inputVariable"), symTab.readAttribute($2, "inputVariable")->line, currentSTScope);
	stInvoke->outputVariable = currentSTScope->checkVariable(symTab.readAttributeValue($2, "outputVariable"), symTab.readAttribute($2, "outputVariable")->line, currentSTScope);
	stInvoke->partnerLink = stProcess->checkPartnerLink(symTab.readAttributeValue($2, "partnerLink"));
        stInvoke->isAsynchronousInvoke = (stInvoke->outputVariable == NULL);

	standardElements se =  StandardElements(NiltTarget_list(), NiltSource_list(), standardJoinCondition());
        tInvoke invoke = Invoke(se, $8);
        activity ai = activityInvoke(invoke);
        tFaultHandlers fh = userDefinedFaultHandler($9, $10);
        tEventHandlers eh = implicitEventHandler();
	eh->id = mkinteger(symTab.insert(K_EVENTHANDLERS));      
        tScope scope = Scope($7, NiltVariable_list(), fh, $11, eh, StopInScope(), ai);

        scope->id = $7->parentId = currentScopeId; 
        invoke->id = ai->id = se->parentId = $2;

        assert(ASTEmap[invoke->id->value] == NULL);
        ASTEmap[invoke->id->value] = new ASTE((kc::impl_activity*)invoke, K_INVOKE);


        fh->inProcess = false;
        fh->parentScopeId = scope->id;
	fh->id = symTab.nextId();
        $11->parentScopeId = scope->id;
        eh->parentScopeId = scope->id;

//CG        symMan.newScopeScope(scope->id);
//CG        symMan.quitScope();

        symTab.addAttribute(currentSymTabEntryKey, symTab.newAttribute(mkcasestring("name"), att.read($3, "name")));
        symTab.addAttribute(currentSymTabEntryKey, symTab.newAttribute(mkcasestring("joinCondition"), att.read($3, "joinCondition")));
//NL        $7->suppressJoinFailure = att.read($3, "suppressJoinFailure",  (att.topSJFStack()).getSJFValue());
//NL        symTab.addAttribute(currentSymTabEntryKey, symTab.newAttribute(mkcasestring("suppressJoinFailure"), $7->suppressJoinFailure));
//NL	att.popSJFStack();
	// inputVariable <=> input for invoke process !!!
        if (symTab.readAttributeValue($2, "inputVariable") != "")
        {
	  stInvoke->channelId = stProcess->addChannel(channelName(symTab.readAttributeValue($2, "portType"), 
								 symTab.readAttributeValue($2, "operation"), 
								 symTab.readAttributeValue($2, "partnerLink")),
								 false);
        }
        if (symTab.readAttributeValue($2, "outputVariable") != "")
        {
	  stInvoke->channelId = stProcess->addChannel(channelName(symTab.readAttributeValue($2, "portType"), 
								 symTab.readAttributeValue($2, "operation"), 
								 symTab.readAttributeValue($2, "partnerLink")),
								 true);
        }
	scope->dpe = invoke->dpe = mkinteger(0);
        // symMan.needsDPE();
        if ($7->hasTarget)
        {
	 symMan.remDPEstart();
	}
	if ($7->dpe->value > 0)
        {
          symMan.addDPEend();
        }
//NL        scope->negativeControlFlow = mkinteger( ((int) isInFH.top()) + 2*((int) isInCH.top().first));
//NL        invoke->negativeControlFlow = mkinteger(0);
        /*
        scope->id = $7->parentId = $2; 
        invoke->id = ai->id = se->parentId = symTab.nextId();
        */
        ((STScope*)symTab.lookup(currentSymTabEntryKey))->hasEventHandler = false;

        currentScopeId = scope->parentScopeId = parent[$2];
        currentSTScope = dynamic_cast<STScope *> (symTab.lookup(currentScopeId->value));

        $$ = activity(activityScope(scope));

        $$->id = scope->id;
//NL        $$->negativeControlFlow = scope->negativeControlFlow;

        assert(ASTEmap[$$->id->value] == NULL);
        ASTEmap[$$->id->value] = new ASTE((kc::impl_activity*)invoke, K_SCOPE);


      // collect source links for new DPE
      STElement* branch = dynamic_cast<STElement *> (symTab.lookup($$->id->value));
      branch->processLinks($$->id->value, currentSymTabEntryKey);


      }
      else
      { 
        //cerr << "don't embed" << endl;

        //restore real scope ID
        currentScopeId = parent[$2];
        currentSTScope = dynamic_cast<STScope *> (symTab.lookup(currentScopeId->value));

	STInvoke *stInvoke = dynamic_cast<STInvoke *> (symTab.lookup($2));
        assert(stInvoke != NULL);

	stInvoke->inputVariable = currentSTScope->checkVariable(symTab.readAttributeValue($2, "inputVariable"), symTab.readAttribute($2, "inputVariable")->line,currentSTScope);
	stInvoke->outputVariable = currentSTScope->checkVariable(symTab.readAttributeValue($2, "outputVariable"), symTab.readAttribute($2, "outputVariable")->line,currentSTScope);
	stInvoke->partnerLink = stProcess->checkPartnerLink(symTab.readAttributeValue($2, "partnerLink"));
        stInvoke->isAsynchronousInvoke = (stInvoke->outputVariable == NULL);

	tInvoke invoke = Invoke($7, $8);

//NL        $7->suppressJoinFailure = att.read($3, "suppressJoinFailure",  (att.topSJFStack()).getSJFValue());
//NL        att.popSJFStack(); symTab.popSJFStack();
	// inputVariable <=> input for invoke process !!!
        if (symTab.readAttributeValue($2, "inputVariable") != "")
        {
	  stInvoke->channelId = stProcess->addChannel(channelName(symTab.readAttributeValue($2, "portType"), 
								 symTab.readAttributeValue($2, "operation"), 
								 symTab.readAttributeValue($2, "partnerLink")),
								 false);
        }
        if (symTab.readAttributeValue($2, "outputVariable") != "")
        {
	  stInvoke->channelId = stProcess->addChannel(channelName(symTab.readAttributeValue($2, "portType"), 
								 symTab.readAttributeValue($2, "operation"), 
								 symTab.readAttributeValue($2, "partnerLink")),
								 true);
        }
        invoke->dpe = symMan.needsDPE();
        if ($7->hasTarget)
        {
	  symMan.remDPEstart();
	}
        if ($7->dpe->value > 0)
        {
          symMan.addDPEend();
        }
//NL        invoke->negativeControlFlow = mkinteger( ((int) isInFH.top()) + 2*((int) isInCH.top().first));
        invoke->id = $7->parentId = $2; 

        $$ = activity(activityInvoke(invoke));
        $$->id = invoke->id;
//NL        $$->negativeControlFlow = invoke->negativeControlFlow;


        assert(ASTEmap[invoke->id->value] == NULL);
        ASTEmap[invoke->id->value] = new ASTE((kc::impl_activity*)invoke, K_INVOKE);


	// collect source links for new DPE
      STElement* branch = dynamic_cast<STElement *> (symTab.lookup($$->id->value));
	branch->processLinks($2->value, currentSymTabEntryKey);
      }    
    }
| K_INVOKE genSymTabEntry_Invoke 
  arbitraryAttributes 
    { 
      //symTab.checkAttributes($2); //att.check($3, K_INVOKE);
//NL      if(att.isAttributeValueEmpty($3, "suppressJoinFailure"))
//NL      {
//NL      	/// parent BPEL-element attribute value
//NL      	att.pushSJFStack($3, (att.topSJFStack()).getSJFValue());
//NL      }
//NL      else
//NL      {
//NL        /// current BPEL-element attribute value
//NL      	att.pushSJFStack($3, att.read($3, "suppressJoinFailure"));      
//NL      }
    }   
  X_SLASH
    { 
      impl_standardElements_StandardElements *noLinks = StandardElements(NiltTarget_list(), NiltSource_list(), standardJoinCondition());
      noLinks->parentId = $2;

      STInvoke *stInvoke = dynamic_cast<STInvoke *> (symTab.lookup($2));
      assert (stInvoke != NULL);

      stInvoke->inputVariable = currentSTScope->checkVariable(symTab.readAttributeValue($2, "inputVariable"), symTab.readAttribute($2, "inputVariable")->line,currentSTScope);
      stInvoke->outputVariable = currentSTScope->checkVariable(symTab.readAttributeValue($2, "outputVariable"), symTab.readAttribute($2, "outputVariable")->line,currentSTScope);
      stInvoke->partnerLink = stProcess->checkPartnerLink(symTab.readAttributeValue($2, "partnerLink"));
      stInvoke->isAsynchronousInvoke = (stInvoke->outputVariable == NULL);


      tInvoke invoke = Invoke(noLinks, NiltCorrelation_list());
//NL      att.popSJFStack(); symTab.popSJFStack();
      // inputVariable <=> input for invoke process !!!
      if (symTab.readAttributeValue($2, "inputVariable") != "")
      {
	  stInvoke->channelId = stProcess->addChannel(channelName(symTab.readAttributeValue($2, "portType"), 
								 symTab.readAttributeValue($2, "operation"), 
								 symTab.readAttributeValue($2, "partnerLink")),
								 false);
      }
      if (symTab.readAttributeValue($2, "outputVariable") != "")
      {
	  stInvoke->channelId = stProcess->addChannel(channelName(symTab.readAttributeValue($2, "portType"), 
								 symTab.readAttributeValue($2, "operation"), 
								 symTab.readAttributeValue($2, "partnerLink")),
								 true);
      }
      invoke->dpe = kc::mkinteger(0);
//NL      invoke->negativeControlFlow = mkinteger( ((int) isInFH.top()) + 2*((int) isInCH.top().first));
      invoke->id = $2; 

      $$ = activity(activityInvoke(invoke));
      $$->id = invoke->id;
//NL      $$->negativeControlFlow = invoke->negativeControlFlow;

      assert(ASTEmap[invoke->id->value] == NULL);
      ASTEmap[invoke->id->value] = new ASTE((kc::impl_activity*)invoke, K_INVOKE);


      // collect source links for new DPE
      STElement* branch = dynamic_cast<STElement *> (symTab.lookup($$->id->value));
      branch->processLinks($$->id->value, currentSymTabEntryKey);
}
;

genSymTabEntry_Invoke:
  { currentSymTabEntryKey = symTab.insert(K_INVOKE);
    $$ = mkinteger(currentSymTabEntryKey);
  }
;


/******************************************************************************
  RECEIVE
******************************************************************************/

tReceive:
  K_RECEIVE genSymTabEntry_Receive arbitraryAttributes X_NEXT
  standardElements tCorrelations X_SLASH K_RECEIVE
    { 
      STReceive *stReceive = dynamic_cast<STReceive *> (symTab.lookup($2));
      assert(stReceive != NULL);

      stReceive->variable = currentSTScope->checkVariable(symTab.readAttributeValue($2, "variable"), symTab.readAttribute($2, "variable")->line,currentSTScope);
      stReceive->partnerLink = stProcess->checkPartnerLink(symTab.readAttributeValue($2, "partnerLink"));
      stReceive->channelId = stProcess->addChannel(channelName(symTab.readAttributeValue($2, "portType"), 
								 symTab.readAttributeValue($2, "operation"), 
								 symTab.readAttributeValue($2, "partnerLink")),
								 true);

      $$ = Receive($5, $6);
      $$->id = $5->parentId = $2; 

      assert(ASTEmap[$$->id->value] == NULL);
      ASTEmap[$$->id->value] = new ASTE((kc::impl_activity*)$$, K_RECEIVE);

      if ($5->hasTarget)
	symMan.remDPEstart();
      if ($5->dpe->value > 0)
        symMan.addDPEend();
    }
| K_RECEIVE genSymTabEntry_Receive
  arbitraryAttributes
  X_SLASH
    { impl_standardElements_StandardElements *noLinks = StandardElements(NiltTarget_list(), NiltSource_list(), standardJoinCondition());
      noLinks->parentId = $2;

      STReceive *stReceive = dynamic_cast<STReceive *> (symTab.lookup($2));
      assert(stReceive != NULL);

      stReceive->variable = currentSTScope->checkVariable(symTab.readAttributeValue($2, "variable"), symTab.readAttribute($2, "variable")->line,currentSTScope);
      stReceive->partnerLink = stProcess->checkPartnerLink(symTab.readAttributeValue($2, "partnerLink"));
      stReceive->channelId = stProcess->addChannel(channelName(symTab.readAttributeValue($2, "portType"), 
								 symTab.readAttributeValue($2, "operation"), 
								 symTab.readAttributeValue($2, "partnerLink")),
								 true);

      $$ = Receive(noLinks, NiltCorrelation_list());
      $$->id = $2;

      assert(ASTEmap[$$->id->value] == NULL);
      ASTEmap[$$->id->value] = new ASTE((kc::impl_activity*)$$, K_RECEIVE);
}
;

genSymTabEntry_Receive:
  { currentSymTabEntryKey = symTab.insert(K_RECEIVE);
    $$ = mkinteger(currentSymTabEntryKey);
  }
;

/******************************************************************************
  REPLY
******************************************************************************/

tReply:
  K_REPLY genSymTabEntry_Reply arbitraryAttributes X_NEXT 
  standardElements tCorrelations X_SLASH K_REPLY
    {
      STReply *stReply = dynamic_cast<STReply*> (symTab.lookup($2));
      assert(stReply != NULL);

      stReply->variable = currentSTScope->checkVariable(symTab.readAttributeValue($2, "variable"), symTab.readAttribute($2, "variable")->line,currentSTScope);
      stReply->partnerLink = stProcess->checkPartnerLink(symTab.readAttributeValue($2, "partnerLink"));
      stReply->channelId = stProcess->addChannel(channelName(symTab.readAttributeValue($2, "portType"), 
								 symTab.readAttributeValue($2, "operation"), 
								 symTab.readAttributeValue($2, "partnerLink")),
								 false);

      if ($5->hasTarget)
	symMan.remDPEstart();
      if ($5->dpe->value > 0)
        symMan.addDPEend();

      $$ = Reply($5, $6);
      $$->id = $5->parentId = $2;

      assert(ASTEmap[$$->id->value] == NULL);
      ASTEmap[$$->id->value] = new ASTE((kc::impl_activity*)$$, K_REPLY);
    }
| K_REPLY genSymTabEntry_Reply arbitraryAttributes X_SLASH
    {
      impl_standardElements_StandardElements *noLinks = StandardElements(NiltTarget_list(), NiltSource_list(), standardJoinCondition());
      noLinks->parentId = $2;

      STReply *stReply = dynamic_cast<STReply*> (symTab.lookup($2));
      assert (stReply != NULL);

      stReply->variable = currentSTScope->checkVariable(symTab.readAttributeValue($2, "variable"), symTab.readAttribute($2, "variable")->line,currentSTScope);
      stReply->partnerLink = stProcess->checkPartnerLink(symTab.readAttributeValue($2, "partnerLink"));
      stReply->channelId = stProcess->addChannel(channelName(symTab.readAttributeValue($2, "portType"), 
								 symTab.readAttributeValue($2, "operation"), 
								 symTab.readAttributeValue($2, "partnerLink")),
								 false);

      $$ = Reply(noLinks, NiltCorrelation_list());
      $$->id = $2;

      assert(ASTEmap[$$->id->value] == NULL);
      ASTEmap[$$->id->value] = new ASTE((kc::impl_activity*)$$, K_REPLY);
    }
;

genSymTabEntry_Reply:
    { currentSymTabEntryKey = symTab.insert(K_REPLY);
        $$ = mkinteger(currentSymTabEntryKey); }
;


/******************************************************************************
  ASSIGN
******************************************************************************/

tAssign:
  K_ASSIGN genSymTabEntry_Assign arbitraryAttributes X_NEXT
  standardElements tCopy_list  X_SLASH K_ASSIGN
    {
      if ($5->hasTarget)
	symMan.remDPEstart();
      if ($5->dpe->value > 0)
        symMan.addDPEend();

      $$ = Assign($5, $6);
      $$->id = $5->parentId = $2;

      assert(ASTEmap[$$->id->value] == NULL);
      ASTEmap[$$->id->value] = new ASTE((kc::impl_activity*)$$, K_ASSIGN);
    }
;

genSymTabEntry_Assign:
    { currentSymTabEntryKey = symTab.insert(K_ASSIGN);
      $$ = mkinteger(currentSymTabEntryKey); }
;

tCopy_list:
  tCopy X_NEXT
    { $$ = ConstCopy_list($1, NiltCopy_list()); }
| tCopy X_NEXT tCopy_list
    { $$ = ConstCopy_list($1, $3); }
;

tCopy:
  K_COPY X_NEXT tFrom X_NEXT tTo X_NEXT X_SLASH K_COPY
    { currentSymTabEntryKey = symTab.insert(K_COPY);
      $$ = Copy($3, $5); }
; 

tFrom:
  K_FROM genSymTabEntry_From arbitraryAttributes X_NEXT X_SLASH K_FROM
    {
      STFromTo *stFrom = dynamic_cast<STFromTo*> (symTab.lookup($2));
      assert (stFrom != NULL);

      stFrom->variable = currentSTScope->checkVariable(symTab.readAttributeValue($2, "variable"), symTab.readAttribute($2, "variable")->line,currentSTScope);
      stFrom->partnerLink = stProcess->checkPartnerLink(symTab.readAttributeValue($2, "partnerLink"));

      $$ = From();
      $$->id = $2;

      assert(ASTEmap[$$->id->value] == NULL);
      ASTEmap[$$->id->value] = new ASTE((kc::impl_activity*)$$, K_FROM);
    }
| K_FROM genSymTabEntry_From arbitraryAttributes X_CLOSE X_NAME X_OPEN X_SLASH K_FROM
    {
      STFromTo *stFrom = dynamic_cast<STFromTo*> (symTab.lookup($2));
      assert (stFrom != NULL);

      stFrom->variable = currentSTScope->checkVariable(symTab.readAttributeValue($2, "variable"), symTab.readAttribute($2, "variable")->line,currentSTScope);
      stFrom->partnerLink = stProcess->checkPartnerLink(symTab.readAttributeValue($2, "partnerLink"));
      stFrom->literal = $5->name;

      $$ = From();
      $$->id = $2;      
      $$->literal = $5;

      assert(ASTEmap[$$->id->value] == NULL);
      ASTEmap[$$->id->value] = new ASTE((kc::impl_activity*)$$, K_FROM);
    }
| K_FROM genSymTabEntry_From arbitraryAttributes X_SLASH
    {
      STFromTo *stFrom = dynamic_cast<STFromTo*> (symTab.lookup($2));
      assert (stFrom != NULL);

      stFrom->variable = currentSTScope->checkVariable(symTab.readAttributeValue($2, "variable"), symTab.readAttribute($2, "variable")->line,currentSTScope);
      stFrom->partnerLink = stProcess->checkPartnerLink(symTab.readAttributeValue($2, "partnerLink"));

      $$ = From();
      $$->id = $2;      

      assert(ASTEmap[$$->id->value] == NULL);
      ASTEmap[$$->id->value] = new ASTE((kc::impl_activity*)$$, K_FROM);
    }
;

genSymTabEntry_From:
    { currentSymTabEntryKey = symTab.insert(K_FROM);
      $$ = mkinteger(currentSymTabEntryKey); }
;

tTo:
  K_TO genSymTabEntry_To arbitraryAttributes X_NEXT X_SLASH K_TO
    {
      STFromTo *stTo = dynamic_cast<STFromTo*> (symTab.lookup($2));
      assert(stTo != NULL);

      stTo->variable = currentSTScope->checkVariable(symTab.readAttributeValue($2, "variable"), symTab.readAttribute($2, "variable")->line,currentSTScope);
      stTo->partnerLink = stProcess->checkPartnerLink(symTab.readAttributeValue($2, "partnerLink"));

      $$ = To();
      $$->id = $2;

      assert(ASTEmap[$$->id->value] == NULL);
      ASTEmap[$$->id->value] = new ASTE((kc::impl_activity*)$$, K_TO);
    }
| K_TO genSymTabEntry_To arbitraryAttributes X_SLASH
    {
      STFromTo *stTo = dynamic_cast<STFromTo*> (symTab.lookup($2));
      assert (stTo != NULL);

      stTo->variable = currentSTScope->checkVariable(symTab.readAttributeValue($2, "variable"), symTab.readAttribute($2, "variable")->line,currentSTScope);
      stTo->partnerLink = stProcess->checkPartnerLink(symTab.readAttributeValue($2, "partnerLink"));

      $$ = To();
      $$->id = $2;

      assert(ASTEmap[$$->id->value] == NULL);
      ASTEmap[$$->id->value] = new ASTE((kc::impl_activity*)$$, K_TO);
    }
;

genSymTabEntry_To:
    { currentSymTabEntryKey = symTab.insert(K_TO);
      $$ = mkinteger(currentSymTabEntryKey); }
;


/******************************************************************************
  WAIT
******************************************************************************/

tWait:
  K_WAIT genSymTabEntry_Wait arbitraryAttributes X_NEXT
  standardElements X_SLASH K_WAIT
    {
      if ($5->hasTarget)
	symMan.remDPEstart();
      if ($5->dpe->value > 0)
        symMan.addDPEend();

      if ( symTab.readAttributeValue($2, "for") != "" )
        $$ = WaitFor($5);
      else
        $$ = WaitUntil($5);

      $$->id = $5->parentId = $2;

      assert(ASTEmap[$$->id->value] == NULL);
      ASTEmap[$$->id->value] = new ASTE((kc::impl_activity*)$$, K_WAIT);
    }
| K_WAIT genSymTabEntry_Wait arbitraryAttributes X_SLASH
    {
      impl_standardElements_StandardElements *noLinks = StandardElements(NiltTarget_list(), NiltSource_list(), standardJoinCondition());
      noLinks->parentId = $2;

      if ( symTab.readAttributeValue($2, "for") != "" )
        $$ = WaitFor(noLinks);
      else
        $$ = WaitUntil(noLinks);

      $$->id = $2;

      assert(ASTEmap[$$->id->value] == NULL);
      ASTEmap[$$->id->value] = new ASTE((kc::impl_activity*)$$, K_WAIT);
    }
;

genSymTabEntry_Wait:
    { currentSymTabEntryKey = symTab.insert(K_WAIT);
      $$ = mkinteger(currentSymTabEntryKey); }
;


/******************************************************************************
  THROW
******************************************************************************/

tThrow:
  K_THROW genSymTabEntry_Throw arbitraryAttributes X_NEXT
  standardElements X_SLASH K_THROW
    {
      STThrow *stThrow = dynamic_cast<STThrow*> (symTab.lookup($2));
      assert (stThrow != NULL);

      stThrow->faultVariable = currentSTScope->checkVariable(symTab.readAttributeValue($2, "variable"), symTab.readAttribute($2, "variable")->line,currentSTScope);

      $$ = Throw($5);

      if ($5->hasTarget)
	symMan.remDPEstart();
      if ($5->dpe->value > 0)
        symMan.addDPEend();

      $$->id = $5->parentId = $2;

      assert(ASTEmap[$$->id->value] == NULL);
      ASTEmap[$$->id->value] = new ASTE((kc::impl_activity*)$$, K_THROW);
    }
| K_THROW genSymTabEntry_Throw arbitraryAttributes X_SLASH
    {
      impl_standardElements_StandardElements *noLinks = StandardElements(NiltTarget_list(), NiltSource_list(), standardJoinCondition());
      noLinks->parentId = $2;

      STThrow *stThrow = dynamic_cast<STThrow*> (symTab.lookup($2));
      assert (stThrow != NULL);

      stThrow->faultVariable = currentSTScope->checkVariable(symTab.readAttributeValue($2, "variable"), symTab.readAttribute($2, "variable")->line,currentSTScope);

      $$ = Throw(noLinks);
      $$->id = $2;

      assert(ASTEmap[$$->id->value] == NULL);
      ASTEmap[$$->id->value] = new ASTE((kc::impl_activity*)$$, K_THROW);
    }
;

genSymTabEntry_Throw:
    { currentSymTabEntryKey = symTab.insert(K_THROW);
      $$ = mkinteger(currentSymTabEntryKey); }
;


/******************************************************************************
  COMPENSATE
******************************************************************************/

tCompensate:
  K_COMPENSATE genSymTabEntry_Compensate
  arbitraryAttributes
    { // symTab.checkAttributes($2); //att.check($3, K_COMPENSATE);

      // compensate only is allowed within Compensation- or FaultHandler
      if ( ! (isInCH.top().first || isInFH.top()) )
      {
        yyerror(string("The activity <compensate> is only allowed with in a Compensation of FaultHandler.\n").c_str());
      }
      if(string(att.read($3, "scope")->name) == "")
      {
        if ( hasCompensate > 0)
        {
          yyerror(string("You may only define one <compensate> if you're using it without attributes.\n").c_str());
        }
        hasCompensate = 1;
      }
      else
      {
        if ( hasCompensate == 1)
        {
          yyerror(string("You may only define one <compensate> if you're using it without attributes.\n").c_str());
        }
        hasCompensate = 2;
      }

//NL      if(att.isAttributeValueEmpty($3, "suppressJoinFailure"))
//NL      {
//NL      	/// parent BPEL-element attribute value
//NL      	att.pushSJFStack($3, (att.topSJFStack()).getSJFValue());
//NL      }
//NL      else
//NL      {
//NL        /// current BPEL-element attribute value
//NL      	att.pushSJFStack($3, att.read($3, "suppressJoinFailure"));      
//NL      }
    }   
  X_NEXT standardElements X_SLASH K_COMPENSATE
    { $$ = Compensate($6);
//NL      $6->suppressJoinFailure = att.read($3, "suppressJoinFailure",  (att.topSJFStack()).getSJFValue());
//NL      att.popSJFStack(); symTab.popSJFStack();
      if ($6->hasTarget)
      {
	symMan.remDPEstart();
      }
      if ($6->dpe->value > 0)
      {
        symMan.addDPEend();
      }
//NL      $$->negativeControlFlow = $6->negativeControlFlow = mkinteger( ((int) isInFH.top()) + 2*((int) isInCH.top().first));
      $$->id = $6->parentId = $2;


      assert(ASTEmap[$$->id->value] == NULL);
      ASTEmap[$$->id->value] = new ASTE((kc::impl_activity*)$$, K_COMPENSATE);

}
| K_COMPENSATE genSymTabEntry_Compensate
  arbitraryAttributes
    { // symTab.checkAttributes($2); //att.check($3, K_COMPENSATE);

      // compensate only is allowed within Compensation- or FaultHandler
      if ( ! (isInCH.top().first || isInFH.top()) )
      {
        yyerror(string("The activity <compensate> is only allowed with in a Compensation of FaultHandler.\n").c_str());
      }
      if(string(att.read($3, "scope")->name) == "")
      {
        if ( hasCompensate > 0)
        {
          yyerror(string("You may only define one <compensate> if you're using it without attributes.\n").c_str());
        }
        hasCompensate = 1;
      }
      else
      {
        if ( hasCompensate == 1)
        {
          yyerror(string("You may only define one <compensate> if you're using it without attributes.\n").c_str());
        }
        hasCompensate = 2;
      }

//NL      if(att.isAttributeValueEmpty($3, "suppressJoinFailure"))
//NL      {
//NL      	/// parent BPEL-element attribute value
//NL      	att.pushSJFStack($3, (att.topSJFStack()).getSJFValue());
//NL      }
//NL      else
//NL      {
//NL        /// current BPEL-element attribute value
//NL      	att.pushSJFStack($3, att.read($3, "suppressJoinFailure"));      
//NL      }
    }   
  X_SLASH
    { impl_standardElements_StandardElements *noLinks = StandardElements(NiltTarget_list(), NiltSource_list(), standardJoinCondition());
      noLinks->parentId = $2;
      $$ = Compensate(noLinks);
//NL      att.popSJFStack(); symTab.popSJFStack();
//NL      $$->negativeControlFlow = noLinks->negativeControlFlow = mkinteger( ((int) isInFH.top()) + 2*((int) isInCH.top().first));
      $$->id = $2;


      assert(ASTEmap[$$->id->value] == NULL);
      ASTEmap[$$->id->value] = new ASTE((kc::impl_activity*)$$, K_COMPENSATE);
}
;

genSymTabEntry_Compensate:
  { currentSymTabEntryKey = symTab.insert(K_COMPENSATE);
    $$ = mkinteger(currentSymTabEntryKey);
  }
;


/******************************************************************************
  TERMINATE
******************************************************************************/

tTerminate:
  K_TERMINATE genSymTabEntry_Terminate arbitraryAttributes X_NEXT
  standardElements X_SLASH K_TERMINATE
    {
      $$ = Terminate($5);
      $$->id = $5->parentId = $2;

      if ($5->hasTarget)
	symMan.remDPEstart();
      if ($5->dpe->value > 0)
        symMan.addDPEend();

      assert(ASTEmap[$$->id->value] == NULL);
      ASTEmap[$$->id->value] = new ASTE((kc::impl_activity*)$$, K_TERMINATE);
    }
| K_TERMINATE genSymTabEntry_Terminate arbitraryAttributes X_SLASH
    {
      impl_standardElements_StandardElements *noLinks = StandardElements(NiltTarget_list(), NiltSource_list(), standardJoinCondition());
      noLinks->parentId = $2;

      $$ = Terminate(noLinks);
      $$->id = $2;

      assert(ASTEmap[$$->id->value] == NULL);
      ASTEmap[$$->id->value] = new ASTE((kc::impl_activity*)$$, K_TERMINATE);
    }
;

genSymTabEntry_Terminate:
    { currentSymTabEntryKey = symTab.insert(K_TERMINATE);
      $$ = mkinteger(currentSymTabEntryKey); }
;

/******************************************************************************
  FLOW
******************************************************************************/

tFlow:
  K_FLOW genSymTabEntry_Flow
  arbitraryAttributes
  X_NEXT
    {
      STFlow *stFlow = dynamic_cast<STFlow *> (symTab.lookup($2));
      assert (stFlow != NULL);

      if (currentSTFlow != NULL)
	stFlow->parentFlowId = currentSTFlow->entryKey;

      currentSTFlow = stFlow;
    } 
  standardElements tLinks activity_list X_SLASH K_FLOW
    {
      $$ = Flow($6, $7, $8);

      STFlow *stFlow = dynamic_cast<STFlow *> (symTab.lookup($2));
      assert (stFlow);

      if (currentSTFlow->parentFlowId != 0)
	currentSTFlow = dynamic_cast<STFlow *> (symTab.lookup(currentSTFlow->parentFlowId));
      else
	currentSTFlow = NULL;

      stFlow->checkLinkUsage();

      $$->dpe = mkinteger((symMan.needsDPE())->value);
      if ($6->hasTarget)
	symMan.remDPEstart();
      if ($6->dpe->value > 0)
        symMan.addDPEend();
      if ($$->dpe->value > 0)
	$6->dpe = mkinteger(1);

      $$->id = $6->parentId = $2;

      assert(ASTEmap[$$->id->value] == NULL);
      ASTEmap[$$->id->value] = new ASTE((kc::impl_activity*)$$, K_FLOW);
    }
;

genSymTabEntry_Flow:
    { currentSymTabEntryKey = symTab.insert(K_FLOW);
      $$ = mkinteger(currentSymTabEntryKey); }
;

activity_list:
  descent_activity_list activity X_NEXT
    {
      $$ = Consactivity_list($2, Nilactivity_list()); 

      for(int i = 0; i < $1->value; ++i)
	symMan.addDPEend();

      $$->dpe = $2->dpe;
    }
| descent_activity_list activity X_NEXT activity_list
    {
      $$ = Consactivity_list($2, $4); 

      for(int i = 0; i < $1->value; ++i)
	symMan.addDPEend();

      $$->dpe = mkinteger($2->dpe->value + $4->dpe->value);
    }
;

descent_activity_list:
    { $$ = symMan.needsDPE();
      symMan.resetDPEend(); }
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
  K_LINK genSymTabEntry_Link arbitraryAttributes X_NEXT X_SLASH K_LINK
    {
      $$ = Link();
      $$->id = $2;

      STLink *stLink = dynamic_cast<STLink *> (symTab.lookup($2));
      assert (stLink != NULL);

      stLink->name = currentSTFlow->addLink(stLink);

      assert(ASTEmap[$$->id->value] == NULL);
      ASTEmap[$$->id->value] = new ASTE((kc::impl_activity*)$$, K_LINK);
    }
| K_LINK genSymTabEntry_Link arbitraryAttributes X_SLASH
    {
      $$ = Link();
      $$->id = $2;

      STLink *stLink = dynamic_cast<STLink *> (symTab.lookup($2));
      assert (stLink != NULL);

      stLink->name = currentSTFlow->addLink(stLink);

      assert(ASTEmap[$$->id->value] == NULL);
      ASTEmap[$$->id->value] = new ASTE((kc::impl_activity*)$$, K_LINK);
    }
;

genSymTabEntry_Link:
    { currentSymTabEntryKey = symTab.insert(K_LINK);
      $$ = mkinteger(currentSymTabEntryKey); }
;


/******************************************************************************
  SWITCH
******************************************************************************/

tSwitch:
  K_SWITCH genSymTabEntry_Switch
  arbitraryAttributes
  X_NEXT 
  standardElements 
    { symMan.addDPEstart(); }
  tCase_list 
  tOtherwise 
  X_SLASH K_SWITCH
    {
      $$ = Switch($5, $7, $8);
      $$->id = $5->parentId = $2;

      symMan.remDPEstart();
      $$->dpe = symMan.needsDPE();
      
      if ($5->hasTarget)
	symMan.remDPEstart();

      if ($5->dpe->value > 0)
        symMan.addDPEend();

      if ($$->dpe->value > 0)
	$5->dpe = mkinteger(1);

      assert(ASTEmap[$$->id->value] == NULL);
      ASTEmap[$$->id->value] = new ASTE((kc::impl_activity*)$$, K_SWITCH);
    }
;

genSymTabEntry_Switch:
    { currentSymTabEntryKey = symTab.insert(K_SWITCH);
      $$ = mkinteger(currentSymTabEntryKey); }
;

tCase_list:
  descent_case_list tCase X_NEXT
    {
      $$ = ConstCase_list($2, NiltCase_list()); 

      for(int i = 0; i < $1->value; ++i)
	symMan.addDPEend();

      $$->dpe = $2->dpe;
    }
| descent_case_list tCase X_NEXT tCase_list
    {
      $$ = ConstCase_list($2, $4); 

      for(int i = 0; i < $1->value; ++i)
	symMan.addDPEend();

      $$->dpe = mkinteger($2->dpe->value + $4->dpe->value);
    }
;

descent_case_list:
    { $$ = symMan.needsDPE();
      symMan.resetDPEend(); }
;

tCase:
  K_CASE genSymTabEntry_Case
  arbitraryAttributes X_NEXT 
    {}
  activity 
  X_NEXT X_SLASH K_CASE
    {
      $$ = Case($6);
      $$->id = $2;    
      $$->dpe = mkinteger((symMan.needsDPE())->value);

      // collect source links for new DPE
      STElement *branch = dynamic_cast<STElement *> (symTab.lookup($$->id->value));
      assert (branch != NULL);

      branch->processLinks($6->id->value, currentSymTabEntryKey);

      assert(ASTEmap[$$->id->value] == NULL);
      ASTEmap[$$->id->value] = new ASTE((kc::impl_activity*)$$, K_CASE);
    }
;

genSymTabEntry_Case:
    { currentSymTabEntryKey = symTab.insert(K_CASE);
      $$ = mkinteger(currentSymTabEntryKey); }
;

tOtherwise:
  /* If the otherwise branch is not explicitly specified, then an otherwise
     branch with an empty activity is deemed to be present. */
    { // creaty empty activit with id, without links etc.
      //int otherwiseId =
      currentSymTabEntryKey = symTab.insert(K_OTHERWISE);
      currentSymTabEntry = symTab.lookup(currentSymTabEntryKey); 
      int emptyId = currentSymTabEntryKey = symTab.insert(K_EMPTY);
      currentSymTabEntry = symTab.lookup(currentSymTabEntryKey); 

      impl_standardElements_StandardElements* noLinks = StandardElements(NiltTarget_list(),NiltSource_list(), standardJoinCondition());
//      noLinks->dpe = kc::mkinteger(0);
      noLinks->parentId = kc::mkinteger(emptyId);
      impl_tEmpty_Empty* implicitEmpty = Empty(noLinks);
      implicitEmpty->id = kc::mkinteger(emptyId);
//NL      implicitEmpty->negativeControlFlow = noLinks->negativeControlFlow = mkinteger(0);
//      implicitEmpty->dpe = mkinteger(0);
      impl_activity *otherwiseActivity = activityEmpty(implicitEmpty);
      otherwiseActivity->id = kc::mkinteger(emptyId);

      $$ = Otherwise(otherwiseActivity);
      $$->dpe = kc::mkinteger(0);

      assert(ASTEmap[$$->id->value] == NULL);
      ASTEmap[$$->id->value] = new ASTE((kc::impl_activity*)$$, K_OTHERWISE);

    }
| K_OTHERWISE X_NEXT 
    { symMan.resetDPEend(); }
  activity X_NEXT X_SLASH K_OTHERWISE X_NEXT
    {
      currentSymTabEntryKey = symTab.insert(K_OTHERWISE);

      $$ = Otherwise($4);
      $$->id = mkinteger(currentSymTabEntryKey);
      $$->dpe = symMan.needsDPE();

      // collect source links for new DPE
      STElement* branch = dynamic_cast<STElement *> (symTab.lookup($$->id->value));
      assert (branch != NULL);

      branch->processLinks($4->id->value, currentSymTabEntryKey);

      assert(ASTEmap[$$->id->value] == NULL);
      ASTEmap[$$->id->value] = new ASTE((kc::impl_activity*)$$, K_OTHERWISE);
    }
;


/******************************************************************************
  WHILE
******************************************************************************/

tWhile:
  K_WHILE genSymTabEntry_While arbitraryAttributes X_NEXT 
  standardElements 
    { symMan.startDPEinWhile(); }
  activity X_NEXT X_SLASH K_WHILE
    {
      $$ = While($5, $7);
      $$->id = $5->parentId = $2; 

      symMan.endDPEinWhile();

      assert(ASTEmap[$$->id->value] == NULL);
      ASTEmap[$$->id->value] = new ASTE((kc::impl_activity*)$$, K_WHILE);
    }
;

genSymTabEntry_While:
    { currentSymTabEntryKey = symTab.insert(K_WHILE);
      $$ = mkinteger(currentSymTabEntryKey); }
;


/******************************************************************************
  SEQUENCE
******************************************************************************/

tSequence:
  K_SEQUENCE genSymTabEntry_Sequence arbitraryAttributes X_NEXT 
  standardElements activity_list X_SLASH K_SEQUENCE
    {
      $$ = Sequence($5, $6);
      $$->id = $5->parentId = $2; 
      $$->dpe = mkinteger((symMan.needsDPE())->value);

      if ($5->hasTarget)
	symMan.remDPEstart();

      if ($5->dpe->value > 0)
        symMan.addDPEend();

      if ($$->dpe->value > 0)
	$6->dpe = mkinteger(1);

      assert(ASTEmap[$$->id->value] == NULL);
      ASTEmap[$$->id->value] = new ASTE((kc::impl_activity*)$$, K_SEQUENCE);
    }
;

genSymTabEntry_Sequence:
    { currentSymTabEntryKey = symTab.insert(K_SEQUENCE);
      $$ = mkinteger(currentSymTabEntryKey); }
;


/******************************************************************************
  PICK
******************************************************************************/

tPick:
  K_PICK genSymTabEntry_Pick arbitraryAttributes X_NEXT 
  standardElements 
    { symMan.addDPEstart(); }
  tOnMessage X_NEXT tOnMessage_list tOnAlarm_list X_SLASH K_PICK
    {
      $$ = Pick($5, ConstOnMessage_list($7, $9), $10);

      symMan.remDPEstart();

      $$->dpe = symMan.needsDPE();

      if ($5->hasTarget)
	symMan.remDPEstart();

      if ($5->dpe->value > 0)
        symMan.addDPEend();

      if ($$->dpe->value > 0)
	$5->dpe = mkinteger(1);

      $$->id = $5->parentId = $2;

      assert(ASTEmap[$$->id->value] == NULL);
      ASTEmap[$$->id->value] = new ASTE((kc::impl_activity*)$$, K_PICK);
    }
;

genSymTabEntry_Pick:
    { currentSymTabEntryKey = symTab.insert(K_PICK);
      $$ = mkinteger(currentSymTabEntryKey); }
;


/******************************************************************************
  SCOPE
******************************************************************************/

tScope:
  K_SCOPE genSymTabEntry_Scope arbitraryAttributes X_NEXT
    { 
      isInFH.push(false);
      isInCH.push(pair<bool,int>(false,hasCompensate));
      parent[$2] = currentScopeId;
      currentScopeId = $2; 
    }
  standardElements 
    {
      currentSTScope = dynamic_cast<STScope *> (symTab.lookup(currentScopeId->value));
      assert(currentSTScope != NULL);

      currentSTScope->parentScopeId = parent[$2]->value;
      (dynamic_cast<STScope *> (symTab.lookup(currentSTScope->parentScopeId)))->childScopes.push_back(currentSTScope);
    }
  tVariables 
  tCorrelationSets 
  tFaultHandlers 
  tCompensationHandler 
  tEventHandlers 
  activity 
  X_NEXT X_SLASH K_SCOPE
    {
      isInFH.pop();
      hasCompensate = isInCH.top().second;
      isInCH.pop();

      $$ = Scope($6, $8, $10, $11, $12, StopInScope(), $13);
      $$->id = $6->parentId = $2;
      $$->parentScopeId = currentScopeId = parent[$2];

      currentSTScope = dynamic_cast<STScope *> (symTab.lookup(currentScopeId->value));
      assert(currentSTScope != NULL);

      $$->dpe = mkinteger((symMan.needsDPE())->value);

      if ($6->hasTarget)
	symMan.remDPEstart();

      ((STScope*)symTab.lookup($2))->hasEventHandler = (string($13->op_name()) == "userDefinedEventHandler");

      if ($6->dpe->value > 0)
        symMan.addDPEend();

      if ($$->dpe->value > 0)
	$6->dpe = mkinteger(1);

      assert(ASTEmap[$$->id->value] == NULL);
      ASTEmap[$$->id->value] = new ASTE((kc::impl_activity*)$$, K_SCOPE);
    }
;

genSymTabEntry_Scope:
    { currentSymTabEntryKey = symTab.insert(K_SCOPE);
      $$ = mkinteger(currentSymTabEntryKey); }
;


/******************************************************************************
  STANDARD ELEMENTS
******************************************************************************/

standardElements:
  tTarget_list tSource_list
    {
      $$ = StandardElements($1, $2, currentJoinCondition);
      currentJoinCondition = standardJoinCondition();

      $$->dpe = $2->dpe;

      if ($1->length() > 0)
      {
	symMan.addDPEstart();
	$$->hasTarget = true;
      }
    }
;

tTarget_list:
  /* empty */
    { $$ = NiltTarget_list(); }
| tTarget X_NEXT tTarget_list
    { $$ = ConstTarget_list($1, $3); }
;

tTarget:
  K_TARGET genSymTabEntry_Target arbitraryAttributes X_NEXT X_SLASH K_TARGET
    {
      $$ = Target();
      $$->id = $2;      

      STSourceTarget *stTarget = dynamic_cast<STSourceTarget *> (symTab.lookup($2));
      assert (stTarget != NULL);

      stTarget->link = currentSTFlow->checkLink(symTab.readAttributeValue($2, "linkName"), $2->value, false);
      stTarget->isSource = false;
      
      assert(ASTEmap[$$->id->value] == NULL);
      ASTEmap[$$->id->value] = new ASTE((kc::impl_activity*)$$, K_TARGET);
    }
| K_TARGET genSymTabEntry_Target arbitraryAttributes X_SLASH
    {
      $$ = Target();
      $$->id = $2;      

      STSourceTarget *stTarget = dynamic_cast<STSourceTarget *> (symTab.lookup($2));
      assert (stTarget != NULL);

      stTarget->link = currentSTFlow->checkLink(symTab.readAttributeValue($2, "linkName"), $2->value, false);
      stTarget->isSource = false;

      assert(ASTEmap[$$->id->value] == NULL);
      ASTEmap[$$->id->value] = new ASTE((kc::impl_activity*)$$, K_TARGET);
    }
;

genSymTabEntry_Target:
    { currentSymTabEntryKey = symTab.insert(K_TARGET);
      $$ = mkinteger(currentSymTabEntryKey); }
;

tSource_list:
  /* empty */
    { $$ = NiltSource_list(); 
      $$->dpe = kc::mkinteger(0); }
| tSource X_NEXT tSource_list
    { $$ = ConstSource_list($1, $3);
      $$->dpe = kc::mkinteger($1->dpe->value + $3->dpe->value); }
;

tSource:
  K_SOURCE genSymTabEntry_Source arbitraryAttributes X_NEXT X_SLASH K_SOURCE
    {
      $$ = Source();
      $$->id = $2;      

      STSourceTarget *stSource = dynamic_cast<STSourceTarget *> (symTab.lookup($2));
      assert (stSource != NULL);

      stSource->link = currentSTFlow->checkLink(symTab.readAttributeValue($2, "linkName"), $2->value, true);
      stSource->isSource = true;
      currentSTScope->addLink(stSource->link);

      symMan.addDPEend();
      $$->dpe = symMan.needsDPE();
      symMan.remDPEend();

      assert(ASTEmap[$$->id->value] == NULL);
      ASTEmap[$$->id->value] = new ASTE((kc::impl_activity*)$$, K_SOURCE);
    }
| K_SOURCE genSymTabEntry_Source
  arbitraryAttributes X_SLASH
    {
      $$ = Source();
      $$->id = $2;      

      STSourceTarget *stSource = dynamic_cast<STSourceTarget *> (symTab.lookup($2));
      assert (stSource != NULL);

      stSource->link = currentSTFlow->checkLink(symTab.readAttributeValue($2, "linkName"), $2->value, true);
      stSource->isSource = true;
      currentSTScope->addLink(stSource->link);

      symMan.addDPEend();
      $$->dpe = symMan.needsDPE();
      symMan.remDPEend();

      assert(ASTEmap[$$->id->value] == NULL);
      ASTEmap[$$->id->value] = new ASTE((kc::impl_activity*)$$, K_SOURCE);
    }
;

genSymTabEntry_Source:
    { currentSymTabEntryKey = symTab.insert(K_SOURCE);
      $$ = mkinteger(currentSymTabEntryKey); }
;

/*---------------------------------------------------------------------------*/

arbitraryAttributes:
  /* empty */
    { $$ = att.nextId(); }
| joinCondition arbitraryAttributes
    { $$ = $2; }
| X_NAME X_EQUALS X_STRING
    { if(currentSymTabEntryKey > 0)
        symTab.addAttribute(currentSymTabEntryKey, symTab.newAttribute($1, $3)); }
  arbitraryAttributes
    { att.define($1, $3);
      $$ = $5;
      temporaryAttributeMap[currentSymTabEntryKey][$1->name] = $3->name;
    }
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
