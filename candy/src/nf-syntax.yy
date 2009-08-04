%{
#include "settings.h"
#include "Graph.h"

// from main.cc
extern Graph* parsedOG;

// from flex
extern char* nf_yytext;
extern int nf_yylex();
extern int nf_yyerror(char const *msg);

// for parser
unsigned int currentCost;
bool currentCostAvailable;
bool currentCostHandled;

bool parsingInterface;
unsigned int countDeclaredEvents;
map< string, bool > parsedEventsNF;
%}

%name-prefix="nf_yy"
%error-verbose
%token_table
%defines


%token KEY_SAFE KEY_INTERFACE KEY_PLACE KEY_INTERNAL KEY_INPUT KEY_OUTPUT
%token KEY_SYNCHRONIZE KEY_SYNCHRONOUS KEY_CONSTRAIN
%token KEY_INITIALMARKING KEY_FINALMARKING KEY_NOFINALMARKING KEY_FINALCONDITION
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
  unsigned int value;
  char* str;
}

%type <value> NUMBER transition_cost
%type <str> IDENT

%start petrinet
%%



petrinet: 
    {
        currentCost = 0;
        currentCostAvailable = false;
        currentCostHandled = false;

        parsingInterface = false;
        countDeclaredEvents = 0;
    }
    places_ports
    {
        DEBUG "countDeclaredEvents '" << countDeclaredEvents << "'" END
        if ( countDeclaredEvents > parsedOG->events.size() ) {
            nf_yyerror("given netfile includes events which are not used in given OG");
            exit(EXIT_FAILURE);
        }
    }
    markings transitions
    {
        DEBUG "parsedEvents '" << parsedEventsNF.size() << "'" END
        if ( parsedEventsNF.size() != parsedOG->events.size() ) {
            nf_yyerror("given netfile does not include all events from given OG");
            exit(EXIT_FAILURE);
        }
    }
  ;



 /**************/
 /*** PLACES ***/
 /**************/

places_ports: 
    KEY_INTERFACE interface KEY_PLACE
    {
        parsingInterface = false;
    }
    places SEMICOLON 
  | KEY_PLACE typed_places ports
  ;

typed_places: 
    internal_places input_places output_places synchronous 
  | places SEMICOLON
  ;

input_places:
    /* empty */
  | KEY_INPUT { parsingInterface = true; } places SEMICOLON
  ;

output_places:
    /* empty */ 
  | KEY_OUTPUT { parsingInterface = true; } places SEMICOLON
  ;

internal_places:
    /* empty */
  | KEY_INTERNAL { parsingInterface = false; } places SEMICOLON                   
  ;

places: 
    capacity place_list
  | places SEMICOLON capacity place_list
  ;

capacity:
    /* empty */           
  | KEY_SAFE COLON
  | KEY_SAFE NUMBER COLON
  ;

place_list:  
    /* empty */           
  | node_name controlcommands
    {
        if ( parsingInterface ) ++countDeclaredEvents;
    }
  | place_list COMMA node_name controlcommands
    {
        if ( parsingInterface ) ++countDeclaredEvents;
    }
  ;

node_name:   
    IDENT  
    { free($1); }
  | NUMBER 
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
    node_name COLON port_participants SEMICOLON
  | port_list node_name COLON port_participants SEMICOLON 
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
    KEY_PORT node_name input_places output_places synchronous              
  | port_list_new KEY_PORT node_name input_places output_places synchronous
  ;

synchronous:
    /* empty */ 
  | KEY_SYNCHRONOUS { parsingInterface = true; } labels SEMICOLON
  ;

labels:
    node_name
    {
        if ( parsingInterface ) ++countDeclaredEvents;
    }
  | labels COMMA node_name
    {
        if ( parsingInterface ) ++countDeclaredEvents;
    }
  ;


 /*******************/
 /*** TRANSITIONS ***/
 /*******************/

transitions: 
    /* empty */ 
  | transitions transition            
  ;

transition: 
    KEY_TRANSITION node_name transition_cost
    { 
      currentCost = $3;
      currentCostHandled = false;
    }
    KEY_CONSUME arcs SEMICOLON
    KEY_PRODUCE arcs SEMICOLON 
    synchronize constrain
  ;

transition_cost:
    /*empty*/                  { $$ = 0; currentCostAvailable = false; }
  | KEY_COST NUMBER SEMICOLON  { $$ = $2; currentCostAvailable = true; }
;

arcs: 
    /* empty */    
  | arc           
  | arcs COMMA arc 
  ;

arc: 
    IDENT COLON NUMBER
    {
        // check if there are cost and if current arc label is a known event
        if ( currentCostAvailable ) {

            // check if current arc label is an event
            map< string, Event* >::iterator iter = parsedOG->events.find($1);
            if ( iter != parsedOG->events.end() ) {

                if ( $3 > 1 ) {
                    // maybe we should use averaged cost?
                    nf_yyerror("cannot compute cost for event from transition with arc weights > 1");
                    exit(EXIT_FAILURE);
                } else if ( parsedEventsNF.find($1) != parsedEventsNF.end() ) {
                    // maybe we should use the maximal cost as we cannot
                    // distinguish from which transition the event came?
                    nf_yyerror("cannot compute cost for event from multiple transition");
                    exit(EXIT_FAILURE);
                } else if ( currentCostHandled ) {
                    // how to deal with non-normal transitions instead?
                    nf_yyerror("cannot compute cost for event from non-normal transition");
                    exit(EXIT_FAILURE);
                } else {
                    (iter->second)->cost = currentCost;
                    parsedEventsNF[$1] = true;
                    currentCostHandled = true;
                }

            } else {
                // read an event which is not used in given OG
                // this is ok since it could be an internal place
            }
        }
        free($1);
    }
  |
    NUMBER COLON NUMBER
    {
        // check if there are cost and if current arc label is a known event
        if ( currentCostAvailable ) {

            // check if current arc label is a known event
            string ident = "" + $1;
            map< string, Event* >::iterator iter = parsedOG->events.find(ident);
            if ( iter != parsedOG->events.end() ) {

                if ( $3 > 1 ) {
                    // maybe we should use averaged cost?
                    nf_yyerror("cannot compute cost for event from transition with arc weights > 1");
                    exit(EXIT_FAILURE);
                } else if ( parsedEventsNF.find(ident) != parsedEventsNF.end() ) {
                    // maybe we should use the maximal cost as we cannot
                    // distinguish from which transition the event came?
                    nf_yyerror("cannot compute cost for event from multiple transition");
                    exit(EXIT_FAILURE);
                } else if ( currentCostHandled ) {
                    // how to deal with non-normal transitions instead?
                    nf_yyerror("cannot compute cost for event from non-normal transition");
                    exit(EXIT_FAILURE);
                } else {
                    (iter->second)->cost = currentCost;
                    parsedEventsNF[ident] = true;
                    currentCostHandled = true;
                }

            } else {
                // read an event which is not used in given OG
                // this is ok since it could be an internal place
            }
        }
    }
  | IDENT 
    {
        // check if there are cost and if current arc label is a known event
        if ( currentCostAvailable ) {

            // check if current arc label is a known event
            map< string, Event* >::iterator iter = parsedOG->events.find($1);
            if ( iter != parsedOG->events.end() ) {

                if ( parsedEventsNF.find($1) != parsedEventsNF.end() ) {
                    // maybe we should use the maximal cost as we cannot
                    // distinguish from which transition the event came?
                    nf_yyerror("cannot compute cost for event from multiple transition");
                    exit(EXIT_FAILURE);
                } else if ( currentCostHandled ) {
                    // how to deal with non-normal transitions instead?
                    nf_yyerror("cannot compute cost for event from non-normal transition");
                    exit(EXIT_FAILURE);
                } else {
                    (iter->second)->cost = currentCost;
                    parsedEventsNF[$1] = true;
                    currentCostHandled = true;
                }

            } else {
                // read an event which is not used in given OG
                // this is ok since it could be an internal place
            }
        }
        free($1);
    }
  | NUMBER 
    {
        // check if there are cost and if current arc label is a known event
        if ( currentCostAvailable ) {

            // check if current arc label is a known event
            string ident = "" + $1;
            map< string, Event* >::iterator iter = parsedOG->events.find(ident);
            if ( iter != parsedOG->events.end() ) {

                if ( parsedEventsNF.find(ident) != parsedEventsNF.end() ) {
                    // maybe we should use the maximal cost as we cannot
                    // distinguish from which transition the event came?
                    nf_yyerror("cannot compute cost for event from multiple transition");
                    exit(EXIT_FAILURE);
                } else if ( currentCostHandled ) {
                    // how to deal with non-normal transitions instead?
                    nf_yyerror("cannot compute cost for event from non-normal transition");
                    exit(EXIT_FAILURE);
                } else {
                    (iter->second)->cost = currentCost;
                    parsedEventsNF[ident] = true;
                    currentCostHandled = true;
                }

            } else {
                // read an event which is not used in given OG
                // this is ok since it could be an internal place
            }
        }
    }
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
    KEY_INITIALMARKING 
    marking_list SEMICOLON final
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

finalmarkings:
    marking_list
  | finalmarkings SEMICOLON marking_list 
  ;

condition:
    KEY_NOFINALMARKING
  | KEY_FINALCONDITION SEMICOLON
  | KEY_FINALCONDITION formula SEMICOLON
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

