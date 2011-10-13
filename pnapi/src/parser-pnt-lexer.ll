 // -*- C++ -*-

 /***************************************************************************** 
  * flex options 
  ****************************************************************************/

/* create a c++ lexer */
%option c++

/* we provide out own class */
%option yyclass="pnapi::parser::pnt::yy::Lexer"

/* we need to prefix its base class */
%option prefix="Pnt"

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

#include "parser-pnt-wrapper.h"

#include <iostream>

/* tokens are defined in a struct in a class */
typedef pnapi::parser::pnt::yy::BisonParser::token tt;

%}


 /*****************************************************************************
  * regular expressions 
  ****************************************************************************/

 /* ignore everything after 3rd '@' */
%s READING
%s IGNORE

%%

 /* when ignoring */
<IGNORE>.               { /* skip */ }

 /* ignore table heads */
<INITIAL>[^\r\n]*       { /* skip */ }
<INITIAL>"\r\n"|[\r\n]  { BEGIN(READING); }

 /* keywords */
"oo"                            { return tt::KEY_INFTY; }

 /* other characters */
":"                             { return tt::COLON; }
","                             { return tt::COMMA; }
"@\r\n"|"@"[\r\n] {if(++atCounter_ >= 3){BEGIN(IGNORE);}else{BEGIN(INITIAL);}; return tt::AT;}

 /* linebreaks (UN*X: \n; MacOS <= 9: \r; Windows: \r\n) -.- */
"\r\n"|[\r\n]                   { return tt::CR; }

 /* identifiers */
[0-9]+                  { yylval->yt_int = atoi(yytext); return tt::NUMBER; }
[^ \t\r\n:,@]+         { yylval->yt_str = strdup(yytext); return tt::IDENT; }

 /* whitespace */
[ \t]                   { /* skip */ } 

 /* anything else */
.                                { LexerError("unexpected lexical token"); }

