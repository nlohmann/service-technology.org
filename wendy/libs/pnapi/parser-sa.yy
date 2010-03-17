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

#include "config.h"

#include "automaton.h"
#include "parser.h"

using namespace pnapi;

#define pnapi_sa_yyerror pnapi::parser::error
#define pnapi_sa_yylex   pnapi::parser::sa::lex
#define yylex_destroy pnapi::parser::sa::lex_destroy
#define pnapi_sa_yyparse pnapi::parser::sa::parse

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
;


input:
  /* empty */
| KEY_INPUT identlist SEMICOLON
  {
    for(int i = 0; i < (int)identlist.size(); ++i)
    {
      pnapi_sa_yyautomaton.addInputLabel(identlist[i]);
      input_.insert(identlist[i]);
    }
    identlist.clear();
  }
;


output:
  /* empty */
| KEY_OUTPUT identlist SEMICOLON
  { 
    for(int i = 0; i < (int)identlist.size(); ++i)
    {
      pnapi_sa_yyautomaton.addOutputLabel(identlist[i]);
      output_.insert(identlist[i]);
    }
    identlist.clear();
  }
;


synchronous:
  /* empty */
| KEY_SYNCHRONOUS identlist SEMICOLON
  {
    for(int i = 0; i < (int)identlist.size(); ++i)
    {
      pnapi_sa_yyautomaton.addSynchronousLabel(identlist[i]);
      synchronous_.insert(identlist[i]);
    }
    identlist.clear();
  }
;


identlist:
  /* empty */
| IDENT
  {
    identlist.push_back(std::string($1));
    free($1);
  }
| identlist COMMA IDENT
  {
    identlist.push_back(std::string($3));
    free($3);
  }
;


nodes:
  /* empty */
| nodes NUMBER annotation successors
  {
    if(states_.count($2) == 0)
    {
      state_ = &pnapi_sa_yyautomaton.createState($2);
      states_[$2] = state_;
    }
    else
    {
      state_ = states_[$2];
    }

    if(initial_)
      state_->setInitial();

    if(final_)
      state_->setFinal();
    
    for(int i = 0; i < (int)succState_.size(); ++i)
    {
      pnapi_sa_yyautomaton.createEdge(*state_, *states_[succState_[i]], succLabel_[i], succType_[i]);
    }
    
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
      state_ = &pnapi_sa_yyautomaton.createState($4);
      states_[$4] = state_;
    }

    edgeLabel_ = $2;
    free($2);
    edgeType_ = Edge::TAU;

    if (input_.count(edgeLabel_) > 0)
    {
      edgeType_ = Edge::INPUT;
    }

    if (output_.count(edgeLabel_) > 0)
    {
      edgeType_ = Edge::OUTPUT;
    }

    if (synchronous_.count(edgeLabel_) > 0)
    {
      edgeType_ = Edge::SYNCHRONOUS;
    }

    succState_.push_back($4);
    succLabel_.push_back(edgeLabel_);
    succType_.push_back(edgeType_);
  }
;

