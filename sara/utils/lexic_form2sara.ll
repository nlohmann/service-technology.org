/*****************************************************************************\
 Sara -- Structures for Automated Reachability Analysis

 Copyright (C) 2011  Harro Wimmel <harro.wimmel@uni-rostock.de>

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
%option prefix="form2sara_"

%{
#include <cstring>
#include <string>
#include "syntax_form2sara.hh"
#include "verbose.h"

std::string form2sara_NAME_token;

void form2sara_error(const char *);
%}


name      [a-zA-Z_][^,;:\t \n\{\}<>+\-]*
number    [0-9][0-9]*

%%

","          { return COMMA; }
"="	{ return EQSIGN; }
"("	{ return POPEN; }
")"	{ return PCLOSE; }
"["	{ return BROPEN; }
"]"	{ return BRCLOSE; }
"{"	{ return BOPEN; }
"}"	{ return BCLOSE; }
"*"	{ return STARSIGN; }
"."	{ return DOTSIGN; }
"reachable"	{ return REACHABLE; }
"invariant"	{ return INVARIANT; }
"equals"	{ return EQUALS; }
"contains"	{ return CONTAINS; }
"not"	{ return NOTSIGN; }
"and"	{ return ANDSIGN; }
"or"	{ return ORSIGN; }
"NOT"	{ return NOTSIGN; }
"AND"	{ return ANDSIGN; }
"OR"	{ return ORSIGN; }

{number}     { form2sara_lval.val = atoi(form2sara_text); return NUMBER; }

{name}       { form2sara_NAME_token = form2sara_text; return NAME; }

[ \t\r]*   { /* skip */ }
\n	{ return EOL; }
<<EOF>>      { return EOF; }
.            { form2sara_error("lexical error"); }

%%

__attribute__((noreturn)) void form2sara_error(const char *msg) {
  status("%d: error near '%s': %s", form2sara_lineno, form2sara_text, msg);
  abort(31, "error while parsing the formula file");
}

