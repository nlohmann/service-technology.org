// -*- C++ -*-

 /*****************************************************************************
  * bison options 
  ****************************************************************************/

/* plain c parser: the prefix is our "namespace" */
%name-prefix="pnapi_woflan_yy"

/* write tokens to parser-woflan.h for use by scanner */
%defines

/* more verbose error reporting */
%error-verbose


 /*****************************************************************************
  * C declarations
  ****************************************************************************/
%{

#include "config.h"

#include "parser.h"
#include "petrinet.h"

#include <sstream>

#define pnapi_woflan_yyerror pnapi::parser::error
#define pnapi_woflan_yylex   pnapi::parser::woflan::lex
#define yylex_destroy pnapi::parser::woflan::lex_destroy
#define pnapi_woflan_yyparse pnapi::parser::woflan::parse


using namespace pnapi;
using namespace pnapi::parser::woflan;

bool needLabel = false;
std::string transName;

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
    check(places_[$2] == NULL, "node name already used");
    places_[$2] = &pnapi_woflan_yynet.createPlace($2, 0, 0);
  }
| KEY_PLACE IDENT KEY_INIT NUMBER
  {
    check(places_[$2] == NULL, "node name already used");
    places_[$2] = &pnapi_woflan_yynet.createPlace($2, 0, 0);
    Place * p = places_[$2];
    p->setTokenCount($4);
  }
;

transition:
  KEY_TRANSITION IDENT 
  {
    check(!pnapi_woflan_yynet.containsNode($2), "node name already used");
    transition_ = &pnapi_woflan_yynet.createTransition($2); 
    transName = $2;
  }
| KEY_TRANSITION { needLabel = true; }
;

label:
  /*empty*/ 
  {
    check(!needLabel, "transition must be labeled");  	
  }
| TILDE IDENT
  {
    if($2 != transName)
      check(!pnapi_woflan_yynet.containsNode($2), "node name already used");
    transition_->setName($2); 
  }
;

ins:
  /*empty*/
| KEY_IN
  { 
    target_ = reinterpret_cast<Node * *>(&transition_);
    source_ = reinterpret_cast<Node * *>(&place_);   
  }
  arcs
;

outs:
  /*empty*/
| KEY_OUT
  { 
    source_ = reinterpret_cast<Node * *>(&transition_);
    target_ = reinterpret_cast<Node * *>(&place_); 
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
    place_ = places_[$1];
    check(place_ != NULL, "unknown place");

    Arc * a = pnapi_woflan_yynet.findArc(**source_, **target_);
    if(a != NULL)
    {
      a->setWeight(a->getWeight() + 1);
    }
    else
    {
      pnapi_woflan_yynet.createArc(**source_, **target_, 1);
    }
  }
;





