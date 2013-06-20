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
%option prefix="lola2sara_"

%{
#include <cstring>
#include <string>
#include "syntax_lola2sara.hh"
#include "verbose.h"

std::string lola2sara_NAME_token;

void lola2sara_error(const char *);
%}


name      [a-zA-Z_][^,;:\t \n\{\}<>=!()\[\]+\-]*
number    [0-9][0-9]*

%%

","     { return COMMA; }
":"     { return COLON; }
";"     { return SEMICOLON; }
">="	{ return GESIGN; }
"<="	{ return LESIGN; }
"="	{ return EQSIGN; }
"!="	{ return NEQSIGN; }
"("	{ return POPEN; }
")"	{ return PCLOSE; }
"["	{ return BROPEN; }
"]"	{ return BRCLOSE; }
"{"	{ return BOPEN; }
"}"	{ return BCLOSE; }
"->"	{ return IMPLYSIGN; }
"-->"	{ return IMPLYSIGN; }
"<->"	{ return EQUIVSIGN; }
"<-->"	{ return EQUIVSIGN; }
"+"	{ return PLUS; }
"-"	{ return MINUS; }
"not"	{ return NOTSIGN; }
"and"	{ return ANDSIGN; }
"or"	{ return ORSIGN; }
"xor"	{ return XORSIGN; }
"NOT"	{ return NOTSIGN; }
"AND"	{ return ANDSIGN; }
"OR"	{ return ORSIGN; }
"XOR"	{ return XORSIGN; }
"implies"	{ return IMPLYSIGN; }
"IMPLIES"	{ return IMPLYSIGN; }
"FORMULA"	{ return FORMULA; }
"formula"	{ return FORMULA; }
"true"	{ return TRUESIGN; }
"TRUE"	{ return TRUESIGN; }
"false"	{ return FALSESIGN; }
"FALSE"	{ return FALSESIGN; }

{number}     { lola2sara_lval.val = atoi(lola2sara_text); return NUMBER; }

{name}       { lola2sara_NAME_token = lola2sara_text; return NAME; }

[ \n\t\r]*   { /* skip */ }
<<EOF>>      { return EOF; }
.            { lola2sara_error("lexical error"); }

%%

__attribute__((noreturn)) void lola2sara_error(const char *msg) {
  status("%d: error near '%s': %s", lola2sara_lineno, lola2sara_text, msg);
  abort(31, "error while parsing the formula file");
}

