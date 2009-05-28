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

%token KEY_INTERFACE KEY_INPUT KEY_OUTPUT KEY_NODES KEY_TAU KEY_FINAL KEY_INITIAL
%token COMMA COLON SEMICOLON ARROW NUMBER IDENT

%union 
{
  int yt_int;
  std::string * yt_string;
  pnapi::parser::sa::Node * yt_node;
}

%type <yt_int> NUMBER
%type <yt_string> IDENT
%type <yt_node> input output ident_list name state_name
%type <yt_node> node_list node node_attributes
%type <yt_node> transition_list transition

%start sa


 /*****************************************************************************
  * grammar rules 
  ****************************************************************************/
%%

sa: 
  KEY_INTERFACE
    input
    output
  KEY_NODES
    node_list
  {
    node = new Node($2, $3, $5);
  }
;

input:
  /* empty */
| KEY_INPUT ident_list SEMICOLON { $$ = new Node(pnapi::parser::sa::INPUT, $2); }
;

output:
  /* empty */
| KEY_OUTPUT ident_list SEMICOLON { $$ = new Node(pnapi::parser::sa::OUTPUT, $2); }
      
ident_list:
  name                    { $$ = new Node($1); }
| ident_list COMMA name   { $$ = $1->addChild($3); }
;

name:
  IDENT                   { cerr << *$1 << endl; $$ = new Node($1);      }
;

node_list:
  /* empty */             { $$ = new Node();        }
| node_list node          { $$ = $1->addChild($2);  }
| node                    { $$ = new Node($1);      }
;

node:
  state_name node_attributes transition_list
    {
      $$ = new Node(pnapi::parser::sa::STATE, $1, $2, $3);
    }
;

node_attributes:
  /* empty */             { $$ = new Node();                              }
| COLON KEY_FINAL               { $$ = new Node(pnapi::parser::sa::FINAL);      }
| COLON KEY_INITIAL             { $$ = new Node(pnapi::parser::sa::INITIAL);    }
| COLON KEY_FINAL COMMA KEY_INITIAL { $$ = new Node(pnapi::parser::sa::INIT_FINAL); }
| COLON KEY_INITIAL COMMA KEY_FINAL { $$ = new Node(pnapi::parser::sa::INIT_FINAL); }
;

transition_list:
  /* empty */
| transition_list transition
    { $$ = $1->addChild($2); }
| transition
    { $$ = new Node($1); }
;

transition:
  name ARROW state_name  { $$ = new Node(pnapi::parser::sa::EDGE, $1, $3); }
;

state_name:
  NUMBER                  { cerr << $1 << endl;
                            $$ = new Node($1); }
;
