 /*****************************************************************************
  * bison options 
  ****************************************************************************/

/* generate a c++ parser */
%skeleton "lalr1.cc"

/* generate your code in your own namespace */
%define namespace "pnapi::parser::sa::yy"

/* do not call the generated class "parser" */
%define parser_class_name "BisonParser"

/* generate needed location classes */
%locations

/* pass overlying parser to generated parser and yylex-wrapper */
%parse-param { Parser& parser_ }
%lex-param   { Parser& parser_ }

/* write tokens to parser-sa.h for use by scanner */
%defines

/* more verbose error reporting */
%error-verbose


 /*****************************************************************************
  * C declarations
  ****************************************************************************/

%code requires {
  /* forward declarations */
  namespace pnapi { namespace parser { namespace sa {
    class Parser;
  } } } /* pnapi::parser::sa */
}

%{

#include "config.h"

#include "automaton.h"
#include "parser-sa-wrapper.h"

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
    for(int i = 0; i < (int)parser_.identlist.size(); ++i)
    {
      parser_.automaton_.addInputLabel(parser_.identlist[i]);
      parser_.input_.insert(parser_.identlist[i]);
    }
    parser_.identlist.clear();
  }
;


output:
  /* empty */
| KEY_OUTPUT identlist SEMICOLON
  { 
    for(int i = 0; i < (int)parser_.identlist.size(); ++i)
    {
      parser_.automaton_.addOutputLabel(parser_.identlist[i]);
      parser_.output_.insert(parser_.identlist[i]);
    }
    parser_.identlist.clear();
  }
;


synchronous:
  /* empty */
| KEY_SYNCHRONOUS identlist SEMICOLON
  {
    for(int i = 0; i < (int)parser_.identlist.size(); ++i)
    {
      parser_.automaton_.addSynchronousLabel(parser_.identlist[i]);
      parser_.synchronous_.insert(parser_.identlist[i]);
    }
    parser_.identlist.clear();
  }
;


identlist:
  /* empty */
| IDENT
  {
    parser_.identlist.push_back(std::string($1));
    free($1);
  }
| identlist COMMA IDENT
  {
    parser_.identlist.push_back(std::string($3));
    free($3);
  }
;


nodes:
  /* empty */
| nodes NUMBER annotation successors
  {
    if(parser_.states_.count($2) == 0)
    {
      parser_.state_ = &(parser_.automaton_.createState($2));
      parser_.states_[$2] = parser_.state_;
    }
    else
    {
      parser_.state_ = parser_.states_[$2];
    }

    if(parser_.initial_)
    {
      parser_.state_->setInitial();
    }

    if(parser_.final_)
    {
      parser_.state_->setFinal();
    }
    
    for(int i = 0; i < (int)parser_.succState_.size(); ++i)
    {
      parser_.automaton_.createEdge(*(parser_.state_), *(parser_.states_[parser_.succState_[i]]), parser_.succLabel_[i], parser_.succType_[i]);
    }
    
    parser_.final_ = false;
    parser_.initial_ = false;
    parser_.succState_.clear();
    parser_.succLabel_.clear();
    parser_.succType_.clear();
  }
;


annotation:
  /* empty */
| COLON KEY_INITIAL                  { parser_.initial_ = true; }
| COLON KEY_FINAL                    { parser_.final_ = true; }
| COLON KEY_INITIAL COMMA KEY_FINAL  { parser_.initial_ = parser_.final_ = true; }
;


successors:
  /* empty */
| successors IDENT ARROW NUMBER
  {
    if(parser_.states_.count($4) == 0)
    {
      parser_.state_ = &(parser_.automaton_.createState($4));
      parser_.states_[$4] = parser_.state_;
    }

    parser_.edgeLabel_ = $2;
    free($2);
    parser_.edgeType_ = Edge::TAU;

    if(parser_.input_.count(parser_.edgeLabel_) > 0)
    {
      parser_.edgeType_ = Edge::INPUT;
    }

    if(parser_.output_.count(parser_.edgeLabel_) > 0)
    {
      parser_.edgeType_ = Edge::OUTPUT;
    }

    if(parser_.synchronous_.count(parser_.edgeLabel_) > 0)
    {
      parser_.edgeType_ = Edge::SYNCHRONOUS;
    }

    parser_.succState_.push_back($4);
    parser_.succLabel_.push_back(parser_.edgeLabel_);
    parser_.succType_.push_back(parser_.edgeType_);
  }
;

