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

%{
/*!
 * \file bpel-abstract.cc
 *
 * \brief Abstract grammar of BPEL (implementation)
 *
 * This file defines and implements the abstract grammar of BPEL and the phyla
 * of the abstract syntax tree (AST).
 * 
 * \author  
 *          - responsible: Niels Lohmann <nlohmann@informatik.hu-berlin.de>
 *          - last changes of: \$Author: nlohmann $ 
 *          
 * \date
 *          - created: 2005-10-03
 *          - last changed: \$Date: 2006/10/13 09:11:40 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file bpel-abstract.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.111 $
 */
%}


%{
/*!
 * \file bpel-abstract.h
 * \brief Abstract grammar of BPEL (interface)
 *
 * See \ref bpel-abstract.cc for more information.
 */

#include "ast-details.h"
extern map<unsigned int, ASTE*> ASTEmap;

%}


%{ KC_TYPES_HEADER

#include <string>
#include <iostream>
#include <map>
#include "helpers.h"

using std::string;
using std::cerr;
using std::endl;
using std::map;

typedef map<string, string> stringMap;

%}



tProcess:
  Process(
      tPartnerLink_list
      tPartner_list
      tVariable_list
      tCorrelationSet_list
      tFaultHandlers
      tCompensationHandler
      tEventHandlers
      tStop
      activity
      )
  {
    int id = 0;
    stringMap attributes;

    bool suppressJF = false;
    bool hasEH = false;
  }
;


tStop:
  StopInProcess()
| StopInScope()
;


/*---------------------------------------------------------------------------*/

activity:
  activityEmpty(tEmpty)
| activityInvoke(tInvoke)
| activityReceive(tReceive)
| activityReply(tReply)
| activityAssign(tAssign)
| activityWait(tWait)
| activityThrow(tThrow)
| activityTerminate(tTerminate)
| activityFlow(tFlow)
| activitySwitch(tSwitch)
| activityWhile(tWhile)
| activitySequence(tSequence)
| activityPick(tPick)
| activityScope(tScope)
| activityCompensate(tCompensate)
  {
    int id;
  }
;

%ctor activityEmpty()		{ id = tEmpty_1->id; }
%ctor activityInvoke()		{ id = tInvoke_1->id; }
%ctor activityReceive()		{ id = tReceive_1->id; }
%ctor activityReply()		{ id = tReply_1->id; }
%ctor activityAssign()		{ id = tAssign_1->id; }
%ctor activityWait()		{ id = tWait_1->id; }
%ctor activityThrow()		{ id = tThrow_1->id; }
%ctor activityTerminate()	{ id = tTerminate_1->id; }
%ctor activityFlow()		{ id = tFlow_1->id; }
%ctor activitySwitch()		{ id = tSwitch_1->id; }
%ctor activityWhile()		{ id = tWhile_1->id; }
%ctor activitySequence()	{ id = tSequence_1->id; }
%ctor activityPick()		{ id = tPick_1->id; }
%ctor activityScope()		{ id = tScope_1->id; }
%ctor activityCompensate()	{ id = tCompensate_1->id; }



/******************************************************************************
  PARTNER LINKS
******************************************************************************/

tPartnerLink_list:
  list tPartnerLink
;

tPartnerLink:
  PartnerLink()
  {
    int id = 0;
    stringMap attributes;

    bool inPartners = false;
  }
;


/******************************************************************************
  PARTNERS
******************************************************************************/

tPartner_list:
  list tPartner
;

tPartner:
  Partner(tPartnerLink_list)
  {
    int id = 0;
    stringMap attributes;
  }
;


/******************************************************************************
  FAULT HANDLERS
******************************************************************************/

tFaultHandlers:
  userDefinedFaultHandler(tCatch_list tCatchAll)
| implicitFaultHandler()
  {
    int id = 0;

    bool inProcess;
    int parentScopeId;
    bool hasCatchAll; 
  }  
;

tCatch_list:
  list tCatch
;

tCatch:
  Catch(activity)
  {
    int id = 0;
    stringMap attributes;

    string variableName;
  }
;

tCatchAll:
  CatchAll(activity)
| NoCatchAll()
  {
    int id = 0;
  }
;


/******************************************************************************
  EVENT HANDLERS
******************************************************************************/

tEventHandlers:
  implicitEventHandler()
| userDefinedEventHandler(tOnMessage_list tOnAlarm_list)
  {
    bool inWhile = false;
    int id = 0;
    int parentScopeId;
  }
;

// onMessage is called onEvent in WS-BPEL 2.0
tOnMessage_list:
  list tOnMessage
;

tOnAlarm_list:
  list tOnAlarm
;





tOnMessage:
  OnMessage(activity)
  {
    int id = 0;
    stringMap attributes;
    bool inWhile = false;

    string channelName;
    string variableName;
  }
;





tOnAlarm:
  OnAlarm(activity)
  {
    int id = 0;
    stringMap attributes;
  } 
;





/******************************************************************************
  COMPENSATION HANDLERS
******************************************************************************/

tCompensationHandler:
  implicitCompensationHandler()
| userDefinedCompensationHandler(activity)
  {
    int id = 0;

    int parentScopeId;
    bool hasCompensateWithoutScope = false;
    bool hasCompensateWithScope = false;

    bool inProcess = false;
  }
;

/******************************************************************************
  VARIABLES
******************************************************************************/

tVariable_list:
  list tVariable
;

tVariable:
  Variable()
  {
    int id = 0;
    stringMap attributes;
  }
;


/******************************************************************************
  CORRELATION SETS
******************************************************************************/

tCorrelationSet_list:
  list tCorrelationSet
;

tCorrelationSet:
  CorrelationSet()
  {
    int id = 0;
    stringMap attributes;
  }
;


/******************************************************************************
  CORRELATIONS
******************************************************************************/

tCorrelation_list:
  list tCorrelation
;

tCorrelation:
  Correlation()
  {
    int id = 0;
    stringMap attributes;
  }
;


/******************************************************************************
  EMPTY
******************************************************************************/

tEmpty:
  Empty(standardElements integer)
  {
    int id;
    stringMap attributes;

    int negativeControlFlow = 0;
    bool suppressJF = false;
  }
;

%ctor Empty()
{
  cerr << "called Empty()" << endl;
}

%ctor Empty(int myid)
{ id = myid;
  assert(ASTEmap[id] == NULL);
  ASTEmap[id] = new ASTE(id, K_EMPTY); }


/******************************************************************************
  INVOKE
******************************************************************************/

tInvoke:
  Invoke(standardElements tCorrelation_list)
  {
    int id;
    stringMap attributes;

    int negativeControlFlow = 0;
    bool suppressJF = false;
    bool inWhile = false;

    bool isSynchronousInvoke = false;

    string channelName;
    string inputVariableName;
    string outputVariableName;
  }
;

%ctor Invoke(int myid = 0)
{ id = myid;
  assert(ASTEmap[id] == NULL);
  ASTEmap[id] = new ASTE(id, K_INVOKE); }


/******************************************************************************
  RECEIVE
******************************************************************************/

tReceive:
  Receive(standardElements tCorrelation_list)
  {
    int id;
    stringMap attributes;

    int negativeControlFlow = 0;
    bool suppressJF = false;
    bool inWhile = false;

    string channelName;
    string variableName;
  }
;

%ctor Receive(int myid = 0)
{ id = myid;
  assert(ASTEmap[id] == NULL);
  ASTEmap[id] = new ASTE(id, K_RECEIVE); }


/******************************************************************************
  REPLY
******************************************************************************/

tReply:
  Reply(standardElements tCorrelation_list)
  {
    int id;
    stringMap attributes;

    int negativeControlFlow = 0;
    bool suppressJF = false;
    bool inWhile = false;

    string channelName;
    string variableName;
  }
;

%ctor Reply(int myid = 0)
{ id = myid;
  assert(ASTEmap[id] == NULL);
  ASTEmap[id] = new ASTE(id, K_REPLY); }


/******************************************************************************
  ASSIGN
******************************************************************************/

tAssign:
  Assign(standardElements tCopy_list)
  {
    int id;
    stringMap attributes;

    int negativeControlFlow = 0;
    bool suppressJF = false;
  }
;

%ctor Assign(int myid = 0)
{ id = myid;
  assert(ASTEmap[id] == NULL);
  ASTEmap[id] = new ASTE(id, K_ASSIGN); }

tCopy_list:
  list tCopy
;

tCopy:
  Copy(tFrom tTo)
  {
    int id = 0;
    stringMap attributes;
  }
;

tFrom:
  From()
  {
    int id = 0;
    stringMap attributes;

    string expression;
    string literal;

    string variableName;
  }
;

tTo:
  To()
  {
    int id = 0;
    stringMap attributes;

    string variableName;
  }
;


/******************************************************************************
  WAIT
******************************************************************************/

tWait:
  Wait(standardElements)
  {
    int id;
    stringMap attributes;

    int negativeControlFlow = 0;
    bool suppressJF = false;

    bool isWaitFor = false;
  }
;

%ctor Wait() {}
%ctor Wait(int myid)
{ cerr << "called Wait(int)" << endl; 
  id = myid;
  assert(ASTEmap[id] == NULL);
  ASTEmap[id] = new ASTE(id, K_WAIT); }


/******************************************************************************
  THROW
******************************************************************************/

tThrow:
  Throw(standardElements)
  {
    int id;
    stringMap attributes;

    int negativeControlFlow = 0;
    bool suppressJF = false;

    string variableName;
  }
;

%ctor Throw(int myid = 0)
{ id = myid;
  assert(ASTEmap[id] == NULL);
  ASTEmap[id] = new ASTE(id, K_THROW); }


/******************************************************************************
  COMPENSATE
******************************************************************************/

tCompensate:
  Compensate(standardElements)
  {
    int id;
    stringMap attributes;

    int negativeControlFlow = 0;
    bool suppressJF = false;

    bool inFaultHandler = true;

    bool hasScopeAttribute = false;
  }
;

%ctor Compensate(int myid = 0)
{ id = myid;
  assert(ASTEmap[id] == NULL);
  ASTEmap[id] = new ASTE(id, K_COMPENSATE); }


/******************************************************************************
  TERMINATE
******************************************************************************/

tTerminate:
  Terminate(standardElements)
  {
    int id;
    stringMap attributes;

    int negativeControlFlow = 0;
    bool suppressJF = false;
  }
;

%ctor Terminate(int myid = 0)
{ id = myid;
  assert(ASTEmap[id] == NULL);
  ASTEmap[id] = new ASTE(id, K_TERMINATE); }


/******************************************************************************
  FLOW
******************************************************************************/

tFlow:
  Flow(standardElements tLink_list activity_list)
  {
    int id;
    stringMap attributes;

    int negativeControlFlow = 0;
    bool suppressJF = false;
  }
;

%ctor Flow() {}
%ctor Flow(int myid)
{ id = myid;
  assert(ASTEmap[id] == NULL);
  ASTEmap[id] = new ASTE(id, K_FLOW); }

activity_list:
  list activity 
;

tLink_list:
  list tLink
;

tLink:
  Link()
  {
    int id = 0;
    stringMap attributes;

    bool usedAsSource = false;
    bool usedAsTarget = false;
  }
;


/******************************************************************************
  SWITCH
******************************************************************************/

tSwitch:
  Switch(standardElements tCase_list tOtherwise)
  {
    int id;
    stringMap attributes;

    int negativeControlFlow = 0;
    bool suppressJF = false;
  }
;

%ctor Switch(int myid = 0)
{ id = myid;
  assert(ASTEmap[id] == NULL);
  ASTEmap[id] = new ASTE(id, K_SWITCH); }

tCase_list:
  list tCase
;

tCase:
  Case(activity)
  {
    int id = 0;
    stringMap attributes;
  }
;

tOtherwise:
  Otherwise(activity)
  {
    int id = 0;
  }
;


/******************************************************************************
  WHILE
******************************************************************************/

tWhile:
  While(standardElements activity)
  {
    int id;
    stringMap attributes;

    int negativeControlFlow = 0;
    bool suppressJF = false;
  }
;

%ctor While(int myid = 0)
{ id = myid;
  assert(ASTEmap[id] == NULL);
  ASTEmap[id] = new ASTE(id, K_WHILE); }


/******************************************************************************
  SEQUENCE
******************************************************************************/

tSequence:
  Sequence(standardElements activity_list)
  {
    int id;
    stringMap attributes;

    int negativeControlFlow = 0;
    bool suppressJF = false;
  }
;

%ctor Sequence(int myid = 0)
{ id = myid;
  assert(ASTEmap[id] == NULL);
  ASTEmap[id] = new ASTE(id, K_SEQUENCE); }


/******************************************************************************
  PICK
******************************************************************************/

tPick:
  Pick(standardElements tOnMessage_list tOnAlarm_list)
  {
    int id;
    stringMap attributes;

    int negativeControlFlow = 0;
    bool suppressJF = false;
  }
;

%ctor Pick(int myid = 0)
{ id = myid;
  assert(ASTEmap[id] == NULL);
  ASTEmap[id] = new ASTE(id, K_PICK); }


/******************************************************************************
  SCOPE
******************************************************************************/

tScope:
  Scope(
      standardElements
      tVariable_list
      tFaultHandlers
      tCompensationHandler
      tEventHandlers
      tStop
      activity
      )
  {
    int id;
    stringMap attributes;

    int negativeControlFlow = 0;
    bool suppressJF = false;
    bool hasEH = false;

    int parentScopeId;
  }
;

%ctor Scope(int myid = 0)
{ id = myid;
  assert(ASTEmap[id] == NULL);
  ASTEmap[id] = new ASTE(id, K_SCOPE); }


/******************************************************************************
  STANDARD ELEMENTS
******************************************************************************/

standardElements:
  StandardElements(tTarget_list tSource_list joinCondition)
  {
    int negativeControlFlow = 0;
    bool suppressJF = false;

    int parentId;
    bool hasTarget = false;
  }
;

tTarget_list:
  list tTarget
  {
    bool hasTarget = false;
  }
;

tTarget:
  Target()
  {
    int id = 0;
    stringMap attributes;

    int negativeControlFlow = 0;
    
    string linkName;
  }
;

tSource_list:
  list tSource
;

tSource:
  Source()
  {
    int id = 0;
    stringMap attributes;

    int negativeControlFlow = 0;

    string linkName;
  }
;


joinCondition:
  standardJoinCondition()
| userDefinedJoinCondition(expression)
;


expression:
  Term(casestring)
| Conjunction(expression expression)
| Disjunction(expression expression)
;
