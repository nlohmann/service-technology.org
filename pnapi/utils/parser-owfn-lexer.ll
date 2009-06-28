 // -*- C++ -*-

 /***************************************************************************** 
  * flex options 
  ****************************************************************************/

/* created lexer should be called "lex.yy.c" to make the ylwrap script work */
%option outfile="lex.yy.c"

/* plain c scanner: the prefix is our "namespace" */
%option prefix="owfn_yy"

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

#ifndef PACKAGE
#define PACKAGE "Fast owfn parser"
#endif

#include "pnapi.h"
#include "parser-owfn.h"

#include <string>
#include <iostream>

extern int owfn_yyerror(char const *msg);

%}


 /*****************************************************************************
  * regular expressions 
  ****************************************************************************/

 /* a start condition to skip comments */
%s COMMENT

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
COST                            { return KEY_COST; }
CONSUME                         { return KEY_CONSUME; }
PRODUCE                         { return KEY_PRODUCE; }
PORT                            { return KEY_PORT; }
PORTS                           { return KEY_PORTS; }
SYNCHRONOUS                     { return KEY_SYNCHRONOUS; }
SYNCHRONIZE                     { return KEY_SYNCHRONIZE; }
CONSTRAIN                       { return KEY_CONSTRAIN; }

 /* keywords for final conditions */
ALL_PLACES_EMPTY                { return KEY_ALL_PLACES_EMPTY; }
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
[0-9][0-9]*                     { owfn_yylval.yt_int = atoi(yytext); return NUMBER; }
"-"[0-9][0-9]*                  { owfn_yylval.yt_int = atoi(yytext); return NEGATIVE_NUMBER; }
[^,;:()\t \n\r\{\}=][^,;:()\t \n\r\{\}=]* { owfn_yylval.yt_string = new std::string(yytext); return IDENT; }

 /* whitespace */
[ \n\r\t]                          { /* skip */ }

 /* anything else */
.                               { owfn_yyerror("unexpected lexical token"); }

%%

int owfn_yyerror(char const *msg) 
{
    std::cerr << PACKAGE << ": " << owfn_yylineno
              << ": ERROR near '" << owfn_yytext 
              << "': " << msg << std::endl;
    exit(EXIT_FAILURE);
}

