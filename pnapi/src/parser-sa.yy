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

#include <string>
#include "parser.h"

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
%token COMMA COLON SEMICOLON ARC NUMBER IDENT

%union 
{
  int yt_int;
  std::string * yt_string;
  pnapi::parser::sa::Node * yt_node;
}

%type <yt_int> NUMBER
%type <yt_string> IDENT
%type <yt_node> input_list output_list name
%type <yt_node> node_list node node_attributes
%type <yt_node> transition_list transition

%start sa


 /*****************************************************************************
  * grammar rules 
  ****************************************************************************/
%%

sa: 
  KEY_INTERFACE
    KEY_INPUT input_list SEMICOLON
    KEY_OUTPUT output_list SEMICOLON
  KEY_NODES
    node_list
  {
    $$ = new Node($3, $6, $9);
  }
;
      
input_list:
  /* empty */             { /*$$ = new Node();*/        }
| name                    { /*$$ = new Node($1);*/      }
| input_list COMMA name   { /*$$ = $1->addChild($3);*/  }
;
          
output_list: 
  /* empty */             { /*$$ = new Node();*/        }
| name                    { /*$$ = new Node($1);*/      }
| output_list COMMA name  { /*$$ = $1->addChild($3);*/  }
;

name:
  IDENT                   { $$ = new Node($1);      }
;

node_list:
  /* empty */             { $$ = new Node();        }
| node_list node          { $$ = $1->addChild($2);  }
| node                    { $$ = new Node($1);      }
;

node:
  NUMBER node_attributes transition_list
    {
      $$ = new Node(pnapi::parser::sa::STATE, $1, $2, $3);
    }
;

node_attributes:
  /* empty */             { $$ = new Node();                              }
| KEY_FINAL               { $$ = new Node(pnapi::parser::sa::FINAL);      }
| KEY_INITIAL             { $$ = new Node(pnapi::parser::sa::INITIAL);    }
| KEY_FINAL KEY_INITIAL   { $$ = new Node(pnapi::parser::sa::INIT_FINAL); }
| KEY_INITIAL KEY_FINAL   { $$ = new Node(pnapi::parser::sa::INIT_FINAL); }
;

transition_list:
  /* empty */
| transition_list transition
    { $$ = $1->addChild($2); }
| transition
    { $$ = new Node($1); }
;

transition:
  name ARC NUMBER         { $$ = new Node(pnapi::parser::sa::EDGE, $1, $3); }
;
