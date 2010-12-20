%{
#include <iostream>
#include "config.h"
#include <map>
#include <vector>
#include <utility>
#include <string.h>
#include <sstream>

#include "cmdline.h"
#include "ReachabilityGraph.h"
#include "TaraHelpers.h"

//Result of lexical analyis
extern char* rg_yytext;
extern int rg_yylex();
extern int rg_yyerror(char const *msg);

// the command line parameters (declared in main.cc)
extern gengetopt_args_info args_info;

//Output stream defined and used in main.cc
extern ReachabilityGraph rg;

unsigned int currentState = 0;
%}

%name-prefix="rg_yy"
%error-verbose
%token_table
%expect 0
%defines

%token KW_STATE KW_LOWLINK KW_SCC COLON COMMA ARROW NUMBER NAME

%union {
  int val;
  char* str;
}

%type <val> NUMBER
%type <str> NAME

%%


rg:
  states
;

states:
  state
| states state
;

state:
  KW_STATE NUMBER
  {
    currentState = $2;
    rg.states.insert(currentState);
  }
   lowlink scc markings_or_transitions
;


scc:
  /* empty */
| KW_SCC scc_members
;

scc_members:
  scc_member
| scc_members scc_member
;

scc_member:
  NUMBER
;

lowlink:
  KW_LOWLINK NUMBER
;

markings_or_transitions:
  /* empty */
| markings
| transitions
| markings 
  transitions
;

markings:
  marking
| markings COMMA marking
;

marking:
  NAME COLON NUMBER
  {
    if ($3 > 0) {
    (rg.tokens[currentState])[rg.insertPlace($1)] = $3;
    }
  }
;

transitions:
  transition
| transitions transition
;

transition:
  NAME ARROW NUMBER
  {
    (rg.delta[currentState])[TaraHelpers::insertTransition(std::string($1))] = $3;
     }
;

