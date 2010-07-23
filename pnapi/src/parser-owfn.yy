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
#include <string>

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
%type <yt_formula> condition formula fo_formula formula_proposition

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
| finalcondition_only
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
  KEY_INTERFACE interface_ports
| KEY_PLACE typed_interface ports
;

interface_place:
  KEY_PLACE places SEMICOLON
;

interface_ports:
  interface_ports_list
| port_list port_list_tail
;

port_list_tail:
  /* empty */
| port_list_tail interface_place
| port_list_tail roles
;

interface_ports_list:
  /* empty */
| interface_ports_list interface_ports_list_element
;

interface_ports_list_element:
  input_places
| output_places
| synchronous
| roles
| interface_place
;

input_places:
  KEY_INPUT { parser_.labelType_ = Label::INPUT; } interface_labels SEMICOLON                
;

output_places:
  KEY_OUTPUT { parser_.labelType_ = Label::OUTPUT; } interface_labels SEMICOLON                 
;

synchronous:
  KEY_SYNCHRONOUS { parser_.labelType_ = Label::SYNCHRONOUS; } interface_labels SEMICOLON
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
    // parser_.check(parser_.labels_[parser_.nodeName_.str()] == NULL, "node name already used");
    // parser_.check(parser_.places_[parser_.nodeName_.str()] == NULL, "node name already used");
    try
    {
      if(parser_.port_ == NULL)
      {
        parser_.label_ = &(parser_.net_.getInterface().addLabel(parser_.nodeName_.str(), parser_.labelType_));
      }
      else
      {
        parser_.label_ = &(parser_.port_->addLabel(parser_.nodeName_.str(), parser_.labelType_));
      }
    }
    catch(exception::Error & e)
    {
      parser_.rethrow(e);
    }

    parser_.labels_[parser_.nodeName_.str()] = parser_.label_;
  }
  controlcommands // NOTE: also kept due to compatibility reasons
| interface_label_list COMMA node_name
  {
    // parser_.check(parser_.labels_[parser_.nodeName_.str()] == NULL, "node name already used");
    // parser_.check(parser_.places_[parser_.nodeName_.str()] == NULL, "node name already used");
    try
    {
      if(parser_.port_ == NULL)
      {
        parser_.label_ = &(parser_.net_.getInterface().addLabel(parser_.nodeName_.str(), parser_.labelType_));
      }
      else
      {
        parser_.label_ = &(parser_.port_->addLabel(parser_.nodeName_.str(), parser_.labelType_));
      }
    }
    catch(exception::Error & e)
    {
      parser_.rethrow(e);
    }

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
  {
    try
    {
      parser_.port_ = &(parser_.net_.getInterface().addPort(parser_.nodeName_.str()));
    }
    catch(exception::Error & e)
    {
      parser_.rethrow(e);
    }
  }
  port_input_places port_output_places port_synchronous              
| port_list KEY_PORT node_name
  {
    try
    {
      parser_.port_ = &(parser_.net_.getInterface().addPort(parser_.nodeName_.str()));
    }
    catch(exception::Error & e)
    {
      parser_.rethrow(e);
    }
  }
  port_input_places port_output_places port_synchronous
;

port_input_places:
  /* empty */
| input_places
;

port_output_places:
  /* empty */
| output_places
;

port_synchronous:
  /* empty */
| synchronous
;

places: 
  capacity place_list { parser_.capacity_ = 0; }
| places SEMICOLON capacity place_list { parser_.capacity_ = 0; }
;

place_list:  
  /* empty */           
| node_name 
  {
    // parser_.check(parser_.labels_[parser_.nodeName_.str()] == NULL, "node name already used");
    // parser_.check(parser_.places_[parser_.nodeName_.str()] == NULL, "node name already used");
    try
    {
      parser_.place_ = &(parser_.net_.createPlace(parser_.nodeName_.str(), 0, parser_.capacity_));
    }
    catch(exception::Error & e)
    {
      parser_.rethrow(e);
    }

    parser_.places_[parser_.nodeName_.str()] = parser_.place_;
  } 
  controlcommands
| place_list COMMA node_name
  {
    // parser_.check(parser_.labels_[parser_.nodeName_.str()] == NULL, "node name already used");
    // parser_.check(parser_.places_[parser_.nodeName_.str()] == NULL, "node name already used");
    try
    {
      parser_.place_ = &(parser_.net_.createPlace(parser_.nodeName_.str(), 0, parser_.capacity_));
    }
    catch(exception::Error & e)
    {
      parser_.rethrow(e);
    }

    parser_.places_[parser_.nodeName_.str()] = parser_.place_;
  } 
  controlcommands
;


roles:
  KEY_ROLES role_names SEMICOLON
;

role_names:
  node_name
  { parser_.net_.addRole(parser_.nodeName_.str()); }
| role_names COMMA node_name
  { parser_.net_.addRole(parser_.nodeName_.str()); }
;


typed_interface:
  typed_interface_list
| places SEMICOLON roles
;

typed_interface_list:
  /* empty */
| typed_interface_list typed_interface_list_element
;

typed_interface_list_element:
  internal_places
| input_places_2
| output_places_2
| synchronous_2
| roles
;

input_places_2:
  KEY_INPUT { parser_.labelType_ = Label::INPUT; } interface_labels_2 SEMICOLON                
;

output_places_2:
  KEY_OUTPUT { parser_.labelType_ = Label::OUTPUT; } interface_labels_2 SEMICOLON                 
;

synchronous_2:
  KEY_SYNCHRONOUS { parser_.labelType_ = Label::SYNCHRONOUS; } interface_labels_2 SEMICOLON
;

  // NOTE: capacity kept due to compatibility reasons; is not actually used
interface_labels_2: 
  capacity interface_label_list_2 { parser_.capacity_ = 0; }
| interface_labels_2 SEMICOLON capacity interface_label_list_2 { parser_.capacity_ = 0; }
;

interface_label_list_2:  
  /* empty */           
| node_name
  {
    parser_.check(parser_.labels_[parser_.nodeName_.str()] == NULL, std::string("label name '") + parser_.nodeName_.str() + "' already used");
    parser_.check(parser_.places_[parser_.nodeName_.str()] == NULL, std::string("node name '") + parser_.nodeName_.str() + "' already used");
    parser_.check(parser_.labelTypes_.find(parser_.nodeName_.str()) == parser_.labelTypes_.end(), std::string("label name '") + parser_.nodeName_.str() + "' already used");

    parser_.labelTypes_[parser_.nodeName_.str()] = parser_.labelType_;
  }
  controlcommands // NOTE: also kept due to compatibility reasons
| interface_label_list_2 COMMA node_name
  {
    parser_.check(parser_.labels_[parser_.nodeName_.str()] == NULL, std::string("label name '") + parser_.nodeName_.str() + "' already used");
    parser_.check(parser_.places_[parser_.nodeName_.str()] == NULL, std::string("node name '") + parser_.nodeName_.str() + "' already used");
    parser_.check(parser_.labelTypes_.find(parser_.nodeName_.str()) == parser_.labelTypes_.end(), std::string("label name '") + parser_.nodeName_.str() + "' already used");

    parser_.labelTypes_[parser_.nodeName_.str()] = parser_.labelType_;
  }
  controlcommands // NOTE: also kept due to compatibility reasons
;

internal_places:
  KEY_INTERNAL places SEMICOLON                   
;

ports:
  /* empty */
  {
    try
    {
      PNAPI_FOREACH(l, parser_.labelTypes_)
      {
        parser_.labels_[l->first] = &(parser_.net_.getInterface().addLabel(l->first, l->second));
      }
    }
    catch(exception::Error & e)
    {
      parser_.rethrow(e);
    }
    parser_.labelTypes_.clear();
  }
| KEY_PORTS port_list2 
;

port_list2:
  node_name COLON
  {
    try
    {
      parser_.port_ = &(parser_.net_.getInterface().addPort(parser_.nodeName_.str()));
    }
    catch(exception::Error & e)
    {
      parser_.rethrow(e);
    }
  }
  port_participants SEMICOLON
| port_list2 node_name COLON
  {
    try
    {
      parser_.port_ = &(parser_.net_.getInterface().addPort(parser_.nodeName_.str()));
    }
    catch(exception::Error & e)
    {
      parser_.rethrow(e);
    }
  }
  port_participants SEMICOLON 
;

port_participants: 
  node_name 
  {
    parser_.label_ = parser_.net_.getInterface().findLabel(parser_.nodeName_.str());
    parser_.check(parser_.label_ == NULL,
                  ((parser_.label_) ? ("interface label already assigned to port '" + parser_.label_->getPort().getName() +"'") : ""));
    parser_.check(parser_.labelTypes_.find(parser_.nodeName_.str()) != parser_.labelTypes_.end(),
                  std::string("unknown label '") + parser_.nodeName_.str() + "'");

    try
    {
      parser_.labels_[parser_.nodeName_.str()] = &(parser_.port_->addLabel(parser_.nodeName_.str(), parser_.labelTypes_[parser_.nodeName_.str()]));
    }
    catch(exception::Error & e)
    {
      parser_.rethrow(e);
    }
  }
| port_participants COMMA node_name
  {
    parser_.label_ = parser_.net_.getInterface().findLabel(parser_.nodeName_.str());
    parser_.check(parser_.label_ == NULL,
                  (parser_.label_) ? ("interface label already assigned to port '" + parser_.label_->getPort().getName() +"'") : "");
    parser_.check(parser_.labelTypes_.find(parser_.nodeName_.str()) != parser_.labelTypes_.end(),
                  std::string("unknown label '") + parser_.nodeName_.str() + "'");

    try
    {
      parser_.labels_[parser_.nodeName_.str()] = &(parser_.port_->addLabel(parser_.nodeName_.str(), parser_.labelTypes_[parser_.nodeName_.str()]));
    }
    catch(exception::Error & e)
    {
      parser_.rethrow(e);
    }
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
  node_name
  {
    parser_.place_ = parser_.places_[parser_.nodeName_.str()];
    parser_.check(parser_.place_ != NULL, "unknown place");
  }
  marking_tail
;

marking_tail:
  COLON NUMBER 
  {      
    if(parser_.markInitial_)
    {  
      parser_.place_->setTokenCount($2);
    }
    else
    {
      (*(parser_.finalMarking_))[*parser_.place_] = $2;
    }
  }
| /* empty */
  {
    if(parser_.markInitial_)
    {  
      parser_.place_->setTokenCount(1);
    }
    else
    {
      (*(parser_.finalMarking_))[*parser_.place_] = 1;
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

finalcondition_only:
  KEY_FINALCONDITION SEMICOLON
| KEY_FINALCONDITION fo_formula SEMICOLON
  {
    parser_.net_.getFinalCondition() = (*$2);
    delete $2;
  }
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
| node_name
  {
    parser_.place_ = parser_.places_[parser_.nodeName_.str()];
    parser_.check(parser_.place_ != NULL, "unknown place");
  }
  formula_proposition
  {
    $$ = $3;
  }
;

formula_proposition:
  OP_EQ NUMBER
  { 
    $$ = new formula::FormulaEqual(*parser_.place_, $2);
  }
| OP_NE NUMBER 
  {
    $$ = new formula::Negation(formula::FormulaEqual(*parser_.place_, $2));
  }
| OP_LT NUMBER
  {
    $$ = new formula::FormulaLess(*parser_.place_, $2);
  }
| OP_GT NUMBER
  {
    $$ = new formula::FormulaGreater(*parser_.place_, $2);
  } 
| OP_GE NUMBER
  {
    $$ = new formula::FormulaGreaterEqual(*parser_.place_, $2);
  }
| OP_LE NUMBER
  {
    $$ = new formula::FormulaLessEqual(*parser_.place_, $2);
  }
;



fo_formula:
  LPAR fo_formula RPAR { $$ = $2; }
| KEY_TRUE          { $$ = new formula::FormulaTrue(); }
| KEY_FALSE         { $$ = new formula::FormulaFalse(); }
| OP_NOT fo_formula
  { 
    $$ = new formula::Negation(*$2);
    delete $2;
  }
| fo_formula OP_OR fo_formula
  {
    $$ = new formula::Disjunction(*$1, *$3);
    delete $1;
    delete $3;
  }
| fo_formula OP_AND fo_formula 
  {
    $$ = new formula::Conjunction(*$1, *$3);
    delete $1;
    delete $3;
  }
| node_name
  {
    parser_.place_ = parser_.places_[parser_.nodeName_.str()];
    if(parser_.place_ == NULL)
    {
      parser_.place_ = parser_.places_[parser_.nodeName_.str()] = &(parser_.net_.createPlace(parser_.nodeName_.str()));
    }
  }
  formula_proposition
  {
    $$ = $3;
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
  KEY_TRANSITION node_name
  {
    // parser_.check(parser_.labels_[parser_.nodeName_.str()] == NULL, "node name already used");
    // parser_.check(!(parser_.net_.containsNode(parser_.nodeName_.str())), "node name already used");
    try
    {
      parser_.transition_ = &(parser_.net_.createTransition(parser_.nodeName_.str()));
    }
    catch(exception::Error & e)
    {
      parser_.rethrow(e);
    }
  }
  transition_cost
  {
    parser_.transition_->setCost($4);
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
  node_name
  {
    parser_.place_ = parser_.places_[parser_.nodeName_.str()];
    parser_.label_ = parser_.labels_[parser_.nodeName_.str()];
    parser_.check(!((parser_.place_ == NULL) && (parser_.label_ == NULL)), "unknown place or interface label");
    parser_.check(parser_.placeSet_.find(parser_.nodeName_.str()) == parser_.placeSet_.end(), parser_.placeSetType_ ? "place already in preset" : "place already in postset");
    parser_.check(!(parser_.placeSetType_ && (parser_.label_ != NULL) && (parser_.label_->getType() != Label::INPUT)), "can only consume from places and input labels");
    parser_.check(!(!parser_.placeSetType_ && (parser_.label_ != NULL) && (parser_.label_->getType() != Label::OUTPUT)), "can only produce to places and output labels");
  }
  arc_tail
;

arc_tail:
  COLON NUMBER
  {
    if(parser_.place_ != NULL)
    {
      parser_.net_.createArc(**(parser_.source_), **(parser_.target_), $2);
    }
    else
    {
      parser_.transition_->addLabel(*(parser_.label_), $2);
    }

    parser_.placeSet_.insert(parser_.nodeName_.str());
  }
| /* empty */
  {
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

