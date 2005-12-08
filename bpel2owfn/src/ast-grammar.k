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
 *          - last changed: \$Date: 2005/12/08 10:49:37 $
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
 * \version \$Revision: 1.33 $
 *          - 2005-11-10 (nlohmann) Added doxygen comments.
 *          - 2005-11-17 (nlohmann) Added default attributes.
 *          - 2005-12-04 (nlohmann) Changed representation of handlers.
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
    casestring uniqueID;
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
| EventHandler(tOnMessage_list tOnAlarm_list)
  {
    integer parentScopeId;
  }
;

tOnMessage_list:
  list tOnMessage
;

tOnAlarm_list:
  list tOnAlarm
;

tOnMessage:
  OnMessage(activity)
  {
    casestring partnerLink;
    casestring portType;
    casestring operation;
    casestring variable;

    casestring uniqueID;
  }
;

tOnAlarm:
  OnAlarm(activity)
  {
    casestring For; // "for" is a keyword...
    casestring until;
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
    casestring uniqueID;
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
    casestring pattern; // originally only in "tCorrelationWithPattern"
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
    casestring suppressJoinFailure = mkcasestring("no");
    
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
    casestring suppressJoinFailure = mkcasestring("no");
	  
    casestring partnerLink;
    casestring portType;
    casestring operation;
    casestring inputVariable;
    casestring outputVariable;
    
    casestring uniqueIDin;
    casestring uniqueIDout;

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
    casestring suppressJoinFailure = mkcasestring("no");

    casestring partnerLink;
    casestring portType;
    casestring operation;
    casestring variable;
    casestring createInstance = mkcasestring("no");

    casestring uniqueID;
    
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
    casestring suppressJoinFailure = mkcasestring("no");

    casestring partnerLink;
    casestring portType;
    casestring operation;
    casestring variable;
    casestring faultName;

    casestring uniqueID;

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
    casestring suppressJoinFailure = mkcasestring("no");

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
    casestring uniqueID;
  }
;

tTo:
  To()
  {
    casestring variable;
    casestring part;
    casestring partnerLink;
    casestring property;

    casestring uniqueID;
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
    casestring suppressJoinFailure = mkcasestring("no");
	  
    casestring For; // "for" is a keyword
    casestring until;

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
    casestring suppressJoinFailure = mkcasestring("no");

    casestring faultName;
    casestring faultVariable;

    casestring uniqueID;

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
    casestring suppressJoinFailure = mkcasestring("no");

    casestring scope;

    bool inFaultHandler = true;
    
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
    casestring suppressJoinFailure = mkcasestring("no");

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

tLink:
  Link()
  {
    casestring name;
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
    casestring suppressJoinFailure = mkcasestring("no");

    integer id;
  }
;

tCase_list:
  list tCase
;

tCase:
  Case(activity)
  {
    casestring condition;
  }
;

tOtherwise:
  Otherwise(activity)
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
    casestring suppressJoinFailure = mkcasestring("no");
    
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
    casestring suppressJoinFailure = mkcasestring("no");

    integer id;
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
    casestring suppressJoinFailure = mkcasestring("no");
    
    casestring createInstance = mkcasestring("no");

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
    casestring suppressJoinFailure = mkcasestring("no");

    casestring variableAccessSerializable = mkcasestring("no");

    integer id;
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
  }
;

tTarget_list:
  list tTarget
;

tTarget:
  Target()
  {
    casestring linkName;
    casestring linkID;
  }
;

tSource_list:
  list tSource
;

tSource:
  Source()
  {
    casestring linkName;
    casestring transitionCondition = mkcasestring("true");
    casestring linkID;
  }
;
