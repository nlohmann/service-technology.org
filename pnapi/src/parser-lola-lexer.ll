 // -*- C++ -*-

 /***************************************************************************** 
  * flex options 
  ****************************************************************************/

/* created lexer should be called "lex.yy.c" to make the ylwrap script work */
%option outfile="lex.yy.c"

/* plain c scanner: the prefix is our "namespace" */
%option prefix="pnapi_lola_yy"

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

#include "parser.h"
#include "parser-lola.h"

#include <cstring>

 // TODO: fix redefinition warnings; "hacks" can remain
#define yystream pnapi::parser::stream
#define yylineno pnapi::parser::line
#define yytext   pnapi::parser::token
#define yyerror  pnapi::parser::error

#define yylex    pnapi::parser::lola::lex
#define yylex_destroy pnapi::parser::lola::lex_destroy

/* hack to read input from a C++ stream */
#define YY_INPUT(buf,result,max_size)		\
   yystream->read(buf, max_size); \
   if (yystream->bad()) \
     YY_FATAL_ERROR("input in flex scanner failed"); \
   result = yystream->gcount();

/* hack to overwrite YY_FATAL_ERROR behavior */
#define fprintf(file,fmt,msg) \
   yyerror(msg);

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
TRUE                            { return KEY_TRUE; }
FALSE                           { return KEY_FALSE; }

 /* keywords */
SAFE                            { return KEY_SAFE; }
PLACE                           { return KEY_PLACE; }
TRANSITION                      { return KEY_TRANSITION; }
MARKING                         { return KEY_MARKING; }
CONSUME                         { return KEY_CONSUME; }
PRODUCE                         { return KEY_PRODUCE; }

 /* other characters */
\:                              { return COLON; }
\;                              { return SEMICOLON; }
,                               { return COMMA; }

 /* identifiers */
[0-9]+          { pnapi_lola_yylval.yt_int = atoi(yytext); return NUMBER; }
"-"[0-9]+       { pnapi_lola_yylval.yt_int = atoi(yytext); return NEGATIVE_NUMBER; }
[^,;:()\t \n\r\{\}]+ { pnapi_lola_yylval.yt_str = strdup(yytext); return IDENT; }

 /* whitespace */
[ \n\r\t]                          { /* skip */ }

 /* anything else */
.                               { yyerror("unexpected lexical token"); }
