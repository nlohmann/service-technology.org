/*****************************************************************************\
 Copyright (c) 2008  Robert Danitz, Christian Gierds, Niels Lohmann,
                     Peter Massuthe, Richard Mu"ller, Janine Ott,
                     Jan Su"rmeli, Daniela Weinberg, Karsten Wolf,
                     Martin Znamirowski

 Copyright (c) 2007  Jan Bretschneider, Robert Danitz, Christian Gierds,
                     Kathrin Kaschner, Leonard Kern, Niels Lohmann,
                     Peter Massuthe, Daniela Weinberg, Karsten Wolf

 Copyright (c) 2006  Kathrin Kaschner, Peter Massuthe, Daniela Weinberg,
                     Karsten Wolf

 This file is part of Fiona, see <http://service-technology.org/fiona>.

 Fiona is free software; you can redistribute it and/or modify it under the
 terms of the GNU General Public License as published by the Free Software
 Foundation; either version 3 of the License, or (at your option) any later
 version.
 
 Fiona is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License along with
 Fiona (see file COPYING). If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

/*!
 * \file    lexer_owfn.cc
 *
 * \brief   the lexer
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 */



/* flex options */
%option outfile="lex.yy.c"
%option prefix="owfn_yy"
%option noyywrap
%option yylineno
%option nodefault
%option debug

/* a start condition to skip comments */
%s COMMENT



%{
#define YY_NO_UNPUT                // We don't need yyunput().
#include "syntax_owfn_wrap.h"      // list of all tokens used
#include <cstring>

extern int owfn_yyerror(const char *msg);
static void setlval();
%}



%%



 /* control comments */ 
"{$"                            { return LCONTROL; }
"$}"                            { return RCONTROL; }

 /* comments */
"{"                             { BEGIN(COMMENT); }
<COMMENT>"}"                    { BEGIN(INITIAL); }
<COMMENT>[^}]*                  { /* skip */ }

 /* control keywords */
MAX_UNIQUE_EVENTS               { return KEY_MAX_UNIQUE_EVENTS; }
ON_LOOP                         { return KEY_ON_LOOP; }
MAX_OCCURRENCES                 { return KEY_MAX_OCCURRENCES; }
TRUE                            { return KEY_TRUE; }
FALSE                           { return KEY_FALSE; }

 /* keywords */
SAFE                            { return KEY_SAFE; }
PLACE                           { return KEY_PLACE; }
INTERFACE                       { return KEY_INTERFACE; }
INTERNAL                        { return KEY_INTERNAL; }
INPUT                           { return KEY_INPUT; }
OUTPUT                          { return KEY_OUTPUT; }
TRANSITION                      { return KEY_TRANSITION; }
INITIALMARKING                  { return KEY_MARKING; }
FINALMARKING                    { return KEY_FINALMARKING; }
NOFINALMARKING                  { return KEY_NOFINALMARKING; }
FINALCONDITION                  { return KEY_FINALCONDITION; }
CONSUME                         { return KEY_CONSUME; }
PRODUCE                         { return KEY_PRODUCE; }
PORT                            { return KEY_PORT; }
PORTS                           { return KEY_PORTS; }
SYNCHRONOUS                     { return KEY_SYNCHRONOUS; }
SYNCHRONIZE                     { return KEY_SYNCHRONIZE; }

 /* keywords for final conditions */
ALL_OTHER_PLACES_EMPTY          { return KEY_ALL_OTHER_PLACES_EMPTY; }
ALL_OTHER_INTERNAL_PLACES_EMPTY { return KEY_ALL_OTHER_INTERNAL_PLACES_EMPTY; }
ALL_OTHER_EXTERNAL_PLACES_EMPTY { return KEY_ALL_OTHER_EXTERNAL_PLACES_EMPTY; }
AND                             { return OP_AND; }
OR                              { return OP_OR; }
NOT                             { return OP_NOT; }
\>                              { return OP_GT; }
\<                              { return OP_LT; }
\>=                             { return OP_GE; }
\<=                             { return OP_LE; }
=                               { return OP_EQ; }
\<\>                            { return OP_NE; }
\#                              { return OP_NE; }
\(                              { return LPAR; }
\)                              { return RPAR; }

 /* other characters */
\:                              { return COLON; }
\;                              { return SEMICOLON; }
,                               { return COMMA; }

 /* identifiers */
[0-9][0-9]*                               { setlval(); return NUMBER; }
"-"[0-9][0-9]*                            { setlval(); return NEGATIVE_NUMBER; }
[^,;:()\t \n\r\{\}=][^,;:()\t \n\r\{\}=]* { setlval(); return IDENT; }

 /* whitespace */
[\n\r]                          { /* skip */ }
[ \t]                           { /* skip */ }

 /* anything else */
.                               { owfn_yyerror("lexical error"); }



%%



/// pass token string as attribute to bison
static void setlval() {
    owfn_yylval.str = strdup(owfn_yytext);
}
