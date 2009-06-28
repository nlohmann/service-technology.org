 // -*- C++ -*-

 /*****************************************************************************
  * bison options 
  ****************************************************************************/

/* plain c parser: the prefix is our "namespace" */
%name-prefix="owfn_yy"

/* write tokens to parser-owfn.h for use by scanner */
%defines

/* more verbose error reporting */
%error-verbose


 /*****************************************************************************
  * C declarations
  ****************************************************************************/
%{

#include "pnapi.h"

using namespace pnapi;

PetriNet owfn_yynet;

/// from flex
extern char* owfn_yytext;
extern int owfn_yylex();
extern int owfn_yyerror(char const *msg);

%}


 /*****************************************************************************
  * types, tokens, start symbol
  ****************************************************************************/

%token KEY_SAFE KEY_INTERFACE KEY_PLACE KEY_INTERNAL KEY_INPUT KEY_OUTPUT
%token KEY_SYNCHRONIZE KEY_SYNCHRONOUS KEY_CONSTRAIN
%token KEY_MARKING KEY_FINALMARKING KEY_NOFINALMARKING KEY_FINALCONDITION
%token KEY_TRANSITION KEY_CONSUME KEY_PRODUCE KEY_PORT KEY_PORTS
%token KEY_ALL_PLACES_EMPTY KEY_COST
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
}

%type <yt_int> NUMBER NEGATIVE_NUMBER
%type <yt_string> IDENT

%start petrinet


 /*****************************************************************************
  * grammar rules 
  ****************************************************************************/
%%

petrinet: 
    places_ports markings transitions
  ;

node_name:   
    IDENT  
  | NUMBER 
  ;


 /**************/
 /*** PLACES ***/
 /**************/

places_ports: 
    KEY_INTERFACE interface KEY_PLACE places SEMICOLON 
  | KEY_PLACE typed_places ports                      
  ;

typed_places: 
    internal_places input_places output_places synchronous 
  | lola_places
  ;

input_places:
    /* empty */
  | KEY_INPUT places SEMICOLON                
  ;

output_places:
    /* empty */ 
  | KEY_OUTPUT places SEMICOLON                 
  ;

internal_places:
    /* empty */
  | KEY_INTERNAL places SEMICOLON                   
  ;

lola_places: 
  places SEMICOLON 
  ;

places: 
    capacity_place_list                 
  | places SEMICOLON capacity_place_list
  ;

capacity_place_list:
    capacity place_list 
  ;

capacity:
    /* empty */      
  | KEY_SAFE COLON   
  | KEY_SAFE NUMBER COLON 
  ;

place_list:  
    /* empty */           
  | place                 
  | place_list COMMA place 
  ;

place: 
    node_name controlcommands
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
    port_participant                         
  | port_participants COMMA port_participant 
  ;

port_participant:
    node_name 
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
    /* empty */ 
  | KEY_SYNCHRONOUS labels SEMICOLON                      
  ;

labels:
    label             
  | labels COMMA label
  ;

label:
    node_name 
  ;


 /*******************/
 /*** TRANSITIONS ***/
 /*******************/

transitions: 
    /* empty */ 
  | transitions transition            
  ;

transition: 
    KEY_TRANSITION node_name transition_cost preset_arcs postset_arcs synchronize constrain
  ;

transition_cost:
    /*empty*/ 
  | KEY_COST NUMBER SEMICOLON                 
;

preset_arcs:
    KEY_CONSUME arcs SEMICOLON 
  ;

postset_arcs:
    KEY_PRODUCE arcs SEMICOLON 
  ;

arcs: 
    /* empty */    
  | arc            
  | arcs COMMA arc 
  ;

arc: 
    node_name COLON NUMBER
  | node_name             
  ;

synchronize:
    /* empty */           
  | KEY_SYNCHRONIZE labels SEMICOLON 
  ;

constrain:
    /* empty */                    
  | KEY_CONSTRAIN labels SEMICOLON 
  ;


 /****************/
 /*** MARKINGS ***/
 /****************/

markings:
    initial final 
  ;

initial:
    KEY_MARKING marking_list SEMICOLON 
  ;

marking_list:
    /* empty */              
  | marking                  
  | marking_list COMMA marking 
  ;

marking: 
    node_name COLON NUMBER 
  | node_name              
  ;

final:
    KEY_FINALMARKING finalmarkings SEMICOLON 
  | condition                                
  ;

condition:
    KEY_NOFINALMARKING                       
  | KEY_FINALCONDITION SEMICOLON             
  | KEY_FINALCONDITION formula SEMICOLON     
  ;

finalmarkings:
    finalmarking                         
  | finalmarkings SEMICOLON finalmarking 
  ;

finalmarking:
    marking_list 
  ;

formula: 
    LPAR formula RPAR 
  | KEY_TRUE         
  | KEY_FALSE            
  | KEY_ALL_PLACES_EMPTY 
  | OP_NOT formula       
  | formula OP_OR formula
  | formula OP_AND formula 
  | formula OP_AND KEY_ALL_OTHER_PLACES_EMPTY
  | formula OP_AND KEY_ALL_OTHER_INTERNAL_PLACES_EMPTY
  | formula OP_AND KEY_ALL_OTHER_EXTERNAL_PLACES_EMPTY
  | node_name OP_EQ NUMBER 
  | node_name OP_NE NUMBER 
  | node_name OP_LT NUMBER 
  | node_name OP_GT NUMBER 
  | node_name OP_GE NUMBER 
  | node_name OP_LE NUMBER 
  ;
