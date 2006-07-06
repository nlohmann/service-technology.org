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
 *          - last changed: \$Date: 2006/07/06 14:19:59 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file bpel-abstract.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.88 $
 */
%}


%{
/*!
 * \file bpel-abstract.h
 * \brief Abstract grammar of BPEL (interface)
 *
 * See \ref bpel-abstract.cc for more information.
 */
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
    integer id;
    bool suppressJF = false;

    casestring A_name = mkcasestring("");
    casestring A_targetNamespace = mkcasestring("");
    casestring A_queryLanguage = mkcasestring("");
    casestring A_expressionLanguage = mkcasestring("");
    casestring A_suppressJoinFailure = mkcasestring("");
    casestring A_enableInstanceCompensation = mkcasestring("");
    casestring A_abstractProcess = mkcasestring("");
    casestring A_xmlns = mkcasestring("");
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
    integer id;
    integer dpe = mkinteger(0);
  }
;


/******************************************************************************
  PARTNER LINKS
******************************************************************************/

tPartnerLink_list:
  list tPartnerLink
;

tPartnerLink:
  PartnerLink()
  {
    integer id;

    bool inPartners = false;

    casestring A_name = mkcasestring("");
    casestring A_partnerLinkType = mkcasestring("");
    casestring A_myRole = mkcasestring("");
    casestring A_partnerRole = mkcasestring("");
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
    integer id;

    casestring A_name = mkcasestring("");
  }
;


/******************************************************************************
  FAULT HANDLERS
******************************************************************************/

tFaultHandlers:
  userDefinedFaultHandler(tCatch_list tCatchAll)
| implicitFaultHandler()
  {
    integer id;

    bool inProcess;
    integer parentScopeId;
    bool hasCatchAll; 
  }  
;

tCatch_list:
  list tCatch
;

tCatch:
  Catch(activity)
  {
    integer id;

    casestring A_faultName = mkcasestring("");
    casestring A_faultVariable = mkcasestring("");
  }
;

tCatchAll:
  CatchAll(activity)
| NoCatchAll()
  {
    integer id;
  }
;


/******************************************************************************
  EVENT HANDLERS
******************************************************************************/

tEventHandlers:
  implicitEventHandler()
| userDefinedEventHandler(tOnMessage_list tOnAlarm_list)
  {
    integer id;
    integer parentScopeId;
  }
;

tOnMessage_list:
  list tOnMessage
  {
    integer dpe = mkinteger(0);
  }
;

tOnAlarm_list:
  list tOnAlarm
  {
    integer dpe = mkinteger(0);
  }
;

tOnMessage:
  OnMessage(activity)
  {
    integer id;
    integer dpe = mkinteger(0);

    casestring A_partnerLink = mkcasestring("");
    casestring A_portType = mkcasestring("");
    casestring A_operation = mkcasestring("");
    casestring A_variable = mkcasestring("");
  }
;

tOnAlarm:
  OnAlarm(activity)
  {
    integer id;
    integer dpe = mkinteger(0);
    
    casestring A_for = mkcasestring("");
    casestring A_until = mkcasestring("");
  } 
;


/******************************************************************************
  COMPENSATION HANDLERS
******************************************************************************/

tCompensationHandler:
  implicitCompensationHandler()
| userDefinedCompensationHandler(activity)
| processCompensationHandler()
  {
    integer id;

    integer parentScopeId;
    bool compensateWithoutScope = false;
    bool compensateWithScope = false;
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
    integer id;

    casestring A_name = mkcasestring("");
    casestring A_messageType = mkcasestring("");
    casestring A_type = mkcasestring("");
    casestring A_element = mkcasestring("");
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
    integer id;

    casestring A_name = mkcasestring("");
    casestring A_properties = mkcasestring("");
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
    integer id;

    casestring A_set = mkcasestring("");
    casestring A_initiate = mkcasestring("");
    casestring A_pattern = mkcasestring("");
  }
;


/******************************************************************************
  EMPTY
******************************************************************************/

tEmpty:
  Empty(standardElements)
  {
    integer id;
    integer negativeControlFlow;
    bool suppressJF = false;

    casestring A_name = mkcasestring("");
    casestring A_joinCondition = mkcasestring("");
    casestring A_suppressJoinFailure = mkcasestring("");
  }
;


/******************************************************************************
  INVOKE
******************************************************************************/

tInvoke:
  Invoke(standardElements tCorrelation_list)
  {
    integer id;
    integer dpe = mkinteger(0);
    integer negativeControlFlow;
    bool suppressJF = false;

    casestring A_name = mkcasestring("");
    casestring A_joinCondition = mkcasestring("");
    casestring A_suppressJoinFailure = mkcasestring("");
    casestring A_partnerLink = mkcasestring("");
    casestring A_portType = mkcasestring("");
    casestring A_operation = mkcasestring("");
    casestring A_inputVariable = mkcasestring("");
    casestring A_outputVariable = mkcasestring("");    
  }
;


/******************************************************************************
  RECEIVE
******************************************************************************/

tReceive:
  Receive(standardElements tCorrelation_list)
  {
    integer id;
    integer negativeControlFlow;
    bool suppressJF = false;

    casestring A_name = mkcasestring("");
    casestring A_joinCondition = mkcasestring("");
    casestring A_suppressJoinFailure = mkcasestring("");
    casestring A_partnerLink = mkcasestring("");
    casestring A_portType = mkcasestring("");
    casestring A_operation = mkcasestring("");
    casestring A_variable = mkcasestring("");
    casestring A_createInstance = mkcasestring("");
  }
;


/******************************************************************************
  REPLY
******************************************************************************/

tReply:
  Reply(standardElements tCorrelation_list)
  {
    integer id;
    integer negativeControlFlow;
    bool suppressJF = false;

    casestring A_name = mkcasestring("");
    casestring A_joinCondition = mkcasestring("");
    casestring A_suppressJoinFailure = mkcasestring("");
    casestring A_partnerLink = mkcasestring("");
    casestring A_portType = mkcasestring("");
    casestring A_operation = mkcasestring("");
    casestring A_variable = mkcasestring("");
    casestring A_faultName = mkcasestring("");
  }
;


/******************************************************************************
  ASSIGN
******************************************************************************/

tAssign:
  Assign(standardElements tCopy_list)
  {
    integer id;
    integer negativeControlFlow;
    bool suppressJF = false;

    casestring A_name = mkcasestring("");
    casestring A_joinCondition = mkcasestring("");
    casestring A_suppressJoinFailure = mkcasestring("");
  }
;

tCopy_list:
  list tCopy
;

tCopy:
  Copy(tFrom tTo)
;

tFrom:
  From()
  {
    integer id;

    casestring literal = mkcasestring("");

    casestring A_variable = mkcasestring("");
    casestring A_part = mkcasestring("");
    casestring A_partnerLink = mkcasestring("");
    casestring A_endpointReference = mkcasestring("");
    casestring A_property = mkcasestring("");
    casestring A_expression = mkcasestring("");
  }
;

tTo:
  To()
  {
    integer id;

    casestring A_variable = mkcasestring("");
    casestring A_part = mkcasestring("");
    casestring A_partnerLink = mkcasestring("");
    casestring A_property = mkcasestring("");
  }
;


/******************************************************************************
  WAIT
******************************************************************************/

tWait:
  WaitFor(standardElements)
| WaitUntil(standardElements)
  {
    integer id;
    integer negativeControlFlow;
    bool suppressJF = false;

    casestring A_name = mkcasestring("");
    casestring A_joinCondition = mkcasestring("");
    casestring A_suppressJoinFailure = mkcasestring("");
    casestring A_for = mkcasestring("");
    casestring A_until = mkcasestring("");
  }
;


/******************************************************************************
  THROW
******************************************************************************/

tThrow:
  Throw(standardElements)
  {
    integer id;
    integer negativeControlFlow;
    bool suppressJF = false;

    casestring A_name = mkcasestring("");
    casestring A_joinCondition = mkcasestring("");
    casestring A_suppressJoinFailure = mkcasestring("");
    casestring A_faultName = mkcasestring("");
    casestring A_faultVariable = mkcasestring("");
  }
;


/******************************************************************************
  COMPENSATE
******************************************************************************/

tCompensate:
  Compensate(standardElements)
  {
    integer id;
    integer negativeControlFlow;
    bool suppressJF = false;

    bool inFaultHandler = true;
    
    casestring A_name = mkcasestring("");
    casestring A_joinCondition = mkcasestring("");
    casestring A_suppressJoinFailure = mkcasestring("");
    casestring A_scope = mkcasestring("");
  }
;


/******************************************************************************
  TERMINATE
******************************************************************************/

tTerminate:
  Terminate(standardElements)
  {
    integer id;
    integer negativeControlFlow;
    bool suppressJF = false;

    casestring A_name = mkcasestring("");
    casestring A_joinCondition = mkcasestring("");
    casestring A_suppressJoinFailure = mkcasestring("");
  }
;


/******************************************************************************
  FLOW
******************************************************************************/

tFlow:
  Flow(standardElements tLink_list activity_list)
  {
    integer id;
    integer dpe = mkinteger(0);
    integer negativeControlFlow;
    bool suppressJF = false;

    casestring A_name = mkcasestring("");
    casestring A_joinCondition = mkcasestring("");
    casestring A_suppressJoinFailure = mkcasestring("");
  }
;

activity_list:
  list activity 
  {
    integer dpe = mkinteger(0);
  }
;

tLink_list:
  list tLink
;

tLink:
  Link()
  {
    integer id;

    casestring A_name = mkcasestring("");
  }
;


/******************************************************************************
  SWITCH
******************************************************************************/

tSwitch:
  Switch(standardElements tCase_list tOtherwise)
  {
    integer id;
    integer dpe = mkinteger(0);
    integer negativeControlFlow;
    bool suppressJF = false;

    casestring A_name = mkcasestring("");
    casestring A_joinCondition = mkcasestring("");
    casestring A_suppressJoinFailure = mkcasestring("");
  }
;

tCase_list:
  list tCase
  {
    integer dpe = mkinteger(0);
  }
;

tCase:
  Case(activity)
  {
    integer id;
    integer dpe = mkinteger(0);
    
    casestring A_condition = mkcasestring("");
  }
;

tOtherwise:
  Otherwise(activity)
  {
    integer id;
    integer dpe = mkinteger(0);
  }
;


/******************************************************************************
  WHILE
******************************************************************************/

tWhile:
  While(standardElements activity)
  {
    integer id;
    integer negativeControlFlow;
    bool suppressJF = false;

    casestring A_name = mkcasestring("");
    casestring A_joinCondition = mkcasestring("");
    casestring A_suppressJoinFailure = mkcasestring("");
    casestring A_condition = mkcasestring("");
  }
;


/******************************************************************************
  SEQUENCE
******************************************************************************/

tSequence:
  Sequence(standardElements activity_list)
  {
    integer id;
    integer dpe = mkinteger(0);
    integer negativeControlFlow;
    bool suppressJF = false;

    casestring A_name = mkcasestring("");
    casestring A_joinCondition = mkcasestring("");
    casestring A_suppressJoinFailure = mkcasestring("");
  }
;


/******************************************************************************
  PICK
******************************************************************************/

tPick:
  Pick(standardElements tOnMessage_list tOnAlarm_list)
  {
    integer id;
    integer dpe = mkinteger(0);
    integer negativeControlFlow;
    bool suppressJF = false;

    casestring A_name = mkcasestring("");
    casestring A_joinCondition = mkcasestring("");
    casestring A_suppressJoinFailure = mkcasestring("");
    casestring A_createInstance = mkcasestring("");
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
      tEventHandlers
      tStop
      activity
      )
  {
    integer id;
    integer dpe = mkinteger(0);
    integer negativeControlFlow;
    bool suppressJF = false;

    integer parentScopeId;

    casestring A_name = mkcasestring("");
    casestring A_joinCondition = mkcasestring("");
    casestring A_suppressJoinFailure = mkcasestring("");
    casestring A_variableAccessSerializable = mkcasestring("");
  }
;



/******************************************************************************
  STANDARD ELEMENTS
******************************************************************************/

standardElements:
  StandardElements(tTarget_list tSource_list joinCondition)
  {
    integer dpe = mkinteger(0);
    integer negativeControlFlow;
    bool suppressJF = false;

    integer parentId;
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
    integer id;
    integer negativeControlFlow;
    
    casestring A_linkName = mkcasestring("");
  }
;

tSource_list:
  list tSource
  {
    integer dpe = mkinteger(0);
  }
;

tSource:
  Source()
  {
    integer id;
    integer dpe = mkinteger(0);
    integer negativeControlFlow;

    casestring A_linkName = mkcasestring("");
    casestring A_transitionCondition = mkcasestring("");
    casestring A_variableAccessSerializable = mkcasestring("");
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
