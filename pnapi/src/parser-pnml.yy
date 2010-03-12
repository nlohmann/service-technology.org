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
#include <cstring>
#include <sstream>
#include <map>
#include <vector>

#define pnapi_pnml_yyerror pnapi::parser::error
#define pnapi_pnml_yylex pnapi::parser::pnml::lex
#define yylex_destory pnapi::parser::pnml::lex_destroy
#define pnapi_pnml_yyparse pnapi::parser::pnml::parse

using namespace pnapi;
using namespace pnapi::parser::pnml;

%}


 /*****************************************************************************
  * types, tokens, start symbol
  ****************************************************************************/

%union
{
  char * s;
}

%token XML_ENDDEF XML_EQ XML_SLASH XML_CLOSE XML_END
%token <s> XML_NAME XML_VALUE XML_DATA XML_START
%type <s> name_opt


 /*****************************************************************************
  * grammar rules 
  ****************************************************************************/

%%

element:
  XML_START
  {
    open_element($1);
    free($1);
  }
  attribute_seq_opt empty_or_content
;

empty_or_content:
  XML_SLASH XML_CLOSE                            { close_element(); }
| XML_CLOSE content XML_END name_opt XML_CLOSE   { close_element(); }
;

content:
  /*empty*/
| content XML_DATA                               { store_data($2); }
| content element
;

name_opt:
  /*empty*/                                      { $$ = strdup(""); }
| XML_NAME                                       { $$ = $1; }
;

attribute_seq_opt:
  /*empty*/
| attribute_seq_opt attribute
;

attribute:
  XML_NAME                                       { free($1); }
| XML_NAME XML_EQ XML_VALUE
  {
    store_attributes($1, $3);
    free($1);
    free($3);
  }
;

