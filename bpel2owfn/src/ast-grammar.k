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
 *          - last changed: \$Date: 2006/01/20 07:47:30 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universit�t zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \note    This file was created using Kimwitu++ version 2.3.8 (C) 1998-2003
 *          Humboldt-University of Berlin reading file bpel-abstract.k. See
 *          http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.51 $
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
    casestring name;
    casestring targetNamespace;
    casestring queryLanguage = mkcasestring("http://www.w3.org/TR/1999/REC-xpath-19991116");
    casestring expressionLanguage = mkcasestring("http://www.w3.org/TR/1999/REC-xpath-19991116");
    casestring suppressJoinFailure = mkcasestring("no");
    casestring enableInstanceCompensation = mkcasestring("no");
    casestring abstractProcess = mkcasestring("no");
    casestring xmlns = mkcasestring("http://schemas.xmlsoap.org/ws/2003/03/business-process/");

    /// has this process an event handler?
    bool hasEH;

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
    
    integer dpe;
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
    casestring name;
    casestring partnerLinkType;
    casestring myRole;
    casestring partnerRole;
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
    casestring name;
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
  }  
;

tCatch_list:
  list tCatch
;

tCatch:
  Catch(activity)
  {
    casestring faultName;
    casestring faultVariable;

    /// additional attribute containing the unique variable name    
    casestring variableID;
  }
;

tCatchAll:
  CatchAll(activity)
| NoCatchAll()
  {
    casestring faultName;
    casestring faultVariable;
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
    casestring partnerLink;
    casestring portType;
    casestring operation;
    casestring variable;

    /// additional attribute containing the unique variable name    
    casestring variableID;

    /// additional attribute containing the channel name
    casestring channelID;

    /// additional attribute for DPE
    integer dpe = mkinteger(0);
  }
;

tOnAlarm:
  OnAlarm(activity)
  {
    casestring For; // "for" is a keyword...
    casestring until;

    /// additional attribute for DPE
    integer dpe = mkinteger(0);
  } 
;


/******************************************************************************
  COMPENSATION HANDLERS
******************************************************************************/

tCompensationHandler:
  implicitCompensationHandler()
| userDefinedCompensationHandler(activity)
  {
    integer parentScopeId;

    /// handler contains <compensate/> (i.e. pattern in Fig. 43)
    bool compensateWithoutScope = false;

    /// handler contains <compensate scope="A"/> (i.e. pattern Fig. 44)
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
    casestring name;
    casestring messageType;
    casestring type;
    casestring element;

    /// additional attribute containing the unique variable name    
    casestring variableID;
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
    casestring properties;
    casestring name;
  }
;


/******************************************************************************
  CORRELATIONS
******************************************************************************/

tCorrelation_list:
  list tCorrelation
  {
    bool initiateCorrelationSet;
  }
;

tCorrelation:
  Correlation()
  {
    casestring set;
    casestring initiate = mkcasestring("no");
    casestring pattern;
  }
;


/******************************************************************************
  EMPTY
******************************************************************************/

tEmpty:
  Empty(standardElements)
  {
    casestring name;
    casestring joinCondition;
    casestring suppressJoinFailure;
    
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
    casestring name;
    casestring joinCondition;
    casestring suppressJoinFailure;    
	  
    casestring partnerLink;
    casestring portType;
    casestring operation;
    casestring inputVariable;
    casestring outputVariable;
    
    /// additional attributes containing the unique variable name    
    casestring variableIDin;
    casestring variableIDout;

    /// additional attribute containing the channel name
    casestring channelID;

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
    casestring name;
    casestring joinCondition;
    casestring suppressJoinFailure;    

    casestring partnerLink;
    casestring portType;
    casestring operation;
    casestring variable;
    casestring createInstance = mkcasestring("no");

    /// additional attribute containing the unique variable name    
    casestring variableID;

    /// additional attribute containing the channel name
    casestring channelID;
    
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
    casestring name;
    casestring joinCondition;
    casestring suppressJoinFailure;        

    casestring partnerLink;
    casestring portType;
    casestring operation;
    casestring variable;
    casestring faultName;

    /// additional attribute containing the unique variable name    
    casestring variableID;

    /// additional attribute containing the channel name
    casestring channelID;

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
    casestring name;
    casestring joinCondition;
    casestring suppressJoinFailure;

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
    casestring variable;
    casestring part;
    casestring query; // only excecutable processes
    casestring partnerLink;
    casestring endpointReference;
    casestring property;
    casestring expression;
    casestring opaque; // only abstract processes

    casestring literalValue = mkcasestring(""); // used in case of <from>literal</from>

    /// additional attribute containing the unique variable name    
    casestring variableID;
  }
;

tTo:
  To()
  {
    casestring variable;
    casestring part;
    casestring partnerLink;
    casestring property;

    /// additional attribute containing the unique variable name    
    casestring variableID;
  }
;


/******************************************************************************
  WAIT
******************************************************************************/

tWait:
  Wait(standardElements)
  {
    casestring name;
    casestring joinCondition;
    casestring suppressJoinFailure;
	  
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
    casestring name;
    casestring joinCondition;
    casestring suppressJoinFailure;

    casestring faultName;
    casestring faultVariable;

    /// additional attribute containing the unique variable name    
    casestring variableID;

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
    casestring name;
    casestring joinCondition;
    casestring suppressJoinFailure;

    casestring scope;

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
    casestring name;
    casestring joinCondition;
    casestring suppressJoinFailure;

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
    casestring name;
    casestring joinCondition;
    casestring suppressJoinFailure;

    /// additional attribute for DPE
    integer dpe = mkinteger(0);

    integer negativeControlFlow;
    integer id;
  }
;

activity_list:
  list activity
;

tLink_list:
  list tLink
;

tLink:
  Link()
  {
    casestring name;

    /// additional attribute containing the unique link name
    casestring linkID;
  }
;


/******************************************************************************
  SWITCH
******************************************************************************/

tSwitch:
  Switch(standardElements tCase_list tOtherwise)
  {
    casestring name;
    casestring joinCondition;
    casestring suppressJoinFailure;

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

    casestring name;
    casestring joinCondition;
    casestring suppressJoinFailure;
    
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
    casestring name;
    casestring joinCondition;
    casestring suppressJoinFailure;

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
    casestring name;
    casestring joinCondition;
    casestring suppressJoinFailure;
    
    casestring createInstance = mkcasestring("no");

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
    casestring name;
    casestring joinCondition;
    casestring suppressJoinFailure;

    casestring variableAccessSerializable = mkcasestring("no");

    integer negativeControlFlow;
    integer id;

    /// additional attribute used for inter-scope communication (push-places)
    integer parentScopeId;

    /// additional attribute for DPE
    integer dpe = mkinteger(0);

    /// has this scope an event handler?
    bool hasEH;    
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
    casestring joinCondition;       // actually a value of the parent

    /// additional attribute for DPE
    integer dpe = mkinteger(0);

    /// used here to create the patterns
    integer negativeControlFlow;
  }
;

tTarget_list:
  list tTarget
;

tTarget:
  Target()
  {
    casestring linkName;

    /// additional attribute containing the unique link name
    casestring linkID;

    /// used here to create the patterns
    integer negativeControlFlow;
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
    casestring linkName;
    casestring transitionCondition = mkcasestring("true");

    /// additional attribute containing the unique link name
    casestring linkID;

    /// additional attribute for DPE
    integer dpe = mkinteger(0);

    /// used here to create the patterns
    integer negativeControlFlow;
  }
;
