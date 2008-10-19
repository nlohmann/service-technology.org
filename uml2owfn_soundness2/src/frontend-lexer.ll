/*****************************************************************************\
  UML2oWFN -- Translating UML2 Activity Diagrams to Petri nets
  Copyright (C) 2007, 2008  Dirk Fahland <dirk.fahland@service-technolog.org>,
                            Martin Znamirowski <znamirow@informatik.hu-berlin.de>
  Copyright (C) 2006, 2007  Niels Lohmann,
                            Christian Gierds, and
                            Martin Znamirowski
  Copyright (C) 2005        Niels Lohmann and
                            Christian Gierds

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

%{
/*!
 * \file frontend-lexer.cc
 *
 * \brief BPEL lexer
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          Christian Gierds <gierds@informatik.hu-berlin.de>,
 *          last changes of \$Author: nielslohmann $
 *
 * \since   2005-11-10
 *
 * \date    \$Date: 2007/06/28 07:38:16 $
 * 
 * \note    This file was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Flex reading file frontend-lexer.ll.
 *          See http://www.gnu.org/software/flex for details.
 *
 * \version \$Revision: 1.67 $
 *
 * \todo    
 *          - Add rules to ignored everything non-BPEL.
 *          - Add a more elegant way to handle XSD-namespaces
 *
 * \ingroup frontend
 */

/*!
 * \fn frontend_lex
 * \brief lex the input file
 * \return tokens from type #yytokentype or calls #frontend_error
 * \ingroup frontend
 */

/*!
 * \var frontend_lineno
 * \brief current line number of the input file
 * \ingroup frontend
 */

/*!
 * \var frontend_text
 * \brief the actual string of the current token
 * \ingroup frontend
 */

/*!
 * \var frontend_in
 * \brief pointer to the input file
 * \ingroup frontend
 */

/*!
 * \var frontend__flex_debug
 * \brief if set to 1 the lexer trace is printed
 * \ingroup debug
 */
%}





 /*
   Flex options:
    - noyywrap: only one input file
    - nodefault: generate no default rule (?)
    - yylineno: manage the current line of the input file
    - debug: allow debug messages -- use command-line option "d flex"
    - always-interactive: to avoid call of fileno() to be ANSI C compliant
    - outfile: filename is set to "ley.yy.c" and is further managed by Automake
    - prefix: all functions and variables are prefixed with "frontend_"
 */
%option noyywrap
%option nodefault
%option yylineno
%option debug
%option always-interactive
%option outfile="lex.yy.c"
%option prefix="frontend_"


%{

#include <cstring>
#include <string>

#include "yystype.h"
#include "frontend-parser.h" 	// list of all tokens used
#include "debug.h"


/// current start condition of the lexer
unsigned int currentView;

bool parseJoinCondition = false;
bool parseUnicode = false;

%}


 /* some macros */
whitespace		[ \t\r\n]*
namestart		[A-Za-z\200-\377_]
namechar		[A-Za-z\200-\377_0-9.\-:]
ns			{namestart}({namechar}|[/:])*":"
number			([0-9]+|[0-9]*"."[0-9]+)
name			{namestart}{namechar}*
variablename		{namestart}({namechar}|[/:]|{whitespace})*
esc			"&#"[0-9]+";"|"&#x"[0-9a-fA-F]+";"
quote			\"
string			{quote}([^"]|{esc})*{quote}
comment			([^-]|"-"[^-])*
xmlheader		([^?]|"-"[^?])*
docu_end		"</documentation>"[ \t\r\n]*"<"
desc_end		"</"{ns}?"description>"[ \t\r\n]*"<"
anno_end		"</"{ns}?"annotationText>"[ \t\r\n]*"<"
opdatacrap		([^o>]|"o"[^p]|"op"[^e]|"ope"[^r]|"oper"[^a]|"opera"[^t]|"operat"[^i]|"operati"[^o]|"operatio"[^n]|"operation"[^a]|"operationa"[^l]|"operational"[^D]|"operationalD"[^a]|"operationalDa"[^t]|"operationalDat"[^a]|"operationalData"[^>])*
extendedAttributescrap		([^e>]|"e"[^x]|"ex"[^t]|"ext"[^e]|"exte"[^n]|"exten"[^d]|"extend"[^e]|"extende"[^d]|"extended"[^A]|"extendedA"[^t]|"extendedAt"[^t]|"extendedAtt"[^r]|"extendedAttr"[^i]|"extendedAttri"[^b]|"extendedAttrib"[^u]|"extendedAttribu"[^t]|"extendedAttribut"[^e]|"extendedAttribute"[^s]|"extendedAttributes"[^>])*
organizationscrap		([^o>]|"o"[^r]|"or"[^g]|"org"[^a]|"orga"[^n]|"organ"[^i]|"organi"[^z]|"organiz"[^a]|"organiza"[^t]|"organizat"[^i]|"organizati"[^o]|"organizatio"[^n]|"organization"[^s]|"organizations"[^>])*
timerSettingcrap		([^t>]|"t"[^i]|"ti"[^m]|"tim"[^e]|"time"[^r]|"timer"[^S]|"timerS"[^e]|"timerSe"[^t]|"timerSet"[^t]|"timerSett"[^i]|"timerSetti"[^n]|"timerSettin"[^g]|"timerSetting"[^>])*
repositorycrap		([^r>]|"r"[^e]|"re"[^p]|"rep"[^o]|"repo"[^s]|"repos"[^i]|"reposi"[^t]|"reposit"[^o]|"reposito"[^r]|"repositor"[^y]|"repository"[^>])*
processModelcrap ([^p>]|"p"[^r]|"pr"[^o]|"pro"[^c]|"proc"[^e]|"proce"[^s]|"proces"[^s]|"process"[^M]|"processM"[^o]|"processMo"[^d]|"processMod"[^e]|"processMode"[^l])*
UB     			[\200-\277]



 /* start conditions of the lexer */
%s ATTRIBUTE
%s COMMENT
%s XMLHEADER
%s DOCUMENTATION
%x DESCRIPTION
%x ANNOTATION
%x OPERATIONALDATA
%x OPERATIONALDATACRITICAL
%x OPERATIONALDATAFINISHED
%x TIMERSETTING
%x TIMERSETTINGCRITICAL
%x TIMERSETTINGFINISHED
%x EXTENDEDATTRIBUTES
%x EXTENDEDATTRIBUTESCRITICAL
%x EXTENDEDATTRIBUTESFINISHED
%x ORGANIZATIONS
%x ORGANIZATIONSCRITICAL
%x ORGANIZATIONSFINISHED
%x RESOURCES
%x RESOURCESCRITICAL
%x RESOURCESFINISHED
%x REPOSITORY
%x REPOSITORYCRITICAL
%x REPOSITORYFINISHED
%x WAITFORPROCESSMODEL
%x WAITFORPROCESSMODELCRITICAL
%x EATING
%x EATEN
%s JOINCONDITION


%%

 /* code to skip UTF-8 characters */
[\300-\337]{UB}             { if (!parseUnicode) { parseUnicode = true;} }
[\340-\357]{UB}{2}          { if (!parseUnicode) { parseUnicode = true;} }
[\360-\367]{UB}{3}          { if (!parseUnicode) { parseUnicode = true;} }
[\370-\373]{UB}{4}          { if (!parseUnicode) { parseUnicode = true;} }
[\374-\375]{UB}{5}          { if (!parseUnicode) { parseUnicode = true; } }


 /* <documentation tags> */
<DOCUMENTATION>{docu_end}	{ /* skip */ BEGIN(currentView); }
<DOCUMENTATION>[^<]		{ /* skip */ }
<INITIAL>"documentation"	{ /* skip */ currentView = YY_START; BEGIN(DOCUMENTATION); }

 /* <description tags> */
<DESCRIPTION>{desc_end}	{ /* skip */ BEGIN(currentView); }
<DESCRIPTION>[^<]		{ /* skip */ }
<INITIAL>{ns}?"description"	{ /* skip */ currentView = YY_START; BEGIN(DESCRIPTION); }

 /* <annotation tags> */
<ANNOTATION>{anno_end}	{ /* skip */ BEGIN(currentView); }
<ANNOTATION>[^<]		{ /* skip */ }
<INITIAL>{ns}?"annotationText"	{ /* skip */ currentView = YY_START; BEGIN(ANNOTATION); }

 /* <Operational Data tags> <timer Setting Tags> */ 

<WAITFORPROCESSMODEL>([^<])  { /* skip */ }
<WAITFORPROCESSMODEL>"<"		{ BEGIN(WAITFORPROCESSMODELCRITICAL); }   
<WAITFORPROCESSMODELCRITICAL>{processModelcrap}	{  } 
<WAITFORPROCESSMODELCRITICAL>">"		{ BEGIN(WAITFORPROCESSMODEL); }
<WAITFORPROCESSMODELCRITICAL>"processModel" 	{ BEGIN(ATTRIBUTE); return (K_PROCESSMODEL);}

<OPERATIONALDATA>([^<]|"<"[^/])  { /* skip */ }
<OPERATIONALDATA>"</"		{ BEGIN(OPERATIONALDATACRITICAL); }   
<OPERATIONALDATACRITICAL>{opdatacrap}	{  } 
<OPERATIONALDATACRITICAL>">"		{ BEGIN(OPERATIONALDATA); }
<OPERATIONALDATACRITICAL>"operationalData>" 	{ BEGIN(OPERATIONALDATAFINISHED);}
<OPERATIONALDATAFINISHED>[^<] {}
<OPERATIONALDATAFINISHED>"<" { BEGIN(INITIAL); }
<INITIAL>{ns}?"operationalData"	{ BEGIN(OPERATIONALDATA); }

<EXTENDEDATTRIBUTES>([^<]|"<"[^/])  { /* skip */ }
<EXTENDEDATTRIBUTES>"</"		{ BEGIN(EXTENDEDATTRIBUTESCRITICAL); }   
<EXTENDEDATTRIBUTESCRITICAL>{extendedAttributescrap}	{  } 
<EXTENDEDATTRIBUTESCRITICAL>">"		{ BEGIN(EXTENDEDATTRIBUTES); }
<EXTENDEDATTRIBUTESCRITICAL>"extendedAttributes>" 	{ BEGIN(EXTENDEDATTRIBUTESFINISHED);}
<EXTENDEDATTRIBUTESFINISHED>[^<] {}
<EXTENDEDATTRIBUTESFINISHED>"<" { BEGIN(INITIAL); }
<INITIAL>{ns}?"extendedAttributes"	{ BEGIN(EXTENDEDATTRIBUTES); }

<ORGANIZATIONS>([^<]|"<"[^/])  { /* skip */ }
<ORGANIZATIONS>"</"		{ BEGIN(ORGANIZATIONSCRITICAL); }   
<ORGANIZATIONSCRITICAL>{organizationscrap}	{  } 
<ORGANIZATIONSCRITICAL>">"		{ BEGIN(ORGANIZATIONS); }
<ORGANIZATIONSCRITICAL>"organizations>" 	{ BEGIN(ORGANIZATIONSFINISHED);}
<ORGANIZATIONSFINISHED>[^<] {}
<ORGANIZATIONSFINISHED>"<" { BEGIN(INITIAL); }
<INITIAL>{ns}?"organizations"	{ BEGIN(ORGANIZATIONS); }

<REPOSITORY>([^<]|"<"[^/])  { /* skip */ }
<REPOSITORY>"</"		{ BEGIN(REPOSITORYCRITICAL); }   
<REPOSITORYCRITICAL>{repositorycrap}	{  } 
<REPOSITORYCRITICAL>">"		{ BEGIN(REPOSITORY); }
<REPOSITORYCRITICAL>"repository>" 	{ BEGIN(REPOSITORYFINISHED);}
<REPOSITORYFINISHED>[^<] {}
<REPOSITORYFINISHED>"<" { BEGIN(INITIAL); }
<INITIAL>{ns}?"repository"	{ BEGIN(REPOSITORY); }

<TIMERSETTING>([^<]|"<"[^/])  { /* skip */ }
<TIMERSETTING>"</"		{ BEGIN(TIMERSETTINGCRITICAL); }   
<TIMERSETTINGCRITICAL>{timerSettingcrap}	{  } 
<TIMERSETTINGCRITICAL>">"		{ BEGIN(TIMERSETTING); }
<TIMERSETTINGCRITICAL>"timerSetting>" 	{ BEGIN(TIMERSETTINGFINISHED);}
<TIMERSETTINGFINISHED>[^<] {}
<TIMERSETTINGFINISHED>"<" { BEGIN(INITIAL); }
<INITIAL>{ns}?"timerSetting"	{ BEGIN(TIMERSETTING); }
 

 /* comments */
"!--"				{ currentView = YY_START; BEGIN(COMMENT); }
<COMMENT>{comment}		{ /* skip */ }
<COMMENT>"-->"[ \t\r\n]*"<"	{ BEGIN(currentView); }

"?xml"				{ currentView = YY_START; BEGIN(XMLHEADER); }
<XMLHEADER>{xmlheader}		{ /* skip */ }
<XMLHEADER>"?>"[ \t\r\n]*"<"	{ BEGIN(currentView); }


 /* everything needed to evaluate join conditons (must be above the attributes section) */
<ATTRIBUTE>"joinCondition"		                { return K_JOINCONDITION; }
<ATTRIBUTE,JOINCONDITION>{ns}?"getLinkStatus"	{ return K_GETLINKSTATUS; }
<ATTRIBUTE,JOINCONDITION>"and"			        { return K_AND; }
<ATTRIBUTE,JOINCONDITION>"or"				{ return K_OR; }
<ATTRIBUTE,JOINCONDITION>"("				{ return LBRACKET; }
<ATTRIBUTE,JOINCONDITION>")"				{ return RBRACKET; }
<ATTRIBUTE,JOINCONDITION>"'"				{ return APOSTROPHE; }
<ATTRIBUTE,JOINCONDITION>{name}                         { frontend_lval.assign(frontend_text);
                                                          return X_NAME; }

 /* attributes */
<ATTRIBUTE>{string}	{ std::string stringwoquotes = std::string(frontend_text).substr(1, strlen(frontend_text)-2);
                          frontend_lval = stringwoquotes.c_str();
                          return X_STRING; }
<ATTRIBUTE>"="		{ return X_EQUALS; }


 /* join conditions */
<JOINCONDITION>"$"{name}		{ frontend_lval = frontend_text;
                                          return VARIABLENAME; }
<JOINCONDITION>"<"                      { BEGIN(INITIAL); return X_OPEN; }


 /* XML-elements */
"<"				{ return X_OPEN; }
"/"				{ return X_SLASH; }
<INITIAL,ATTRIBUTE>">"		{ if (!parseJoinCondition) { BEGIN(INITIAL); } else { parseJoinCondition = false; BEGIN(JOINCONDITION); }; return X_CLOSE; }
">"[ \t\r\n]*"<"		{ BEGIN(INITIAL); return X_NEXT; }


 /* names of BPEL-elements */
<INITIAL>{ns}?"assign"			{ BEGIN(ATTRIBUTE); return K_ASSIGN; }
<INITIAL>{ns}?"branches"		{ BEGIN(ATTRIBUTE); return K_BRANCHES; }
<INITIAL>{ns}?"case"			{ BEGIN(ATTRIBUTE); return K_CASE; }
<INITIAL>{ns}?"catch"			{ BEGIN(ATTRIBUTE); return K_CATCH; }
<INITIAL>{ns}?"catchAll"		{ BEGIN(ATTRIBUTE); return K_CATCHALL; }
<INITIAL>{ns}?"compensate"		{ BEGIN(ATTRIBUTE); return K_COMPENSATE; }
<INITIAL>{ns}?"compensateScope"		{ BEGIN(ATTRIBUTE); return K_COMPENSATESCOPE; }
<INITIAL>{ns}?"compensationHandler"	{ return K_COMPENSATIONHANDLER; }
<INITIAL>{ns}?"completionCondition"	{ return K_COMPLETIONCONDITION; }
<INITIAL>{ns}?"copy"			{ return K_COPY; }
<INITIAL>{ns}?"correlations"		{ return K_CORRELATIONS; }
<INITIAL>{ns}?"correlationSet"		{ BEGIN(ATTRIBUTE); return K_CORRELATIONSET; }
<INITIAL>{ns}?"correlationSets"		{ return K_CORRELATIONSETS; }
<INITIAL>{ns}?"else"			{ BEGIN(ATTRIBUTE); return K_ELSE; }
<INITIAL>{ns}?"elseif"			{ BEGIN(ATTRIBUTE); return K_ELSEIF; }
<INITIAL>{ns}?"empty"			{ BEGIN(ATTRIBUTE); return K_EMPTY; }
<INITIAL>{ns}?"eventHandlers"		{ return K_EVENTHANDLERS; }
<INITIAL>{ns}?"exit"			{ BEGIN(ATTRIBUTE); return K_EXIT; }
<INITIAL>{ns}?"extension"		{ BEGIN(ATTRIBUTE); return K_EXTENSION; }
<INITIAL>{ns}?"extensionActivity"	{ BEGIN(ATTRIBUTE); return K_EXTENSIONACTIVITY; }
<INITIAL>{ns}?"extensionAssignOperation"	{ BEGIN(ATTRIBUTE); return K_EXTENSIONASSIGNOPERATION; }
<INITIAL>{ns}?"extensions"		{ BEGIN(ATTRIBUTE); return K_EXTENSIONS; }
<INITIAL>{ns}?"faultHandlers"		{ return K_FAULTHANDLERS; }
<INITIAL>{ns}?"finalCounterValue"	{ BEGIN(ATTRIBUTE); return K_FINALCOUNTERVALUE; }
<INITIAL>{ns}?"flow"			{ BEGIN(ATTRIBUTE); return K_FLOW; }
<INITIAL>{ns}?"for"			{ BEGIN(ATTRIBUTE); return K_FOR; }
<INITIAL>{ns}?"forEach"			{ BEGIN(ATTRIBUTE); return K_FOREACH; }
<INITIAL>{ns}?"from"			{ BEGIN(ATTRIBUTE); return K_FROM; }
<INITIAL>{ns}?"fromPart"		{ BEGIN(ATTRIBUTE); return K_FROMPART; }
<INITIAL>{ns}?"fromParts"		{ BEGIN(ATTRIBUTE); return K_FROMPARTS; }
<INITIAL>{ns}?"if"			{ BEGIN(ATTRIBUTE); return K_IF; }
<INITIAL>{ns}?"import"			{ BEGIN(ATTRIBUTE); return K_IMPORT; }
<INITIAL>{ns}?"invoke"			{ BEGIN(ATTRIBUTE); return K_INVOKE; }
<INITIAL>{ns}?"joinCondition"		{ BEGIN(ATTRIBUTE); parseJoinCondition = true; return K_JOINCONDITION; }
<INITIAL>{ns}?"link"			{ BEGIN(ATTRIBUTE); return K_LINK; }
<INITIAL>{ns}?"links"			{ BEGIN(ATTRIBUTE); return K_LINKS; }
<INITIAL>{ns}?"literal"			{ BEGIN(ATTRIBUTE); return K_LITERAL; }
<INITIAL>{ns}?"messageExchange"		{ BEGIN(ATTRIBUTE); return K_MESSAGEEXCHANGE; }
<INITIAL>{ns}?"messageExchanges"	{ BEGIN(ATTRIBUTE); return K_MESSAGEEXCHANGES; }
<INITIAL>{ns}?"onAlarm"			{ BEGIN(ATTRIBUTE); return K_ONALARM; }
<INITIAL>{ns}?"onEvent"			{ BEGIN(ATTRIBUTE); return K_ONEVENT; }
<INITIAL>{ns}?"onMessage"		{ BEGIN(ATTRIBUTE); return K_ONMESSAGE; }
<INITIAL>{ns}?"opaqueActivity"		{ BEGIN(ATTRIBUTE); return K_OPAQUEACTIVITY; }
<INITIAL>{ns}?"opaqueFrom"		{ return K_OPAQUEFROM; }
<INITIAL>{ns}?"otherwise"		{ return K_OTHERWISE; }
<INITIAL>{ns}?"partner"			{ BEGIN(ATTRIBUTE); return K_PARTNER; }
<INITIAL>{ns}?"partnerLink"		{ BEGIN(ATTRIBUTE); return K_PARTNERLINK; }
<INITIAL>{ns}?"partnerLinks"		{ BEGIN(ATTRIBUTE); return K_PARTNERLINKS; }
<INITIAL>{ns}?"partners"		{ return K_PARTNERS; }
<INITIAL>{ns}?"pick"			{ BEGIN(ATTRIBUTE); return K_PICK; }
<INITIAL>{ns}?"query"			{ BEGIN(ATTRIBUTE); return K_QUERY; }
<INITIAL>{ns}?"receive"			{ BEGIN(ATTRIBUTE); return K_RECEIVE; }
<INITIAL>{ns}?"reply"			{ BEGIN(ATTRIBUTE); return K_REPLY; }
<INITIAL>{ns}?"repeatEvery"		{ BEGIN(ATTRIBUTE); return K_REPEATEVERY; }
<INITIAL>{ns}?"repeatUntil"		{ BEGIN(ATTRIBUTE); return K_REPEATUNTIL; }
<INITIAL>{ns}?"rethrow"			{ BEGIN(ATTRIBUTE); return K_RETHROW; }
<INITIAL>{ns}?"scope"			{ BEGIN(ATTRIBUTE); return K_SCOPE; }
<INITIAL>{ns}?"sequence"		{ BEGIN(ATTRIBUTE); return K_SEQUENCE; }
<INITIAL>{ns}?"sources"			{ BEGIN(ATTRIBUTE); return K_SOURCES; }
<INITIAL>{ns}?"startCounterValue"	{ BEGIN(ATTRIBUTE); return K_STARTCOUNTERVALUE; }
<INITIAL>{ns}?"switch"			{ BEGIN(ATTRIBUTE); return K_SWITCH; }
<INITIAL>{ns}?"targets"			{ BEGIN(ATTRIBUTE); return K_TARGETS; }
<INITIAL>{ns}?"terminate"		{ BEGIN(ATTRIBUTE); return K_TERMINATE; }
<INITIAL>{ns}?"terminationHandler"	{ BEGIN(ATTRIBUTE); return K_TERMINATIONHANDLER; }
<INITIAL>{ns}?"throw"			{ BEGIN(ATTRIBUTE); return K_THROW; }
<INITIAL>{ns}?"to"			{ BEGIN(ATTRIBUTE); return K_TO; }
<INITIAL>{ns}?"toPart"			{ BEGIN(ATTRIBUTE); return K_TOPART; }
<INITIAL>{ns}?"toParts"			{ BEGIN(ATTRIBUTE); return K_TOPARTS; }
<INITIAL>{ns}?"transitionCondition"	{ BEGIN(ATTRIBUTE); return K_TRANSITIONCONDITION; }
<INITIAL>{ns}?"until"			{ BEGIN(ATTRIBUTE); return K_UNTIL; }
<INITIAL>{ns}?"validate"		{ BEGIN(ATTRIBUTE); return K_VALIDATE; }
<INITIAL>{ns}?"variable"		{ BEGIN(ATTRIBUTE); return K_VARIABLE; }
<INITIAL>{ns}?"variables"		{ return K_VARIABLES; }
<INITIAL>{ns}?"wait"			{ BEGIN(ATTRIBUTE); return K_WAIT; }
<INITIAL>{ns}?"while"			{ BEGIN(ATTRIBUTE); return K_WHILE; }

 /* non-terminals for BPEL4Chor */
<INITIAL>{ns}?"topology"		{ BEGIN(ATTRIBUTE); return K_TOPOLOGY; }
<INITIAL>{ns}?"participantTypes"	{ BEGIN(ATTRIBUTE); return K_PARTICIPANTTYPES; }
<INITIAL>{ns}?"participantType"		{ BEGIN(ATTRIBUTE); return K_PARTICIPANTTYPE; }
<INITIAL>{ns}?"participants"		{ BEGIN(ATTRIBUTE); return K_PARTICIPANTS; }
<INITIAL>{ns}?"participant"		{ BEGIN(ATTRIBUTE); return K_PARTICIPANT; }
<INITIAL>{ns}?"participantSet"		{ BEGIN(ATTRIBUTE); return K_PARTICIPANTSET; }
<INITIAL>{ns}?"messageLinks"		{ BEGIN(ATTRIBUTE); return K_MESSAGELINKS; }
<INITIAL>{ns}?"messageLink"		{ BEGIN(ATTRIBUTE); return K_MESSAGELINK; }

 /* non-terminals for WSDL */
<INITIAL>{ns}?"types"			{ BEGIN(ATTRIBUTE); return K_TYPES; }
<INITIAL>{ns}?"portType"		{ BEGIN(ATTRIBUTE); return K_PORTTYPE; }
<INITIAL>{ns}?"fault"			{ BEGIN(ATTRIBUTE); return K_FAULT; }
<INITIAL>{ns}?"operation"		{ BEGIN(ATTRIBUTE); return K_OPERATION; }
<INITIAL>{ns}?"definitions"		{ BEGIN(ATTRIBUTE); return K_DEFINITIONS; }
<INITIAL>{ns}?"message"			{ BEGIN(ATTRIBUTE); return K_MESSAGE; }
<INITIAL>{ns}?"part"			{ BEGIN(ATTRIBUTE); return K_PART; }
<INITIAL>{ns}?"binding"			{ BEGIN(ATTRIBUTE); return K_BINDING; }
<INITIAL>{ns}?"service"			{ BEGIN(ATTRIBUTE); return K_SERVICE; }
<INITIAL>{ns}?"port"			{ BEGIN(ATTRIBUTE); return K_PORT; }

 /* non-terminals for IBM Tool */
<INITIAL>{ns}?"model"			{ BEGIN(WAITFORPROCESSMODEL);}
<INITIAL>{ns}?"processModel"			{ BEGIN(EATING); return K_PROCESSMODEL;}
<INITIAL>{ns}?"processes"		{ BEGIN(ATTRIBUTE); return K_PROCESSES; }
<INITIAL>{ns}?"services"			{ BEGIN(ATTRIBUTE); return K_SERVICES; }
<INITIAL>{ns}?"tasks"		{ BEGIN(ATTRIBUTE); return K_TASKS; }
<INITIAL>{ns}?"repositories"		{ BEGIN(ATTRIBUTE); return K_REPOSITORIES; }
<INITIAL>{ns}?"process"		{ BEGIN(ATTRIBUTE); return K_PROCESS; }
<INITIAL>{ns}?"service"			{ BEGIN(ATTRIBUTE); return K_SERVICE; }
<INITIAL>{ns}?"task"		{ BEGIN(ATTRIBUTE); return K_TASK; }
<INITIAL>{ns}?"humanTask"		{ BEGIN(ATTRIBUTE); return K_TASK; }
<INITIAL>{ns}?"repository"		{ BEGIN(ATTRIBUTE); return K_REPOSITORY; }
<INITIAL>{ns}?"inputs"			{ BEGIN(ATTRIBUTE); return K_INPUTS; }
<INITIAL>{ns}?"outputs"			{ BEGIN(ATTRIBUTE); return K_OUTPUTS; }
<INITIAL>{ns}?"input"			{ BEGIN(ATTRIBUTE); return K_INPUT; }
<INITIAL>{ns}?"output"			{ BEGIN(ATTRIBUTE); return K_OUTPUT; }
<INITIAL>{ns}?"inputCriterion"			{ BEGIN(ATTRIBUTE); return K_INPUTCRITERION; }
<INITIAL>{ns}?"outputCriterion"			{ BEGIN(ATTRIBUTE); return K_OUTPUTCRITERION; }
<INITIAL>{ns}?"precondition"			{ BEGIN(ATTRIBUTE); return K_PRECONDITION; }
<INITIAL>{ns}?"postcondition"			{ BEGIN(ATTRIBUTE); return K_POSTCONDITION; }
<INITIAL>{ns}?"flowContent"			{ BEGIN(ATTRIBUTE); return K_FLOWCONTENT; }
<INITIAL>{ns}?"startNode"			{ BEGIN(ATTRIBUTE); return K_STARTNODE; }
<INITIAL>{ns}?"entryPoint"			{ BEGIN(ATTRIBUTE); return K_ENTRYPOINT; }
<INITIAL>{ns}?"stopNode"			{ BEGIN(ATTRIBUTE); return K_STOPNODE; }
<INITIAL>{ns}?"endNode"			{ BEGIN(ATTRIBUTE); return K_ENDNODE; }
<INITIAL>{ns}?"decision"			{ BEGIN(ATTRIBUTE); return K_DECISION; }
<INITIAL>{ns}?"merge"			{ BEGIN(ATTRIBUTE); return K_MERGE; }
<INITIAL>{ns}?"fork"			{ BEGIN(ATTRIBUTE); return K_FORK; }
<INITIAL>{ns}?"join"			{ BEGIN(ATTRIBUTE); return K_JOIN; }
<INITIAL>{ns}?"notificationBroadcaster"			{ BEGIN(ATTRIBUTE); return K_NOTIFICATIONBROADCASTER; }
<INITIAL>{ns}?"notificationReceiver"			{ BEGIN(ATTRIBUTE); return K_NOTIFICATIONRECEIVER; }
<INITIAL>{ns}?"notificationFilter"			{ BEGIN(ATTRIBUTE); return K_NOTIFICATIONFILTER; }
<INITIAL>{ns}?"observer"			{ BEGIN(ATTRIBUTE); return K_OBSERVER; }
<INITIAL>{ns}?"timer"			{ BEGIN(ATTRIBUTE); return K_TIMER; }
<INITIAL>{ns}?"map"			{ BEGIN(ATTRIBUTE); return K_MAP; }
<INITIAL>{ns}?"loop"			{ BEGIN(ATTRIBUTE); return K_LOOP; }
<INITIAL>{ns}?"forLoop"			{ BEGIN(ATTRIBUTE); return K_FORLOOP; }
<INITIAL>{ns}?"callToProcess"			{ BEGIN(ATTRIBUTE); return K_CALLTOPROCESS; }
<INITIAL>{ns}?"callToService"			{ BEGIN(ATTRIBUTE); return K_CALLTOSERVICE; }
<INITIAL>{ns}?"callToTask"			{ BEGIN(ATTRIBUTE); return K_CALLTOTASK; }
<INITIAL>{ns}?"additionalInput"			{ BEGIN(ATTRIBUTE); return K_ADDITIONALINPUT; }
<INITIAL>{ns}?"additionalOutput"			{ BEGIN(ATTRIBUTE); return K_ADDITIONALOUTPUT; }
<INITIAL>{ns}?"inputRepositoryValue"			{ BEGIN(ATTRIBUTE); return K_INPUTREPOSITORYVALUE; }
<INITIAL>{ns}?"inputConstantValue"			{ BEGIN(ATTRIBUTE); return K_INPUTCONSTANTVALUE; }
<INITIAL>{ns}?"outputRepositoryValue"			{ BEGIN(ATTRIBUTE); return K_OUTPUTREPOSITORYVALUE; }
<INITIAL>{ns}?"localRepository"			{ BEGIN(ATTRIBUTE); return K_LOCALREPOSITORY; }
<INITIAL>{ns}?"globalRepository"			{ BEGIN(ATTRIBUTE); return K_GLOBALREPOSITORY; }
<INITIAL>{ns}?"literalValue"			{ BEGIN(ATTRIBUTE); return K_LITERALVALUE; }
<INITIAL>{ns}?"publicInstance"			{ BEGIN(ATTRIBUTE); return K_PUBLICINSTANCE; }
<INITIAL>{ns}?"connection"			{ BEGIN(ATTRIBUTE); return K_CONNECTION; }
<INITIAL>{ns}?"source"			{ BEGIN(ATTRIBUTE); return K_SOURCE; }
<INITIAL>{ns}?"target"			{ BEGIN(ATTRIBUTE); return K_TARGET; }
<INITIAL>{ns}?"annotation"			{ BEGIN(ATTRIBUTE); return K_ANNOTATION; }
<INITIAL>{ns}?"annotationText"			{ BEGIN(ATTRIBUTE); return K_ANNOTATIONTEXT; }
<INITIAL>{ns}?"annotatedNode"			{ BEGIN(ATTRIBUTE); return K_ANNOTATEDNODE; }
<INITIAL>{ns}?"loopCondition"			{ BEGIN(ATTRIBUTE); return K_LOOPCONDITION; }
<INITIAL>{ns}?"observationExpression"			{ BEGIN(ATTRIBUTE); return K_OBSERVATIONEXPRESSION; }
<INITIAL>{ns}?"condition"			{ BEGIN(ATTRIBUTE); return K_CONDITION; }
<INITIAL>{ns}?"inputBranch"			{ BEGIN(ATTRIBUTE); return K_INPUTBRANCH; }
<INITIAL>{ns}?"outputBranch"			{ BEGIN(ATTRIBUTE); return K_OUTPUTBRANCH; }
<INITIAL>{ns}?"classifiers"			{ BEGIN(ATTRIBUTE); return K_CLASSIFIER; }
<INITIAL>{ns}?"classifierValue"			{ BEGIN(ATTRIBUTE); return K_CLASSIFIERVALUE; }
<INITIAL>{ns}?"name"			{ BEGIN(ATTRIBUTE); return K_NAME; }
<INITIAL>{ns}?"value"			{ BEGIN(ATTRIBUTE); return K_VALUE; }
<INITIAL>{ns}?"constantValue"			{ BEGIN(ATTRIBUTE); return K_CONSTANTVALUE; }
<INITIAL>{ns}?"repositoryValue"			{ BEGIN(ATTRIBUTE); return K_REPOSITORYVALUE; }
<INITIAL>{ns}?"correlation"			{ BEGIN(ATTRIBUTE); return K_CORRELATION; }
<INITIAL>{ns}?"constraint"			{ BEGIN(ATTRIBUTE); return K_CONSTRAINT; }
<INITIAL>{ns}?"predicate"			{ BEGIN(ATTRIBUTE); return K_PREDICATE; }
<INITIAL>{ns}?"relatedInputCriteria"			{ BEGIN(ATTRIBUTE); return K_RELATEDINPUTCRITERIA; }
<INITIAL>{ns}?"metaInformation"			{ BEGIN(ATTRIBUTE); return K_METAINFORMATION; }
<INITIAL>{ns}?"resources"			{ BEGIN(ATTRIBUTE); return K_RESOURCES; }
<INITIAL>{ns}?"roleRequirement"					{ BEGIN(ATTRIBUTE); return K_RESOURCEREQUIREMENT; }
<INITIAL>{ns}?"resourceQuantity"				{ BEGIN(ATTRIBUTE); return K_RESOURCEQUANTITY; }
<INITIAL>{ns}?"bulkResourceRequirement"			{ BEGIN(ATTRIBUTE); return K_RESOURCEREQUIREMENT; }
<INITIAL>{ns}?"individualResourceRequirement" 	{ BEGIN(ATTRIBUTE); return K_RESOURCEREQUIREMENT; }


 /* non-terminals for WS-BPEL Partner Link Type */
<INITIAL>{ns}?"partnerLinkType"		{ BEGIN(ATTRIBUTE); return K_PARTNERLINKTYPE; }
<INITIAL>{ns}?"role"			{ BEGIN(ATTRIBUTE); return K_ROLE; }

 /* non-terminals for XML Schema */
<INITIAL>{ns}?"schema"			{ BEGIN(ATTRIBUTE); return K_SCHEMA; }

 /* non-terminals for Variable Properties */
<INITIAL>{ns}?"property"		{ BEGIN(ATTRIBUTE); return K_PROPERTY; }
<INITIAL>{ns}?"propertyAlias"		{ BEGIN(ATTRIBUTE); return K_PROPERTYALIAS; }



 /* white space */
{whitespace}			{ /* skip white space */ }


 /* some things for transition conditions */
"&ge;"		{ return GREATEROREQUAL; }
">="		{ return GREATEROREQUAL; }
"&gt;"		{ return GREATER; }
"&lt;"		{ return LESS; }
"&le;"		{ return LESSOREQUAL; }
"="		{ return EQUAL; }
"!="		{ return NOTEQUAL; }
"'"		{ return APOSTROPHE; }


 /* names and numbers */
<INITIAL>"$"{variablename}	{ frontend_lval = frontend_text;
                                  return VARIABLENAME; }

<INITIAL>{name}			{ frontend_lval = frontend_text;
                                  return X_NAME; }
{number}	{ frontend_lval = frontend_text;
                  return NUMBER; }

<EATING>[^>]
<EATING>">" {BEGIN(EATEN); return X_CLOSE;}
<EATEN>([^!]|"!"[^$]|"!$"[^&])*

 /* end of input file */
<<EOF>>				{ return EOF; }


 /* anything else */
.				{ if (debug_level >= TRACE_WARNINGS) frontend_error("lexical error"); }
