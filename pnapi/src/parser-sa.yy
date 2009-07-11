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
#include <string>
#include "parser.h"
#include "state.h"

using namespace pnapi;

#undef yylex
#undef yyparse
#undef yyerror

#define yyerror pnapi::parser::error

#define yylex   pnapi::parser::sa::lex
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
  std::string * yt_string;
}

%type <yt_int> NUMBER
%type <yt_string> IDENT

%start sa


 /*****************************************************************************
  * grammar rules 
  ****************************************************************************/
%%

sa:
  KEY_INTERFACE input output synchronous KEY_NODES nodes
;


input:
  /* empty */
| KEY_INPUT identlist SEMICOLON
  { 
    for (int i = 0; i < identlist.size(); i++)
      pnapi_sa_yyautomaton.addInput(identlist[i]);
    identlist.clear();
  }
;


output:
  /* empty */
| KEY_OUTPUT identlist SEMICOLON
  { 
    for (int i = 0; i < identlist.size(); i++)
      pnapi_sa_yyautomaton.addOutput(identlist[i]);
    identlist.clear();
  }
;


synchronous:
  /* empty */
| KEY_SYNCHRONOUS identlist SEMICOLON
;


identlist:
  /* empty */
| IDENT                    { identlist.push_back(*$1); delete $1; }
| identlist COMMA IDENT    { identlist.push_back(*$3); delete $3; }
;


nodes:
  /* empty */
| nodes NUMBER annotation successors
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
    
    for (int i = 0; i < succState_.size(); i++)
      pnapi_sa_yyautomaton.createEdge(*state_, *states_[succState_[i]], succLabel_[i], succType_[i]);
    
    final_ = false;
    initial_ = false;
    succState_.clear();
    succLabel_.clear();
    succType_.clear();
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
    if (states_.count($4) == 0)
    {
      edgeState_ = &pnapi_sa_yyautomaton.createState($4);
      states_[$4] = edgeState_;
    }
    else
      edgeState_ = states_[$4];

    edgeLabel_ = *$2;
    delete $2;
    edgeType_ = Automaton::TAU;
    if (pnapi_sa_yyautomaton.input().count(edgeLabel_) > 0)
      edgeType_ = Automaton::INPUT;
    if (pnapi_sa_yyautomaton.output().count(edgeLabel_) > 0)
      edgeType_ = Automaton::OUTPUT;
    succState_.push_back($4);
    succLabel_.push_back(edgeLabel_);
    succType_.push_back(edgeType_);
  }
;

