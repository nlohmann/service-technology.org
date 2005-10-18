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
  {
    casestring name;
    casestring joinCondition;
    casestring suppressJoinFailure;
    
    integer id;
  }
;


/******************************************************************************
  INVOKE
******************************************************************************/

tInvoke:
  Invoke(standardElements tCorrelation_list tCatch_list tCatchAll_list tCompensationHandler_opt)
| Invoke2(standardElements tCorrelation_list)
  {
    casestring name;
    casestring joinCondition;
    casestring suppressJoinFailure;	
	  
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
    casestring createInstance;

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
    casestring name;
    casestring joinCondition;
    casestring suppressJoinFailure;	
	  
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
    casestring suppressJoinFailure;	

    casestring faultName;
    casestring faultVariable;

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
  }
;


/******************************************************************************
  SWITCH
******************************************************************************/

tSwitch:
  Switch(standardElements tCase_list tOtherwise_list)
  {
    casestring name;
    casestring joinCondition;
    casestring suppressJoinFailure;

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

    casestring name;
    casestring joinCondition;
    casestring suppressJoinFailure;
    
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
    casestring suppressJoinFailure;
    
    casestring createInstance;

    integer id;
  }
;


/******************************************************************************
  SCOPE
******************************************************************************/

tScope:
  Scope(standardElements tVariable_list tFaultHandlers_opt tCompensationHandler_opt tEventHandlers_opt activity)
  {
    casestring name;
    casestring joinCondition;
    casestring suppressJoinFailure;	

    casestring variableAccessSerializable;

    integer id;
  }
;



/******************************************************************************
  STANDARD ELEMENTS
******************************************************************************/

standardElements:
  StandardElements(tTarget_list tSource_list)
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
