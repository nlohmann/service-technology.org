 // -*- C++ -*-

 /*****************************************************************************
  * bison options 
  ****************************************************************************/

/* generate a c++ parser */
%skeleton "lalr1.cc"

/* generate your code in your own namespace */
%define namespace "pnapi::parser::owfn::yy"

/* do not call the generated class "parser" */
%define parser_class_name "BisonParser"

/* generate needed location classes */
%locations

/* pass overlying parser to generated parser and yylex-wrapper */
%parse-param { Parser& parser_ }
%lex-param   { Parser& parser_ }

/* write tokens to parser-owfn.h for use by scanner */
%defines

/* more verbose error reporting */
%error-verbose


 /*****************************************************************************
  * C declarations
  ****************************************************************************/

%code requires {
  /* forward declarations */
  namespace pnapi { namespace parser { namespace owfn {
    class Parser;
  } } } /* pnapi::parser::owfn */
}

%{

#include "formula.h"
#include "marking.h"
#include "parser-owfn-wrapper.h"
#include "petrinet.h"

#include <sstream>

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
    parser_.net_.setConstraintLabels(parser_.constrains_); 
  }
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
| KEY_INPUT { parser_.labelType_ = Label::INPUT; } interface_labels SEMICOLON                
;

output_places:
  /* empty */ 
| KEY_OUTPUT { parser_.labelType_ = Label::OUTPUT; } interface_labels SEMICOLON                 
;

synchronous:
  /* empty */ 
| KEY_SYNCHRONOUS { parser_.labelType_ = Label::SYNCHRONOUS; } interface_labels SEMICOLON
;

  // NOTE: capacity kept due to compatibility reasons; is not actually used
interface_labels: 
  capacity interface_label_list { parser_.capacity_ = 0; }
| interface_labels SEMICOLON capacity interface_label_list { parser_.capacity_ = 0; }
;

capacity:
  /* empty */           
| KEY_SAFE COLON        { parser_.capacity_ = 1; }
| KEY_SAFE NUMBER COLON { parser_.capacity_ = $2; }
;

interface_label_list:  
  /* empty */           
| node_name 
  {
    parser_.check(parser_.labels_[parser_.nodeName_.str()] == NULL, "node name already used");
    parser_.check(parser_.places_[parser_.nodeName_.str()] == NULL, "node name already used");
    parser_.label_ = &(parser_.port_->addLabel(parser_.nodeName_.str(), parser_.labelType_));
    parser_.labels_[parser_.nodeName_.str()] = parser_.label_;
  }
  controlcommands // NOTE: also kept due to compatibility reasons
| interface_label_list COMMA node_name
  {
    parser_.check(parser_.labels_[parser_.nodeName_.str()] == NULL, "node name already used");
    parser_.check(parser_.places_[parser_.nodeName_.str()] == NULL, "node name already used");
    parser_.label_ = &(parser_.port_->addLabel(parser_.nodeName_.str(), parser_.labelType_));
    parser_.labels_[parser_.nodeName_.str()] = parser_.label_;
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
    if(parser_.place_ != NULL) // is only NULL when parsing interface
    {
      parser_.place_->setMaxOccurrence($3);
    }
  }
  commands
| KEY_MAX_OCCURRENCES OP_EQ NEGATIVE_NUMBER
  {
    if(parser_.place_ != NULL) // is only NULL when parsing interface
    {
      parser_.place_->setMaxOccurrence($3);
    }
  }
  commands
;

port_list:
  KEY_PORT node_name
  { parser_.port_ = &(parser_.net_.getInterface().addPort(parser_.nodeName_.str())); } 
  input_places output_places synchronous              
| port_list KEY_PORT node_name
  { parser_.port_ = &(parser_.net_.getInterface().addPort(parser_.nodeName_.str())); }
  input_places output_places synchronous
;


places: 
  capacity place_list { parser_.capacity_ = 0; }
| places SEMICOLON capacity place_list { parser_.capacity_ = 0; }
;

place_list:  
  /* empty */           
| node_name 
  {
    parser_.check(parser_.labels_[parser_.nodeName_.str()] == NULL, "node name already used");
    parser_.check(parser_.places_[parser_.nodeName_.str()] == NULL, "node name already used");
    parser_.place_ = &(parser_.net_.createPlace(parser_.nodeName_.str(), 0, parser_.capacity_));
    parser_.places_[parser_.nodeName_.str()] = parser_.place_;
  } 
  controlcommands
| place_list COMMA node_name
  {
    parser_.check(parser_.labels_[parser_.nodeName_.str()] == NULL, "node name already used");
    parser_.check(parser_.places_[parser_.nodeName_.str()] == NULL, "node name already used");
    parser_.place_ = &(parser_.net_.createPlace(parser_.nodeName_.str(), 0, parser_.capacity_));
    parser_.places_[parser_.nodeName_.str()] = parser_.place_;
  } 
  controlcommands
;


roles:
  /* empty */
| KEY_ROLES role_names SEMICOLON
;

role_names:
  node_name
  { parser_.net_.addRole(parser_.nodeName_.str()); }
| role_names COMMA node_name
  { parser_.net_.addRole(parser_.nodeName_.str()); }
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
  { parser_.port_ = &(parser_.net_.getInterface().addPort(parser_.nodeName_.str())); }
  port_participants SEMICOLON
| port_list2 node_name COLON
  { parser_.port_ = &(parser_.net_.getInterface().addPort(parser_.nodeName_.str())); }
  port_participants SEMICOLON 
;

port_participants: 
  node_name 
  {
    Label * l = parser_.labels_[parser_.nodeName_.str()];
    parser_.check(l != NULL, "unknown interface label");
    parser_.check((l->getPort().getName() == ""), "interface label already assigned to port '" + l->getPort().getName() +"'");
     
    Label::Type t = l->getType();
    parser_.net_.getInterface().getPort()->removeLabel(parser_.nodeName_.str());
    parser_.labels_[parser_.nodeName_.str()] = &(parser_.port_->addLabel(parser_.nodeName_.str(), t));
  }
| port_participants COMMA node_name
  {
    Label * l = parser_.labels_[parser_.nodeName_.str()];
    parser_.check(l != NULL, "unknown interface label");
    parser_.check((l->getPort().getName() == ""), "interface label already assigned to port '" + l->getPort().getName() +"'");
     
    Label::Type t = l->getType();
    parser_.net_.getInterface().getPort()->removeLabel(parser_.nodeName_.str());
    parser_.labels_[parser_.nodeName_.str()] = &(parser_.port_->addLabel(parser_.nodeName_.str(), t));
  } 
;


 /****************/
 /*** MARKINGS ***/
 /****************/


markings:
  KEY_INITIALMARKING { parser_.markInitial_ = true; } 
  marking_list SEMICOLON { parser_.markInitial_ = false; } final
;

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
      
    if(parser_.markInitial_)
    {  
      p->setTokenCount($3);
    }
    else
    {
      (*(parser_.finalMarking_))[*p] = $3;
    }
  }
| node_name              
  { 
    Place * p = parser_.places_[parser_.nodeName_.str()];
    parser_.check(p != NULL, "unknown place");
    
    if(parser_.markInitial_)
    {  
      p->setTokenCount(1);
    }
    else
    {
      (*(parser_.finalMarking_))[*p] = 1;
    }
  }
;

final:
  KEY_FINALMARKING
  { parser_.finalMarking_ = new Marking(parser_.net_, true); } 
  finalmarkings SEMICOLON
  {
    delete parser_.finalMarking_;
    parser_.finalMarking_ = NULL;
  } 
| condition 
  {
    parser_.net_.getFinalCondition() = (*$1);
    if(parser_.wildcardGiven_)
    {
      parser_.wildcardGiven_ = false;
      parser_.net_.getFinalCondition().allOtherPlacesEmpty(parser_.net_);
    }
    delete $1; 
  }
;

finalmarkings:
  marking_list
  {
    parser_.net_.getFinalCondition().addMarking(*(parser_.finalMarking_));
    delete parser_.finalMarking_;
    parser_.finalMarking_ = new Marking(parser_.net_, true);
  }
| finalmarkings SEMICOLON marking_list 
  {
    parser_.net_.getFinalCondition().addMarking(*(parser_.finalMarking_));
    delete parser_.finalMarking_;
    parser_.finalMarking_ = new Marking(parser_.net_, true);
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
    parser_.wildcardGiven_ = true;
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
    parser_.wildcardGiven_ = true;
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
    Place * p = parser_.places_[parser_.nodeName_.str()];
    parser_.check(p != NULL, "unknown place");
    
    $$ = new formula::FormulaEqual(*p, $3);
  }
| node_name OP_NE NUMBER 
  {
    Place * p = parser_.places_[parser_.nodeName_.str()];
    parser_.check(p != NULL, "unknown place");

    $$ = new formula::Negation(formula::FormulaEqual(*p, $3));
  }
| node_name OP_LT NUMBER
  {
    Place * p = parser_.places_[parser_.nodeName_.str()];
    parser_.check(p != NULL, "unknown place");

    $$ = new formula::FormulaLess(*p, $3);
  }
| node_name OP_GT NUMBER
  {
    Place * p = parser_.places_[parser_.nodeName_.str()];
    parser_.check(p != NULL, "unknown place");

    $$ = new formula::FormulaGreater(*p, $3);
  } 
| node_name OP_GE NUMBER
  {
    Place * p = parser_.places_[parser_.nodeName_.str()];
    parser_.check(p != NULL, "unknown place");

    $$ = new formula::FormulaGreaterEqual(*p, $3);
  }
| node_name OP_LE NUMBER
  {
    Place * p = parser_.places_[parser_.nodeName_.str()];
    parser_.check(p != NULL, "unknown place");

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
    parser_.check(parser_.labels_[parser_.nodeName_.str()] == NULL, "node name already used");
    parser_.check(!(parser_.net_.containsNode(parser_.nodeName_.str())), "node name already used");
    parser_.transition_ = &(parser_.net_.createTransition(parser_.nodeName_.str())); 
    parser_.transition_->setCost($3);
  }
  transition_roles
  KEY_CONSUME 
  { 
    parser_.target_ = reinterpret_cast<Node * *>(&(parser_.transition_));
    parser_.source_ = reinterpret_cast<Node * *>(&(parser_.place_)); 
    parser_.placeSet_.clear();
    parser_.placeSetType_ = true;
  } 
  arcs SEMICOLON KEY_PRODUCE 
  { 
    parser_.source_ = reinterpret_cast<Node * *>(&(parser_.transition_));
    parser_.target_ = reinterpret_cast<Node * *>(&(parser_.place_)); 
    parser_.placeSet_.clear();
    parser_.placeSetType_ = false;
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
    parser_.check(parser_.transition_->getPetriNet().isRoleSpecified(parser_.nodeName_.str()), "role has not been specified");
    parser_.transition_->addRole(parser_.nodeName_.str());
  }
| transition_role_names COMMA node_name
  {
    parser_.check(parser_.transition_->getPetriNet().isRoleSpecified(parser_.nodeName_.str()), "role has not been specified");
    parser_.transition_->addRole(parser_.nodeName_.str());
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
    parser_.place_ = parser_.places_[parser_.nodeName_.str()];
    parser_.label_ = parser_.labels_[parser_.nodeName_.str()];
    parser_.check(!((parser_.place_ == NULL) && (parser_.label_ == NULL)), "unknown place");
    parser_.check(parser_.placeSet_.find(parser_.nodeName_.str()) == parser_.placeSet_.end(), parser_.placeSetType_ ? "place already in preset" : "place already in postset");
    parser_.check(!(parser_.placeSetType_ && (parser_.label_ != NULL) && (parser_.label_->getType() != Label::INPUT)), "can only consume from places and input labels");
    parser_.check(!(!parser_.placeSetType_ && (parser_.label_ != NULL) && (parser_.label_->getType() != Label::OUTPUT)), "can only produce to places and output labels");

    if(parser_.place_ != NULL)
    {
      parser_.net_.createArc(**(parser_.source_), **(parser_.target_), $3);
    }
    else
    {
      parser_.transition_->addLabel(*(parser_.label_), $3);
    }

    parser_.placeSet_.insert(parser_.nodeName_.str());
  }
| node_name 
  {
    parser_.place_ = parser_.places_[parser_.nodeName_.str()];
    parser_.label_ = parser_.labels_[parser_.nodeName_.str()];
    parser_.check(!((parser_.place_ == NULL) && (parser_.label_ == NULL)), "unknown place");
    parser_.check(parser_.placeSet_.find(parser_.nodeName_.str()) == parser_.placeSet_.end(), parser_.placeSetType_ ? "place already in preset" : "place already in postset");
    parser_.check(!(parser_.placeSetType_ && (parser_.label_ != NULL) && (parser_.label_->getType() != Label::INPUT)), "can only consume from places and input labels");
    parser_.check(!(!parser_.placeSetType_ && (parser_.label_ != NULL) && (parser_.label_->getType() != Label::OUTPUT)), "can only produce to places and output labels");

    if(parser_.place_ != NULL)
    {
      parser_.net_.createArc(**(parser_.source_), **(parser_.target_));
    }
    else
    {
      parser_.transition_->addLabel(*(parser_.label_));
    }

    parser_.placeSet_.insert(parser_.nodeName_.str());
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
    parser_.label_ = parser_.labels_[parser_.nodeName_.str()];
    parser_.check(parser_.label_ != NULL, "unknown label");
    parser_.check(parser_.label_->getType() == Label::SYNCHRONOUS, "can only synchronize with synchronous labels");
    parser_.transition_->addLabel(*(parser_.label_));
  }
| synchronize_labels COMMA node_name
  {
    parser_.label_ = parser_.labels_[parser_.nodeName_.str()];
    parser_.check(parser_.label_ != NULL, "unknown label");
    parser_.check(parser_.label_->getType() == Label::SYNCHRONOUS, "can only synchronize with synchronous labels");
    parser_.transition_->addLabel(*(parser_.label_));
  }
;

constrain:
  /* empty */                    
| KEY_CONSTRAIN constrain_labels SEMICOLON
;

constrain_labels:
  node_name
  { parser_.constrains_[parser_.transition_].insert(parser_.nodeName_.str()); }
| constrain_labels COMMA node_name
  { parser_.constrains_[parser_.transition_].insert(parser_.nodeName_.str()); }
;

