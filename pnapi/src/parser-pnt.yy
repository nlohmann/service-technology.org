// -*- C++ -*-

 /*****************************************************************************
  * bison options 
  ****************************************************************************/

/* generate a c++ parser */
%skeleton "lalr1.cc"

/* generate your code in your own namespace */
%define namespace "pnapi::parser::pnt::yy"

/* do not call the generated class "parser" */
%define parser_class_name "BisonParser"

/* generate needed location classes */
%locations

/* pass overlying parser to generated parser and yylex-wrapper */
%parse-param { Parser& parser_ }
%lex-param   { Parser& parser_ }

/* write tokens to parser-woflan.h for use by scanner */
%defines

/* more verbose error reporting */
%error-verbose


 /*****************************************************************************
  * C declarations
  ****************************************************************************/

%code requires {
  /* forward declarations */
  namespace pnapi { namespace parser { namespace pnt {
    class Parser;
  } } } /* pnapi::parser::pnt */
}

%{

#include "config.h"

#include "parser-pnt-wrapper.h"
#include "petrinet.h"

#include <sstream>
#include <iostream> // TODO: remove me!

%}


 /*****************************************************************************
  * types, tokens, start symbol
  ****************************************************************************/

%token AT CR HEADER COLON COMMA PLACEHEADER TRANSHEADER KEY_INFTY
%token NUMBER IDENT

%union 
{
  int yt_int;
  char * yt_str;
}

%type <yt_int> NUMBER capacity
%type <yt_str> IDENT

%start net


 /*****************************************************************************
  * grammar rules 
  ****************************************************************************/

%%

net:
  netstruct AT placedata AT
  { parser_.marking_.clear(); }
  transdata AT
  {
    parser_.completeNet();
  }
;

netstruct:
  placedef
| netstruct placedef
;

placedef:
  NUMBER NUMBER
  {
    // mark place
    parser_.marking_[parser_.currentPlace_ = $1] = $2;
    parser_.currentArcList_ = &parser_.preset_;
  }
  prelist postlist CR
;

prelist:
  /* empty */
| prelist NUMBER
  {
    parser_.arc_ = (int *) malloc(3 * sizeof(int));
    parser_.arc_[0] = parser_.currentPlace_;
    parser_.arc_[1] = $2;
    parser_.arc_[2] = 1;
    parser_.currentArcList_->push_back(parser_.arc_);
  }
| prelist NUMBER COLON NUMBER
  {
    parser_.arc_ = (int *) malloc(3 * sizeof(int));
    parser_.arc_[0] = parser_.currentPlace_;
    parser_.arc_[1] = $2;
    parser_.arc_[2] = $4;
    parser_.currentArcList_->push_back(parser_.arc_);
  }
;

postlist:
  /* empty */
| COMMA { parser_.currentArcList_ = &parser_.postset_; } prelist
;

placedata:
  /* empty */
| placedata NUMBER COLON IDENT capacity NUMBER CR
  {
    try
    {
      parser_.places_[$2] = &parser_.net_.createPlace(std::string($4), parser_.marking_[$2], $5);
    }
    catch (exception::Error & e)
    {
      // do not leave memory leaks
      free($4);
      parser_.rethrow(e);
    }

    free($4);
  }
;

capacity:
  KEY_INFTY { $$ = 0; }
| NUMBER { $$ = $1; }
;

transdata:
  /* empty */
| transdata NUMBER COLON IDENT NUMBER NUMBER CR
  {
    try
    {
      parser_.transitions_[$2] = &parser_.net_.createTransition(std::string($4));
    }
    catch (exception::Error & e)
    {
      // do not leave memory leaks
      free($4);
      parser_.rethrow(e);
    }

    free($4);
  }
;

