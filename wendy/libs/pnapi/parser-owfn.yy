 // -*- C++ -*-

 /*****************************************************************************
  * bison options 
  ****************************************************************************/

/* plain c parser: the prefix is our "namespace" */
%name-prefix="pnapi_owfn_yy"

/* write tokens to parser-owfn.h for use by scanner */
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
#define yylex pnapi::parser::owfn::lex
#define yyparse pnapi::parser::owfn::parse


using namespace pnapi;
using namespace pnapi::parser::owfn;

%}


 /*****************************************************************************
  * types, tokens, start symbol
  ****************************************************************************/

%token KEY_SAFE KEY_INTERFACE KEY_PLACE KEY_INTERNAL KEY_INPUT KEY_OUTPUT
%token KEY_SYNCHRONIZE KEY_SYNCHRONOUS KEY_CONSTRAIN
%token KEY_INITIALMARKING KEY_FINALMARKING KEY_NOFINALMARKING KEY_FINALCONDITION
%token KEY_TRANSITION KEY_CONSUME KEY_PRODUCE KEY_PORT KEY_PORTS
%token KEY_ALL_PLACES_EMPTY KEY_COST
%token KEY_ALL_OTHER_PLACES_EMPTY
%token KEY_ALL_OTHER_INTERNAL_PLACES_EMPTY
%token KEY_ALL_OTHER_EXTERNAL_PLACES_EMPTY
%token KEY_MAX_UNIQUE_EVENTS KEY_ON_LOOP KEY_MAX_OCCURRENCES
%token KEY_TRUE KEY_FALSE LCONTROL RCONTROL
%token COMMA COLON SEMICOLON IDENT NUMBER NEGATIVE_NUMBER
%token LPAR RPAR

%left     OP_OR OP_AND OP_NOT
%nonassoc OP_EQ OP_NE OP_GT OP_LT OP_GE OP_LE

%union 
{
  int yt_int;
  pnapi::formula::Formula * yt_formula;
}

%type <yt_int> NUMBER NEGATIVE_NUMBER 
%type <yt_int> transition_cost
%type <yt_formula> condition formula

%start petrinet


 /*****************************************************************************
  * grammar rules 
  ****************************************************************************/
%%

petrinet: 
    { port_ = ""; }
    places_ports markings transitions
    { pnapi_owfn_yynet.setConstraintLabels(constrains_); }
  ;



 /**************/
 /*** PLACES ***/
 /**************/

places_ports: 
    KEY_INTERFACE interface KEY_PLACE 
    {
     placeType_ = Node::INTERNAL;
     port_ = "";
    } 
    places SEMICOLON 
  | KEY_PLACE {placeType_ = Node::INTERNAL;} typed_places ports
  ;

typed_places: 
    internal_places input_places output_places synchronous 
  | places SEMICOLON
  ;

input_places:
    /* empty */
  | KEY_INPUT {placeType_ = Node::INPUT;} places SEMICOLON                
  ;

output_places:
    /* empty */ 
  | KEY_OUTPUT {placeType_ = Node::OUTPUT;} places SEMICOLON                 
  ;

internal_places:
    /* empty */
  | KEY_INTERNAL {placeType_ = Node::INTERNAL;} places SEMICOLON                   
  ;

places: 
    capacity place_list { capacity_ = 0; }
  | places SEMICOLON capacity place_list { capacity_ = 0; }
  ;

capacity:
    /* empty */           
  | KEY_SAFE COLON        {capacity_ = 1; }
  | KEY_SAFE NUMBER COLON {capacity_ = $2;}
  ;

place_list:  
    /* empty */           
  | node_name 
    {
     check(places_[nodeName_.str()] == 0, "node name already used");
     place_ = & pnapi_owfn_yynet.createPlace(nodeName_.str(), placeType_, 0, capacity_, port_);
     places_[nodeName_.str()] = place_;
    } 
    controlcommands
  | place_list COMMA node_name
    {
     check(places_[nodeName_.str()] == 0, "node name already used");
     place_ = & pnapi_owfn_yynet.createPlace(nodeName_.str(), placeType_, 0, capacity_, port_);
     places_[nodeName_.str()] = place_;
    } 
    controlcommands
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


controlcommands:
    /* emtpy */
  | LCONTROL commands RCONTROL
  ;

commands:
    /* empty */
  | KEY_MAX_UNIQUE_EVENTS OP_EQ NUMBER commands
  | KEY_ON_LOOP OP_EQ KEY_TRUE commands
  | KEY_ON_LOOP OP_EQ KEY_FALSE commands
  | KEY_MAX_OCCURRENCES OP_EQ NUMBER 
    { place_->setMaxOccurrence($3); }
    commands
  | KEY_MAX_OCCURRENCES OP_EQ NEGATIVE_NUMBER 
    { place_->setMaxOccurrence($3); }
    commands
  ;



ports:
    /* empty */         
  | KEY_PORTS port_list 
  ;

port_list:
    node_name COLON { port_ = nodeName_.str(); } port_participants SEMICOLON
  | port_list node_name COLON { port_ = nodeName_.str(); } port_participants SEMICOLON 
  ;

port_participants: 
    node_name 
    {
      Place* p = places_[nodeName_.str()];
      check(p != 0, "unknown place");
      check(p->getType() != Place::INTERNAL, "interface place expectd");
      check(p->getPort().empty(), "place already assigned to port '" + p->getPort() +"'");
       
      p->setPort(port_);
    } 
  | port_participants COMMA node_name
    {
      Place* p = places_[nodeName_.str()];
      check(p != 0, "unknown place");
      check(p->getType() != Place::INTERNAL, "interface place expectd");
      check(p->getPort().empty(), "place already assigned to port '" + p->getPort() +"'");

      p->setPort(port_);
    } 
  ;



interface:
    input_places output_places synchronous 
  | port_list_new                          
  ;

port_list_new:
    KEY_PORT node_name { port_ = nodeName_.str(); } 
    input_places output_places synchronous              
  | port_list_new KEY_PORT node_name { port_ = nodeName_.str(); }
    input_places output_places synchronous
  ;

synchronous:
    /* empty */ 
  | KEY_SYNCHRONOUS { labels_.clear(); checkLabels_ = false; } labels SEMICOLON
    { pnapi_owfn_yynet.setSynchronousLabels(labels_); }
  ;

labels:
    node_name 
    { 
      check(!(checkLabels_ && (pnapi_owfn_yynet.getSynchronousLabels().find(ident) == pnapi_owfn_yynet.getSynchronousLabels().end())),
             "undeclared label");
      labels_.insert(std::string(ident)); 
    } 
  | labels COMMA node_name 
    {
      check(!(checkLabels_ && (pnapi_owfn_yynet.getSynchronousLabels().find(ident) == pnapi_owfn_yynet.getSynchronousLabels().end())),
             "undeclared label"); 
      labels_.insert(std::string(ident)); 
    } 
  ;


 /*******************/
 /*** TRANSITIONS ***/
 /*******************/

transitions: 
    /* empty */ 
  | transitions transition            
  ;

transition: 
    KEY_TRANSITION node_name transition_cost
    { 
      check(!pnapi_owfn_yynet.containsNode(ident), "node name already used");
      transition_ = & pnapi_owfn_yynet.createTransition(nodeName_.str()); 
      transition_->setCost($3);
    }
    KEY_CONSUME 
    { 
      target_ = (Node**)(&transition_); source_ = (Node**)(&place_); 
      placeSet_.clear();
      placeSetType_ = true;
    } 
    arcs SEMICOLON 
    KEY_PRODUCE 
    { 
      source_ = (Node**)(&transition_); target_ = (Node**)(&place_); 
      placeSet_.clear();
      placeSetType_ = false;
    } 
    arcs SEMICOLON 
    synchronize constrain
  ;

transition_cost:
    /*empty*/                  { $$ = 0;  }
  | KEY_COST NUMBER SEMICOLON  { $$ = $2; }
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
      check(placeSet_.find(place_) == placeSet_.end(), placeSetType_ ? "place already in preset" : "place already in postset");
      check(!(placeSetType_ && (place_->getType() == Place::OUTPUT)), "output places are not allowed in preset");
      check(!(!placeSetType_ && (place_->getType() == Place::INPUT)), "input places are not allowed in postset");
      
      pnapi_owfn_yynet.createArc(**source_, **target_, $3);
      placeSet_.insert(place_);
    }
  | node_name 
    {
      place_ = places_[nodeName_.str()];
      check(place_ != 0, "unknown place");
      check(placeSet_.find(place_) == placeSet_.end(), placeSetType_ ? "place already in preset" : "place already in postset");
      check(!(placeSetType_ && (place_->getType() == Place::OUTPUT)), "output places are not allowed in preset");
      check(!(!placeSetType_ && (place_->getType() == Place::INPUT)), "input places are not allowed in postset");
      
      pnapi_owfn_yynet.createArc(**source_, **target_);
      placeSet_.insert(place_);
    }	    
  ;

synchronize:
    /* empty */           
  | KEY_SYNCHRONIZE { labels_.clear(); checkLabels_ = true; }
    labels SEMICOLON { transition_->setSynchronizeLabels(labels_); }
  ;

constrain:
    /* empty */                    
  | KEY_CONSTRAIN { labels_.clear(); } labels SEMICOLON 
    { constrains_[transition_] = labels_; }
  ;


 /****************/
 /*** MARKINGS ***/
 /****************/

markings:
    KEY_INITIALMARKING { markInitial_ = true; } 
    marking_list SEMICOLON {markInitial_ = false; } final
  ;

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
      
      if(markInitial_)
      {  
        p->mark($3);
      }
      else
      {
        (*finalMarking_)[*p] = $3;
      }
    }
  | node_name              
    { 
      Place* p = places_[nodeName_.str()];
      check(p != 0, "unknown place");
      
      if(markInitial_)
      {  
        p->mark(1);
      }
      else
      {
        (*finalMarking_)[*p] = 1;
      }
    }
  ;

final:
    KEY_FINALMARKING { finalMarking_ = new Marking(pnapi_owfn_yynet, true); } 
    finalmarkings SEMICOLON 
  | condition 
    {
      pnapi_owfn_yynet.finalCondition() = (*$1);
      delete $1; 
    }
  ;

finalmarkings:
    marking_list
    {
      pnapi_owfn_yynet.finalCondition().addMarking(*finalMarking_);
      delete finalMarking_;
      finalMarking_ = new Marking(pnapi_owfn_yynet, true);
    }
  | finalmarkings SEMICOLON marking_list 
    {
      pnapi_owfn_yynet.finalCondition().addMarking(*finalMarking_);
      delete finalMarking_;
      finalMarking_ = new Marking(pnapi_owfn_yynet, true);
    }
  ;

condition:
    KEY_NOFINALMARKING { $$ = new formula::FormulaTrue(); }
  | KEY_FINALCONDITION SEMICOLON { $$ = new formula::FormulaFalse(); }
  | KEY_FINALCONDITION formula SEMICOLON { $$ = $2; }
  ;

formula: 
    LPAR formula RPAR { $$ = $2; }
  | KEY_TRUE          { $$ = new formula::FormulaTrue(); }
  | KEY_FALSE         { $$ = new formula::FormulaFalse(); }
  | KEY_ALL_PLACES_EMPTY 
    { 
      formula::Conjunction c;
      $$ = new formula::Conjunction(c, *((std::set<const Place*>*)&pnapi_owfn_yynet.getPlaces())); 
    }
  | OP_NOT formula
    { 
      $$ = new formula::Negation(*$2);
      delete $2;
    }
  | formula OP_OR formula
    {
      $$ = new formula::Disjunction(*$1, *$3);
      delete $1;
      delete $3;
    }
  | formula OP_AND formula 
    {
      $$ = new formula::Conjunction(*$1, *$3);
      delete $1;
      delete $3;
    }
  | formula OP_AND KEY_ALL_OTHER_PLACES_EMPTY
    {
      $$ = new formula::Conjunction(*$1, *((std::set<const Place*>*)&pnapi_owfn_yynet.getPlaces()));
      delete $1;
    }
  | formula OP_AND KEY_ALL_OTHER_INTERNAL_PLACES_EMPTY
    {
      $$ = new formula::Conjunction(*$1, *((std::set<const Place*>*)&pnapi_owfn_yynet.getInternalPlaces()));
      delete $1;
    }
  | formula OP_AND KEY_ALL_OTHER_EXTERNAL_PLACES_EMPTY
    {
      $$ = new formula::Conjunction(*$1, *((std::set<const Place*>*)&pnapi_owfn_yynet.getInterfacePlaces()));
      delete $1;
    }
  | node_name OP_EQ NUMBER
    {
      Place* p = places_[nodeName_.str()];
      check(p != 0, "unknown place");
      
      $$ = new formula::FormulaEqual(*p, $3);
    }
  | node_name OP_NE NUMBER 
    {
      Place* p = places_[nodeName_.str()];
      check(p != 0, "unknown place");

      $$ = new formula::Negation(formula::FormulaEqual(*p, $3));
    }
  | node_name OP_LT NUMBER
    {
      Place* p = places_[nodeName_.str()];
      check(p != 0, "unknown place");

      $$ = new formula::FormulaLess(*p, $3);
    }
  | node_name OP_GT NUMBER
    {
      Place* p = places_[nodeName_.str()];
      check(p != 0, "unknown place");

      $$ = new formula::FormulaGreater(*p, $3);
    } 
  | node_name OP_GE NUMBER
    {
      Place* p = places_[nodeName_.str()];
      check(p != 0, "unknown place");

      $$ = new formula::FormulaGreaterEqual(*p, $3);
    }
  | node_name OP_LE NUMBER
    {
      Place* p = places_[nodeName_.str()];
      check(p != 0, "unknown place");

      $$ = new formula::FormulaLessEqual(*p, $3);
    }
  ;

