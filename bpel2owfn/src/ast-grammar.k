tProcess:
  Process(tPartnerLink_list tPartner_list tVariable_list tCorrelationSet_list tFaultHandlers_opt tCompensationHandler_opt tEventHandlers_opt activity)
  {
    casestring name;
    casestring targetNamespace;
    casestring queryLanguage;
    casestring expressionLanguage;
    casestring suppressJoinFailure;
    casestring enableInstanceCompensation;
    casestring abstractProcess; 
  }
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

tFaultHandlers_opt:
  list tFaultHandlers
;
  
tFaultHandlers:
  FaultHandlers(tCatch_list tCatchAll_list)
;

tCatch_list:
  list tCatch
;

tCatchAll_list:
  list tCatchAll
;

tCatch:
  Catch(activity)
  {
    casestring faultName;
    casestring faultVariable;
  }
;

tCatchAll:
  CatchAll(activity)
;


/******************************************************************************
  EVENT HANDLERS
******************************************************************************/

tEventHandlers_opt:
  list tEventHandlers
;
  
tEventHandlers:
  EventHandlers(tOnMessage_list tOnAlarm_list)
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
  }
;

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
  
tCompensationHandler:
  CompensationHandler(activity)
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
;

tCorrelation:
  Correlation()
  {
    casestring set;
    casestring initiate;
    casestring pattern; // originally only in "tCorrelationWithPattern"
  }
;


/******************************************************************************
  EMPTY
******************************************************************************/

tEmpty:
  Empty(standardElements)
;


/******************************************************************************
  INVOKE
******************************************************************************/

tInvoke:
  Invoke(standardElements tCorrelation_list tCatch_list tCatchAll_list tCompensationHandler_opt)
  {
    casestring partnerLink;
    casestring portType;
    casestring operation;
    casestring inputVariable;
    casestring outputVariable;
  }
;


/******************************************************************************
  RECEIVE
******************************************************************************/

tReceive:
  Receive(standardElements tCorrelation_list)
  {
    casestring partnerLink;
    casestring portType;
    casestring operation;
    casestring variable;
    casestring createInstance;
  }
;


/******************************************************************************
  REPLY
******************************************************************************/

tReply:
  Reply(standardElements tCorrelation_list)
  {
    casestring partnerLink;
    casestring portType;
    casestring operation;
    casestring variable;
    casestring faultName;
  }
;


/******************************************************************************
  ASSIGN
******************************************************************************/

tAssign:
  Assign(standardElements tCopy_list)
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
    casestring query;
    casestring property;
    casestring partnerLink;
    casestring endpointReference;
    casestring expression;
    casestring opaque;
  }
;

tTo:
  To()
  {
    casestring expression;
    casestring opaque;
    casestring endpointReference;
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
  }
;


/******************************************************************************
  THROW
******************************************************************************/

tThrow:
  Throw(standardElements)
  {
    casestring faultName;
    casestring faultVariable;
  }
;


/******************************************************************************
  COMPENSATE
******************************************************************************/

tCompensate:
  Compensate(standardElements)
  {
    casestring scope;
  }
;


/******************************************************************************
  TERMINATE
******************************************************************************/

tTerminate:
  Terminate(standardElements)
;


/******************************************************************************
  FLOW
******************************************************************************/

tFlow:
  Flow(standardElements tLink_list activity_list)
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
  }
;


/******************************************************************************
  SWITCH
******************************************************************************/

tSwitch:
  Switch(standardElements tCase_list tOtherwise_list)
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

tOtherwise_list:
  list tOtherwise
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
  }
;


/******************************************************************************
  SEQUENCE
******************************************************************************/

tSequence:
  Sequence(standardElements activity_list)
;


/******************************************************************************
  PICK
******************************************************************************/

tPick:
  Pick(standardElements tOnMessage_list tOnAlarm_list)
  {
    casestring createInstance;
  }
;


/******************************************************************************
  SCOPE
******************************************************************************/

tScope:
  Scope(standardElements tVariable_list tFaultHandlers_opt tCompensationHandler_opt tEventHandlers_opt activity)
  {
    casestring variableAccessSerializable;
  }
;



/******************************************************************************
  STANDARD ELEMENTS
******************************************************************************/

standardElements:
  StandardElements(tTarget_list tSource_list)
  {
    casestring name;
    casestring joinCondition;
    casestring suppressJoinFailure;
  }
;

tTarget_list:
  list tTarget
;

tTarget:
  Target()
  {
    casestring linkName;
  }
;

tSource_list:
  list tSource
;

tSource:
  Source()
  {
    casestring linkName;
    casestring transitionCondition;
  }
;
