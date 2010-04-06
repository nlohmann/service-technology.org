 // -*- C++ -*-

 /***************************************************************************** 
  * flex options 
  ****************************************************************************/

/* create a c++ lexer */
%option c++

/* we provide out own class */
%option yyclass="pnapi::parser::owfn::yy::Lexer"

/* we need to prefix its base class */
%option prefix="Owfn"

/* created lexer should be called "lex.yy.c" to make the ylwrap script work */
%option outfile="lex.yy.c"

/* we read only one file */
%option noyywrap

/* yyunput not needed (fix compiler warning) */
%option nounput

/* maintain line number for error reporting */
%option yylineno


 /***************************************************************************** 
  * C declarations 
  ****************************************************************************/
%{

#include "config.h"

#include "formula.h"
#include "parser-owfn-wrapper.h"

#include <iostream>

/* tokens are defined in a struct in a class */
typedef pnapi::parser::owfn::yy::BisonParser::token tt;

%}


 /*****************************************************************************
  * regular expressions 
  ****************************************************************************/

 /* a start condition to skip comments */
%s COMMENT

/* 
 * The state "IDENT2" is used for transition identifier.
 * Unlike place identifier these can contain the char "=",
 */
%s IDENT2

%%

 /* control comments */ 
"{$"                            { return tt::LCONTROL; }
"$}"                            { return tt::RCONTROL; }

 /* comments */
"{"                             { BEGIN(COMMENT); }
<COMMENT>"}"                    { BEGIN(INITIAL); }
<COMMENT>[^}]*                  { /* skip */ }

 /* control keywords */
MAX_UNIQUE_EVENTS               { return tt::KEY_MAX_UNIQUE_EVENTS; }
ON_LOOP                         { return tt::KEY_ON_LOOP; }
MAX_OCCURRENCES                 { return tt::KEY_MAX_OCCURRENCES; }
TRUE                            { return tt::KEY_TRUE; }
FALSE                           { return tt::KEY_FALSE; }

 /* keywords */
SAFE                            { return tt::KEY_SAFE; }
PLACE                           { return tt::KEY_PLACE; }
INTERFACE                       { return tt::KEY_INTERFACE; }
INTERNAL                        { return tt::KEY_INTERNAL; }
INPUT                           { return tt::KEY_INPUT; }
OUTPUT                          { return tt::KEY_OUTPUT; }

TRANSITION                      { BEGIN(IDENT2); return tt::KEY_TRANSITION; }
<IDENT2>[ \n\r\t]               { /* skip whitespaces */ }
<IDENT2>[^,;:()\t \n\r\{\}]+    { BEGIN(INITIAL); yylval->yt_str = strdup(yytext); return tt::IDENT; }
<IDENT2>.                       { LexerError("Unexpected symbol at transition identifier"); }

INITIALMARKING                  { return tt::KEY_INITIALMARKING; }
FINALMARKING                    { return tt::KEY_FINALMARKING; }
NOFINALMARKING                  { return tt::KEY_NOFINALMARKING; }
FINALCONDITION                  { return tt::KEY_FINALCONDITION; }
COST                            { return tt::KEY_COST; }
CONSUME                         { return tt::KEY_CONSUME; }
PRODUCE                         { return tt::KEY_PRODUCE; }
PORT                            { return tt::KEY_PORT; }
PORTS                           { return tt::KEY_PORTS; }
ROLES				{ return tt::KEY_ROLES; }
SYNCHRONOUS                     { return tt::KEY_SYNCHRONOUS; }
SYNCHRONIZE                     { return tt::KEY_SYNCHRONIZE; }
CONSTRAIN                       { return tt::KEY_CONSTRAIN; }

 /* keywords for final conditions */
ALL_PLACES_EMPTY                { return tt::KEY_ALL_PLACES_EMPTY; }
ALL_OTHER_PLACES_EMPTY          { return tt::KEY_ALL_OTHER_PLACES_EMPTY; }
ALL_OTHER_INTERNAL_PLACES_EMPTY { return tt::KEY_ALL_OTHER_INTERNAL_PLACES_EMPTY; }
ALL_OTHER_EXTERNAL_PLACES_EMPTY { return tt::KEY_ALL_OTHER_EXTERNAL_PLACES_EMPTY; }
AND                             { return tt::OP_AND; }
OR                              { return tt::OP_OR; }
NOT                             { return tt::OP_NOT; }
\>                              { return tt::OP_GT; }
\<                              { return tt::OP_LT; }
\>=                             { return tt::OP_GE; }
\<=                             { return tt::OP_LE; }
=                               { return tt::OP_EQ; }
\<\>                            { return tt::OP_NE; }
\#                              { return tt::OP_NE; }
\(                              { return tt::LPAR; }
\)                              { return tt::RPAR; }

 /* other characters */
\:                              { return tt::COLON; }
\;                              { return tt::SEMICOLON; }
,                               { return tt::COMMA; }

 /* identifiers */
[0-9]+                          { yylval->yt_int = atoi(yytext); return tt::NUMBER; }
"-"[0-9]+                       { yylval->yt_int = atoi(yytext); return tt::NEGATIVE_NUMBER; }
[^,;:()\t \n\r\{\}=]+           { yylval->yt_str = strdup(yytext); return tt::IDENT; }

 /* whitespace */
[ \n\r\t]                       { /* skip */ }

 /* anything else */
.                               { LexerError("unexpected lexical token"); }

%%

