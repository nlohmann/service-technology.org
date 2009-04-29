// -*- C++ -*-

 /*****************************************************************************
  * bison options 
  ****************************************************************************/

/* plain c parser: the prefix is our "namespace" */
%name-prefix="pnapi_lola_"

/* write tokens to parser-lola.h for use by scanner */
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

#define yylex   pnapi::parser::lola::lex
#define yyparse pnapi::parser::lola::parse

%}


 /*****************************************************************************
  * types, tokens, start symbol
  ****************************************************************************/

%token KEY_SAFE KEY_INTERFACE KEY_PLACE KEY_INTERNAL KEY_INPUT KEY_OUTPUT
%token KEY_SYNCHRONIZE KEY_SYNCHRONOUS
%token KEY_MARKING KEY_FINALMARKING KEY_NOFINALMARKING KEY_FINALCONDITION
%token KEY_TRANSITION KEY_CONSUME KEY_PRODUCE KEY_PORT KEY_PORTS
%token KEY_ALL_PLACES_EMPTY
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
  std::string * yt_string;
  pnapi::parser::owfn::Node * yt_node;
}

%type <yt_int> NUMBER NEGATIVE_NUMBER
%type <yt_string> IDENT

%type <yt_node> node_name 
%type <yt_node> place places place_list
%type <yt_node>   capacity_place_list capacity lola_places
%type <yt_node> transition transitions arc arcs preset_arcs postset_arcs
%type <yt_node> marking marking_list initial

%start net


 /*****************************************************************************
  * grammar rules 
  ****************************************************************************/
%%

net: KEY_PLACE lola_places initial transitions
     { node = new Node($2, $3, $4); }
     ;

node_name:   
    IDENT  { $$ = new Node($1); }
  | NUMBER { $$ = new Node($1); }
  ;


 /* PLACE */

lola_places: 
places SEMICOLON { $$ = new Node(pnapi::parser::owfn::INTERNAL, $1); }
  ;

places: 
    capacity_place_list                  { $$ = new Node($1);     }
  | places SEMICOLON capacity_place_list { $$ = $1->addChild($3); }
  ;

capacity_place_list:
  capacity place_list { $$ = new Node(pnapi::parser::owfn::CAPACITY, $1, $2); }
  ;

capacity:
    /* empty */           { $$ = new Node(0);  }
  | KEY_SAFE COLON        { $$ = new Node(1);  }
  | KEY_SAFE NUMBER COLON { $$ = new Node($2); }
  ;

place_list:  
    place                  { $$ = new Node($1);     }
  | place_list COMMA place { $$ = $1->addChild($3); }
  ;

place: 
  node_name { $$ = new Node(pnapi::parser::owfn::PLACE, $1); }
  ;


 /* MARKING */

initial:
  KEY_MARKING marking_list SEMICOLON { $$ = new Node(pnapi::parser::owfn::INITIALMARKING, $2); }
  ;

marking_list:
    /* empty */                { $$ = new Node();       }
  | marking                    { $$ = new Node($1);     }
  | marking_list COMMA marking { $$ = $1->addChild($3); }
  ;

marking: 
  node_name COLON NUMBER { $$ = new Node(pnapi::parser::owfn::MARK, $1, $3); }
  ;


 /* TRANSITION */

transitions: 
    transitions transition { $$ = $1->addChild($2); }
  | transition             { $$ = new Node($1);     }
  ;

transition: 
  KEY_TRANSITION node_name preset_arcs postset_arcs
    { $$ = new Node(pnapi::parser::owfn::TRANSITION, $2, $3, $4); }
  ;

preset_arcs:
  KEY_CONSUME arcs SEMICOLON { $$ = new Node(pnapi::parser::owfn::PRESET, $2); }
  ;

postset_arcs:
  KEY_PRODUCE arcs SEMICOLON { $$ = new Node(pnapi::parser::owfn::POSTSET, $2); }
  ;

arcs: 
    /* empty */    { $$ = new Node();       }
  | arc            { $$ = new Node($1);     }
  | arcs COMMA arc { $$ = $1->addChild($3); }
  ;

arc: 
  node_name COLON NUMBER { $$ = new Node(pnapi::parser::owfn::ARC, $1, $3); }
  ;
