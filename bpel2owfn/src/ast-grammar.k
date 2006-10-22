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
 *          - last changed: \$Date: 2006/10/22 14:37:33 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file bpel-abstract.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.128 $
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
extern int ASTEid;

%}


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
    assert(ASTEmap[id] == NULL); \
  } \
  else { \
    id = integer_1->value; \
  } \
  if(ASTEmap[id] == NULL); \
    ASTEmap[id] = new ASTE(id, token); \
  LEAVE("[AST]"); \
}


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
      integer
      )
  {
    int id;
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
  activityReceive(tReceive)
| activityReply(tReply)
| activityInvoke(tInvoke)
| activityAssign(tAssign)
| activityValidate(tValidate)
| activityEmpty(tEmpty)
| activityWait(tWait)
| activityTerminate(tTerminate)
| activityExit(tExit)
| activityThrow(tThrow)
| activityRethrow(tRethrow)
| activityCompensate(tCompensate)
| activityCompensateScope(tCompensateScope)
| activitySequence(tSequence)
| activitySwitch(tSwitch)
| activityIf(tIf)
| activityWhile(tWhile)
| activityRepeatUntil(tRepeatUntil)
| activityForEach(tForEach)
| activityPick(tPick)
| activityFlow(tFlow)
| activityScope(tScope)
  {
    int id;
  }
;

activity_list: list activity;





/******************************************************************************
  PARTNER LINKS
******************************************************************************/

tPartnerLink_list:
  list tPartnerLink
;

tPartnerLink:
  PartnerLink(integer)
  {
    int id;
    stringMap attributes;

    bool inPartners = false;
  }
;


/******************************************************************************
  PARTNERS
******************************************************************************/

tPartner_list: list tPartner;

tPartner:
  Partner(tPartnerLink_list integer)
  {
    int id;
  }
;


/******************************************************************************
  FAULT HANDLERS
******************************************************************************/

tFaultHandlers:
  userDefinedFaultHandler(tCatch_list tCatchAll integer)
| implicitFaultHandler(integer)
  {
    int id;

    bool inProcess;
    int parentScopeId;
    bool hasCatchAll; 
  }  
;

tCatch_list: list tCatch;

tCatch:
  Catch(activity integer)
| NoCatch()
  {
    int id;
    stringMap attributes;

    string variableName;
  }
;

tCatchAll:
  CatchAll(activity integer)
| NoCatchAll()
  {
    int id;
  }
;


/******************************************************************************
  EVENT HANDLERS
******************************************************************************/

tEventHandlers:
  implicitEventHandler(integer)
| userDefinedEventHandler(tOnMessage_list tOnAlarm_list integer)
  {
    bool inWhile = false;
    int id;
    int parentScopeId;
  }
;

// onMessage is called onEvent in WS-BPEL 2.0
tOnMessage_list: list tOnMessage;

tOnMessage:
  OnMessage(activity integer)
  {
    int id;
    stringMap attributes;
    bool inWhile = false;

    string channelName;
    string variableName;
  }
;

tOnAlarm_list: list tOnAlarm;

tOnAlarm:
  OnAlarm(activity integer)
  {
    int id;
    stringMap attributes;
  } 
;


/******************************************************************************
  COMPENSATION HANDLERS
******************************************************************************/

tCompensationHandler:
  implicitCompensationHandler(integer)
| userDefinedCompensationHandler(activity integer)
  {
    int id;

    int parentScopeId;
    bool hasCompensateWithoutScope = false;
    bool hasCompensateWithScope = false;

    bool inProcess = false;
  }
;


/******************************************************************************
  VARIABLES
******************************************************************************/

tVariable_list: list tVariable;

tVariable:
  Variable(integer)
  {
    int id;
  }
;


/******************************************************************************
  CORRELATION SETS
******************************************************************************/

tCorrelationSet_list: list tCorrelationSet;

tCorrelationSet:
  CorrelationSet(integer)
  {
    int id;
  }
;


/******************************************************************************
  CORRELATIONS
******************************************************************************/

tCorrelation_list: list tCorrelation;

tCorrelation:
  Correlation(integer)
  {
    int id;
  }
;


/******************************************************************************
  TO & FROM PARTS                                                (WS-BPEL 2.0)
******************************************************************************/

tToPart_list: list tToPart;

tToPart:
  ToPart(integer)
  {
    int id;
  }
;

tFromPart_list: list tFromPart;

tFromPart:
  FromPart(integer)
  {
    int id;
  }
;





/******************************************************************************
  RECEIVE
******************************************************************************/

tReceive:
  Receive(standardElements tCorrelation_list integer)
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


/******************************************************************************
  REPLY
******************************************************************************/

tReply:
  Reply(standardElements tCorrelation_list integer)
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


/******************************************************************************
  INVOKE
******************************************************************************/

tInvoke:
  Invoke(standardElements tCorrelation_list integer)
| annotatedInvoke(standardElements tCorrelation_list tCatch tCatchAll tCompensationHandler integer)
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


/******************************************************************************
  ASSIGN
******************************************************************************/

tAssign:
  Assign(standardElements tCopy_list integer)
  {
    int id;
    stringMap attributes;

    int negativeControlFlow = 0;
    bool suppressJF = false;
  }
;

tCopy_list: list tCopy;

tCopy:
  Copy(tFrom tTo integer)
  {
    int id;
    stringMap attributes;
  }
;

tFrom:
  From(integer)
  {
    int id;
    stringMap attributes;

    string expression;
    string literal;

    string variableName;
  }
;

tTo:
  To(integer)
  {
    int id;
    stringMap attributes;

    string variableName;
  }
;


/******************************************************************************
  VALIDATE                                                       (WS-BPEL 2.0)
******************************************************************************/

tValidate:
  Validate(standardElements integer)
  {
    int id;
  }
;


/******************************************************************************
  EMPTY
******************************************************************************/

tEmpty:
  Empty(standardElements integer)
  {
    int id;
  }
;


/******************************************************************************
  WAIT
******************************************************************************/

tWait:
  Wait(standardElements integer)
| WaitFor(standardElements integer)
| WaitUntil(standardElements integer)
  {
    int id;
  }
;


/******************************************************************************
  TERMINATE                                                      (BPEL4WS 1.1)
******************************************************************************/

tTerminate:
  Terminate(standardElements integer)
  {
    int id;
  }
;


/******************************************************************************
  EXIT                                                           (WS-BPEL 2.0)
******************************************************************************/

tExit:
  Exit(standardElements integer)
  {
    int id;
  }
;


/******************************************************************************
  THROW
******************************************************************************/

tThrow:
  Throw(standardElements integer)
  {
    int id;
    stringMap attributes;

    int negativeControlFlow = 0;
    bool suppressJF = false;

    string variableName;
  }
;


/******************************************************************************
  RETHROW                                                        (WS-BPEL 2.0)
******************************************************************************/

tRethrow:
  Rethrow(standardElements integer)
  {
    int id;
    stringMap attributes;

    int negativeControlFlow = 0;
    bool suppressJF = false;

    string variableName;
  }
;


/******************************************************************************
  COMPENSATE
******************************************************************************/

tCompensate:
  Compensate(standardElements integer)
  {
    int id;
    stringMap attributes;

    int negativeControlFlow = 0;
    bool suppressJF = false;

    bool inFaultHandler = true;

    bool hasScopeAttribute = false;
  }
;


/******************************************************************************
  COMPENSATESCOPE                                                (WS-BPEL 2.0)
******************************************************************************/

tCompensateScope:
  CompensateScope(standardElements integer)
  {
    int id;
    stringMap attributes;

    int negativeControlFlow = 0;
    bool suppressJF = false;

    bool inFaultHandler = true;

    bool hasScopeAttribute = true;
  }
;


/******************************************************************************
  SEQUENCE
******************************************************************************/

tSequence:
  Sequence(standardElements activity_list integer)
  {
    int id;
  }
;


/******************************************************************************
  SWITCH                                                         (BPEL4WS 1.1)
******************************************************************************/

tSwitch:
  Switch(standardElements tCase_list tOtherwise integer)
  {
    int id;
  }
;

tCase_list: list tCase;

tCase:
  Case(activity integer)
  {
    int id;
  }
;

tOtherwise:
  Otherwise(activity integer)
| NoOtherwise()
  {
    int id;
  }
;


/******************************************************************************
  IF                                                             (WS-BPEL 2.0)
******************************************************************************/

tIf:
  If(standardElements tElseIf_list tElse integer)
  {
    int id;
  }
;

tElseIf_list: list tElseIf;

tElseIf:
  ElseIf(activity integer)
  {
    int id;
  }
;

tElse:
  Else(activity integer)
| NoElse(integer)
  {
    int id;
  }
;


/******************************************************************************
  WHILE
******************************************************************************/

tWhile:
  While(standardElements activity integer)
  {
    int id;
  }
;


/******************************************************************************
  REPEATUNTIL                                                    (WS-BPEL 2.0)
******************************************************************************/

tRepeatUntil:
  RepeatUntil(standardElements activity integer)
  {
    int id;
  }
;


/******************************************************************************
  FOREACH                                                       (WS-BPEL 2.0)
******************************************************************************/

tForEach:
  ForEach(standardElements tScope integer)
  {
    int id;
  }
;


/******************************************************************************
  PICK
******************************************************************************/

tPick:
  Pick(standardElements tOnMessage_list tOnAlarm_list integer)
  {
    int id;
  }
;


/******************************************************************************
  FLOW
******************************************************************************/

tFlow:
  Flow(standardElements tLink_list activity_list integer)
  {
    int id;
  }
;

tLink_list: list tLink;

tLink:
  Link(integer)
  {
    int id;
  }
;


/******************************************************************************
  SCOPE
******************************************************************************/

tScope:
  Scope(
      standardElements
      tVariable_list
      tFaultHandlers
      tCompensationHandler
      tTerminationHandler
      tEventHandlers
      tStop
      activity
      integer
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


/******************************************************************************
  TERMINATION HANDLER
******************************************************************************/

tTerminationHandler:
  standardTerminationHandler(integer)
| TerminationHandler(activity integer)
  {
    bool inWhile = false;
    int id;
    int parentScopeId;
  }
;





/******************************************************************************
  STANDARD ELEMENTS
******************************************************************************/

standardElements:
  StandardElements(tTarget_list tSource_list joinCondition)
| NoStandardElements()
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
  Target(integer)
  {
    int id;
    stringMap attributes;

    int negativeControlFlow = 0;
    
    string linkName;
  }
;

tSource_list: list tSource;

tSource:
  Source(integer)
  {
    int id;
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





/******************************************************************************
  CONSTRUCTORS
******************************************************************************/

%ctor activityReceive()			{ id = tReceive_1->id; }
%ctor activityReply()			{ id = tReply_1->id; }
%ctor activityInvoke()			{ id = tInvoke_1->id; }
%ctor activityAssign()			{ id = tAssign_1->id; }
%ctor activityValidate()		{ id = tValidate_1->id; }
%ctor activityEmpty()			{ id = tEmpty_1->id; }
%ctor activityWait()			{ id = tWait_1->id; }
%ctor activityTerminate()		{ id = tTerminate_1->id; }
%ctor activityExit()			{ id = tExit_1->id; }
%ctor activityThrow()			{ id = tThrow_1->id; }
%ctor activityRethrow()			{ id = tRethrow_1->id; }
%ctor activityCompensate()		{ id = tCompensate_1->id; }
%ctor activityCompensateScope()		{ id = tCompensateScope_1->id; }
%ctor activitySequence()		{ id = tSequence_1->id; }
%ctor activitySwitch()			{ id = tSwitch_1->id; }
%ctor activityIf()			{ id = tIf_1->id; }
%ctor activityWhile()			{ id = tWhile_1->id; }
%ctor activityRepeatUntil()		{ id = tRepeatUntil_1->id; }
%ctor activityForEach()			{ id = tForEach_1->id; }
%ctor activityPick()			{ id = tPick_1->id; }
%ctor activityFlow()			{ id = tFlow_1->id; }
%ctor activityScope()			{ id = tScope_1->id; }

%ctor Process()				{ CTOR(K_PROCESS) }
%ctor PartnerLink()			{ CTOR(K_PARTNERLINK) }
%ctor Partner()				{ CTOR(K_PARTNER) }
%ctor implicitFaultHandler()		{ CTOR(K_FAULTHANDLERS) }
%ctor userDefinedFaultHandler()		{ CTOR(K_FAULTHANDLERS) }
%ctor Catch()				{ CTOR(K_CATCH) }
%ctor CatchAll()			{ CTOR(K_CATCHALL) }
%ctor implicitEventHandler()		{ CTOR(K_EVENTHANDLERS) }
%ctor userDefinedEventHandler()		{ CTOR(K_EVENTHANDLERS) }
%ctor OnMessage()			{ CTOR(K_ONMESSAGE) }
%ctor OnAlarm()				{ CTOR(K_ONALARM) }
%ctor implicitCompensationHandler()	{ CTOR(K_COMPENSATIONHANDLER) }
%ctor userDefinedCompensationHandler()	{ CTOR(K_COMPENSATIONHANDLER) }
%ctor Variable()			{ CTOR(K_VARIABLE) }
%ctor CorrelationSet()			{ CTOR(K_CORRELATIONSET) }
%ctor Correlation()			{ CTOR(K_CORRELATION) }
%ctor ToPart()				{ CTOR(K_TOPART) }
%ctor FromPart()			{ CTOR(K_FROMPART) }
%ctor Receive()				{ CTOR(K_RECEIVE) }
%ctor Reply()				{ CTOR(K_REPLY) }
%ctor Invoke()				{ CTOR(K_INVOKE) }
%ctor annotatedInvoke()			{ CTOR(K_INVOKE) }
%ctor Assign()				{ CTOR(K_ASSIGN) }
%ctor Copy()				{ CTOR(K_COPY) }
%ctor From()				{ CTOR(K_FROM) }
%ctor To()				{ CTOR(K_TO) }
%ctor Validate()			{ CTOR(K_VALIDATE) }
%ctor Empty()				{ CTOR(K_EMPTY) }
%ctor Wait() 				{ CTOR(K_WAIT) }
%ctor WaitFor() 			{ CTOR(K_WAIT) }
%ctor WaitUntil() 			{ CTOR(K_WAIT) }
%ctor Terminate()			{ CTOR(K_TERMINATE) }
%ctor Exit()				{ CTOR(K_EXIT) }
%ctor Throw()				{ CTOR(K_THROW) }
%ctor Rethrow()				{ CTOR(K_RETHROW) }
%ctor Compensate()			{ CTOR(K_COMPENSATE) }
%ctor CompensateScope()			{ CTOR(K_COMPENSATESCOPE) }
%ctor Sequence()			{ CTOR(K_SEQUENCE) }
%ctor Switch()				{ CTOR(K_SWITCH) }
%ctor Case()				{ CTOR(K_CASE) }
%ctor Otherwise()			{ CTOR(K_OTHERWISE) }
%ctor If() 				{ CTOR(K_IF) }
%ctor ElseIf()				{ CTOR(K_ELSEIF) }
%ctor Else()				{ CTOR(K_ELSE) }
%ctor NoElse()				{ CTOR(K_ELSE) }
%ctor While()				{ CTOR(K_WHILE) }
%ctor RepeatUntil()			{ CTOR(K_REPEATUNTIL) }
%ctor ForEach()				{ CTOR(K_FOREACH) }
%ctor Pick()				{ CTOR(K_PICK) }
%ctor Flow()				{ CTOR(K_FLOW) }
%ctor Link()				{ CTOR(K_LINK) }
%ctor Scope()				{ CTOR(K_SCOPE) }
%ctor standardTerminationHandler()	{ CTOR(K_TERMINATIONHANDLER) }
%ctor TerminationHandler()		{ CTOR(K_TERMINATIONHANDLER) }
%ctor Target()				{ CTOR(K_TARGET) }
%ctor Source()				{ CTOR(K_SOURCE) }
