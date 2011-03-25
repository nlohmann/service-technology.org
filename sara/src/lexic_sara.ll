/*****************************************************************************\
 Sara -- Structures for Automated Reachability Analysis

 Copyright (C) 2009  Harro Wimmel <harro.wimmel@uni-rostock.de>

 Sara is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Sara is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Sara.  If not, see <http://www.gnu.org/licenses/>. 
\*****************************************************************************/

%option noyywrap
%option yylineno
%option nodefault
%option outfile="lex.yy.c"
%option prefix="sara_"

%{
#include <cstring>
#include <string>
#include "syntax_sara.h"
#include "verbose.h"

std::string sara_NAME_token;

void sara_error(const char *);
%}

%s xname nname

ename     [^,;:\t \n][^,;:\t \n]*
name      [^,;:\t \n\{\}<>+\-0-9][^,;:\t \n\{\}<>+\-]*
number    "-"?[0-9][0-9]*
problem PROBLEM
goal GOAL
reachability REACHABILITY
realizability REALIZABILITY
nfile FILE
ntype TYPE
typeowfn OWFN
typelola LOLA
typepnml PNML
initialm INITIAL
finalm FINAL
cover COVER
reach REACH
constraints CONSTRAINTS
result RESULT
negate NEGATE
resor OR

%%

":"          { return COLON; }
","          { return COMMA; }
";"		{ return SEMICOLON; }
">"		{ return MYGEQ; }
"<"		{ return MYLEQ; }
"+"		{ return PLUS; }
"-"		{ return MINUS; }
{problem}	{ BEGIN(xname); return PROBLEM; }
{goal}		{ return GOAL; }
{reachability}	{ return REACHABILITY; }
{realizability} { return REALIZABILITY; }
{nfile}	{ return NFILE; }
{ntype}	{ BEGIN(nname); return NTYPE; }
{typeowfn}	{ return TYPEOWFN; }
{typelola}	{ return TYPELOLA; }
{typepnml}	{ return TYPEPNML; }
{initialm}	{ return INITIALM; }
{finalm}	{ return FINALM; }
{cover} { return COVER; }
{reach}	{ return REACH; }
{constraints}	{ return CONSTRAINTS; }
{result} { BEGIN(xname); return RESULT; }
{negate} { return NEGATE; }
{resor} { return RESOR; }
{number}     { sara_lval.val = atoi(sara_text); return NUMBER; }

<nname>{name}       { sara_NAME_token = sara_text; return NAME; }
<xname>{ename}       { sara_NAME_token = sara_text; return ENAME; }

[ \t\r\n]*   { /* skip */ }
<<EOF>>      { return EOF; }
.            { sara_error("lexical error"); }

%%

__attribute__((noreturn)) void sara_error(const char *msg) {
  sara::status("%d: error near '%s': %s", sara_lineno, sara_text, msg);
  sara::abort(5, "error while parsing the sara problem file");
}

