// -*- C++ -*-

 /*****************************************************************************
  * bison options 
  ****************************************************************************/

/* generate a c++ parser */
%skeleton "lalr1.cc"

/* generate your code in your own namespace */
%define namespace "pnapi::parser::woflan::yy"

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
  namespace pnapi { namespace parser { namespace woflan {
    class Parser;
  } } } /* pnapi::parser::woflan */
}

%{

#include "config.h"

#include "parser-woflan-wrapper.h"
#include "petrinet.h"

#include <string.h>
#include <stdlib.h>

#include <sstream>


char *substr(const char *pstr, int start, int numchars){

char *pnew = (char*) malloc(numchars+1);
strncpy(pnew, pstr + start, numchars);
pnew[numchars] = '\0';
return pnew;

}

%}


 /*****************************************************************************
  * types, tokens, start symbol
  ****************************************************************************/

%token LCONTROL RCONTROL KEY_TRUE KEY_FALSE TILDE
%token KEY_TRANSITION KEY_PLACE KEY_IN KEY_OUT KEY_INIT
%token COLON SEMICOLON COMMA NUMBER NEGATIVE_NUMBER IDENT


%union 
{
  int yt_int;
  char * yt_str;
}

%type <yt_int> NUMBER NEGATIVE_NUMBER
%type <yt_str> IDENT

%start net


 /*****************************************************************************
  * grammar rules 
  ****************************************************************************/

%%

net:
  node 
| net node  
;

node:
  place SEMICOLON
| transition label ins outs SEMICOLON
;

place:
  KEY_PLACE IDENT 
  {
    char* strName = substr($2, 1, strlen($2) -2);
    parser_.places_[strName] = &(parser_.net_.createPlace(strName, 0, 0));
    free($2);
  }
| KEY_PLACE IDENT KEY_INIT NUMBER
  {
    char* strName = substr($2, 1, strlen($2) -2);
    parser_.check(parser_.places_[strName] == NULL, "node name already used");
    parser_.places_[strName] = &(parser_.net_.createPlace(strName, 0, 0));
    Place * p = parser_.places_[strName];
    p->setTokenCount($4);
    free($2);
  }
;

transition:
  KEY_TRANSITION IDENT 
  {
    char* strName = substr($2, 1, strlen($2) -2);
    parser_.check(!(parser_.net_.containsNode(strName)), "node name already used");
    parser_.transition_ = &(parser_.net_.createTransition(strName)); 
    parser_.transName = strName;
    free($2);
  }
| KEY_TRANSITION { parser_.needLabel = true; }
;

label:
  /*empty*/ 
  {
    parser_.check(!(parser_.needLabel), "transition must be labeled");  	
  }
| TILDE IDENT
  {
    char* strName = substr($2, 1, strlen($2) -2);
    if(strName != parser_.transName)
    {
      parser_.check(!(parser_.net_.containsNode(strName)), "node name already used");
    }
    parser_.transition_->setName(strName); 
    free($2);
  }
;

ins:
  /*empty*/
| KEY_IN
  { 
    parser_.target_ = reinterpret_cast<Node * *>(&(parser_.transition_));
    parser_.source_ = reinterpret_cast<Node * *>(&(parser_.place_));   
  }
  arcs
;

outs:
  /*empty*/
| KEY_OUT
  { 
    parser_.source_ = reinterpret_cast<Node * *>(&(parser_.transition_));
    parser_.target_ = reinterpret_cast<Node * *>(&(parser_.place_)); 
  }
  arcs
;

arcs:
  arc
| arcs arc
;

arc:
  IDENT
  {
    char* strName = substr($1, 1, strlen($1) -2);
    parser_.place_ = parser_.places_[strName];
    parser_.check(parser_.place_ != NULL, "unknown place");

    Arc * a = parser_.net_.findArc(**(parser_.source_), **(parser_.target_));
    if(a != NULL)
    {
      a->setWeight(a->getWeight() + 1);
    }
    else
    {
      parser_.net_.createArc(**(parser_.source_), **(parser_.target_), 1);
    }
    free($1);
  }
;


