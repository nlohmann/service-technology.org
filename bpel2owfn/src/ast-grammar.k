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
 *          - last changed: \$Date: 2005/11/30 14:33:53 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \note    This file was created using Kimwitu++ version 2.3.8 (C) 1998-2003
 *          Humboldt-University of Berlin reading file bpel-abstract.k. See
 *          http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.17 $
 *          - 2005-11-10 (nlohmann) Added doxygen comments.
 *          - 2005-11-17 (nlohmann) Added default attributes.
 * 
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



%{
/*!
 * \class kc::impl_tProcess
 * \brief BPEL's <process>
*/
%}

tProcess:
  Process(tPartnerLink_list tPartner_list tVariable_list tCorrelationSet_list tFaultHandlers_opt tCompensationHandler_opt tEventHandlers_opt tStop activity)
  {
    casestring name;
    casestring targetNamespace;
    casestring queryLanguage = mkcasestring("http://www.w3.org/TR/1999/REC-xpath-19991116");
    casestring expressionLanguage = mkcasestring("http://www.w3.org/TR/1999/REC-xpath-19991116");
    casestring suppressJoinFailure = mkcasestring("no");
    casestring enableInstanceCompensation = mkcasestring("no");
    casestring abstractProcess = mkcasestring("no");
    casestring xmlns = mkcasestring("http://schemas.xmlsoap.org/ws/2003/03/business-process/");

    integer id;
  }
;


tStop:
  StopInProcess()
| StopInScope()
;


/*---------------------------------------------------------------------------*/

%{
/*!
 * \class kc::impl_activity
 * \brief BPEL's activities
*/
%}

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
  }
;


/******************************************************************************
  PARTNER LINKS
******************************************************************************/

tPartnerLink_list:
  list tPartnerLink
;

%{
/*!
 * \class kc::impl_tPartnerLink
 * \brief BPEL's <partnerLink>
*/
%}

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

%{
/*!
 * \class kc::impl_tPartner
 * \brief BPEL's <partner>
*/
%}

tPartner:
  Partner(tPartnerLink_list)
  {
    casestring name;
  }
;


/******************************************************************************
  FAULT HANDLERS
******************************************************************************/

tFaultHandlers_opt:
  list tFaultHandlers
  {
    bool inProcess;
  }  
;

%{
/*!
 * \class kc::impl_tFaultHandlers
 * \brief BPEL's <faultHandlers>
*/
%}

tFaultHandlers:
  FaultHandlers(tCatch_list tCatchAll_list)
;

tCatch_list:
  list tCatch
;

tCatchAll_list:
  list tCatchAll
;

%{
/*!
 * \class kc::impl_tCatch
 * \brief BPEL's <catch>
*/
%}

tCatch:
  Catch(activity)
  {
    casestring faultName;
    casestring faultVariable;
  }
;

%{
/*!
 * \class kc::impl_tCatchAll
 * \brief BPEL's <catchAll>
*/
%}

tCatchAll:
  CatchAll(activity)
  {
    casestring faultName;
    casestring faultVariable;
  }
;


/******************************************************************************
  EVENT HANDLERS
******************************************************************************/

tEventHandlers_opt:
  list tEventHandlers
;

%{
/*!
 * \class kc::impl_tEventHandlers
 * \brief BPEL's <eventHandlers>
*/
%}

tEventHandlers:
  EventHandlers(tOnMessage_list tOnAlarm_list)
;

tOnMessage_list:
  list tOnMessage
;

tOnAlarm_list:
  list tOnAlarm
;

%{
/*!
 * \class kc::impl_tOnMessage
 * \brief BPEL's <onMessage>
*/
%}

tOnMessage:
  OnMessage(activity)
  {
    casestring partnerLink;
    casestring portType;
    casestring operation;
    casestring variable;
  }
;

%{
/*!
 * \class kc::impl_tonAlarm
 * \brief BPEL's <onAlarm>
*/
%}

tOnAlarm:
  OnAlarm(activity)
  {
    casestring For; // "for" a keyword...
    casestring until;
  } 
;


/******************************************************************************
  COMPENSATION HANDLERS
******************************************************************************/

tCompensationHandler_opt:
  list tCompensationHandler
;

%{
/*!
 * \class kc::impl_tCompensationHandler
 * \brief BPEL's <compensationHandler>
*/
%}

tCompensationHandler:
  CompensationHandler(activity)
;


/******************************************************************************
  VARIABLES
******************************************************************************/

tVariable_list:
  list tVariable
;

%{
/*!
 * \class kc::impl_tVariable
 * \brief BPEL's <variable>
*/
%}

tVariable:
  Variable()
  {
    casestring name;
    casestring messageType;
    casestring type;
    casestring element;
    casestring uniqueID;
  }
;


/******************************************************************************
  CORRELATION SETS
******************************************************************************/

tCorrelationSet_list:
  list tCorrelationSet
;

%{
/*!
 * \class kc::impl_tCorrelationSet
 * \brief BPEL's <correlationSet>
*/
%}

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

%{
/*!
 * \class kc::impl_tCorrelation
 * \brief BPEL's <correlation>
*/
%}

tCorrelation:
  Correlation()
  {
    casestring set;
    casestring initiate = mkcasestring("no");
    casestring pattern; // originally only in "tCorrelationWithPattern"
  }
;


/******************************************************************************
  EMPTY
******************************************************************************/

%{
/*!
 * \class kc::impl_tEmpty
 * \brief BPEL's <empty>
*/
%}


tEmpty:
  Empty(standardElements)
  {
    casestring name;
    casestring joinCondition;
    casestring suppressJoinFailure = mkcasestring("no");
    
    integer id;
  }
;


/******************************************************************************
  INVOKE
******************************************************************************/

%{
/*!
 * \class kc::impl_tInvoke
 * \brief BPEL's <invoke>
*/
%}

tInvoke:
  Invoke(standardElements tCorrelation_list tCatch_list tCatchAll_list tCompensationHandler_opt)
| Invoke2(standardElements tCorrelation_list)
  {
    casestring name;
    casestring joinCondition;
    casestring suppressJoinFailure = mkcasestring("no");
	  
    casestring partnerLink;
    casestring portType;
    casestring operation;
    casestring inputVariable;
    casestring outputVariable;
    
    integer id;
  }	
;


/******************************************************************************
  RECEIVE
******************************************************************************/

%{
/*!
 * \class kc::impl_tReceive
 * \brief BPEL's <receive>
*/
%}

tReceive:
  Receive(standardElements tCorrelation_list)
  {
    casestring name;
    casestring joinCondition;
    casestring suppressJoinFailure = mkcasestring("no");

    casestring partnerLink;
    casestring portType;
    casestring operation;
    casestring variable;
    casestring createInstance = mkcasestring("no");

    integer id;
  }
;


/******************************************************************************
  REPLY
******************************************************************************/

%{
/*!
 * \class kc::impl_tReply
 * \brief BPEL's <reply>
*/
%}

tReply:
  Reply(standardElements tCorrelation_list)
  {
    casestring name;
    casestring joinCondition;
    casestring suppressJoinFailure = mkcasestring("no");

    casestring partnerLink;
    casestring portType;
    casestring operation;
    casestring variable;
    casestring faultName;

    integer id;
  }
;


/******************************************************************************
  ASSIGN
******************************************************************************/

%{
/*!
 * \class kc::impl_tAssign
 * \brief BPEL's <assign>
*/
%}

tAssign:
  Assign(standardElements tCopy_list)
  {
    casestring name;
    casestring joinCondition;
    casestring suppressJoinFailure = mkcasestring("no");

    integer id;
  }
;

tCopy_list:
  list tCopy
;

%{
/*!
 * \class kc::impl_tCopy
 * \brief BPEL's <copy>
*/
%}

tCopy:
  Copy(tFrom tTo)
;

%{
/*!
 * \class kc::impl_tFrom
 * \brief BPEL's <from>
*/
%}

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
  }
;

%{
/*!
 * \class kc::impl_tTo
 * \brief BPEL's <to>
*/
%}

tTo:
  To()
  {
    //casestring expression; (prohibited)
    //casestring opaque; (prohibited)
    //casestring endpointReference; (prohibited)
    casestring variable;
    casestring part;
    casestring partnerLink;
    casestring property;
  }
;


/******************************************************************************
  WAIT
******************************************************************************/

%{
/*!
 * \class kc::impl_tWait
 * \brief BPEL's <wait>
*/
%}

tWait:
  Wait(standardElements)
  {
    casestring name;
    casestring joinCondition;
    casestring suppressJoinFailure = mkcasestring("no");
	  
    casestring For; // "for" is a keyword
    casestring until;

    integer id;
  }
;


/******************************************************************************
  THROW
******************************************************************************/

%{
/*!
 * \class kc::impl_tThrow
 * \brief BPEL's <throw>
*/
%}

tThrow:
  Throw(standardElements)
  {
    casestring name;
    casestring joinCondition;
    casestring suppressJoinFailure = mkcasestring("no");

    casestring faultName;
    casestring faultVariable;

    integer id;
  }
;


/******************************************************************************
  COMPENSATE
******************************************************************************/

%{
/*!
 * \class kc::impl_tCompensate
 * \brief BPEL's <compensate>
*/
%}

tCompensate:
  Compensate(standardElements)
  {
    casestring name;
    casestring joinCondition;
    casestring suppressJoinFailure = mkcasestring("no");

    casestring scope;

    bool inFaultHandler = true;
    
    integer id;
  }
;


/******************************************************************************
  TERMINATE
******************************************************************************/

%{
/*!
 * \class kc::impl_tTerminate
 * \brief BPEL's <terminate>
*/
%}

tTerminate:
  Terminate(standardElements)
  {
    casestring name;
    casestring joinCondition;
    casestring suppressJoinFailure = mkcasestring("no");

    integer id;
  }
;


/******************************************************************************
  FLOW
******************************************************************************/

%{
/*!
 * \class kc::impl_tFlow
 * \brief BPEL's <flow>
*/
%}

tFlow:
  Flow(standardElements tLink_list activity_list)
  {
    casestring name;
    casestring joinCondition;
    casestring suppressJoinFailure = mkcasestring("no");

    integer id;
  }
;

activity_list:
  list activity
;

tLink_list:
  list tLink
;

%{
/*!
 * \class kc::impl_tLink
 * \brief BPEL's <link>
*/
%}

tLink:
  Link()
  {
    casestring name;
  }
;


/******************************************************************************
  SWITCH
******************************************************************************/

%{
/*!
 * \class kc::impl_tSwitch
 * \brief BPEL's <switch>
*/
%}

tSwitch:
  Switch(standardElements tCase_list tOtherwise_list)
  {
    casestring name;
    casestring joinCondition;
    casestring suppressJoinFailure = mkcasestring("no");

    integer id;
  }
;

tCase_list:
  list tCase
;

%{
/*!
 * \class kc::impl_tCase
 * \brief BPEL's <case>
*/
%}

tCase:
  Case(activity)
  {
    casestring condition;
  }
;

tOtherwise_list:
  list tOtherwise
;

%{
/*!
 * \class kc::impl_tOtherwise
 * \brief BPEL's <otherwise>
*/
%}

tOtherwise:
  Otherwise(activity)
;


/******************************************************************************
  WHILE
******************************************************************************/

%{
/*!
 * \class kc::impl_tWhile
 * \brief BPEL's <while>
*/
%}

tWhile:
  While(standardElements activity)
  {
    casestring condition;

    casestring name;
    casestring joinCondition;
    casestring suppressJoinFailure = mkcasestring("no");
    
    integer id;
  }
;


/******************************************************************************
  SEQUENCE
******************************************************************************/

%{
/*!
 * \class kc::impl_tSequence
 * \brief BPEL's <sequence>
*/
%}

tSequence:
  Sequence(standardElements activity_list)
  {
    casestring name;
    casestring joinCondition;
    casestring suppressJoinFailure = mkcasestring("no");

    integer id;
  }
;


/******************************************************************************
  PICK
******************************************************************************/

%{
/*!
 * \class kc::impl_tPick
 * \brief BPEL's <pick>
*/
%}

tPick:
  Pick(standardElements tOnMessage_list tOnAlarm_list)
  {
    casestring name;
    casestring joinCondition;
    casestring suppressJoinFailure = mkcasestring("no");
    
    casestring createInstance = mkcasestring("no");

    integer id;
  }
;


/******************************************************************************
  SCOPE
******************************************************************************/

%{
/*!
 * \class kc::impl_tScope
 * \brief BPEL's <scope>
*/
%}

tScope:
  Scope(standardElements tVariable_list tFaultHandlers_opt tCompensationHandler_opt tEventHandlers_opt activity)
  {
    casestring name;
    casestring joinCondition;
    casestring suppressJoinFailure = mkcasestring("no");

    casestring variableAccessSerializable = mkcasestring("no");

    integer id;
  }
;



/******************************************************************************
  STANDARD ELEMENTS
******************************************************************************/

%{
/*!
 * \class kc::impl_standardElements
 * \brief BPEL's standard elements
*/
%}

standardElements:
  StandardElements(tTarget_list tSource_list)
;

tTarget_list:
  list tTarget
;

%{
/*!
 * \class kc::impl_tTarget
 * \brief BPEL's <target>
*/
%}

tTarget:
  Target()
  {
    casestring linkName;
  }
;

tSource_list:
  list tSource
;

%{
/*!
 * \class kc::impl_tSource
 * \brief BPEL's <source>
*/
%}

tSource:
  Source()
  {
    casestring linkName;
    casestring transitionCondition = mkcasestring("true");
  }
;
