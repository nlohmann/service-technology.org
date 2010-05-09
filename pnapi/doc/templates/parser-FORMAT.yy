// -*- C++ -*-

 /*****************************************************************************
  * bison options 
  ****************************************************************************/

/* generate a c++ parser */
%skeleton "lalr1.cc"

/* generate your code in your own namespace */
%define namespace "pnapi::parser::FORMAT::yy"

/* do not call the generated class "parser" */
%define parser_class_name "BisonParser"

/* generate needed location classes */
%locations

/* pass overlying parser to generated parser and yylex-wrapper */
%parse-param { Parser& parser_ }
%lex-param   { Parser& parser_ }

/* write tokens to parser-lola.h for use by scanner */
%defines

/* more verbose error reporting */
%error-verbose


 /*****************************************************************************
  * C declarations
  ****************************************************************************/

%code requires {
  /* forward declarations */
  namespace pnapi { namespace parser { namespace FORMAT {
    class Parser;
  } } } /* pnapi::parser::lola */
}

%{

#include "config.h"

#include "parser-FORMAT-wrapper.h"
#include "petrinet.h"

#include <sstream>

%}


 /*****************************************************************************
  * types, tokens, start symbol
  ****************************************************************************/

%token LCONTROL RCONTROL KEY_TRUE KEY_FALSE KEY_SAFE KEY_PLACE
%token KEY_TRANSITION KEY_MARKING KEY_CONSUME KEY_PRODUCE
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
  KEY_PLACE places SEMICOLON KEY_MARKING 
  marking_list SEMICOLON transitions
;

node_name:   
  IDENT  
  { 
    // clear stringstream
    parser_.nodeName_.str("");
    parser_.nodeName_.clear();

    parser_.nodeName_ << $1;
    free($1); 
  }
| NUMBER 
  { 
    // clear stringstream
    parser_.nodeName_.str("");
    parser_.nodeName_.clear();

    parser_.nodeName_ << $1; 
  }
;


 /* PLACE */

places: 
  capacity place_list                  { parser_.capacity_ = 0; }                
| places SEMICOLON capacity place_list { parser_.capacity_ = 0; }
;

capacity:
  /* empty */           
| KEY_SAFE COLON        { parser_.capacity_ = 1; }
| KEY_SAFE NUMBER COLON { parser_.capacity_ = $2; }
;

place_list:  
  node_name    
  {
    parser_.check(parser_.places_[parser_.nodeName_.str()] == NULL, "node name already used");
    parser_.places_[parser_.nodeName_.str()] = &(parser_.net_.createPlace(parser_.nodeName_.str(), 0, parser_.capacity_));
  }	      
| place_list COMMA node_name
  {
    parser_.check(parser_.places_[parser_.nodeName_.str()] == NULL, "node name already used");
    parser_.places_[parser_.nodeName_.str()] = &(parser_.net_.createPlace(parser_.nodeName_.str(), 0, parser_.capacity_));
  }
;



 /* MARKING */

marking_list:
  /* empty */                
| marking                    
| marking_list COMMA marking 
;

marking: 
  node_name COLON NUMBER 
  { 
    Place * p = parser_.places_[parser_.nodeName_.str()];
    parser_.check(p != NULL, "unknown place");      
      
    p->setTokenCount($3);
  }
;


 /* TRANSITION */

transitions: 
  transitions transition 
| transition             
;

transition: 
  KEY_TRANSITION node_name 
  { 
    parser_.check(!parser_.net_.containsNode(parser_.nodeName_.str()), "node name already used");
    parser_.transition_ = &(parser_.net_.createTransition(parser_.nodeName_.str())); 
  }
  KEY_CONSUME
  {
    parser_.target_ = reinterpret_cast<Node * *>(&(parser_.transition_));
    parser_.source_ = reinterpret_cast<Node * *>(&(parser_.place_)); 
  } 
  arcs SEMICOLON
  KEY_PRODUCE 
  { 
    parser_.source_ = reinterpret_cast<Node * *>(&(parser_.transition_));
    parser_.target_ = reinterpret_cast<Node * *>(&(parser_.place_)); 
  }
  arcs SEMICOLON 
;

arcs: 
  /* empty */    
| arc            
| arcs COMMA arc 
;

arc: 
  node_name COLON NUMBER 
  {
    parser_.place_ = parser_.places_[parser_.nodeName_.str()];
    parser_.check(parser_.place_ != NULL, "unknown place");

    Arc * a = parser_.net_.findArc(**(parser_.source_), **(parser_.target_));
    if(a != NULL)
    {
      a->setWeight(a->getWeight() + $3);
    }
    else
    {
      parser_.net_.createArc(**(parser_.source_), **(parser_.target_), $3);
    }
  }
;

