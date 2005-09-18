%token K_ASSIGN K_CASE K_CATCH K_CATCHALL K_COMPENSATE K_COMPENSATIONHANDLER
%token K_COPY K_CORRELATION K_CORRELATIONS K_CORRELATIONSET K_CORRELATIONSETS
%token K_EMPTY K_EVENTHANDLERS K_FAULTHANDLERS K_FLOW K_FROM K_INVOKE K_LINK
%token K_LINKS K_ONALARM K_ONMESSAGE K_OTHERWISE K_PARTNER K_PARTNERLINK
%token K_PARTNERLINKS K_PARTNERS K_PICK K_PROCESS K_RECEIVE K_REPLY K_SCOPE
%token K_SEQUENCE K_SOURCE K_SWITCH K_TARGET K_TERMINATE K_THROW K_TO
%token K_VARIABLE K_VARIABLES K_WAIT K_WHILE

%token X_OPEN X_SLASH X_CLOSE X_NEXT X_NAME X_STRING X_EQUALS

%start tProcess


%token_table


%{

#define YYDEBUG 1
#define YYERROR_VERBOSE 1  ///< for verbose error messages

extern int yyerror(const char *);
extern char* yytext;

extern int yylex();

%}


%%


tProcess:
  X_OPEN K_PROCESS attributes X_NEXT
  tPartnerLinks_opt
  tVariables_opt
  tCorrelationSets_opt
  tFaultHandlers_opt
  activity
  X_NEXT X_SLASH K_PROCESS X_CLOSE
;

activity:
  tEmpty
| tInvoke
| tReceive
| tReply
| tAssign
// tWait
// tThrow
// tTerminate
| tFlow
| tSwitch
| tWhile
| tSequence
// tPick
// tScope
;

tPartnerLinks_opt:
  /* empty */
| tPartnerLinks X_NEXT
;

tPartnerLinks:
  K_PARTNERLINKS attributes X_NEXT
  tPartnerLink_list //1-oo
  X_SLASH K_PARTNERLINKS
;

tPartnerLink_list:
  tPartnerLink X_NEXT
| tPartnerLink X_NEXT tPartnerLink_list
;

tPartnerLink:
  K_PARTNERLINK attributes X_NEXT X_SLASH K_PARTNERLINK
| K_PARTNERLINK attributes X_SLASH
;

// TODO partner

tFaultHandlers_opt:
  /* empty */
| tFaultHandlers X_NEXT
;

tFaultHandlers:
  K_FAULTHANDLERS X_NEXT
  tCatch_list // 0-oo
  tCatchAll_list // 0-oo
  X_SLASH K_FAULTHANDLERS
;

tCatch:
  K_CATCH attributes X_NEXT
  tActivityOrCompensateContainer
  X_SLASH K_CATCH
;

tCatch_list:
  /* empty */
| tCatch X_NEXT tCatch_list
;

tCatchAll_opt:
  /* empty */
| tCatchAll X_NEXT
;

tCatchAll:
  K_CATCHALL attributes X_NEXT
  tActivityOrCompensateContainer
  X_SLASH K_CATCHALL
;

tCatchAll_list:
  /* empty */
| tCatchAll X_NEXT tCatchAll_list
;


tActivityContainer:
  activity X_NEXT
;

tActivityOrCompensateContainer:
  activity X_NEXT
| tCompensate X_NEXT
;

// TODO tEventHandlers

// TODO tOnMessage

// TODO tOnAlarm


tCompensationHandler_opt:
  /* empty */
| tCompensationHandler X_NEXT
;

tCompensationHandler:
  K_COMPENSATIONHANDLER X_NEXT X_SLASH K_COMPENSATIONHANDLER
;

tVariables_opt:
  /* empty */
| tVariables X_NEXT
;

tVariables:
  K_VARIABLES X_NEXT
  tVariable_list // 1-oo
  X_SLASH K_VARIABLES
;

tVariable_list:
  tVariable X_NEXT
| tVariable X_NEXT tVariable_list
;

tVariable:
  K_VARIABLE attributes X_NEXT X_SLASH K_VARIABLE
| K_VARIABLE attributes X_SLASH
;

tCorrelationSets_opt:
  /* empty */
| tCorrelationSets X_NEXT
;

tCorrelationSets:
  K_CORRELATIONSETS attributes X_NEXT
  tCorrelationSet_list //1-oo
  X_SLASH K_CORRELATIONSETS
;

tCorrelationSet_list:
  tCorrelationSet X_NEXT
| tCorrelationSet X_NEXT tCorrelationSet_list
;

tCorrelationSet:
  K_CORRELATIONSET attributes X_NEXT
  X_SLASH K_CORRELATIONSET
| K_CORRELATIONSET attributes X_SLASH
;


tActivity:
  tTarget_list //0-oo
  tSource_list //0-oo
;

tTarget_list:
  /* empty */
| tTarget X_NEXT tTarget_list
;

tTarget:
  K_TARGET attributes X_NEXT X_SLASH K_TARGET
| K_TARGET attributes X_SLASH
;

tSource_list:
  /* empty */
| tSource X_NEXT tSource_list
;

tSource:
  K_SOURCE attributes X_NEXT X_SLASH K_SOURCE
| K_SOURCE attributes X_SLASH
;

tEmpty:
  K_EMPTY attributes X_NEXT
  tActivity
  X_SLASH K_EMPTY
| K_EMPTY attributes X_SLASH
;

tCorrelations_opt:
  /* empty */
| tCorrelations X_NEXT
;

tCorrelations:
  K_CORRELATIONS attributes X_NEXT
  tCorrelation_list //1-oo
  X_SLASH K_CORRELATIONS
;

tCorrelation_list:
  tCorrelation X_NEXT
| tCorrelation X_NEXT tCorrelation_list
;

tCorrelation:
  K_CORRELATION attributes X_NEXT X_SLASH K_CORRELATION
  X_SLASH K_CORRELATION
| K_CORRELATION attributes X_SLASH
;

tCorrelationsWithPattern_opt:
  /* empty */
| tCorrelationsWithPattern X_NEXT
;

tCorrelationsWithPattern:
  K_CORRELATIONS attributes X_NEXT
  tCorrelationWithPattern_list
  X_SLASH K_CORRELATIONS
;

tCorrelationWithPattern_list:
  tCorrelationWithPattern X_NEXT
| tCorrelationWithPattern X_NEXT tCorrelationWithPattern_list
;

tCorrelationWithPattern:
  K_CORRELATION attributes X_NEXT X_SLASH K_CORRELATION
  X_SLASH K_CORRELATION
| K_CORRELATION attributes X_SLASH
;

tInvoke:
  K_INVOKE attributes X_NEXT
  tActivity
  tCorrelationsWithPattern_opt
  tCatch_list //0-oo
  tCatchAll_opt
  tCompensationHandler_opt
  X_SLASH K_INVOKE
| K_INVOKE attributes X_SLASH
;

tReceive:
  K_RECEIVE attributes X_NEXT
  tActivity
  tCorrelations_opt
  X_SLASH K_RECEIVE
| K_RECEIVE attributes X_SLASH
;

tReply:
  K_REPLY attributes X_NEXT
  tActivity
  tCorrelations_opt
  X_SLASH K_REPLY
| K_REPLY attributes X_SLASH
;

tAssign:
  K_ASSIGN attributes X_NEXT
  tActivity
  tCopy_list //1-oo
  X_SLASH K_ASSIGN
;

tCopy_list:
  tCopy X_NEXT
| tCopy X_NEXT tCopy_list
;

tCopy:
  K_COPY attributes X_NEXT
  tActivity
  tFrom X_NEXT
  tTo X_NEXT
  X_SLASH K_COPY
; 

tFrom:
  K_FROM attributes X_NEXT X_SLASH K_FROM
| K_FROM attributes X_SLASH
;

tTo:
  K_TO attributes X_NEXT X_SLASH K_TO
| K_TO attributes X_SLASH
;

// TODO tWait

// TODO tThrow

tCompensate:
  K_COMPENSATE attributes X_NEXT X_SLASH K_COMPENSATE
  tActivity
| K_COMPENSATE attributes X_SLASH
;

// TODO tTerminate

tFlow:
  K_FLOW attributes X_NEXT
  tActivity
  tLinks_opt
  activity_list //1-oo
  X_SLASH K_FLOW
;

activity_list:
  activity X_NEXT
| activity X_NEXT activity_list
;

tLinks_opt:
  /* empty */
| tLinks X_NEXT
;

tLinks:
  K_LINKS X_NEXT
  tLink_list // 1-oo
  X_SLASH K_LINKS
;

tLink_list:
  tLink X_NEXT
| tLink X_NEXT tLink_list
;

tLink:
  K_LINK attributes X_NEXT X_SLASH K_LINK
| K_LINK attributes X_SLASH
;

tSwitch:
  K_SWITCH attributes X_NEXT
  tActivity
  tCase_list //1-oo
  tOtherwise_opt
  X_SLASH K_SWITCH
;

tCase_list:
  tCase X_NEXT
| tCase X_NEXT tCase_list
;

tCase:
  K_CASE attributes X_NEXT
  tActivityContainer
  X_SLASH K_CASE
;

tOtherwise_opt:
  /* empty */
| tOtherwise X_NEXT
;

tOtherwise:
  K_OTHERWISE attributes X_NEXT
  tActivityContainer
  X_SLASH K_OTHERWISE
;

tWhile:
  K_WHILE attributes X_NEXT
  tActivity
  activity X_NEXT
  X_SLASH K_WHILE
;

tSequence:
  K_SEQUENCE attributes X_NEXT
  tActivity
  activity_list //1-oo
  X_SLASH K_SEQUENCE
;

// TODO tScope

attributes:
  /* empty */
| X_NAME X_EQUALS X_STRING attributes
;
