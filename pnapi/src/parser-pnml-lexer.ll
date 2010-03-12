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

%option 8bit nodefault


 /***************************************************************************** 
  * C declarations 
  ****************************************************************************/
%{

#include "parser.h"
#include "parser-pnml.h"

#include <cstring>
#include <cstdio>
#include <ctype.h>
#include <cstring>
#include <cstdlib>

 // TODO: get rid of warnings; hacks ok

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

using namespace pnapi::parser::pnml;

static int keep;                        /* To store start condition */

static char * word(char * s)
{
  char * buf;
  int i, k;
  for(k = 0; (isspace(s[k]) || (s[k] == '<')); ++k);
  for(i = k; (s[i] && (!isspace(s[i]))); ++i);
  buf = (char*)malloc((i - k + 1) * sizeof(char));
  strncpy(buf, &s[k], i - k);
  buf[i - k] = '\0';
  return buf;
}


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
<INITIAL>"/"               { return XML_SLASH; }
<INITIAL>"="               { return XML_EQ; }
<INITIAL>">"               { BEGIN(CONTENT); return XML_CLOSE; }
<INITIAL>{name}            { pnapi_pnml_yylval.s = strdup(yytext); return XML_NAME; }
<INITIAL>{string}          { pnapi_pnml_yylval.s = strdup(yytext); return XML_VALUE; }

{open}{name}               { BEGIN(INITIAL); pnapi_pnml_yylval.s= word(yytext); return XML_START; }
{open}"/"                  { BEGIN(INITIAL); return XML_END; }

<CONTENT>{data}            { pnapi_pnml_yylval.s = strdup(yytext); return XML_DATA; }

.                          { yyerror("lexial error"); }
<<EOF>>                    { return EOF; }

