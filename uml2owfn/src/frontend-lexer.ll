/*****************************************************************************\
 UML2oWFN -- Translating UML2 Activity Diagrams to Petri nets

 Copyright (C) 2007, 2008, 2009  Dirk Fahland and Martin Znamirowski

 UML2oWFN is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 UML2oWFN is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with UML2oWFN.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

%{
/*!
 * \file frontend-lexer.cc
 *
 * \brief generic XML lexer
 *
 * \note    This file was created using Flex reading file frontend-lexer.ll.
 *          See http://www.gnu.org/software/flex for details.
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
UB     			[\200-\277]



 /* start conditions of the lexer */
%s ATTRIBUTE
%s COMMENT
%s XMLHEADER
%s TAG

%%

 /* code to skip UTF-8 characters */
[\300-\337]{UB}             { if (!parseUnicode) { parseUnicode = true;} }
[\340-\357]{UB}{2}          { if (!parseUnicode) { parseUnicode = true;} }
[\360-\367]{UB}{3}          { if (!parseUnicode) { parseUnicode = true;} }
[\370-\373]{UB}{4}          { if (!parseUnicode) { parseUnicode = true;} }
[\374-\375]{UB}{5}          { if (!parseUnicode) { parseUnicode = true; } }

 /* comments */
"!--"				{ currentView = YY_START; BEGIN(COMMENT); }
<COMMENT>{comment}		{ /* skip */ }
<COMMENT>"-->"[ \t\r\n]*"<"	{ BEGIN(currentView); }

<INITIAL>"<?xml"				{ currentView = YY_START; BEGIN(XMLHEADER); }
<XMLHEADER>{xmlheader}		{ /* skip */ }
<XMLHEADER>"?>"	{ BEGIN(currentView); }

 /* attributes */
<ATTRIBUTE>{name}	{ frontend_lval = frontend_text;
                          return X_NAME; }
<ATTRIBUTE>{string}	{ std::string stringwoquotes = std::string(frontend_text).substr(1, strlen(frontend_text)-2);
                          frontend_lval = stringwoquotes.c_str();
                          return X_STRING; }
<ATTRIBUTE>"="		{ return X_EQUALS; }

 /* XML-elements */
<INITIAL>"<"				{  BEGIN(TAG); return X_OPEN;}
<INITIAL>"<"{name}":"	    {  BEGIN(TAG); return X_OPEN;}
<INITIAL>"</"				{  BEGIN(TAG); return X_END_OPEN;}
<INITIAL>"</"{name}":"	    {  BEGIN(TAG); return X_END_OPEN;}
<ATTRIBUTE>"/>"         	{  BEGIN(INITIAL); return X_END_CLOSE; }
<INITIAL,ATTRIBUTE>">"		{ BEGIN(INITIAL); return X_CLOSE; }

 /* names of BPEL-elements */
<TAG>{name}		{ BEGIN(ATTRIBUTE); frontend_lval = frontend_text; return K_TAG; }
 /* white space */
{whitespace}			{ /* skip white space */ }

 /* names and numbers */
<INITIAL>{name}			{ frontend_lval = frontend_text;
                                  return X_LITERAL; }
<INITIAL>{number}		{ frontend_lval = frontend_text;
                                  return X_LITERAL; }

 /* end of input file */
<<EOF>>				{ return EOF; }


 /* anything else */
.				{ if (debug_level >= TRACE_WARNINGS) frontend_error("lexical error"); }
