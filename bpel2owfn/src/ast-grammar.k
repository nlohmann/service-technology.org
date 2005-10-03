tProcess:
  Process(activity)
;

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

activity_list:
  list activity
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
  Invoke(standardElements)
;


/******************************************************************************
  RECEIVE
******************************************************************************/

tReceive:
  Receive(standardElements)
;


/******************************************************************************
  REPLY
******************************************************************************/

tReply:
  Reply(standardElements)
;


/******************************************************************************
  ASSIGN
******************************************************************************/

tAssign:
  Assign(standardElements)
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
  SCOPE
******************************************************************************/

tScope:
  Scope(standardElements activity)
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
