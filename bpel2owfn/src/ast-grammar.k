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
 *          - last changed: \$Date: 2006/07/08 16:31:14 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file bpel-abstract.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.91 $
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


%{ KC_TYPES_HEADER

#include <string>
using std::string;

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
    bool suppressJF = false;
    bool hasEH = false;

    string A_name;
    string A_targetNamespace;
    string A_queryLanguage;
    string A_expressionLanguage;
    string A_suppressJoinFailure;
    string A_enableInstanceCompensation;
    string A_abstractProcess;
    string A_xmlns;
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
    int id = 0;
    int dpe = 0;
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
    int id = 0;

    bool inPartners = false;

    string A_name;
    string A_partnerLinkType;
    string A_myRole;
    string A_partnerRole;
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

    string A_name;
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

    string A_faultName;
    string A_faultVariable;
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
    int id = 0;
    int parentScopeId;
  }
;

tOnMessage_list:
  list tOnMessage
  {
    int dpe = 0;
  }
;

tOnAlarm_list:
  list tOnAlarm
  {
    int dpe = 0;
  }
;

tOnMessage:
  OnMessage(activity)
  {
    int id = 0;
    int dpe = 0;

    string A_partnerLink;
    string A_portType;
    string A_operation;
    string A_variable;
  }
;

tOnAlarm:
  OnAlarm(activity)
  {
    int id = 0;
    int dpe = 0;
    
    string A_for;
    string A_until;
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
    int id = 0;

    int parentScopeId;
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
    int id = 0;

    string A_name;
    string A_messageType;
    string A_type;
    string A_element;
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

    string A_name;
    string A_properties;
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

    string A_set;
    string A_initiate;
    string A_pattern;
  }
;


/******************************************************************************
  EMPTY
******************************************************************************/

tEmpty:
  Empty(standardElements)
  {
    int id = 0;
    int negativeControlFlow = 0;
    bool suppressJF = false;

    string A_name;
    string A_joinCondition;
    string A_suppressJoinFailure;
  }
;


/******************************************************************************
  INVOKE
******************************************************************************/

tInvoke:
  Invoke(standardElements tCorrelation_list)
  {
    int id = 0;
    int dpe = 0;
    int negativeControlFlow = 0;
    bool suppressJF = false;

    string A_name;
    string A_joinCondition;
    string A_suppressJoinFailure;
    string A_partnerLink;
    string A_portType;
    string A_operation;
    string A_inputVariable;
    string A_outputVariable;    
  }
;


/******************************************************************************
  RECEIVE
******************************************************************************/

tReceive:
  Receive(standardElements tCorrelation_list)
  {
    int id = 0;
    int negativeControlFlow = 0;
    bool suppressJF = false;

    string A_name;
    string A_joinCondition;
    string A_suppressJoinFailure;
    string A_partnerLink;
    string A_portType;
    string A_operation;
    string A_variable;
    string A_createInstance;
  }
;


/******************************************************************************
  REPLY
******************************************************************************/

tReply:
  Reply(standardElements tCorrelation_list)
  {
    int id = 0;
    int negativeControlFlow = 0;
    bool suppressJF = false;

    string A_name;
    string A_joinCondition;
    string A_suppressJoinFailure;
    string A_partnerLink;
    string A_portType;
    string A_operation;
    string A_variable;
    string A_faultName;
  }
;


/******************************************************************************
  ASSIGN
******************************************************************************/

tAssign:
  Assign(standardElements tCopy_list)
  {
    int id = 0;
    int negativeControlFlow = 0;
    bool suppressJF = false;

    string A_name;
    string A_joinCondition;
    string A_suppressJoinFailure;
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
    int id = 0;

    string literal;

    string A_variable;
    string A_part;
    string A_partnerLink;
    string A_endpointReference;
    string A_property;
    string A_expression;
  }
;

tTo:
  To()
  {
    int id = 0;

    string A_variable;
    string A_part;
    string A_partnerLink;
    string A_property;
  }
;


/******************************************************************************
  WAIT
******************************************************************************/

tWait:
  WaitFor(standardElements)
| WaitUntil(standardElements)
  {
    int id = 0;
    int negativeControlFlow = 0;
    bool suppressJF = false;

    string A_name;
    string A_joinCondition;
    string A_suppressJoinFailure;
    string A_for;
    string A_until;
  }
;


/******************************************************************************
  THROW
******************************************************************************/

tThrow:
  Throw(standardElements)
  {
    int id = 0;
    int negativeControlFlow = 0;
    bool suppressJF = false;

    string A_name;
    string A_joinCondition;
    string A_suppressJoinFailure;
    string A_faultName;
    string A_faultVariable;
  }
;


/******************************************************************************
  COMPENSATE
******************************************************************************/

tCompensate:
  Compensate(standardElements)
  {
    int id = 0;
    int negativeControlFlow = 0;
    bool suppressJF = false;

    bool inFaultHandler = true;
    
    string A_name;
    string A_joinCondition;
    string A_suppressJoinFailure;
    string A_scope;
  }
;


/******************************************************************************
  TERMINATE
******************************************************************************/

tTerminate:
  Terminate(standardElements)
  {
    int id = 0;
    int negativeControlFlow = 0;
    bool suppressJF = false;

    string A_name;
    string A_joinCondition;
    string A_suppressJoinFailure;
  }
;


/******************************************************************************
  FLOW
******************************************************************************/

tFlow:
  Flow(standardElements tLink_list activity_list)
  {
    int id = 0;
    int dpe = 0;
    int negativeControlFlow = 0;
    bool suppressJF = false;

    string A_name;
    string A_joinCondition;
    string A_suppressJoinFailure;
  }
;

activity_list:
  list activity 
  {
    int dpe = 0;
  }
;

tLink_list:
  list tLink
;

tLink:
  Link()
  {
    int id = 0;

    string A_name;

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
    int id = 0;
    int dpe = 0;
    int negativeControlFlow = 0;
    bool suppressJF = false;

    string A_name;
    string A_joinCondition;
    string A_suppressJoinFailure;
  }
;

tCase_list:
  list tCase
  {
    int dpe = 0;
  }
;

tCase:
  Case(activity)
  {
    int id = 0;
    int dpe = 0;
    
    string A_condition;
  }
;

tOtherwise:
  Otherwise(activity)
  {
    int id = 0;
    int dpe = 0;
  }
;


/******************************************************************************
  WHILE
******************************************************************************/

tWhile:
  While(standardElements activity)
  {
    int id = 0;
    int negativeControlFlow = 0;
    bool suppressJF = false;

    string A_name;
    string A_joinCondition;
    string A_suppressJoinFailure;
    string A_condition;
  }
;


/******************************************************************************
  SEQUENCE
******************************************************************************/

tSequence:
  Sequence(standardElements activity_list)
  {
    int id = 0;
    int dpe = 0;
    int negativeControlFlow = 0;
    bool suppressJF = false;

    string A_name;
    string A_joinCondition;
    string A_suppressJoinFailure;
  }
;


/******************************************************************************
  PICK
******************************************************************************/

tPick:
  Pick(standardElements tOnMessage_list tOnAlarm_list)
  {
    int id = 0;
    int dpe = 0;
    int negativeControlFlow = 0;
    bool suppressJF = false;

    string A_name;
    string A_joinCondition;
    string A_suppressJoinFailure;
    string A_createInstance;
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
    int id = 0;
    int dpe = 0;
    int negativeControlFlow = 0;
    bool suppressJF = false;
    bool hasEH = false;

    int parentScopeId;

    string A_name;
    string A_joinCondition;
    string A_suppressJoinFailure;
    string A_variableAccessSerializable;
  }
;



/******************************************************************************
  STANDARD ELEMENTS
******************************************************************************/

standardElements:
  StandardElements(tTarget_list tSource_list joinCondition)
  {
    int dpe = 0;
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
    int negativeControlFlow = 0;
    
    string A_linkName;
  }
;

tSource_list:
  list tSource
  {
    int dpe = 0;
  }
;

tSource:
  Source()
  {
    int id = 0;
    int dpe = 0;
    int negativeControlFlow = 0;

    string A_linkName;
    string A_transitionCondition;
    string A_variableAccessSerializable;
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
