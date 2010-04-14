 // -*- C++ -*-

 /***************************************************************************** 
  * flex options 
  ****************************************************************************/

/* create a c++ lexer */
%option c++

/* we provide out own class */
%option yyclass="pnapi::parser::pnml::yy::Lexer"

/* we need to prefix its base class */
%option prefix="Pnml"

/* created lexer should be called "lex.yy.c" to make the ylwrap script work */
%option outfile="lex.yy.c"

/* we read only one file */
%option noyywrap

/* yyunput not needed (fix compiler warning) */
%option nounput

/* maintain line number for error reporting */
%option yylineno

%option 8bit nodefault

/* get rid of isatty */
%option never-interactive

 /***************************************************************************** 
  * C declarations 
  ****************************************************************************/
%{

#include "config.h"

#include "parser-pnml-wrapper.h"

#include <iostream>

/* tokens are defined in a struct in a class */
typedef pnapi::parser::pnml::yy::BisonParser::token tt;

%}


ws              [ \t\r\n]*
open            "<"
namestart       [A-Za-z\200-\377_]
namechar        [A-Za-z\200-\377_0-9.-]
name            {namestart}{namechar}*
esc             "&#"[0-9]+";"|"&#x"[0-9a-fA-F]+";"
data            ([^<\n&]|\n[^<&]|\n{esc}|{esc})+
comment         {ws}*{open}"!--"([^-]|"-"[^-]|"--"[^>])*"-->"
string          \"([^"&]|{esc})*\"|\'([^'&]|{esc})*\'

/*
 * The CONTENT mode is used for the content of elements, i.e.,
 * between the ">" and "<" of element tags.
 * The INITIAL mode is used outside the top level element
 * and inside markup.
 */

%s CONTENT


%%

{comment}                  { /* skip */ }
<INITIAL>"<?"{data}*"?>"   { /* skip */ }

{ws}                       { /* skip */ }
<INITIAL>"/"               { return tt::XML_SLASH; }
<INITIAL>"="               { return tt::XML_EQ; }
<INITIAL>">"               { BEGIN(CONTENT); return tt::XML_CLOSE; }
<INITIAL>{name}            { yylval->s = strdup(yytext); return tt::XML_NAME; }
<INITIAL>{string}          { yylval->s = strdup(yytext); return tt::XML_VALUE; }

{open}{name}               { BEGIN(INITIAL); yylval->s = word(yytext); return tt::XML_START; }
{open}"/"                  { BEGIN(INITIAL); return tt::XML_END; }

<CONTENT>{data}            { yylval->s = strdup(yytext); return tt::XML_DATA; }
 /* the next line was added to parse ProM's PNML output */
<CONTENT>.                 { yylval->s = strdup(yytext); return tt::XML_DATA; }

.                          { LexerError("lexial error"); }
<<EOF>>                    { return EOF; }

