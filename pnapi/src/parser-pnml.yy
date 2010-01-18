 // -*- C++ -*-

 /*****************************************************************************
  * bison options 
  ****************************************************************************/

/* plain c parser: the prefix is our "namespace" */
%name-prefix="pnapi_pnml_yy"

/* write tokens to parser-pnml.h for use by scanner */
%defines

/* more verbose error reporting */
%error-verbose


 /*****************************************************************************
  * C declarations
  ****************************************************************************/
%{

#include "parser.h"
#include <string>
#include <sstream>

#undef yylex
#undef yyparse
#undef yyerror

#define yyerror pnapi::parser::error
#define yylex pnapi::parser::pnml::lex
#define yylex_destory pnapi::parser::pnml::lex_destroy
#define yyparse pnapi::parser::pnml::parse


using namespace pnapi;
using namespace pnapi::parser::pnml;

%}


 /*****************************************************************************
  * types, tokens, start symbol
  ****************************************************************************/

%token X_OPEN X_SLASH X_CLOSE X_NEXT X_STRING X_EQUALS
%token NUMBER IDENT
%token KEY_PNML KEY_MODULE KEY_PORT KEY_PORTS KEY_NET KEY_TEXT KEY_PLACE
%token KEY_INITIALMARKING KEY_TRANSITION KEY_ARC KEY_FINALMARKINGS KEY_MARKING

%union 
{
  int yt_int;
  char * yt_str;
}

%type <yt_int> NUMBER 
%type <yt_str> IDENT

%start petrinet


 /*****************************************************************************
  * grammar rules 
  ****************************************************************************/
%%

petrinet:
  X_OPEN KEY_PNML X_NEXT module X_SLASH KEY_PNML X_CLOSE
;

module:
  KEY_MODULE X_NEXT ports net finalmarkings X_SLASH KEY_MODULE X_NEXT
;

ports:
  KEY_PORTS X_NEXT X_SLASH KEY_PORTS X_NEXT
;

net:
  KEY_NET arbitraryAttributes X_NEXT nodes X_SLASH KEY_NET X_NEXT
;

nodes:
  /* empty */
| nodes node
;

node:
  place
| transition
| arc
;

place:
  KEY_PLACE arbitraryAttributes X_NEXT initialmarking X_SLASH KEY_PLACE X_NEXT
| KEY_PLACE arbitraryAttributes X_SLASH X_NEXT
;

initialmarking:
  /* empty */
| KEY_INITIALMARKING arbitraryAttributes X_NEXT text X_SLASH KEY_INITIALMARKING X_NEXT
;

text:
  KEY_TEXT arbitraryAttributes X_CLOSE NUMBER X_OPEN X_SLASH KEY_TEXT X_NEXT
;

transition:
  KEY_TRANSITION arbitraryAttributes X_SLASH X_NEXT
;

arc:
  KEY_ARC arbitraryAttributes X_SLASH X_NEXT
;

finalmarkings:
  KEY_FINALMARKINGS arbitraryAttributes X_NEXT markings X_SLASH KEY_FINALMARKINGS X_NEXT
;

markings:
  /* empty */
| markings marking
;

marking:
  KEY_MARKING arbitraryAttributes X_NEXT mappings X_SLASH KEY_MARKING X_NEXT
;

mappings:
  /* empty */
| mappings mapping
;

mapping:
  KEY_PLACE arbitraryAttributes X_NEXT text X_SLASH KEY_PLACE X_NEXT
;

arbitraryAttributes:
  /* empty */
| IDENT X_EQUALS X_STRING arbitraryAttributes
;

