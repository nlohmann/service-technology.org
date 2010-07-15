 // -*- C++ -*-

 /***************************************************************************** 
  * flex options 
  ****************************************************************************/

/* create a c++ lexer */
%option c++

/* we provide out own class */
%option yyclass="pnapi::parser::FORMAT::yy::Lexer"

/* we need to prefix its base class */
%option prefix="FORMAT"

/* created lexer should be called "lex.yy.c" to make the ylwrap script work */
%option outfile="lex.yy.c"

/* we read only one file */
%option noyywrap

/* yyunput not needed (fix compiler warning) */
%option nounput

/* maintain line number for error reporting */
%option yylineno

/* get rid of isatty */
%option never-interactive

 /***************************************************************************** 
  * C declarations 
  ****************************************************************************/
%{

#include "parser-FORMAT-wrapper.h"

#include <iostream>

/* tokens are defined in a struct in a class */
typedef pnapi::parser::FORMAT::yy::BisonParser::token tt;

%}


 /*****************************************************************************
  * regular expressions 
  ****************************************************************************/

 /* a start condition to skip comments */
%s COMMENT

%%

 /* control comments */ 
"{$"                            { return tt::LCONTROL; }
"$}"                            { return tt::RCONTROL; }

 /* comments */
"{"                             { BEGIN(COMMENT); }
<COMMENT>"}"                    { BEGIN(INITIAL); }
<COMMENT>[^}]*                  { /* skip */ }

 /* control keywords */
TRUE                            { return tt::KEY_TRUE; }
FALSE                           { return tt::KEY_FALSE; }

 /* keywords */
SAFE                            { return tt::KEY_SAFE; }
PLACE                           { return tt::KEY_PLACE; }
TRANSITION                      { return tt::KEY_TRANSITION; }
MARKING                         { return tt::KEY_MARKING; }
CONSUME                         { return tt::KEY_CONSUME; }
PRODUCE                         { return tt::KEY_PRODUCE; }

 /* other characters */
\:                              { return tt::COLON; }
\;                              { return tt::SEMICOLON; }
,                               { return tt::COMMA; }

 /* identifiers */
[0-9]+          { yylval->yt_int = atoi(yytext); return tt::NUMBER; }
"-"[0-9]+       { yylval->yt_int = atoi(yytext); return tt::NEGATIVE_NUMBER; }
[^,;:()\t \n\r\{\}]+ { yylval->yt_str = strdup(yytext); return tt::IDENT; }

 /* whitespace */
[ \n\r\t]                          { /* skip */ }

 /* anything else */
.                               { LexerError("unexpected lexical token"); }

