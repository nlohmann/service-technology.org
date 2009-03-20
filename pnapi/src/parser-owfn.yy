 // -*- C++ -*-

 /*****************************************************************************
  * bison options 
  ****************************************************************************/

/* plain c parser: the prefix is our "namespace" */
%name-prefix="pnapi_owfn_"

/* write tokens to parser-owfn.h for use by scanner */
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

#define yylex pnapi::parser::owfn::lex
#define yyparse pnapi::parser::owfn::parse

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
%type <yt_node> place places place_list internal_places input_places
%type <yt_node>   output_places capacity_place_list capacity places_ports 
%type <yt_node>   typed_places ports port_definition port_participants
%type <yt_node>   port_participant port_list lola_places interface port_list_new
%type <yt_node>   port_definition_new
%type <yt_node> transition transitions arc arcs preset_arcs postset_arcs
%type <yt_node> markings marking marking_list initial final finalmarkings
%type <yt_node>   finalmarking formula condition

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
    KEY_INTERFACE interface KEY_PLACE places SEMICOLON 
      { $$ = new Node($2, new Node(INTERNAL, $4)); }
  | KEY_PLACE typed_places ports                      
      { $$ = new Node($2, $3); }
  ;

typed_places: 
    internal_places input_places output_places { $$ = new Node($1, $2, $3); }
  | lola_places                                { $$ = $1;                   }
  ;

input_places:
    KEY_INPUT places SEMICOLON { $$ = new Node(INPUT, $2); }
  | /* empty */                { $$ = new Node();          }
  ;

output_places:
    KEY_OUTPUT places SEMICOLON { $$ = new Node(OUTPUT, $2); }
  | /* empty */                 { $$ = new Node();           }
  ;

internal_places:
    KEY_INTERNAL places SEMICOLON { $$ = new Node(INTERNAL, $2); }
  | /* empty */                   { $$ = new Node();             }
  ;

lola_places: 
  places SEMICOLON { $$ = new Node(INTERNAL, $1); }
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
    /* empty */         { $$ = new Node(); }
  | KEY_PORTS port_list { $$ = $2;         }
  ;

port_list:
    port_definition           { $$ = new Node($1);     }
  | port_list port_definition { $$ = $1->addChild($2); }
  ;

port_definition:
  node_name COLON port_participants SEMICOLON { $$ = new Node(PORT, $1, $3); }
  ;

port_participants:
    port_participant                         { $$ = new Node($1);     }
  | port_participants COMMA port_participant { $$ = $1->addChild($3); }
  ;

port_participant:
  node_name { $$ = new Node(PORT_PLACE, $1); }
  ;

interface:
    input_places output_places synchronous { $$ = new Node($1, $2); }
  | port_list_new                          { $$ = $1;               }
  ;

port_list_new:
    port_definition_new               { $$ = new Node($1);     }
  | port_list_new port_definition_new { $$ = $1->addChild($2); }
  ;

port_definition_new:
  KEY_PORT node_name input_places output_places synchronous
    { $$ = new Node(PORT, $2, new Node($3, $4)); }
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
initial final { $$ = new Node($1, $2); }
  ;

initial:
  KEY_MARKING marking_list SEMICOLON { $$ = new Node(INITIALMARKING, $2); }
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
    KEY_FINALMARKING finalmarkings SEMICOLON { $$ = $2;                      }
  | condition                                { $$ = new Node(CONDITION, $1); }
  ;

condition:
    KEY_NOFINALMARKING                       { $$ = new Node(FORMULA_FALSE); }
  | KEY_FINALCONDITION SEMICOLON             { $$ = new Node(FORMULA_TRUE);  }
  | KEY_FINALCONDITION formula SEMICOLON     { $$ = $2;                      }
  ;

finalmarkings:
    finalmarking                         { $$ = new Node($1);     }
  | finalmarkings SEMICOLON finalmarking { $$ = $1->addChild($3); }
  ;

finalmarking:
  marking_list { $$ = new Node(FINALMARKING, $1); }
  ;

formula: 
    LPAR formula RPAR      { $$ = $2;                            } 
  | KEY_TRUE               { $$ = new Node(FORMULA_TRUE);        }
  | KEY_FALSE              { $$ = new Node(FORMULA_FALSE);       }
  | KEY_ALL_PLACES_EMPTY   { $$ = new Node(FORMULA_APE);         }
  | OP_NOT formula         { $$ = new Node(FORMULA_NOT, $2);     }
  | formula OP_OR formula  { $$ = new Node(FORMULA_OR, $1, $3);  }
  | formula OP_AND formula { $$ = new Node(FORMULA_AND, $1, $3); }
  | formula OP_AND KEY_ALL_OTHER_PLACES_EMPTY
                           { $$ = new Node(FORMULA_AAOPE, $1);   }
  | formula OP_AND KEY_ALL_OTHER_INTERNAL_PLACES_EMPTY
                           { $$ = new Node(FORMULA_AAOIPE, $1);  }
  | formula OP_AND KEY_ALL_OTHER_EXTERNAL_PLACES_EMPTY
                           { $$ = new Node(FORMULA_AAOEPE, $1);  }
  | node_name OP_EQ NUMBER { $$ = new Node(FORMULA_EQ, $1, $3);  }
  | node_name OP_NE NUMBER { $$ = new Node(FORMULA_NE, $1, $3);  }
  | node_name OP_LT NUMBER { $$ = new Node(FORMULA_LT, $1, $3);  }
  | node_name OP_GT NUMBER { $$ = new Node(FORMULA_GT, $1, $3);  }
  | node_name OP_GE NUMBER { $$ = new Node(FORMULA_GE, $1, $3);  }
  | node_name OP_LE NUMBER { $$ = new Node(FORMULA_LE, $1, $3);  }
  ;
