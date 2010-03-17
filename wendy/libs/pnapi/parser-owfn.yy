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

#include "formula.h"
#include "marking.h"
#include "parser.h"
#include "petrinet.h"

#include <sstream>

#define pnapi_owfn_yyerror pnapi::parser::error
#define pnapi_owfn_yylex pnapi::parser::owfn::lex
#define yylex_destory pnapi::parser::owfn::lex_destroy
#define pnapi_owfn_yyparse pnapi::parser::owfn::parse

using namespace pnapi;
using namespace pnapi::parser::owfn;

%}


 /*****************************************************************************
  * types, tokens, start symbol
  ****************************************************************************/

%token KEY_SAFE KEY_INTERFACE KEY_PLACE KEY_INTERNAL KEY_INPUT KEY_OUTPUT
%token KEY_SYNCHRONIZE KEY_SYNCHRONOUS KEY_CONSTRAIN
%token KEY_INITIALMARKING KEY_FINALMARKING KEY_NOFINALMARKING KEY_FINALCONDITION
%token KEY_TRANSITION KEY_CONSUME KEY_PRODUCE KEY_PORT KEY_PORTS KEY_ROLES
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
  char * yt_str;
}

%type <yt_int> NUMBER NEGATIVE_NUMBER 
%type <yt_int> transition_cost
%type <yt_str> IDENT
%type <yt_formula> condition formula

%start petrinet


 /*****************************************************************************
  * grammar rules 
  ****************************************************************************/

%%

petrinet:
  interface markings transitions
  { 
    pnapi_owfn_yynet.setConstraintLabels(constrains_); 
  }
;

node_name:   
  IDENT  
  { 
    // clear stringstream
    nodeName_.str("");
    nodeName_.clear();

    nodeName_ << $1;
    free($1); 
  }
| NUMBER 
  { 
    // clear stringstream
    nodeName_.str("");
    nodeName_.clear();

    nodeName_ << $1; 
  }
;


 /*****************/
 /*** INTERFACE ***/
 /*****************/


interface: 
  KEY_INTERFACE interface_ports KEY_PLACE places SEMICOLON roles
| KEY_PLACE typed_places roles ports
;

interface_ports:
  input_places output_places synchronous 
| port_list                          
;

input_places:
  /* empty */
| KEY_INPUT { labelType_ = Label::INPUT; } interface_labels SEMICOLON                
;

output_places:
  /* empty */ 
| KEY_OUTPUT { labelType_ = Label::OUTPUT; } interface_labels SEMICOLON                 
;

synchronous:
  /* empty */ 
| KEY_SYNCHRONOUS { labelType_ = Label::SYNCHRONOUS; } interface_labels SEMICOLON
;

  // NOTE: capacity kept due to compatibility reasons; is not actually used
interface_labels: 
  capacity interface_label_list { capacity_ = 0; }
| interface_labels SEMICOLON capacity interface_label_list { capacity_ = 0; }
;

capacity:
  /* empty */           
| KEY_SAFE COLON        { capacity_ = 1; }
| KEY_SAFE NUMBER COLON { capacity_ = $2; }
;

interface_label_list:  
  /* empty */           
| node_name 
  {
    check(labels_[nodeName_.str()] == NULL, "node name already used");
    check(places_[nodeName_.str()] == NULL, "node name already used");
    label_ = & port_->addLabel(nodeName_.str(), labelType_);
    labels_[nodeName_.str()] = label_;
  } 
  controlcommands // NOTE: also kept due to compatibility reasons
| interface_label_list COMMA node_name
  {
    check(labels_[nodeName_.str()] == NULL, "node name already used");
    check(places_[nodeName_.str()] == NULL, "node name already used");
    label_ = & port_->addLabel(nodeName_.str(), labelType_);
    labels_[nodeName_.str()] = label_;
  } 
  controlcommands // NOTE: also kept due to compatibility reasons
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
  {
    if(place_ != NULL) // is only NULL when parsing interface
    {
      place_->setMaxOccurrence($3);
    }
  }
  commands
| KEY_MAX_OCCURRENCES OP_EQ NEGATIVE_NUMBER
  {
    if(place_ != NULL) // is only NULL when parsing interface
    {
      place_->setMaxOccurrence($3);
    }
  }
  commands
;

port_list:
  KEY_PORT node_name
  { port_ = &pnapi_owfn_yynet.getInterface().addPort(nodeName_.str()); } 
  input_places output_places synchronous              
| port_list KEY_PORT node_name
  { port_ = &pnapi_owfn_yynet.getInterface().addPort(nodeName_.str()); }
  input_places output_places synchronous
;


places: 
  capacity place_list { capacity_ = 0; }
| places SEMICOLON capacity place_list { capacity_ = 0; }
;

place_list:  
  /* empty */           
| node_name 
  {
    check(labels_[nodeName_.str()] == NULL, "node name already used");
    check(places_[nodeName_.str()] == NULL, "node name already used");
    place_ = &pnapi_owfn_yynet.createPlace(nodeName_.str(), 0, capacity_);
    places_[nodeName_.str()] = place_;
  } 
  controlcommands
| place_list COMMA node_name
  {
    check(labels_[nodeName_.str()] == NULL, "node name already used");
    check(places_[nodeName_.str()] == NULL, "node name already used");
    place_ = &pnapi_owfn_yynet.createPlace(nodeName_.str(), 0, capacity_);
    places_[nodeName_.str()] = place_;
  } 
  controlcommands
;


roles:
  /* empty */
| KEY_ROLES role_names SEMICOLON
;

role_names:
  node_name
  { pnapi_owfn_yynet.addRole(nodeName_.str()); }
| role_names COMMA node_name
  { pnapi_owfn_yynet.addRole(nodeName_.str()); }
;


typed_places: 
  internal_places input_places output_places synchronous 
| places SEMICOLON
;

internal_places:
  /* empty */
| KEY_INTERNAL places SEMICOLON                   
;


ports:
  /* empty */         
| KEY_PORTS port_list2 
;

port_list2:
  node_name COLON
  { port_ = & pnapi_owfn_yynet.getInterface().addPort(nodeName_.str()); }
  port_participants SEMICOLON
| port_list2 node_name COLON
  { port_ = & pnapi_owfn_yynet.getInterface().addPort(nodeName_.str()); }
  port_participants SEMICOLON 
;

port_participants: 
  node_name 
  {
    Label * l = labels_[nodeName_.str()];
    check(l != NULL, "unknown interface label");
    check((l->getPort().getName() == ""), "interface label already assigned to port '" + l->getPort().getName() +"'");
     
    Label::Type t = l->getType();
    pnapi_owfn_yynet.getInterface().getPort()->removeLabel(nodeName_.str());
    labels_[nodeName_.str()] = & port_->addLabel(nodeName_.str(), t);
  } 
| port_participants COMMA node_name
  {
    Label * l = labels_[nodeName_.str()];
    check(l != NULL, "unknown interface label");
    check((l->getPort().getName() == ""), "interface label already assigned to port '" + l->getPort().getName() +"'");
     
    Label::Type t = l->getType();
    pnapi_owfn_yynet.getInterface().getPort()->removeLabel(nodeName_.str());
    labels_[nodeName_.str()] = & port_->addLabel(nodeName_.str(), t);
  } 
;


 /****************/
 /*** MARKINGS ***/
 /****************/


markings:
  KEY_INITIALMARKING { markInitial_ = true; } 
  marking_list SEMICOLON { markInitial_ = false; } final
;

marking_list:
  /* empty */              
| marking                  
| marking_list COMMA marking 
;

marking: 
  node_name COLON NUMBER 
  { 
    Place * p = places_[nodeName_.str()];
    check(p != NULL, "unknown place");      
      
    if(markInitial_)
    {  
      p->setTokenCount($3);
    }
    else
    {
      (*finalMarking_)[*p] = $3;
    }
  }
| node_name              
  { 
    Place * p = places_[nodeName_.str()];
    check(p != NULL, "unknown place");
    
    if(markInitial_)
    {  
      p->setTokenCount(1);
    }
    else
    {
      (*finalMarking_)[*p] = 1;
    }
  }
;

final:
  KEY_FINALMARKING
  { finalMarking_ = new Marking(pnapi_owfn_yynet, true); } 
  finalmarkings SEMICOLON
  {
    delete finalMarking_;
    finalMarking_ = NULL;
  } 
| condition 
  {
    pnapi_owfn_yynet.getFinalCondition() = (*$1);
    if(wildcardGiven_)
    {
      wildcardGiven_ = false;
      pnapi_owfn_yynet.getFinalCondition().allOtherPlacesEmpty(pnapi_owfn_yynet);
    }
    delete $1; 
  }
;

finalmarkings:
  marking_list
  {
    pnapi_owfn_yynet.getFinalCondition().addMarking(*finalMarking_);
    delete finalMarking_;
    finalMarking_ = new Marking(pnapi_owfn_yynet, true);
  }
| finalmarkings SEMICOLON marking_list 
  {
    pnapi_owfn_yynet.getFinalCondition().addMarking(*finalMarking_);
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
    wildcardGiven_ = true;
    $$ = new formula::FormulaTrue();
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
    wildcardGiven_ = true;
    $$ = $1;
  }
| formula OP_AND KEY_ALL_OTHER_INTERNAL_PLACES_EMPTY
  {
    $$ = $1; // obsolete; kept due to compatibility
  }
| formula OP_AND KEY_ALL_OTHER_EXTERNAL_PLACES_EMPTY
  {
    $$ = $1; // obsolete; kept due to compatibility
  }
| node_name OP_EQ NUMBER
  {
    Place * p = places_[nodeName_.str()];
    check(p != NULL, "unknown place");
    
    $$ = new formula::FormulaEqual(*p, $3);
  }
| node_name OP_NE NUMBER 
  {
    Place * p = places_[nodeName_.str()];
    check(p != NULL, "unknown place");

    $$ = new formula::Negation(formula::FormulaEqual(*p, $3));
  }
| node_name OP_LT NUMBER
  {
    Place * p = places_[nodeName_.str()];
    check(p != NULL, "unknown place");

    $$ = new formula::FormulaLess(*p, $3);
  }
| node_name OP_GT NUMBER
  {
    Place * p = places_[nodeName_.str()];
    check(p != NULL, "unknown place");

    $$ = new formula::FormulaGreater(*p, $3);
  } 
| node_name OP_GE NUMBER
  {
    Place * p = places_[nodeName_.str()];
    check(p != NULL, "unknown place");

    $$ = new formula::FormulaGreaterEqual(*p, $3);
  }
| node_name OP_LE NUMBER
  {
    Place * p = places_[nodeName_.str()];
    check(p != NULL, "unknown place");

    $$ = new formula::FormulaLessEqual(*p, $3);
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
    check(labels_[nodeName_.str()] == NULL, "node name already used");
    check(!pnapi_owfn_yynet.containsNode(nodeName_.str()), "node name already used");
    transition_ = & pnapi_owfn_yynet.createTransition(nodeName_.str()); 
    transition_->setCost($3);
  }
  transition_roles
  KEY_CONSUME 
  { 
    target_ = reinterpret_cast<Node * *>(&transition_);
    source_ = reinterpret_cast<Node * *>(&place_); 
    placeSet_.clear();
    placeSetType_ = true;
  } 
  arcs SEMICOLON KEY_PRODUCE 
  { 
    source_ = reinterpret_cast<Node * *>(&transition_);
    target_ = reinterpret_cast<Node * *>(&place_); 
    placeSet_.clear();
    placeSetType_ = false;
  } 
  arcs SEMICOLON synchronize constrain
;

transition_cost:
  /*empty*/                  { $$ = 0;  }
| KEY_COST NUMBER SEMICOLON  { $$ = $2; }
;

transition_roles:
  /* empty */
| KEY_ROLES transition_role_names SEMICOLON
;

transition_role_names:
  node_name
  {
    check(transition_->getPetriNet().isRoleSpecified(nodeName_.str()), "role has not been specified");
    transition_->addRole(nodeName_.str());
  }
| transition_role_names COMMA node_name
  {
    check(transition_->getPetriNet().isRoleSpecified(nodeName_.str()), "role has not been specified");
    transition_->addRole(nodeName_.str());
  }
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
    label_ = labels_[nodeName_.str()];
    check(!((place_ == NULL) && (label_ == NULL)), "unknown place");
    check(placeSet_.find(nodeName_.str()) == placeSet_.end(), placeSetType_ ? "place already in preset" : "place already in postset");
    check(!(placeSetType_ && (label_ != NULL) && (label_->getType() != Label::INPUT)), "can only consume from places and input labels");
    check(!(!placeSetType_ && (label_ != NULL) && (label_->getType() != Label::OUTPUT)), "can only produce to places and output labels");

    if(place_ != NULL)
    {
      pnapi_owfn_yynet.createArc(**source_, **target_, $3);
    }
    else
    {
      transition_->addLabel(*label_, $3);
    }

    placeSet_.insert(nodeName_.str());
  }
| node_name 
  {
    place_ = places_[nodeName_.str()];
    label_ = labels_[nodeName_.str()];
    check(!((place_ == NULL) && (label_ == NULL)), "unknown place");
    check(placeSet_.find(nodeName_.str()) == placeSet_.end(), placeSetType_ ? "place already in preset" : "place already in postset");
    check(!(placeSetType_ && (label_ != NULL) && (label_->getType() != Label::INPUT)), "can only consume from places and input labels");
    check(!(!placeSetType_ && (label_ != NULL) && (label_->getType() != Label::OUTPUT)), "can only produce to places and output labels");

    if(place_ != NULL)
    {
      pnapi_owfn_yynet.createArc(**source_, **target_);
    }
    else
    {
      transition_->addLabel(*label_);
    }

    placeSet_.insert(nodeName_.str());
  }	    
;

synchronize:
  /* empty */           
| KEY_SYNCHRONIZE synchronize_labels SEMICOLON
;

synchronize_labels:
  /* empty */
| node_name
  {
    label_ = labels_[nodeName_.str()];
    check(label_ != NULL, "unknown label");
    check(label_->getType() == Label::SYNCHRONOUS, "can only synchronize with synchronous labels");
    transition_->addLabel(*label_);
  }
| synchronize_labels COMMA node_name
  {
    label_ = labels_[nodeName_.str()];
    check(label_ != NULL, "unknown label");
    check(label_->getType() == Label::SYNCHRONOUS, "can only synchronize with synchronous labels");
    transition_->addLabel(*label_);
  }
;

constrain:
  /* empty */                    
| KEY_CONSTRAIN constrain_labels SEMICOLON
;

constrain_labels:
  node_name
  { constrains_[transition_].insert(nodeName_.str()); }
| constrain_labels COMMA node_name
  { constrains_[transition_].insert(nodeName_.str()); }
;

