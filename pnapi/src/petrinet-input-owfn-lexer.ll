/* -*- mode: c++ -*- */

/*!
 * \file    petrinet-input-owfn-lexer.ll
 *
 * \brief   lexical scanner for OWFN files
 *
 * \author  Robert Waltemath <robert.waltemath@uni-rostock.de>
 *          last changes of: $Author$
 *
 * \since   2008/11/10
 *
 * \date    $Date$
 *
 * \note    This file is part of ...
 *
 * \version $Revision$
 */


 /***************************************************************************** 
  * flex options 
  ****************************************************************************/

/* plain c scanner: the prefix is our "namespace" */
%option prefix="pnapi_owfn_"

/* we read only one file */
%option noyywrap


 /***************************************************************************** 
  * C declarations 
  ****************************************************************************/
%{

#include "petrinet-input-owfn-parser.h"

extern int pnapi_owfn_error(const char *);

%}


 /*****************************************************************************
  * regular expressions 
  ****************************************************************************/

%s COMMENT

%%

"{$"                            { return LCONTROL; }
"$}"                            { return RCONTROL; }

"{"                             { BEGIN(COMMENT); }
<COMMENT>"}"                    { BEGIN(INITIAL); }
<COMMENT>[^}]*                  {}

MAX_UNIQUE_EVENTS               { return KEY_MAX_UNIQUE_EVENTS; }
ON_LOOP                         { return KEY_ON_LOOP; }
MAX_OCCURRENCES                 { return KEY_MAX_OCCURRENCES; }
TRUE                            { return KEY_TRUE; }
FALSE                           { return KEY_FALSE; }

SAFE                            { return KEY_SAFE;}
PLACE                           { return KEY_PLACE; }
INTERNAL                        { return KEY_INTERNAL; }
INPUT                           { return KEY_INPUT; }
OUTPUT                          { return KEY_OUTPUT; }
TRANSITION                      { return KEY_TRANSITION; }
INITIALMARKING                  { return KEY_MARKING; }
FINALMARKING                    { return KEY_FINALMARKING; }
FINALCONDITION                  { return KEY_FINALCONDITION; }
CONSUME                         { return KEY_CONSUME; }
PRODUCE                         { return KEY_PRODUCE; }
PORTS                           { return KEY_PORTS; }
ALL_OTHER_PLACES_EMPTY          { return KEY_ALL_OTHER_PLACES_EMPTY; }
ALL_OTHER_INTERNAL_PLACES_EMPTY { return KEY_ALL_OTHER_INTERNAL_PLACES_EMPTY; }
ALL_OTHER_EXTERNAL_PLACES_EMPTY { return KEY_ALL_OTHER_EXTERNAL_PLACES_EMPTY; }
AND                             { return OP_AND;}
OR                              { return OP_OR;}
NOT                             { return OP_NOT;}
\>                              { return OP_GT;}
\<                              { return OP_LT;}
\>=                             { return OP_GE;}
\<=                             { return OP_LE;}
=                               { return OP_EQ;}
\<\>                            { return OP_NE;}
\#                              { return OP_NE;}
\:                              { return COLON; }
\;                              { return SEMICOLON; }
,                               { return COMMA; }
\(                              { return LPAR;}
\)                              { return RPAR;}
[0-9][0-9]*                     {
  pnapi_owfn_lval.yt_string = yytext;
  return NUMBER; }
"-"[0-9][0-9]*                  { 
  pnapi_owfn_lval.yt_string = yytext;
  return NEGATIVE_NUMBER; }
[^,;:()\t \n\r\{\}=][^,;:()\t \n\r\{\}=]* { 
  pnapi_owfn_lval.yt_string = yytext;
  return IDENT; }
[\n\r]                          { break; }
[ \t]                           { break; }
.                               { pnapi_owfn_error("lexical error"); }
