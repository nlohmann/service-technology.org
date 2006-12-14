/*****************************************************************************\
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
 * \file    ast-grammar.cc
 *
 * \brief   Abstract grammar of BPEL
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          Christian Gierds <gierds@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nielslohmann $ 
 * 
 * \since   2005-10-03
 *
 * \date    \$Date: 2006/12/14 18:31:03 $
 * 
 * \note    This file is part of the tool GNU BPEL2oWFN and was created during
 *          the project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file bpel-abstract.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.156 $
 */
%}


%{
/*!
 * \file ast-grammar.h
 * \brief Abstract grammar of BPEL (interface)
 *
 * See \ref ast-grammar.cc for more information.
 */

#include "ast-details.h"
#include "ast-config.h"
#include "frontend-parser.h"
extern map<unsigned int, ASTE*> ASTEmap;
extern int ASTEid;

%}





/* every phylum has an id */
%member unsigned int abstract_phylum::id;



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
| activitySequence(tSequence)
| activitySwitch(tSwitch)
| activityIf(tIf)
| activityWhile(tWhile)
| activityRepeatUntil(tRepeatUntil)
| activityForEach(tForEach)
| activityPick(tPick)
| activityFlow(tFlow)
| activityScope(tScope)
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
;


/******************************************************************************
  PARTNERS
******************************************************************************/

tPartner_list: list tPartner;

tPartner:
  Partner(tPartnerLink_list integer)
;


/******************************************************************************
  FAULT HANDLERS
******************************************************************************/

tFaultHandlers:
  userDefinedFaultHandler(tCatch_list tCatchAll integer)
| implicitFaultHandler(integer)
;

tCatch_list: list tCatch;

tCatch:
  Catch(activity integer)
| NoCatch()
;

tCatchAll:
  CatchAll(activity integer)
| NoCatchAll()
;


/******************************************************************************
  EVENT HANDLERS
******************************************************************************/

tEventHandlers:
  implicitEventHandler(integer)
| userDefinedEventHandler(tOnMessage_list tOnAlarm_list integer)
;

// onMessage is called onEvent in WS-BPEL 2.0
tOnMessage_list: list tOnMessage;

tOnMessage:
  OnMessage(tCorrelation_list tFromPart_list activity integer)
;

tOnAlarm_list: list tOnAlarm;

tOnAlarm:
  OnAlarm(activity integer)
;


/******************************************************************************
  COMPENSATION HANDLERS
******************************************************************************/

tCompensationHandler:
  implicitCompensationHandler(integer)
| userDefinedCompensationHandler(activity integer)
  {
    bool hasCompensateWithoutScope = false;
    bool hasCompensateWithScope = false;
  }
;


/******************************************************************************
  VARIABLES
******************************************************************************/

tVariable_list: list tVariable;

tVariable:
  Variable(integer)
;


/******************************************************************************
  CORRELATION SETS
******************************************************************************/

tCorrelationSet_list: list tCorrelationSet;

tCorrelationSet:
  CorrelationSet(integer)
;


/******************************************************************************
  CORRELATIONS
******************************************************************************/

tCorrelation_list: list tCorrelation;

tCorrelation:
  Correlation(integer)
;


/******************************************************************************
  TO & FROM PARTS                                                (WS-BPEL 2.0)
******************************************************************************/

tToPart_list: list tToPart;

tToPart:
  ToPart(integer)
;

tFromPart_list: list tFromPart;

tFromPart:
  FromPart(integer)
;





/******************************************************************************
  RECEIVE
******************************************************************************/

tReceive:
  Receive(standardElements tCorrelation_list tFromPart_list integer)
;


/******************************************************************************
  REPLY
******************************************************************************/

tReply:
  Reply(standardElements tCorrelation_list tToPart_list integer)
;


/******************************************************************************
  INVOKE
******************************************************************************/

tInvoke:
  /* will be disposed during postprocessing: */
  Invoke(standardElements tCorrelation_list tToPart_list tFromPart_list integer)
| SynchronousInvoke(standardElements tCorrelation_list tToPart_list tFromPart_list integer)
| AsynchronousInvoke(standardElements tCorrelation_list tToPart_list tFromPart_list integer)
  /* will be disposed during postprocessing: */
| annotatedInvoke(standardElements tCorrelation_list tCatch tCatchAll tCompensationHandler tToPart_list tFromPart_list integer)
;


/******************************************************************************
  ASSIGN
******************************************************************************/

tAssign:
  Assign(standardElements tCopy_list integer)
;

tCopy_list: list tCopy;

tCopy:
  Copy(tFrom tTo integer)
;

tFrom:
  From(integer)
;

tTo:
  To(integer)
;


/******************************************************************************
  VALIDATE                                                       (WS-BPEL 2.0)
******************************************************************************/

tValidate:
  Validate(standardElements integer)
;


/******************************************************************************
  EMPTY
******************************************************************************/

tEmpty:
  Empty(standardElements integer)
;


/******************************************************************************
  WAIT
******************************************************************************/

/* will be disposed during postprocessing */
tWait:
  Wait(standardElements integer)
| WaitFor(standardElements integer)
| WaitUntil(standardElements integer)
;


/******************************************************************************
  TERMINATE                                                      (BPEL4WS 1.1)
******************************************************************************/

tTerminate:
  Terminate(standardElements integer)
;


/******************************************************************************
  EXIT                                                           (WS-BPEL 2.0)
******************************************************************************/

tExit:
  Exit(standardElements integer)
;


/******************************************************************************
  THROW
******************************************************************************/

tThrow:
  Throw(standardElements integer)
;


/******************************************************************************
  RETHROW                                                        (WS-BPEL 2.0)
******************************************************************************/

tRethrow:
  Rethrow(standardElements integer)
;


/******************************************************************************
  COMPENSATE / COMPENSATESCOPE                                   (WS-BPEL 2.0)
******************************************************************************/

tCompensate:
  Compensate(standardElements integer)
| CompensateScope(standardElements casestring integer)
;


/******************************************************************************
  SEQUENCE
******************************************************************************/

tSequence:
  Sequence(standardElements activity_list integer)
;


/******************************************************************************
  SWITCH                                                         (BPEL4WS 1.1)
******************************************************************************/

tSwitch:
  Switch(standardElements tCase_list tOtherwise integer)
;

tCase_list: list tCase;

tCase:
  Case(activity integer)
;

tOtherwise:
  Otherwise(activity integer)
| NoOtherwise()
;


/******************************************************************************
  IF                                                             (WS-BPEL 2.0)
******************************************************************************/

tIf:
  If(standardElements tElseIf_list tElse integer)
;

tElseIf_list: list tElseIf;

tElseIf:
  ElseIf(activity integer)
;

tElse:
  Else(activity integer)
| NoElse(integer)
;


/******************************************************************************
  WHILE
******************************************************************************/

tWhile:
  While(standardElements activity integer)
;


/******************************************************************************
  REPEATUNTIL                                                    (WS-BPEL 2.0)
******************************************************************************/

tRepeatUntil:
  RepeatUntil(standardElements activity integer)
;


/******************************************************************************
  FOREACH                                                        (WS-BPEL 2.0)
******************************************************************************/

tForEach:
  ForEach(standardElements casestring casestring casestring tScope integer)
;


/******************************************************************************
  PICK
******************************************************************************/

tPick:
  Pick(standardElements tOnMessage_list tOnAlarm_list integer)
;


/******************************************************************************
  FLOW
******************************************************************************/

tFlow:
  Flow(standardElements tLink_list activity_list integer)
;

tLink_list: list tLink;

tLink:
  Link(integer)
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
      tCorrelationSet_list
      tStop
      activity
      tPartnerLink_list
      integer
      )
;


/******************************************************************************
  TERMINATION HANDLER
******************************************************************************/

tTerminationHandler:
  standardTerminationHandler(integer)
| TerminationHandler(activity integer)
;





/******************************************************************************
  STANDARD ELEMENTS
******************************************************************************/

standardElements:
  StandardElements(tTarget_list tSource_list joinCondition)
| NoStandardElements()
  {
    bool suppressJF = false;
    int parentId;
  }
;

tTarget_list:
  list tTarget
;

tTarget:
  Target(integer)
;

tSource_list: list tSource;

tSource:
  Source(integer)
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
%ctor SynchronousInvoke()		{ CTOR(K_INVOKE) }
%ctor AsynchronousInvoke()		{ CTOR(K_INVOKE) }
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
