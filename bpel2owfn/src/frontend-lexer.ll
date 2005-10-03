%option noyywrap
%option yylineno
%option nodefault
%option debug

%{

// from kimwitu++
#include "bpel-kc-k.h"
#include "bpel-kc-yystype.h"


#include "bpel-syntax.h"
#define ATTRIBUTE 1
#define COMMENT 2

extern int yyerror(const char *msg);

/// Store the current start condition of the lexer to return safely after
/// comments.
int currentView;

%}


namestart		[A-Za-z\200-\377_]
namechar		[A-Za-z\200-\377_0-9.-:]
name			{namestart}{namechar}*
esc			"&#"[0-9]+";"|"&#x"[0-9a-fA-F]+";"
string			\"([^"]|{esc})*\"
comment			([^-]|"-"[^-])*


 /* start conditions of the lexer */
%s ATTRIBUTE
%s COMMENT


%%


 /* comments */
"!--"				{ currentView = YY_START; BEGIN(COMMENT); }
<COMMENT>{comment}		{ /* skip */ }
<COMMENT>"-->"[ \t\r\n]*"<"	{ BEGIN(currentView); }


 /* attributes */
<ATTRIBUTE>{name}	{ return X_NAME; }
<ATTRIBUTE>{string}	{ return X_STRING; }
<ATTRIBUTE>"="		{ return X_EQUALS; }


 /* XML-elements */
"<"			{ return X_OPEN; }
"/"			{ return X_SLASH; }
">"			{ BEGIN(INITIAL); return X_CLOSE; }
">"[ \t\r\n]*"<"	{ BEGIN(INITIAL); return X_NEXT; }


 /* names of BPEL-elements */
"assign"		{ BEGIN(ATTRIBUTE); return K_ASSIGN; }
"case"			{ BEGIN(ATTRIBUTE); return K_CASE; }
"catch"			{ BEGIN(ATTRIBUTE); return K_CATCH; }
"catchAll"		{ BEGIN(ATTRIBUTE); return K_CATCHALL; }
"compensate"		{ BEGIN(ATTRIBUTE); return K_COMPENSATE; }
"compensationHandler"	{ BEGIN(ATTRIBUTE); return K_COMPENSATIONHANDLER; }
"copy"			{ BEGIN(ATTRIBUTE); return K_COPY; }
"correlation"		{ BEGIN(ATTRIBUTE); return K_CORRELATION; }
"correlations"		{ BEGIN(ATTRIBUTE); return K_CORRELATIONS; }
"correlationSet"	{ BEGIN(ATTRIBUTE); return K_CORRELATIONSET; }
"correlationSets"	{ BEGIN(ATTRIBUTE); return K_CORRELATIONSETS; }
"empty"			{ BEGIN(ATTRIBUTE); return K_EMPTY; }
"eventHandlers"		{ BEGIN(ATTRIBUTE); return K_EVENTHANDLERS; }
"faultHandlers"		{ BEGIN(ATTRIBUTE); return K_FAULTHANDLERS; }
"flow"			{ BEGIN(ATTRIBUTE); return K_FLOW; }
"from"			{ BEGIN(ATTRIBUTE); return K_FROM; }
"invoke"		{ BEGIN(ATTRIBUTE); return K_INVOKE; }
"link"			{ BEGIN(ATTRIBUTE); return K_LINK; }
"links"			{ BEGIN(ATTRIBUTE); return K_LINKS; }
"onAlarm"		{ BEGIN(ATTRIBUTE); return K_ONALARM; }
"onMessage"		{ BEGIN(ATTRIBUTE); return K_ONMESSAGE; }
"otherwise"		{ BEGIN(ATTRIBUTE); return K_OTHERWISE; }
"partner"		{ BEGIN(ATTRIBUTE); return K_PARTNER; }
"partnerLink"		{ BEGIN(ATTRIBUTE); return K_PARTNERLINK; }
"partnerLinks"		{ BEGIN(ATTRIBUTE); return K_PARTNERLINKS; }
"partners"		{ BEGIN(ATTRIBUTE); return K_PARTNERS; }
"pick"			{ BEGIN(ATTRIBUTE); return K_PICK; }
"process"		{ BEGIN(ATTRIBUTE); return K_PROCESS; }
"receive"		{ BEGIN(ATTRIBUTE); return K_RECEIVE; }
"reply"			{ BEGIN(ATTRIBUTE); return K_REPLY; }
"scope"			{ BEGIN(ATTRIBUTE); return K_SCOPE; }
"sequence"		{ BEGIN(ATTRIBUTE); return K_SEQUENCE; }
"source"		{ BEGIN(ATTRIBUTE); return K_SOURCE; }
"switch"		{ BEGIN(ATTRIBUTE); return K_SWITCH; }
"target"		{ BEGIN(ATTRIBUTE); return K_TARGET; }
"terminate"		{ BEGIN(ATTRIBUTE); return K_TERMINATE; }
"throw"			{ BEGIN(ATTRIBUTE); return K_THROW; }
"to"			{ BEGIN(ATTRIBUTE); return K_TO; }
"variable"		{ BEGIN(ATTRIBUTE); return K_VARIABLE; }
"variables"		{ BEGIN(ATTRIBUTE); return K_VARIABLES; }
"wait"			{ BEGIN(ATTRIBUTE); return K_WAIT; }
"while"			{ BEGIN(ATTRIBUTE); return K_WHILE; }


{name}			{ BEGIN(ATTRIBUTE); return X_ELEMENTNAME; }

 /* white space */
[ \t\r\n]*		{ /* skip */ }


 /* end of input file */
<<EOF>>			{ return EOF; }


 /* anything else */
.			{ yyerror("parse error"); }
