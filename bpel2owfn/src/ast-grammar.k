/*****************************************************************************\
 * Copyright 2007       Niels Lohmann                                        *
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
 *          This grammar describes the abstract syntax tree.
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          Christian Gierds <gierds@informatik.hu-berlin.de>,
 *          last changes of: \$Author: znamirow $ 
 * 
 * \since   2005-10-03
 *
 * \date    \$Date: 2007/05/25 10:59:37 $
 * 
 * \note    This file is part of the tool GNU BPEL2oWFN and was created during
 *          the project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file ast-grammar.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.175 $
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
#include "globals.h"
#include "frontend-parser.h"
%}





/* every phylum has an id */
%member unsigned int abstract_phylum::id;



tProcess:
  Process(
      tPartnerLink_list
      tPartner_list
      tMessageExchange_list
      tVariable_list
      tCorrelationSet_list
      tFaultHandlers
      tEventHandlers
      activity
      integer
      )
;


/*---------------------------------------------------------------------------*/

activity:
  activityReceive(tReceive)
| activityReply(tReply)
| activityInvoke(tInvoke)
| activityAssign(tAssign)
| activityValidate(tValidate)
| activityEmpty(tEmpty)
| activityOpaqueActivity(tOpaqueActivity)
| activityWait(tWait)
| activityExit(tExit)
| activityThrow(tThrow)
| activityRethrow(tRethrow)
| activityCompensate(tCompensate)
| activitySequence(tSequence)
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
  volatile_standardFaultHandlers(integer)
| FaultHandlers(tCatch_list tCatchAll integer)
;

tCatch_list: list tCatch;

tCatch:
  Catch(activity integer)
;

tCatchAll:
  CatchAll(activity integer)
| NoCatchAll()
;


/******************************************************************************
  TERMINATION HANDLER
******************************************************************************/

tTerminationHandler:
  volatile_standardTerminationHandler(integer)
| TerminationHandler(activity integer)
;


/******************************************************************************
  COMPENSATION HANDLER
******************************************************************************/

tCompensationHandler:
  volatile_standardCompensationHandler(integer)
| CompensationHandler(activity integer)
;


/******************************************************************************
  EVENT HANDLERS
******************************************************************************/

tEventHandlers:
  emptyEventHandlers(integer)
| EventHandlers(tOnMessage_list tOnAlarm_list integer)
;

// onMessage is called onEvent in WS-BPEL 2.0
tOnMessage_list: list tOnMessage;

tOnMessage:
  OnMessage(tCorrelation_list tFromPart_list activity integer)
;

tOnAlarm_list: list tOnAlarm;

tOnAlarm:
  OnAlarm(tRepeatEvery activity integer)
;

tRepeatEvery:
   emptyRepeatEvery(integer)
|  RepeatEvery(integer)
;


/******************************************************************************
  VARIABLES
******************************************************************************/

tVariable_list: list tVariable;

tVariable:
  Variable(integer)
;

/******************************************************************************
  MESSAGE EXCHANGES
******************************************************************************/

tMessageExchange_list: list tMessageExchange;

tMessageExchange:
  MessageExchange(integer)
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
  TO & FROM PARTS
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
  volatile_Invoke(standardElements tCorrelation_list tToPart_list tFromPart_list integer)
| SynchronousInvoke(standardElements tCorrelation_list tToPart_list tFromPart_list integer)
| AsynchronousInvoke(standardElements tCorrelation_list tToPart_list tFromPart_list integer)
| volatile_annotatedInvoke(standardElements tCorrelation_list tCatch_list tCatchAll tCompensationHandler tToPart_list tFromPart_list integer)
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
  VALIDATE
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
  OPAQUE ACTIVITY
******************************************************************************/

tOpaqueActivity:
  OpaqueActivity(standardElements integer)
;


/******************************************************************************
  WAIT
******************************************************************************/

tWait:
  Wait(standardElements integer)
;


/******************************************************************************
  EXIT / TERMINATE
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
  RETHROW
******************************************************************************/

tRethrow:
  Rethrow(standardElements integer)
;


/******************************************************************************
  COMPENSATE / COMPENSATESCOPE
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
  IF
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
  REPEATUNTIL
******************************************************************************/

tRepeatUntil:
  RepeatUntil(standardElements activity integer)
;


/******************************************************************************
  FOREACH
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
      tMessageExchange_list
      tFaultHandlers
      tCompensationHandler
      tTerminationHandler
      tEventHandlers
      tCorrelationSet_list
      activity
      tPartnerLink_list
      integer
      )
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
  {
    int parentId;
  }
;


expression:
  Term(casestring)
| Conjunction(expression expression)
| Disjunction(expression expression)
  {
    int parentId;
    bool value = false;
  }
;





/******************************************************************************
  CONSTRUCTORS
******************************************************************************/

%ctor activityAssign()			{ id = tAssign_1->id; }
%ctor activityCompensate()		{ id = tCompensate_1->id; }
%ctor activityEmpty()			{ id = tEmpty_1->id; }
%ctor activityExit()			{ id = tExit_1->id; }
%ctor activityFlow()			{ id = tFlow_1->id; }
%ctor activityForEach()			{ id = tForEach_1->id; }
%ctor activityIf()			{ id = tIf_1->id; }
%ctor activityInvoke()			{ id = tInvoke_1->id; }
%ctor activityOpaqueActivity()		{ id = tOpaqueActivity_1->id; }
%ctor activityPick()			{ id = tPick_1->id; }
%ctor activityReceive()			{ id = tReceive_1->id; }
%ctor activityRepeatUntil()		{ id = tRepeatUntil_1->id; }
%ctor activityReply()			{ id = tReply_1->id; }
%ctor activityRethrow()			{ id = tRethrow_1->id; }
%ctor activityScope()			{ id = tScope_1->id; }
%ctor activitySequence()		{ id = tSequence_1->id; }
%ctor activityThrow()			{ id = tThrow_1->id; }
%ctor activityValidate()		{ id = tValidate_1->id; }
%ctor activityWait()			{ id = tWait_1->id; }
%ctor activityWhile()			{ id = tWhile_1->id; }

%ctor Assign()				{ CTOR(K_ASSIGN) }
%ctor AsynchronousInvoke()		{ CTOR(K_INVOKE) }
%ctor Catch()				{ CTOR(K_CATCH) }
%ctor CatchAll()			{ CTOR(K_CATCHALL) }
%ctor Compensate()			{ CTOR(K_COMPENSATE) }
%ctor CompensateScope()			{ CTOR(K_COMPENSATESCOPE) }
%ctor CompensationHandler()		{ CTOR(K_COMPENSATIONHANDLER); if(globals::parsing) globals::process_information.compensation_handlers++; }
%ctor Copy()				{ CTOR(K_COPY) }
%ctor Correlation()			{ CTOR(K_CORRELATION) }
%ctor CorrelationSet()			{ CTOR(K_CORRELATIONSET) }
%ctor Else()				{ CTOR(K_ELSE) }
%ctor ElseIf()				{ CTOR(K_ELSEIF) }
%ctor Empty()				{ CTOR(K_EMPTY) }
%ctor emptyEventHandlers()		{ CTOR(K_EVENTHANDLERS) }
%ctor EventHandlers()			{ CTOR(K_EVENTHANDLERS); if(globals::parsing) globals::process_information.event_handlers++; }
%ctor Exit()				{ CTOR(K_EXIT) }
%ctor FaultHandlers()			{ CTOR(K_FAULTHANDLERS); if(globals::parsing) globals::process_information.fault_handlers++; }
%ctor Flow()				{ CTOR(K_FLOW) }
%ctor ForEach()				{ CTOR(K_FOREACH) }
%ctor From()				{ CTOR(K_FROM) }
%ctor FromPart()			{ CTOR(K_FROMPART) }
%ctor If() 				{ CTOR(K_IF) }
%ctor Link()				{ CTOR(K_LINK) }
%ctor MessageExchange()			{ CTOR(K_MESSAGEEXCHANGE) }
%ctor NoElse()				{ CTOR(K_ELSE) }
%ctor OnAlarm()				{ CTOR(K_ONALARM) }
%ctor OnMessage()			{ CTOR(K_ONMESSAGE) }
%ctor OpaqueActivity()			{ CTOR(K_OPAQUEACTIVITY) }
%ctor Partner()				{ CTOR(K_PARTNER) }
%ctor PartnerLink()			{ CTOR(K_PARTNERLINK) }
%ctor Pick()				{ CTOR(K_PICK) }
%ctor Process()				{ CTOR(K_PROCESS) }
%ctor Receive()				{ CTOR(K_RECEIVE) }
%ctor RepeatUntil()			{ CTOR(K_REPEATUNTIL) }
%ctor Reply()				{ CTOR(K_REPLY) }
%ctor Rethrow()				{ CTOR(K_RETHROW) }
%ctor Scope()				{ CTOR(K_SCOPE) }
%ctor Sequence()			{ CTOR(K_SEQUENCE) }
%ctor Source()				{ CTOR(K_SOURCE) }
%ctor SynchronousInvoke()		{ CTOR(K_INVOKE) }
%ctor Target()				{ CTOR(K_TARGET) }
%ctor TerminationHandler()		{ CTOR(K_TERMINATIONHANDLER); }
%ctor Throw()				{ CTOR(K_THROW) }
%ctor To()				{ CTOR(K_TO) }
%ctor ToPart()				{ CTOR(K_TOPART) }
%ctor Validate()			{ CTOR(K_VALIDATE) }
%ctor Variable()			{ CTOR(K_VARIABLE) }
%ctor volatile_annotatedInvoke()		{ CTOR(K_INVOKE) }
%ctor volatile_Invoke()				{ CTOR(K_INVOKE) }
%ctor volatile_standardCompensationHandler()	{ CTOR(K_COMPENSATIONHANDLER); }
%ctor volatile_standardFaultHandlers()		{ CTOR(K_FAULTHANDLERS); }
%ctor volatile_standardTerminationHandler()	{ CTOR(K_TERMINATIONHANDLER); }
%ctor Wait() 				{ CTOR(K_WAIT) }
%ctor While()				{ CTOR(K_WHILE) }
