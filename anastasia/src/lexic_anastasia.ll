/*****************************************************************************\
 Anastasia -- An Analyzer for Siphon and Traps in Arbitrary Nets

 Copyright (C) 2010  Harro Wimmel <harro.wimmel@uni-rostock.de>

 Anastasia is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Anastasia is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Anastasia.  If not, see <http://www.gnu.org/licenses/>. 
\*****************************************************************************/

%option noyywrap
%option yylineno
%option default
%option outfile="lex.yy.c"
%option prefix="anastasia_"

%{
#include <cstring>
#include <string>
#include "syntax_anastasia.hh"
#include "verbose.h"

std::string anastasia_NAME_token;

void anastasia_error(const char *);
%}

%x stateformula

setname	[A-Z][^,() \t\n]*
varname	[a-z][^,;<\- \t\n]*
formula FORMULA
siphon Siphon
trap Trap
maxtrap MaxTrap
empty Empty
nempty Nonempty
marked Marked
unmarked Unmarked
subset Subset
inscc InSCC
coverscc CoverSCC
opand and
opor or
opimpl ->
opequ <->
bigand AND
bigor OR
%%


<stateformula><<EOF>> { anastasia_error("end of file inside formula"); }
<stateformula>";" { BEGIN(INITIAL); return COMMA; }
<stateformula>"-"			{ return NEGATION; }
<stateformula>","			{ return COMMA; }
<stateformula>"("			{ return POPEN; }
<stateformula>")"			{ return PCLOSE; }
<stateformula>":"			{ return COLON; }
<stateformula>{siphon}	{ return SIPHON; }
<stateformula>{trap}		{ return TRAP; }
<stateformula>{maxtrap}	{ return MAXTRAP; }
<stateformula>{empty}		{ return EMPTY; }
<stateformula>{nempty}	{ return NEMPTY; }
<stateformula>{marked}	{ return MARKED; }
<stateformula>{unmarked}	{ return UNMARKED; }
<stateformula>{subset}	{ return SUBSET; }
<stateformula>{inscc}	{ return INSCC; }
<stateformula>{coverscc}	{ return COVERSCC; }
<stateformula>{opand}		{ return OPAND; }
<stateformula>{opor}		{ return OPOR; }
<stateformula>{opimpl}	{ return OPIMPL; }
<stateformula>{opequ}		{ return OPEQU; }
<stateformula>{bigand}	{ return BIGAND; }
<stateformula>{bigor}		{ return BIGOR; }

<stateformula>{setname}       { anastasia_NAME_token = anastasia_text; return SETNAME; }
<stateformula>{varname}       { anastasia_NAME_token = anastasia_text; return VARNAME; }

<*>[ \t\r\n]*   { /* skip */ }
<stateformula>.            { anastasia_error("lexical error"); }

{formula} { BEGIN(stateformula); return FORMULA; }
<<EOF>> { return EOF; }
. {}
%%
__attribute__((noreturn)) void anastasia_error(const char *msg) {
  status("%d: error near '%s': %s", anastasia_lineno, anastasia_text, msg);
  abort(5, "error while parsing the user-defined formula");
}

