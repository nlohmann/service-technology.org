 // -*- C++ -*-

 /*****************************************************************************
  * bison options 
  ****************************************************************************/

/* plain c parser: the prefix is our "namespace" */
%name-prefix="pnapi_owfn_"

/* write tokens to parser-owfn.h for use by scanner */
%defines


 /*****************************************************************************
  * C declarations
  ****************************************************************************/
%{

#include <string>
#include "parser.h"

#undef yylex
#undef yyparse
#undef yyerror

#define yylex pnapi::parser::owfn::lex
#define yyparse pnapi::parser::owfn::parse
#define yyerror pnapi::parser::owfn::error

%}


 /*****************************************************************************
  * types, tokens, start symbol
  ****************************************************************************/

%token KEY_SAFE KEY_INTERFACE KEY_PLACE KEY_INTERNAL KEY_INPUT KEY_OUTPUT
%token KEY_SYNCHRONIZE KEY_SYNCHRONOUS
%token KEY_MARKING KEY_FINALMARKING KEY_NOFINALMARKING KEY_FINALCONDITION
%token KEY_TRANSITION KEY_CONSUME KEY_PRODUCE KEY_PORT KEY_PORTS
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
%type <yt_node> place places place_list internal_places input_places
%type <yt_node>   output_places capacity_place_list capacity places_ports 
%type <yt_node>   typed_places
%type <yt_node> transition transitions arc arcs preset_arcs postset_arcs
%type <yt_node> markings marking marking_list initial

%start petrinet


 /*****************************************************************************
  * grammar rules 
  ****************************************************************************/
%%

petrinet: 
  places_ports markings transitions { node = new Node($1, $2, $3); }
  ;

node_name:   
    IDENT  { $$ = new Node($1); }
  | NUMBER { $$ = new Node($1); }
  ;


 /**************/
 /*** PLACES ***/
 /**************/

places_ports: 
    KEY_INTERFACE interface KEY_PLACE places SEMICOLON { $$ = new Node();   }
  | KEY_PLACE typed_places ports                       { $$ = $2; }
  ;

typed_places: 
    internal_places input_places output_places { $$ = new Node($1, $2, $3); }
  | lola_places                                { $$ = new Node();           }
  ;

input_places:
  KEY_INPUT places SEMICOLON { $$ = new Node(INPUT, $2); }
  ;

output_places:
  KEY_OUTPUT places SEMICOLON { $$ = new Node(OUTPUT, $2); }
  ;

internal_places:
  KEY_INTERNAL places SEMICOLON { $$ = new Node(INTERNAL, $2); }
  ;

lola_places: 
  places SEMICOLON
  ;

places: 
    capacity_place_list                  { $$ = new Node($1);     }
  | places SEMICOLON capacity_place_list { $$ = $1->addChild($3); }
  ;

capacity_place_list:
  capacity place_list { $$ = new Node(CAPACITY, $1, $2); }
  ;

capacity:
    /* empty */           { $$ = new Node(0);  }
  | KEY_SAFE COLON        { $$ = new Node(1);  }
  | KEY_SAFE NUMBER COLON { $$ = new Node($2); }
  ;

place_list:  
    /* empty */            { $$ = new Node();       }
  | place                  { $$ = new Node($1);     }
  | place_list COMMA place { $$ = $1->addChild($3); }
  ;

place: 
  node_name controlcommands { $$ = new Node(PLACE, $1); }
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
  | KEY_MAX_OCCURRENCES OP_EQ NUMBER commands
  | KEY_MAX_OCCURRENCES OP_EQ NEGATIVE_NUMBER commands
  ;

ports:
    /* empty */
  | KEY_PORTS port_list
  ;

port_list:
    port_definition
  | port_list port_definition
  ;

port_definition:
  node_name COLON port_participants SEMICOLON
  ;

port_participants:
    node_name
  | port_participants COMMA node_name
  ;

interface:
    input_places output_places synchronous
  | port_list_new
  ;

port_list_new:
    port_definition_new
  | port_list_new port_definition_new
  ;

port_definition_new:
  KEY_PORT node_name input_places output_places synchronous
  ;

synchronous:
    KEY_SYNCHRONOUS labels SEMICOLON
  | /* empty */
  ;

labels:
    node_name
  | labels COMMA node_name
  ;


 /*******************/
 /*** TRANSITIONS ***/
 /*******************/

transitions: 
    transitions transition { $$ = $1->addChild($2); }
  | /* empty */            { $$ = new Node();       }
  ;

transition: 
  KEY_TRANSITION node_name preset_arcs postset_arcs synchronize
    { $$ = new Node(TRANSITION, $2, $3, $4); }
  ;

preset_arcs:
  KEY_CONSUME arcs SEMICOLON { $$ = new Node(PRESET, $2); }
  ;

postset_arcs:
  KEY_PRODUCE arcs SEMICOLON { $$ = new Node(POSTSET, $2); }
  ;

arcs: 
    /* empty */    { $$ = new Node();       }
  | arc            { $$ = new Node($1);     }
  | arcs COMMA arc { $$ = $1->addChild($3); }
  ;

arc: 
    node_name COLON NUMBER { $$ = new Node(ARC, $1, $3); }
  | node_name              { $$ = new Node(ARC, $1, 1);  }
  ;

synchronize:
    /* empty */
  | KEY_SYNCHRONIZE
  ;


 /****************/
 /*** MARKINGS ***/
 /****************/

markings:
  initial final { $$ = $1; }
  ;

initial:
  KEY_MARKING marking_list SEMICOLON { $$ = new Node(INITIAL, $2); }
  ;

marking_list:
    /* empty */                { $$ = new Node();       }
  | marking                    { $$ = new Node($1);     }
  | marking_list COMMA marking { $$ = $1->addChild($3); }
  ;

marking: 
    node_name COLON NUMBER { $$ = new Node(MARK, $1, $3); }
  | node_name              { $$ = new Node(MARK, $1, 1);  }
  ;

final:
    KEY_NOFINALMARKING
  | KEY_FINALMARKING multiplefinalmarkinglists SEMICOLON
    /*| KEY_FINALCONDITION statepredicate SEMICOLON */
  | KEY_FINALCONDITION SEMICOLON 
  ;

multiplefinalmarkinglists:
    finalmarkinglist 
  | multiplefinalmarkinglists SEMICOLON finalmarkinglist
  ;

finalmarkinglist: 
    /* empty */
  | finalmarking
  | finalmarkinglist COMMA finalmarking
  ;

finalmarking: 
    node_name COLON NUMBER 
  | node_name
  ;

/*
statepredicate: 
  LPAR statepredicate RPAR             { $$ = $2;                            } 
| OP_NOT statepredicate                { $$ = new Node(FORMULA_NOT, $2);     }
| statepredicate OP_OR statepredicate  { $$ = new Node(FORMULA_OR, $1, $3);  }
| statepredicate OP_AND statepredicate { $$ = new Node(FORMULA_AND, $1, $3); }
| statepredicate OP_AND KEY_ALL_OTHER_PLACES_EMPTY
                                       { $$ = new Node(FORMULA_AAOPE, $1);   }
| statepredicate OP_AND KEY_ALL_OTHER_INTERNAL_PLACES_EMPTY
                                       { $$ = new Node(FORMULA_AAOIPE, $1);  }
| statepredicate OP_AND KEY_ALL_OTHER_EXTERNAL_PLACES_EMPTY
                                       { $$ = new Node(FORMULA_AAOEPE, $1);  }
| node_name OP_EQ NUMBER               { $$ = new Node(FORMULA_EQ, $1, $3);  }
| node_name OP_NE NUMBER               { $$ = new Node(FORMULA_NE, $1, $3);  }
| node_name OP_LT NUMBER               { $$ = new Node(FORMULA_LT, $1, $3);  }
| node_name OP_GT NUMBER               { $$ = new Node(FORMULA_GT, $1, $3);  }
| node_name OP_GE NUMBER               { $$ = new Node(FORMULA_GE, $1, $3);  }
| node_name OP_LE NUMBER               { $$ = new Node(FORMULA_LE, $1, $3);  }
;
*/
