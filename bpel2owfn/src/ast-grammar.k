tProcess:
  Process(tPartnerLink_list tPartner_list tVariable_list tCorrelationSet_list tFaultHandlers_opt tCompensationHandler_opt tEventHandlers_opt activity)
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
;


/******************************************************************************
  PARTNERS
******************************************************************************/

tPartner_list:
  list tPartner
;

tPartner:
  Partner(tPartnerLink_list)
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
;

tOnAlarm:
  OnAlarm(activity)
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
;


/******************************************************************************
  CORRELATION SETS
******************************************************************************/

tCorrelationSet_list:
  list tCorrelationSet
;

tCorrelationSet:
  CorrelationSet()
;


/******************************************************************************
  CORRELATIONS
******************************************************************************/

tCorrelation_list:
  list tCorrelation
;

tCorrelation:
  Correlation()
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
;


/******************************************************************************
  RECEIVE
******************************************************************************/

tReceive:
  Receive(standardElements tCorrelation_list)
;


/******************************************************************************
  REPLY
******************************************************************************/

tReply:
  Reply(standardElements tCorrelation_list)
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
;

tTo:
  To()
;


/******************************************************************************
  WAIT
******************************************************************************/

tWait:
  Wait(standardElements)
;


/******************************************************************************
  THROW
******************************************************************************/

tThrow:
  Throw(standardElements)
;


/******************************************************************************
  COMPENSATE
******************************************************************************/

tCompensate:
  Compensate(standardElements)
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
;


/******************************************************************************
  SCOPE
******************************************************************************/

tScope:
  Scope(standardElements tVariable_list tFaultHandlers_opt tCompensationHandler_opt tEventHandlers_opt activity)
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
;

tSource_list:
  list tSource
;

tSource:
  Source()
;
