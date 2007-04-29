/*****************************************************************************\
 * Copyright 2007        Niels Lohmann, Christian Gierds                     *
 * Copyright 2005, 2006  Niels Lohmann                                       *
 *                                                                           *
 * This file is part of GNU BPEL2oWFN.                                       *
 *                                                                           *
 * GNU BPEL2oWFN is free software; you can redistribute it and/or modify it  *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * GNU BPEL2oWFN is distributed in the hope that it will be useful, but      *
 * WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General  *
 * Public License for more details.                                          *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with GNU BPEL2oWFN; see file COPYING. if not, write to the Free Software  *
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA. *
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
 * \date    \$Date: 2007/04/29 17:13:15 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Flex reading file frontend-lexer.ll.
 *          See http://www.gnu.org/software/flex for details.
 *
 * \version \$Revision: 1.62 $
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

#include "ast-config.h"		// all you need from Kimwitu++
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
number			[0-9]+
name			{namestart}{namechar}*
variablename		{namestart}({namechar}|[/:]|{whitespace})*
esc			"&#"[0-9]+";"|"&#x"[0-9a-fA-F]+";"
quote			\"
string			{quote}([^"]|{esc})*{quote}
comment			([^-]|"-"[^-])*
xmlheader		([^?]|"-"[^?])*
bpwsns			"bpws:"|"bpel:"
docu_end		"</documentation>"[ \t\r\n]*"<"
UB     			[\200-\277]



 /* start conditions of the lexer */
%s ATTRIBUTE
%s COMMENT
%s XMLHEADER
%s DOCUMENTATION
%s JOINCONDITION


%%

 /* code to skip UTF-8 characters */
[\300-\337]{UB}             { if (!parseUnicode) { parseUnicode = true; genericError(113, string(frontend_text), toString(frontend_lineno), ERRORLEVEL_SYNTAX); } }
[\340-\357]{UB}{2}          { if (!parseUnicode) { parseUnicode = true; genericError(113, string(frontend_text), toString(frontend_lineno), ERRORLEVEL_SYNTAX); } }
[\360-\367]{UB}{3}          { if (!parseUnicode) { parseUnicode = true; genericError(113, string(frontend_text), toString(frontend_lineno), ERRORLEVEL_SYNTAX); } }
[\370-\373]{UB}{4}          { if (!parseUnicode) { parseUnicode = true; genericError(113, string(frontend_text), toString(frontend_lineno), ERRORLEVEL_SYNTAX); } }
[\374-\375]{UB}{5}          { if (!parseUnicode) { parseUnicode = true; genericError(113, string(frontend_text), toString(frontend_lineno), ERRORLEVEL_SYNTAX); } }


 /* <documentation tags> */
<DOCUMENTATION>{docu_end}	{ /* skip */ BEGIN(currentView); }
<DOCUMENTATION>[^<]		{ /* skip */ }
<INITIAL>"documentation"	{ /* skip */ currentView = YY_START; BEGIN(DOCUMENTATION); }


 /* comments */
"!--"				{ currentView = YY_START; BEGIN(COMMENT); }
<COMMENT>{comment}		{ /* skip */ }
<COMMENT>"-->"[ \t\r\n]*"<"	{ BEGIN(currentView); }

"?xml"				{ currentView = YY_START; BEGIN(XMLHEADER); }
<XMLHEADER>{xmlheader}		{ /* skip */ }
<XMLHEADER>"?>"[ \t\r\n]*"<"	{ BEGIN(currentView); }


 /* everything needed to evaluate join conditons (must be above the attributes section) */
<ATTRIBUTE>"joinCondition"		                { return K_JOINCONDITION; }
<ATTRIBUTE,JOINCONDITION>{bpwsns}?"getLinkStatus"	{ return K_GETLINKSTATUS; }
<ATTRIBUTE,JOINCONDITION>"and"			        { return K_AND; }
<ATTRIBUTE,JOINCONDITION>"or"				{ return K_OR; }
<ATTRIBUTE,JOINCONDITION>"("				{ return LBRACKET; }
<ATTRIBUTE,JOINCONDITION>")"				{ return RBRACKET; }
<ATTRIBUTE,JOINCONDITION>"'"				{ return APOSTROPHE; }
<ATTRIBUTE,JOINCONDITION>{name}                         { frontend_lval.yt_casestring = kc::mkcasestring(frontend_text);
                                                          return X_NAME; }

 /* attributes */
<ATTRIBUTE>{string}	{ std::string stringwoquotes = std::string(frontend_text).substr(1, strlen(frontend_text)-2);
                          frontend_lval.yt_casestring = kc::mkcasestring(stringwoquotes.c_str());
                          return X_STRING; }
<ATTRIBUTE>"="		{ return X_EQUALS; }


 /* join conditions */
<JOINCONDITION>"$"{name}		{ frontend_lval.yt_casestring = kc::mkcasestring(frontend_text);
                                          return VARIABLENAME; }
<JOINCONDITION>"<"                      { BEGIN(INITIAL); return X_OPEN; }


 /* XML-elements */
"<"				{ return X_OPEN; }
"/"				{ return X_SLASH; }
<INITIAL,ATTRIBUTE>">"		{ if (!parseJoinCondition) { BEGIN(INITIAL); } else { parseJoinCondition = false; BEGIN(JOINCONDITION); }; return X_CLOSE; }
">"[ \t\r\n]*"<"		{ BEGIN(INITIAL); return X_NEXT; }


 /* names of BPEL-elements */
<INITIAL>{bpwsns}?"assign"		{ BEGIN(ATTRIBUTE); return K_ASSIGN; }
<INITIAL>{bpwsns}?"branches"		{ BEGIN(ATTRIBUTE); return K_BRANCHES; }
<INITIAL>{bpwsns}?"case"		{ BEGIN(ATTRIBUTE); return K_CASE; }
<INITIAL>{bpwsns}?"catch"		{ BEGIN(ATTRIBUTE); return K_CATCH; }
<INITIAL>{bpwsns}?"catchAll"		{ BEGIN(ATTRIBUTE); return K_CATCHALL; }
<INITIAL>{bpwsns}?"compensate"		{ BEGIN(ATTRIBUTE); return K_COMPENSATE; }
<INITIAL>{bpwsns}?"compensateScope"	{ BEGIN(ATTRIBUTE); return K_COMPENSATESCOPE; }
<INITIAL>{bpwsns}?"compensationHandler"	{ return K_COMPENSATIONHANDLER; }
<INITIAL>{bpwsns}?"completionCondition"	{ return K_COMPLETIONCONDITION; }
<INITIAL>{bpwsns}?"copy"		{ return K_COPY; }
<INITIAL>{bpwsns}?"condition"		{ BEGIN(ATTRIBUTE); return K_CONDITION; }
<INITIAL>{bpwsns}?"correlation"		{ BEGIN(ATTRIBUTE); return K_CORRELATION; }
<INITIAL>{bpwsns}?"correlations"	{ return K_CORRELATIONS; }
<INITIAL>{bpwsns}?"correlationSet"	{ BEGIN(ATTRIBUTE); return K_CORRELATIONSET; }
<INITIAL>{bpwsns}?"correlationSets"	{ return K_CORRELATIONSETS; }
<INITIAL>{bpwsns}?"else"		{ BEGIN(ATTRIBUTE); return K_ELSE; }
<INITIAL>{bpwsns}?"elseif"		{ BEGIN(ATTRIBUTE); return K_ELSEIF; }
<INITIAL>{bpwsns}?"empty"		{ BEGIN(ATTRIBUTE); return K_EMPTY; }
<INITIAL>{bpwsns}?"eventHandlers"	{ return K_EVENTHANDLERS; }
<INITIAL>{bpwsns}?"exit"		{ BEGIN(ATTRIBUTE); return K_EXIT; }
<INITIAL>{bpwsns}?"extension"		{ BEGIN(ATTRIBUTE); return K_EXTENSION; }
<INITIAL>{bpwsns}?"extensionActivity"	{ BEGIN(ATTRIBUTE); return K_EXTENSIONACTIVITY; }
<INITIAL>{bpwsns}?"extensionAssignOperation"	{ BEGIN(ATTRIBUTE); return K_EXTENSIONASSIGNOPERATION; }
<INITIAL>{bpwsns}?"extensions"		{ BEGIN(ATTRIBUTE); return K_EXTENSIONS; }
<INITIAL>{bpwsns}?"faultHandlers"	{ return K_FAULTHANDLERS; }
<INITIAL>{bpwsns}?"finalCounterValue"	{ BEGIN(ATTRIBUTE); return K_FINALCOUNTERVALUE; }
<INITIAL>{bpwsns}?"flow"		{ BEGIN(ATTRIBUTE); return K_FLOW; }
<INITIAL>{bpwsns}?"for"			{ BEGIN(ATTRIBUTE); return K_FOR; }
<INITIAL>{bpwsns}?"forEach"		{ BEGIN(ATTRIBUTE); return K_FOREACH; }
<INITIAL>{bpwsns}?"from"		{ BEGIN(ATTRIBUTE); return K_FROM; }
<INITIAL>{bpwsns}?"fromPart"		{ BEGIN(ATTRIBUTE); return K_FROMPART; }
<INITIAL>{bpwsns}?"fromParts"		{ BEGIN(ATTRIBUTE); return K_FROMPARTS; }
<INITIAL>{bpwsns}?"if"			{ BEGIN(ATTRIBUTE); return K_IF; }
<INITIAL>{bpwsns}?"import"		{ BEGIN(ATTRIBUTE); return K_IMPORT; }
<INITIAL>{bpwsns}?"invoke"		{ BEGIN(ATTRIBUTE); return K_INVOKE; }
<INITIAL>{bpwsns}?"joinCondition"	{ BEGIN(ATTRIBUTE); parseJoinCondition = true; return K_JOINCONDITION; }
<INITIAL>{bpwsns}?"link"		{ BEGIN(ATTRIBUTE); return K_LINK; }
<INITIAL>{bpwsns}?"links"		{ BEGIN(ATTRIBUTE); return K_LINKS; }
<INITIAL>{bpwsns}?"literal"		{ BEGIN(ATTRIBUTE); return K_LITERAL; }
<INITIAL>{bpwsns}?"messageExchange"	{ BEGIN(ATTRIBUTE); return K_MESSAGEEXCHANGE; }
<INITIAL>{bpwsns}?"messageExchanges"	{ BEGIN(ATTRIBUTE); return K_MESSAGEEXCHANGES; }
<INITIAL>{bpwsns}?"onAlarm"		{ BEGIN(ATTRIBUTE); return K_ONALARM; }
<INITIAL>{bpwsns}?"onEvent"		{ BEGIN(ATTRIBUTE); return K_ONEVENT; }
<INITIAL>{bpwsns}?"onMessage"		{ BEGIN(ATTRIBUTE); return K_ONMESSAGE; }
<INITIAL>{bpwsns}?"opaqueActivity"	{ BEGIN(ATTRIBUTE); return K_OPAQUEACTIVITY; }
<INITIAL>{bpwsns}?"opaqueFrom"		{ return K_OPAQUEFROM; }
<INITIAL>{bpwsns}?"otherwise"		{ return K_OTHERWISE; }
<INITIAL>{bpwsns}?"partner"		{ BEGIN(ATTRIBUTE); return K_PARTNER; }
<INITIAL>{bpwsns}?"partnerLink"		{ BEGIN(ATTRIBUTE); return K_PARTNERLINK; }
<INITIAL>{bpwsns}?"partnerLinks"	{ BEGIN(ATTRIBUTE); return K_PARTNERLINKS; }
<INITIAL>{bpwsns}?"partners"		{ return K_PARTNERS; }
<INITIAL>{bpwsns}?"pick"		{ BEGIN(ATTRIBUTE); return K_PICK; }
<INITIAL>{bpwsns}?"process"		{ BEGIN(ATTRIBUTE); return K_PROCESS; }
<INITIAL>{bpwsns}?"query"		{ BEGIN(ATTRIBUTE); return K_QUERY; }
<INITIAL>{bpwsns}?"receive"		{ BEGIN(ATTRIBUTE); return K_RECEIVE; }
<INITIAL>{bpwsns}?"reply"		{ BEGIN(ATTRIBUTE); return K_REPLY; }
<INITIAL>{bpwsns}?"repeatEvery"		{ BEGIN(ATTRIBUTE); return K_REPEATEVERY; }
<INITIAL>{bpwsns}?"repeatUntil"		{ BEGIN(ATTRIBUTE); return K_REPEATUNTIL; }
<INITIAL>{bpwsns}?"rethrow"		{ BEGIN(ATTRIBUTE); return K_RETHROW; }
<INITIAL>{bpwsns}?"scope"		{ BEGIN(ATTRIBUTE); return K_SCOPE; }
<INITIAL>{bpwsns}?"sequence"		{ BEGIN(ATTRIBUTE); return K_SEQUENCE; }
<INITIAL>{bpwsns}?"source"		{ BEGIN(ATTRIBUTE); return K_SOURCE; }
<INITIAL>{bpwsns}?"sources"		{ BEGIN(ATTRIBUTE); return K_SOURCES; }
<INITIAL>{bpwsns}?"startCounterValue"	{ BEGIN(ATTRIBUTE); return K_STARTCOUNTERVALUE; }
<INITIAL>{bpwsns}?"switch"		{ BEGIN(ATTRIBUTE); return K_SWITCH; }
<INITIAL>{bpwsns}?"target"		{ BEGIN(ATTRIBUTE); return K_TARGET; }
<INITIAL>{bpwsns}?"targets"		{ BEGIN(ATTRIBUTE); return K_TARGETS; }
<INITIAL>{bpwsns}?"terminate"		{ BEGIN(ATTRIBUTE); return K_TERMINATE; }
<INITIAL>{bpwsns}?"terminationHandler"	{ BEGIN(ATTRIBUTE); return K_TERMINATIONHANDLER; }
<INITIAL>{bpwsns}?"throw"		{ BEGIN(ATTRIBUTE); return K_THROW; }
<INITIAL>{bpwsns}?"to"			{ BEGIN(ATTRIBUTE); return K_TO; }
<INITIAL>{bpwsns}?"toPart"		{ BEGIN(ATTRIBUTE); return K_TOPART; }
<INITIAL>{bpwsns}?"toParts"		{ BEGIN(ATTRIBUTE); return K_TOPARTS; }
<INITIAL>{bpwsns}?"transitionCondition"	{ BEGIN(ATTRIBUTE); return K_TRANSITIONCONDITION; }
<INITIAL>{bpwsns}?"until"		{ BEGIN(ATTRIBUTE); return K_UNTIL; }
<INITIAL>{bpwsns}?"validate"		{ BEGIN(ATTRIBUTE); return K_VALIDATE; }
<INITIAL>{bpwsns}?"variable"		{ BEGIN(ATTRIBUTE); return K_VARIABLE; }
<INITIAL>{bpwsns}?"variables"		{ return K_VARIABLES; }
<INITIAL>{bpwsns}?"wait"		{ BEGIN(ATTRIBUTE); return K_WAIT; }
<INITIAL>{bpwsns}?"while"		{ BEGIN(ATTRIBUTE); return K_WHILE; }

 /* symbols for BPEL4Chor */
<INITIAL>"topology"			{ BEGIN(ATTRIBUTE); return K_TOPOLOGY; }
<INITIAL>"participantTypes"		{ BEGIN(ATTRIBUTE); return K_PARTICIPANTTYPES; }
<INITIAL>"participantType"		{ BEGIN(ATTRIBUTE); return K_PARTICIPANTTYPE; }
<INITIAL>"participants"			{ BEGIN(ATTRIBUTE); return K_PARTICIPANTS; }
<INITIAL>"participant"			{ BEGIN(ATTRIBUTE); return K_PARTICIPANT; }
<INITIAL>"participantSet"		{ BEGIN(ATTRIBUTE); return K_PARTICIPANTSET; }
<INITIAL>"messageLinks"			{ BEGIN(ATTRIBUTE); return K_MESSAGELINKS; }
<INITIAL>"messageLink"			{ BEGIN(ATTRIBUTE); return K_MESSAGELINK; }


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
<INITIAL>"$"{variablename}	{ frontend_lval.yt_casestring = kc::mkcasestring(frontend_text);
                                  return VARIABLENAME; }

<INITIAL>{name}			{ frontend_lval.yt_casestring = kc::mkcasestring(frontend_text);
                                  return X_NAME; }
{number}	{ frontend_lval.yt_casestring = kc::mkcasestring(frontend_text);
                  return NUMBER; }


 /* end of input file */
<<EOF>>				{ return EOF; }


 /* anything else */
.				{ if (debug_level >= TRACE_WARNINGS) frontend_error("lexical error"); }
