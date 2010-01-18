 // -*- C++ -*-

 /***************************************************************************** 
  * flex options 
  ****************************************************************************/

/* created lexer should be called "lex.yy.c" to make the ylwrap script work */
%option outfile="lex.yy.c"

/* plain c scanner: the prefix is our "namespace" */
%option prefix="pnapi_pnml_yy"

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
#include "parser-pnml.h"

#include <cstring>

#define yystream pnapi::parser::stream
#define yylineno pnapi::parser::line
#define yytext   pnapi::parser::token
#define yyerror  pnapi::parser::error

#define yylex    pnapi::parser::pnml::lex
#define yylex_destroy pnapi::parser::pnml::lex_destroy

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

esc       "&#"[0-9]+";"|"&#x"[0-9a-fA-F]+";"
quote     \"
string    {quote}([^"]|{esc})*{quote}
comment   ([^-]|"-"[^-])*

 /* start conditions of the lexer */
%s COMMENT


%%

 /* XML-elements */
"<"                      { return X_OPEN; }
"/"                      { return X_SLASH; }
">"                      { return X_CLOSE; }
">"[ \t\r\n]*"<"         { return X_NEXT; }

 /* comments */
"!--"                       { BEGIN(COMMENT); }
<COMMENT>{comment}          { /* skip */ }
<COMMENT>"-->"[ \t\r\n]*"<" { BEGIN(INITIAL); }


"pnml"                   { return KEY_PNML; }
"text"                   { return KEY_TEXT; }
"place"                  { return KEY_PLACE; }
"initialMarking"         { return KEY_INITIALMARKING; }
"finalmarkings"          { return KEY_FINALMARKINGS; }
"marking"                { return KEY_MARKING; }
"transition"             { return KEY_TRANSITION; }
"arc"                    { return KEY_ARC; }
"module"                 { return KEY_MODULE; }
"port"                   { return KEY_PORT; }
"ports"                  { return KEY_PORTS; }
"net"                    { return KEY_NET; }

"="                      { return X_EQUALS; }

 /* identifiers */
[0-9]+                   { pnapi_pnml_yylval.yt_int = atoi(yytext); return NUMBER; }
[a-zA-Z0-9]+             { pnapi_pnml_yylval.yt_str = strdup(yytext); return IDENT; }
{string}                 { return X_STRING; }


 /* whitespace */
[ \n\r\t]                { /* skip */ }

 /* anything else */
.                        { yyerror("unexpected lexical token"); }

%%
