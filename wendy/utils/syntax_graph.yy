%token KW_STATE KW_PROG KW_LOWLINK KW_SCC
%token COLON COMMA ARROW INDENT QUESTION
%token NUMBER NAME MESSAGE PATHARROW

%token_table
%defines
%name-prefix="graph_"


%{
#include <map>
#include "Graph.h"

#define YYERROR_VERBOSE
#define YYMAXDEPTH 111655350

int currentState = 0;

/// a marking of the PN API net
std::map<const pnapi::Place*, unsigned int> marking;

extern Graph g;

extern int graph_lex();
extern int graph_error(const char *);
%}

%union {
  unsigned int val;
  char *name;
}

%type <val> NUMBER
%type <name> NAME

%%

states:
  state
| state states
;

state:
  KW_STATE NUMBER { currentState = $2; } prog lowlink scc
  markings {
      const bool final = Graph::net.getFinalCondition().isSatisfied(pnapi::Marking(marking, &(Graph::net)));
      if (final) {
          g.addFinal(currentState);
      }
      marking.clear();
  } transitions
;

prog:
  /* empty */
| KW_PROG NUMBER
;

lowlink:
  /* empty */
| KW_LOWLINK NUMBER
;

scc:
  /* empty */
| KW_SCC statenumbers
;

statenumbers:
  NUMBER
| NUMBER statenumbers;

markings:
  marking
| markings COMMA marking
;

marking:
  NAME COLON NUMBER { g.addMarking(currentState, $1, $3); marking[Graph::net.findPlace($1)] = $3; free($1); }
;

transitions:
  /* empty */
| transitions transition
;

transition:
  NAME ARROW NUMBER { g.addEdge(currentState, $3, $1); free($1); }
| NAME ARROW QUESTION { free($1); }
| NAME PATHARROW NUMBER { free ($1); }
;
