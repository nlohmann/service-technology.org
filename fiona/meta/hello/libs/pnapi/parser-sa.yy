 /*****************************************************************************
  * bison options 
  ****************************************************************************/

/* plain c parser: the prefix is our "namespace" */
%name-prefix="pnapi_sa_yy"

/* write tokens to parser-sa.h for use by scanner */
%defines

/* more verbose error reporting */
%error-verbose


 /*****************************************************************************
  * C declarations
  ****************************************************************************/
%{

#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include "parser.h"
#include "pnapi.h"

using namespace pnapi;

#undef yylex
#undef yyparse
#undef yyerror

#define yyerror pnapi::parser::error

#define yylex   pnapi::parser::sa::lex
#define yylex_destroy pnapi::parser::sa::lex_destroy
#define yyparse pnapi::parser::sa::parse

%}


 /*****************************************************************************
  * types, tokens, start symbol
  ****************************************************************************/

%token KEY_INTERFACE KEY_INPUT KEY_OUTPUT KEY_NODES KEY_FINAL KEY_INITIAL
%token KEY_SYNCHRONOUS COMMA COLON SEMICOLON ARROW NUMBER IDENT

%union 
{
  int yt_int;
  char * yt_str;
}

%type <yt_int> NUMBER
%type <yt_str> IDENT

%start sa


 /*****************************************************************************
  * grammar rules 
  ****************************************************************************/
%%

sa:
  KEY_INTERFACE input output synchronous KEY_NODES nodes
  {
    if (sa2sm)
    {
      std::set<Transition *> transitions = pnapi_sa_yynet.getTransitions();
      for (std::set<Transition *>::iterator t = transitions.begin(); t != transitions.end(); t++)
      {
        // same reason as in automaton.cc method stateMachine (see FIXME)
        // making a copy of all transitions and give them the labels
        Transition &tt = pnapi_sa_yynet.createTransition("", synchlabel[*t]);
        for (std::set<Arc *>::iterator f = (*t)->getPresetArcs().begin(); f != (*t)->getPresetArcs().end(); f++)
          pnapi_sa_yynet.createArc((*f)->getPlace(), tt, (*f)->getWeight());
        for (std::set<Arc *>::iterator f = (*t)->getPostsetArcs().begin(); f != (*t)->getPostsetArcs().end(); f++)
          pnapi_sa_yynet.createArc(tt, (*f)->getPlace(), (*f)->getWeight());
  
        pnapi_sa_yynet.deleteTransition(**t);
      }
    } 
  }
;


input:
  /* empty */
| KEY_INPUT identlist SEMICOLON
  {
    for (int i = 0; i < (int) identlist.size(); i++)
    {
      if (sa2sm)
      {
        Place *p = &pnapi_sa_yynet.createPlace(identlist[i], Node::INPUT);
        label2places_[identlist[i]] = p;
      }
      else
        pnapi_sa_yyautomaton.addInput(identlist[i]);
      input_.insert(identlist[i]);
    }
    identlist.clear();
  }
;


output:
  /* empty */
| KEY_OUTPUT identlist SEMICOLON
  { 
    for (int i = 0; i < (int) identlist.size(); i++)
    {
      if (sa2sm)
      {
        Place *p = &pnapi_sa_yynet.createPlace(identlist[i], Node::OUTPUT);
        label2places_[identlist[i]] = p;
      }
      else
        pnapi_sa_yyautomaton.addOutput(identlist[i]);
      output_.insert(identlist[i]);
    }
    identlist.clear();
  }
;


synchronous:
  /* empty */
| KEY_SYNCHRONOUS identlist SEMICOLON
  {
    for (int i = 0; i < (int) identlist.size(); i++)
      synchronous_.insert(identlist[i]);
    identlist.clear();
  }
;


identlist:
  /* empty */
| IDENT                    { identlist.push_back(std::string($1)); free($1); }
| identlist COMMA IDENT    { identlist.push_back(std::string($3)); free($3); }
;


nodes:
  /* empty */
| nodes NUMBER annotation successors
  {
    if (sa2sm)
    {
      if (places_.count($2) == 0)
      {
        std::stringstream ss;
        std::string num;
        ss << $2;
        ss >> num;
        place_ = &pnapi_sa_yynet.createPlace("p"+num);
        places_[$2] = place_;
      }
      else
        place_ = places_[$2];
      if (initial_)
        place_->mark();
      if (final_)
      {
        finalPlaces_.push_back(place_);
      }
        
      for (int i = 0; i < (int) succState_.size(); i++)
      {
        Transition *t = &pnapi_sa_yynet.createTransition();
        if (succType_[i] == Automaton::INPUT)
          pnapi_sa_yynet.createArc(*label2places_[succLabel_[i]], *t);
        if (succType_[i] == Automaton::OUTPUT)
          pnapi_sa_yynet.createArc(*t, *label2places_[succLabel_[i]]);
        if (succType_[i] == Automaton::SYNCHRONOUS)
        {
          synchlabel[t].insert(succLabel_[i]);
        }
          
        pnapi_sa_yynet.createArc(*place_, *t);
        pnapi_sa_yynet.createArc(*t, *places_[succState_[i]]);
        t = NULL;
      }
      
      final_ = false;
      initial_ = false;
      succState_.clear();
      succLabel_.clear();
      succType_.clear();
    }
    else
    {
	    if (states_.count($2) == 0)
	    {
	    	state_ = &pnapi_sa_yyautomaton.createState($2);
	      states_[$2] = state_;
	    }
	    else
	      state_ = states_[$2];
	    if (initial_)
	      state_->initial();
	    if (final_)
	      state_->final();
	    
	    for (int i = 0; i < (int) succState_.size(); i++)
	      pnapi_sa_yyautomaton.createEdge(*state_, *states_[succState_[i]], succLabel_[i], succType_[i]);
	    
	    final_ = false;
	    initial_ = false;
	    succState_.clear();
	    succLabel_.clear();
	    succType_.clear();
	  }
  }
;


annotation:
  /* empty */
| COLON KEY_INITIAL                  { initial_ = true; }
| COLON KEY_FINAL                    { final_ = true; }
| COLON KEY_INITIAL COMMA KEY_FINAL  { initial_ = final_ = true; }
;


successors:
  /* empty */
| successors IDENT ARROW NUMBER
  {
    if (sa2sm)
    {
      if (places_.count($4) == 0)
      {
        std::stringstream ss;
        std::string num;
        ss << $4;
        ss >> num;
        edgePlace_ = &pnapi_sa_yynet.createPlace("p"+num);
        places_[$4] = edgePlace_;
      }
    }
    else
    {
	    if (states_.count($4) == 0)
	    {
	      edgeState_ = &pnapi_sa_yyautomaton.createState($4);
	      states_[$4] = edgeState_;
	    }
	  }

    edgeLabel_ = $2;
    free($2);
    edgeType_ = Automaton::TAU;
    if (input_.count(edgeLabel_) > 0)
      edgeType_ = Automaton::INPUT;
    if (output_.count(edgeLabel_) > 0)
      edgeType_ = Automaton::OUTPUT;
    if (synchronous_.count(edgeLabel_) > 0)
      edgeType_ = Automaton::SYNCHRONOUS;
    succState_.push_back($4);
    succLabel_.push_back(edgeLabel_);
    succType_.push_back(edgeType_);
  }
;

