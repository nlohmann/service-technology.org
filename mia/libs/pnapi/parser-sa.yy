 /*****************************************************************************
  * bison options 
  ****************************************************************************/

/* plain c parser: the prefix is our "namespace" */
%name-prefix="pnapi_sa_"

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

using std::cerr;
using std::endl;

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
  pnapi::parser::sa::Node * yt_node;
}

%type <yt_int> NUMBER
%type <yt_string> IDENT
%type <yt_node> input output synchronous
%type <yt_node> identlist identnode
%type <yt_node> nodes node state annotation 
%type <yt_node> successors successor

%start sa


 /*****************************************************************************
  * grammar rules 
  ****************************************************************************/
%%

sa:
  KEY_INTERFACE input output synchronous KEY_NODES nodes
  { node = new Node($2, $3, $6); }
;


input:
  /* empty */
  { $$ = new Node(); }
| KEY_INPUT identlist SEMICOLON
  { $$ = new Node(pnapi::parser::sa::INPUT, $2); }
;


output:
  /* empty */
  { $$ = new Node(); }
| KEY_OUTPUT identlist SEMICOLON
  { $$ = new Node(pnapi::parser::sa::OUTPUT, $2); }
;


synchronous:
  /* empty */
  { $$ = new Node(); }
| KEY_SYNCHRONOUS identlist SEMICOLON
  { $$ = new Node(/* TODO: implement synchronous part */); }
;


identlist:
  /* empty */
  { $$ = new Node(); }
| IDENT
  { $$ = new Node($1); }
| identlist COMMA identnode
  { $1->addChild($3); }
;


identnode:
  IDENT
  { $$ = new Node($1); }
;


nodes:
  node
  { $$ = new Node($1); }
| nodes node
  { $1->addChild($2); }
;


node:
  state annotation successors
  { $$ = new Node(pnapi::parser::sa::STATE, $1, $2, $3); }
;


state:
  NUMBER
  { $$ = new Node($1); }
;


annotation:
  /* empty */
  { $$ = new Node(); }
| COLON KEY_INITIAL
  { $$ = new Node(pnapi::parser::sa::INITIAL); }
| COLON KEY_FINAL
  { $$ = new Node(pnapi::parser::sa::FINAL); }
| COLON KEY_INITIAL COMMA KEY_FINAL
  { $$ = new Node(pnapi::parser::sa::INIT_FINAL); }
;


successors:
  /* empty */
  { $$ = new Node(); }
| successors successor
  { $1->addChild($2); }
;


successor:
  identnode ARROW state
  { $$ = new Node(pnapi::parser::sa::EDGE, $1, $3); }
;