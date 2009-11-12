%{

extern int owfn_yylex();
extern int owfn_yyerror(const char *);

%}

%name-prefix="owfn_yy"
%defines
%error-verbose
%token_table

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
  int yt_int;
  char * yt_str;
}

%type <yt_int> NUMBER NEGATIVE_NUMBER 
%type <yt_int> transition_cost
%type <yt_str> IDENT

%start petrinet

%%

petrinet: 
  places_ports markings transitions
;

places_ports: 
  KEY_INTERFACE interface KEY_PLACE places SEMICOLON 
| KEY_PLACE typed_places ports
;

typed_places: 
  internal_places input_places output_places synchronous 
| places SEMICOLON
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
| node_name
  controlcommands
| place_list COMMA node_name 
  controlcommands
;

node_name:   
  IDENT
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
| KEY_MAX_OCCURRENCES OP_EQ NUMBER 
  commands
| KEY_MAX_OCCURRENCES OP_EQ NEGATIVE_NUMBER
  commands
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
  KEY_PORT node_name
  input_places 
  output_places 
  synchronous              
| port_list_new KEY_PORT node_name
  input_places 
  output_places 
  synchronous
;

synchronous:
  /* empty */ 
| KEY_SYNCHRONOUS labels SEMICOLON
;

labels:
  node_name
| labels COMMA node_name
;

transitions: 
    /* empty */ 
  | transitions transition            
  ;

transition: 
  KEY_TRANSITION node_name transition_cost
  KEY_CONSUME arcs SEMICOLON 
  KEY_PRODUCE arcs SEMICOLON 
  synchronize 
  constrain
;

transition_cost:
  /*empty*/
| KEY_COST NUMBER SEMICOLON
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
| KEY_CONSTRAIN 
  labels SEMICOLON
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

%%

int main()
{
  return owfn_yyparse();
}
