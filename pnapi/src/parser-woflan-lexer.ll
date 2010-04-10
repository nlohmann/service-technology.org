 // -*- C++ -*-

 /***************************************************************************** 
  * flex options 
  ****************************************************************************/

/* create a c++ lexer */
%option c++

/* we provide out own class */
%option yyclass="pnapi::parser::woflan::yy::Lexer"

/* we need to prefix its base class */
%option prefix="Woflan"

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

#include "parser-woflan-wrapper.h"

#include <iostream>

/* tokens are defined in a struct in a class */
typedef pnapi::parser::woflan::yy::BisonParser::token tt;

%}


 /*****************************************************************************
  * regular expressions 
  ****************************************************************************/

%%

 /* comments */

--.*$                            { /* skip */ }


 /* keywords */
place                           { return tt::KEY_PLACE; }
trans	                        { return tt::KEY_TRANSITION; }
in 				{ return tt::KEY_IN; }
out 				{ return tt::KEY_OUT; }
init				{ return tt::KEY_INIT; }

 /* other characters */
\:                              { return tt::COLON; }
\;                              { return tt::SEMICOLON; }
,                               { return tt::COMMA; }
~				{ return tt::TILDE; }

 /* identifiers */
[0-9]+          { yylval->yt_int = atoi(yytext); return tt::NUMBER; }
"\""[^\"]+"\""          { yylval->yt_str = strdup(yytext); return tt::IDENT; }

 /* whitespace */
[ \n\r\t]                        { /* skip */ } 

 /* anything else */
.                                { LexerError("unexpected lexical token"); }
