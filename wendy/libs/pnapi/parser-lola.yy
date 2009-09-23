// -*- C++ -*-

 /*****************************************************************************
  * bison options 
  ****************************************************************************/

/* plain c parser: the prefix is our "namespace" */
%name-prefix="pnapi_lola_yy"

/* write tokens to parser-lola.h for use by scanner */
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
#define yylex   pnapi::parser::lola::lex
#define yyparse pnapi::parser::lola::parse


using namespace pnapi;
using namespace pnapi::parser::lola;

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
}

%type <yt_int> NUMBER NEGATIVE_NUMBER

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
      nodeName_.str("");
      nodeName_.clear();

      nodeName_ << ident; 
    }
  | NUMBER 
    { 
      // clear stringstream
      nodeName_.str("");
      nodeName_.clear();

      nodeName_ << $1; 
    }
  ;


 /* PLACE */

places: 
    capacity place_list                  { capacity_ = 0; }                
  | places SEMICOLON capacity place_list { capacity_ = 0; }
  ;

capacity:
    /* empty */           
  | KEY_SAFE COLON        { capacity_ = 1; }
  | KEY_SAFE NUMBER COLON { capacity_ = $2; }
  ;

place_list:  
    node_name    
    {
      check(places_[nodeName_.str()] == 0, "node name already used");
      places_[nodeName_.str()] = & pnapi_lola_yynet.createPlace(nodeName_.str(), Node::INTERNAL, 0, capacity_);
    }	      
  | place_list COMMA node_name
    {
      check(places_[nodeName_.str()] == 0, "node name already used");
      places_[nodeName_.str()] = & pnapi_lola_yynet.createPlace(nodeName_.str(), Node::INTERNAL, 0, capacity_);
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
      Place* p = places_[nodeName_.str()];
      check(p != 0, "unknown place");      
        
      p->mark($3);
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
      check(!pnapi_lola_yynet.containsNode(nodeName_.str()), "node name already used");
      transition_ = & pnapi_lola_yynet.createTransition(nodeName_.str()); 
    }
    KEY_CONSUME 
    { 
      target_ = (Node**)(&transition_); source_ = (Node**)(&place_); 
      placeSetType_ = true;
    } 
    arcs SEMICOLON
    KEY_PRODUCE 
    { 
      source_ = (Node**)(&transition_); target_ = (Node**)(&place_); 
      placeSetType_ = false;
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
      place_ = places_[nodeName_.str()];
      check(place_ != 0, "unknown place");

      Arc * a = pnapi_lola_yynet.findArc(**source_, **target_);
      if(a != NULL)
      {
        a->setWeight(a->getWeight() + $3);
      }
      else
      {
        check(!(placeSetType_ && (place_->getType() == Place::OUTPUT)), "output places are not allowed in preset");
        check(!(!placeSetType_ && (place_->getType() == Place::INPUT)), "input places are not allowed in postset");
      
        pnapi_lola_yynet.createArc(**source_, **target_, $3);
      }
    }
  ;
