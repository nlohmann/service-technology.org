 // -*- C++ -*-

 /***************************************************************************** 
  * flex options 
  ****************************************************************************/

/* created lexer should be called "lex.yy.c" to make the ylwrap script work */
%option outfile="lex.yy.c"

/* plain c scanner: the prefix is our "namespace" */
%option prefix="pnapi_woflan_yy"

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
#include "parser-woflan.h"

#include <iostream>

#define yystream pnapi::parser::stream
#define yylineno pnapi::parser::line
#define yytext   pnapi::parser::token
#define yyerror  pnapi::parser::error

#define yylex    pnapi::parser::woflan::lex
#define yylex_destroy pnapi::parser::woflan::lex_destroy

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

%%

 /* comments */

--.*$                            { /* skip */ }


 /* keywords */
place                           { return KEY_PLACE; }
trans	                        { return KEY_TRANSITION; }
in 				{ return KEY_IN; }
out 				{ return KEY_OUT; }
init				{ return KEY_INIT; }

 /* other characters */
\:                              { return COLON; }
\;                              { return SEMICOLON; }
,                               { return COMMA; }
~				{ return TILDE; }

 /* identifiers */
[0-9]+          { pnapi_woflan_yylval.yt_int = atoi(yytext); return NUMBER; }
[^,;:()\t \n\r\{\}]+ { pnapi_woflan_yylval.yt_str = strdup(yytext); return IDENT; }

 /* whitespace */
[ \n\r\t]                        { /* skip */ } 

 /* anything else */
.                                { yyerror("unexpected lexical token"); }
