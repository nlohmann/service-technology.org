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
 *          - last changed: \$Date: 2006/07/04 12:45:46 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file bpel-abstract.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.82 $
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

    // attributes
    casestring name = mkcasestring("");
    casestring targetNamespace = mkcasestring("");
    casestring queryLanguage = mkcasestring("");
    casestring expressionLanguage = mkcasestring("");
    casestring suppressJoinFailure = mkcasestring("");
    casestring enableInstanceCompensation = mkcasestring("");
    casestring abstractProcess = mkcasestring("");
    casestring xmlns = mkcasestring("");

    bool suppressJF = false; // required initialization!
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

    /** 
     * 0 = positive control flow
     * 1 = control flow in fault handler
     * 2 = control flow in compensation handler
     */
    integer negativeControlFlow;
    
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

    // attributes
    casestring name = mkcasestring("");
    casestring partnerLinkType = mkcasestring("");
    casestring myRole = mkcasestring("");
    casestring partnerRole = mkcasestring("");
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

    // attribute
    casestring name = mkcasestring("");
  }
;


/******************************************************************************
  FAULT HANDLERS
******************************************************************************/

tFaultHandlers:
  userDefinedFaultHandler(tCatch_list tCatchAll)
| implicitFaultHandler()
  {
    bool inProcess;
    integer parentScopeId;

    // needed to compact pattern
    bool hasCatchAll; 
    integer id;
  }  
;

tCatch_list:
  list tCatch
;

tCatch:
  Catch(activity)
  {
    integer id;

    // attributes
    casestring faultName = mkcasestring("");
    casestring faultVariable = mkcasestring("");
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
    integer parentScopeId;
    integer id;
  }
;

tOnMessage_list:
  list tOnMessage
  {
    /// additional attribute for DPE
    integer dpe = mkinteger(0);
  }
;

tOnAlarm_list:
  list tOnAlarm
  {
    /// additional attribute for DPE
    integer dpe = mkinteger(0);
  }
;

tOnMessage:
  OnMessage(activity)
  {
    /// additional attribute for DPE
    integer dpe = mkinteger(0);
    
    integer id;

    // attributes
    casestring partnerLink = mkcasestring("");
    casestring portType = mkcasestring("");
    casestring operation = mkcasestring("");
    casestring variable = mkcasestring("");
  }
;

tOnAlarm:
  OnAlarm(activity)
  {
    /// additional attribute for DPE
    integer dpe = mkinteger(0);
    
    integer id;

    // attributes
    casestring For = mkcasestring("");
    casestring until = mkcasestring("");
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
    integer parentScopeId;

    /// handler contains <compensate/> (i.e. pattern in Fig. 43)
    bool compensateWithoutScope = false;

    /// handler contains <compensate scope="A"/> (i.e. pattern Fig. 44)
    bool compensateWithScope = false;
    integer id;
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

    // attributes
    casestring name = mkcasestring("");
    casestring messageType = mkcasestring("");
    casestring type = mkcasestring("");
    casestring element = mkcasestring("");
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

    // attributes
    casestring name = mkcasestring("");
    casestring properties = mkcasestring("");
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

    // attributes
    casestring set = mkcasestring("");
    casestring initiate = mkcasestring("");
    casestring pattern = mkcasestring("");
  }
;


/******************************************************************************
  EMPTY
******************************************************************************/

tEmpty:
  Empty(standardElements)
  {
    integer negativeControlFlow;
    integer id;

    // attributes
    casestring name = mkcasestring("");
    casestring joinCondition = mkcasestring("");
    casestring suppressJoinFailure = mkcasestring("");

    bool suppressJF = false; // required initialization!
  }
;


/******************************************************************************
  INVOKE
******************************************************************************/

tInvoke:
  Invoke(standardElements tCorrelation_list)
  {
    /// additional attribute for DPE
    integer dpe = mkinteger(0);

    integer negativeControlFlow;
    integer id;

    // attributes
    casestring name = mkcasestring("");
    casestring joinCondition = mkcasestring("");
    casestring suppressJoinFailure = mkcasestring("");
    casestring partnerLink = mkcasestring("");
    casestring portType = mkcasestring("");
    casestring operation = mkcasestring("");
    casestring inputVariable = mkcasestring("");
    casestring outputVariable = mkcasestring("");    

    bool suppressJF = false; // required initialization!
  }
;


/******************************************************************************
  RECEIVE
******************************************************************************/

tReceive:
  Receive(standardElements tCorrelation_list)
  {
    integer negativeControlFlow;
    integer id;

    // attributes
    casestring name = mkcasestring("");
    casestring joinCondition = mkcasestring("");
    casestring suppressJoinFailure = mkcasestring("");
    casestring partnerLink = mkcasestring("");
    casestring portType = mkcasestring("");
    casestring operation = mkcasestring("");
    casestring variable = mkcasestring("");
    casestring createInstance = mkcasestring("");

    bool suppressJF = false; // required initialization!
  }
;


/******************************************************************************
  REPLY
******************************************************************************/

tReply:
  Reply(standardElements tCorrelation_list)
  {
    integer negativeControlFlow;
    integer id;

    // attributes
    casestring name = mkcasestring("");
    casestring joinCondition = mkcasestring("");
    casestring suppressJoinFailure = mkcasestring("");
    casestring partnerLink = mkcasestring("");
    casestring portType = mkcasestring("");
    casestring operation = mkcasestring("");
    casestring variable = mkcasestring("");
    casestring faultName = mkcasestring("");

    bool suppressJF = false; // required initialization!
  }
;


/******************************************************************************
  ASSIGN
******************************************************************************/

tAssign:
  Assign(standardElements tCopy_list)
  {
    integer negativeControlFlow;
    integer id;

    // attributes
    casestring name = mkcasestring("");
    casestring joinCondition = mkcasestring("");
    casestring suppressJoinFailure = mkcasestring("");

    bool suppressJF = false; // required initialization!
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

    // attributes
    casestring name = mkcasestring("");
    casestring joinCondition = mkcasestring("");
    casestring suppressJoinFailure = mkcasestring("");
    casestring variable = mkcasestring("");
    casestring part = mkcasestring("");
    casestring partnerLink = mkcasestring("");
    casestring endpointReference = mkcasestring("");
    casestring property = mkcasestring("");
    casestring expression = mkcasestring("");

    casestring literal = mkcasestring("");
  }
;

tTo:
  To()
  {
    integer id;

    // attributes
    casestring name = mkcasestring("");
    casestring joinCondition = mkcasestring("");
    casestring suppressJoinFailure = mkcasestring("");
    casestring variable = mkcasestring("");
    casestring part = mkcasestring("");
    casestring partnerLink = mkcasestring("");
    casestring property = mkcasestring("");
  }
;


/******************************************************************************
  WAIT
******************************************************************************/

tWait:
  WaitFor(standardElements)
| WaitUntil(standardElements)
  {
    integer negativeControlFlow;
    integer id;

    // attributes
    casestring name = mkcasestring("");
    casestring joinCondition = mkcasestring("");
    casestring suppressJoinFailure = mkcasestring("");
    casestring For = mkcasestring("");
    casestring until = mkcasestring("");

    bool suppressJF = false; // required initialization!
  }
;


/******************************************************************************
  THROW
******************************************************************************/

tThrow:
  Throw(standardElements)
  {
    integer negativeControlFlow;
    integer id;

    // attributes
    casestring name = mkcasestring("");
    casestring joinCondition = mkcasestring("");
    casestring suppressJoinFailure = mkcasestring("");
    casestring faultName = mkcasestring("");
    casestring faultVariable = mkcasestring("");

    bool suppressJF = false; // required initialization!
  }
;


/******************************************************************************
  COMPENSATE
******************************************************************************/

tCompensate:
  Compensate(standardElements)
  {
    /// additional attribute used to identify the correct pattern
    bool inFaultHandler = true;
    
    integer negativeControlFlow;
    integer id;

    // attributes
    casestring name = mkcasestring("");
    casestring joinCondition = mkcasestring("");
    casestring suppressJoinFailure = mkcasestring("");

    bool suppressJF = false; // required initialization!
  }
;


/******************************************************************************
  TERMINATE
******************************************************************************/

tTerminate:
  Terminate(standardElements)
  {
    integer negativeControlFlow;
    integer id;

    // attributes
    casestring name = mkcasestring("");
    casestring joinCondition = mkcasestring("");
    casestring suppressJoinFailure = mkcasestring("");

    bool suppressJF = false; // required initialization!
  }
;


/******************************************************************************
  FLOW
******************************************************************************/

tFlow:
  Flow(standardElements tLink_list activity_list)
  {
    /// additional attribute for DPE
    integer dpe = mkinteger(0);

    integer negativeControlFlow;
    integer id;

    // attributes
    casestring name = mkcasestring("");
    casestring joinCondition = mkcasestring("");
    casestring suppressJoinFailure = mkcasestring("");

    bool suppressJF = false; // required initialization!
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

    // attribute
    casestring name = mkcasestring("");
  }
;


/******************************************************************************
  SWITCH
******************************************************************************/

tSwitch:
  Switch(standardElements tCase_list tOtherwise)
  {
    /// additional attribute for DPE
    integer dpe = mkinteger(0);

    integer negativeControlFlow;
    integer id;

    // attributes
    casestring name = mkcasestring("");
    casestring joinCondition = mkcasestring("");
    casestring suppressJoinFailure = mkcasestring("");

    bool suppressJF = false; // required initialization!
  }
;

tCase_list:
  list tCase
  {
    /// additional attribute for DPE
    integer dpe = mkinteger(0);
  }
;

tCase:
  Case(activity)
  {
    /// additional attribute for DPE
    integer dpe = mkinteger(0);
    
    integer id;

    // attribute
    casestring condition = mkcasestring("");
  }
;

tOtherwise:
  Otherwise(activity)
  {
    /// additional attribute for DPE
    integer dpe = mkinteger(0);
    integer id;
  }
;


/******************************************************************************
  WHILE
******************************************************************************/

tWhile:
  While(standardElements activity)
  {
    integer negativeControlFlow;
    integer id;

    // attributes
    casestring name = mkcasestring("");
    casestring joinCondition = mkcasestring("");
    casestring suppressJoinFailure = mkcasestring("");
    casestring condition = mkcasestring("");

    bool suppressJF = false; // required initialization!
  }
;


/******************************************************************************
  SEQUENCE
******************************************************************************/

tSequence:
  Sequence(standardElements activity_list)
  {
    integer negativeControlFlow;
    integer id;

    /// additional attribute for DPE
    integer dpe = mkinteger(0);

    // attributes
    casestring name = mkcasestring("");
    casestring joinCondition = mkcasestring("");
    casestring suppressJoinFailure = mkcasestring("");

    bool suppressJF = false; // required initialization!
  }
;


/******************************************************************************
  PICK
******************************************************************************/

tPick:
  Pick(standardElements tOnMessage_list tOnAlarm_list)
  {
    /// additional attribute for DPE
    integer dpe = mkinteger(0);

    integer negativeControlFlow;
    integer id;

    // attributes
    casestring name = mkcasestring("");
    casestring joinCondition = mkcasestring("");
    casestring suppressJoinFailure = mkcasestring("");
    casestring createInstance = mkcasestring("");

    bool suppressJF = false; // required initialization!
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
    integer negativeControlFlow;
    integer id;

    /// additional attribute used for inter-scope communication (push-places)
    integer parentScopeId;

    /// additional attribute for DPE
    integer dpe = mkinteger(0);

    // attributes
    casestring name = mkcasestring("");
    casestring joinCondition = mkcasestring("");
    casestring suppressJoinFailure = mkcasestring("");
    casestring variableAccessSerializable = mkcasestring("");

    bool suppressJF = false; // required initialization!
  }
;



/******************************************************************************
  STANDARD ELEMENTS
******************************************************************************/

standardElements:
  StandardElements(tTarget_list tSource_list joinCondition)
  {
    integer parentId;
    casestring suppressJoinFailure = mkcasestring(""); // actually a value of the parent

    /// additional attribute for DPE
    integer dpe = mkinteger(0);

    /// used here to create the patterns
    integer negativeControlFlow;

    ///
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
    /// used here to create the patterns
    integer negativeControlFlow;
    
    integer id;

    // attributes
    casestring linkName = mkcasestring("");
  }
;

tSource_list:
  list tSource
  {
    /// additional attribute for DPE
    integer dpe = mkinteger(0);
  }
;

tSource:
  Source()
  {
    /// additional attribute for DPE
    integer dpe = mkinteger(0);

    /// used here to create the patterns
    integer negativeControlFlow;

    integer id;

    // attributes
    casestring linkName = mkcasestring("");
    casestring transitionCondition = mkcasestring("");
    casestring variableAccessSerializable = mkcasestring("");
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
