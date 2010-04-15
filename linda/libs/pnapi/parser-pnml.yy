 // -*- C++ -*-

 /*****************************************************************************
  * bison options 
  ****************************************************************************/

/* generate a c++ parser */
%skeleton "lalr1.cc"

/* generate your code in your own namespace */
%define namespace "pnapi::parser::pnml::yy"

/* do not call the generated class "parser" */
%define parser_class_name "BisonParser"

/* generate needed location classes */
%locations

/* pass overlying parser to generated parser and yylex-wrapper */
%parse-param { Parser& parser_ }
%lex-param   { Parser& parser_ }

/* write tokens to parser-pnml.h for use by scanner */
%defines

/* more verbose error reporting */
%error-verbose


 /*****************************************************************************
  * C declarations
  ****************************************************************************/

%code requires {
  /* forward declarations */
  namespace pnapi { namespace parser { namespace pnml {
    class Parser;
  } } } /* pnapi::parser::pnml */
}

%{

#include "config.h"

#include "parser-pnml-wrapper.h"

#include <cstring>

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
    parser_.open_element($1);
    free($1);
  }
  attribute_seq_opt empty_or_content
;

empty_or_content:
  XML_SLASH XML_CLOSE                            { parser_.close_element(); }
| XML_CLOSE content XML_END name_opt XML_CLOSE   { parser_.close_element(); }
;

content:
  /*empty*/
| content XML_DATA                               { parser_.store_data($2); }
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
    parser_.store_attributes($1, $3);
    free($1);
    free($3);
  }
;

