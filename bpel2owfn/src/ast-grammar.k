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
 *          - last changed: \$Date: 2006/03/27 09:21:51 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universit�t zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file bpel-abstract.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.75 $
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
    casestring faultName;
    integer id;
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
  }
;

tOnAlarm:
  OnAlarm(activity)
  {
    casestring For; // "for" is a keyword...
    casestring until;

    /// additional attribute for DPE
    integer dpe = mkinteger(0);
    
    integer id;
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
  }
;


/******************************************************************************
  CORRELATIONS
******************************************************************************/

tCorrelation_list:
  list tCorrelation
  {
//    bool initiateCorrelationSet;
  }
;

tCorrelation:
  Correlation()
  {
    integer id;
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
  }
;

tTo:
  To()
  {
    integer id;
  }
;


/******************************************************************************
  WAIT
******************************************************************************/

tWait:
  Wait(standardElements)
  {
    casestring For; // "for" is a keyword
    casestring until;

    integer negativeControlFlow;
    integer id;
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
    casestring condition;

    /// additional attribute for DPE
    integer dpe = mkinteger(0);
    
    integer id;
  }
;

tOtherwise:
  Otherwise(activity)
  {
    /// additional attribute for DPE
    integer dpe = mkinteger(0);
  }
;


/******************************************************************************
  WHILE
******************************************************************************/

tWhile:
  While(standardElements activity)
  {
    casestring condition;
    integer negativeControlFlow;
    integer id;
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
  }
;



/******************************************************************************
  STANDARD ELEMENTS
******************************************************************************/

standardElements:
  StandardElements(tTarget_list tSource_list)
  {
    integer parentId;
    casestring suppressJoinFailure; // actually a value of the parent

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
    /// additional attribute containing the unique link name
    casestring linkID;

    /// used here to create the patterns
    integer negativeControlFlow;
    
    integer id;
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
    /// additional attribute containing the unique link name
    casestring linkID;

    /// additional attribute for DPE
    integer dpe = mkinteger(0);

    /// used here to create the patterns
    integer negativeControlFlow;
    
    integer id;
  }
;
