 // -*- C++ -*-

 /***************************************************************************** 
  * flex options 
  ****************************************************************************/

/* created lexer should be called "lex.yy.c" to make the ylwrap script work */
%option outfile="lex.yy.c"

/* plain c scanner: the prefix is our "namespace" */
%option prefix="pnapi_onwd_"

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
#include "parser-onwd.h"

#define yystream pnapi::parser::stream
#define yylineno pnapi::parser::line
#define yytext   pnapi::parser::token
#define yyerror  pnapi::parser::error

#define yylex    pnapi::parser::onwd::lex

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

%s COMMENT

%%

"{"            { BEGIN(COMMENT); }
<COMMENT>"}"   { BEGIN(INITIAL); }
<COMMENT>[^}]* {}

INSTANCE       { return KEY_INSTANCE; }
WIRING         { return KEY_WIRING; }
\;             { return SYM_SEMICOLON; }
\:             { return SYM_COLON; }
\,             { return SYM_COMMA; }
\.             { return SYM_DOT; }
\-\>           { return SYM_SINGLE_ARR; }
\=\>           { return SYM_DOUBLE_ARR; }

[a-z][a-zA-Z0-9_]* { 
  pnapi_onwd_lval.yt_string = new std::string(yytext);
  return CL_IDENTIFIER; 
}

\"[^\"]*\" { 
  yytext[strlen(yytext)-1] = '\0';
  pnapi_onwd_lval.yt_string = new std::string(&yytext[1]);
  return CL_STRING; 
}

[\n\r] { break; }
[ \t]  { break; }

 /* anything else */
.                               { yyerror("unexpected lexical token"); }
