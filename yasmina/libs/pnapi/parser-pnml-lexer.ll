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
xmlheader ([^?]|"-"[^?])*
namestart		[A-Za-z\200-\377_]
namechar		[A-Za-z\200-\377_0-9.\-:]
ns        {namestart}({namechar}|[/:])*":"
text            [\040-\176]*

 /* start conditions of the lexer */
%s COMMENT
%s XMLHEADER
%s ATTRIBUTE


%%

 /* XML-elements */
"<"                      { return X_OPEN; }
"/"                      { return X_SLASH; }
">"                      { BEGIN(INITIAL); return X_CLOSE; }
">"[ \t\r\n]*"<"         { BEGIN(INITIAL); return X_NEXT; }

 /* comments */
"!--"                        { BEGIN(COMMENT); }
<COMMENT>{comment}           { /* skip */ }
<COMMENT>"-->"[ \t\r\n]*"<"  { BEGIN(INITIAL); }

"?"                          { BEGIN(XMLHEADER); }
<XMLHEADER>{xmlheader}       { /* skip */ }
<XMLHEADER>"?>"[ \t\r\n]*"<" { BEGIN(INITIAL); }


{ns}?"arc"                    { BEGIN(ATTRIBUTE); return KEY_ARC; }
{ns}?"finalmarkings"          { BEGIN(ATTRIBUTE); return KEY_FINALMARKINGS; }
{ns}?"initialMarking"         { BEGIN(ATTRIBUTE); return KEY_INITIALMARKING; }
{ns}?"input"                  { BEGIN(ATTRIBUTE); return KEY_INPUT; }
{ns}?"inscription"            { BEGIN(ATTRIBUTE); return KEY_INSCRIPTION; }
{ns}?"marking"                { BEGIN(ATTRIBUTE); return KEY_MARKING; }
{ns}?"module"                 { BEGIN(ATTRIBUTE); return KEY_MODULE; }
{ns}?"name"                   { BEGIN(ATTRIBUTE); return KEY_NAME; }
{ns}?"net"                    { BEGIN(ATTRIBUTE); return KEY_NET; }
{ns}?"output"                 { BEGIN(ATTRIBUTE); return KEY_OUTPUT; }
{ns}?"place"                  { BEGIN(ATTRIBUTE); return KEY_PLACE; }
{ns}?"pnml"                   { BEGIN(ATTRIBUTE); return KEY_PNML; }
{ns}?"port"                   { BEGIN(ATTRIBUTE); return KEY_PORT; }
{ns}?"ports"                  { BEGIN(ATTRIBUTE); return KEY_PORTS; }
{ns}?"receive"                { BEGIN(ATTRIBUTE); return KEY_RECEIVE; }
{ns}?"send"                   { BEGIN(ATTRIBUTE); return KEY_SEND; }
{ns}?"synchronize"            { BEGIN(ATTRIBUTE); return KEY_SYNCHRONIZE; }
{ns}?"synchronous"            { BEGIN(ATTRIBUTE); return KEY_SYNCHRONOUS; }
{ns}?"text"                   { BEGIN(ATTRIBUTE); return KEY_TEXT; }
{ns}?"transition"             { BEGIN(ATTRIBUTE); return KEY_TRANSITION; }
{ns}?"value"                  { BEGIN(ATTRIBUTE); return KEY_VALUE; }


 /* attributes */
<ATTRIBUTE>"="           { return X_EQUALS; }
<ATTRIBUTE>{string}      { pnapi_pnml_yylval.yt_str = strdup(yytext);
                           // strip the quotes
                           pnapi_pnml_yylval.yt_str[strlen(yytext)-1] = 0;
                           pnapi_pnml_yylval.yt_str++;
                           return X_STRING; }

[a-zA-Z0-9]+"."[0-9]+ { pnapi_pnml_yylval.yt_str = strdup(yytext); return IDENT; }
[a-zA-Z0-9]+".["[^\]]*"]" { pnapi_pnml_yylval.yt_str = strdup(yytext); return IDENT; }

[a-zA-Z0-9.]+             { pnapi_pnml_yylval.yt_str = strdup(yytext); return IDENT; }


 /* whitespace */
[ \n\r\t]                { /* skip */ }

 /* anything else */
.                        { yyerror("unexpected lexical token"); }

%%
