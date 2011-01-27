%token KW_STATE KW_PROG KW_LOWLINK KW_SCC
%token COLON COMMA ARROW INDENT BANG STAR QUESTION
%token NUMBER NAME MESSAGE PATHARROW

%token_table
%defines
%name-prefix="graph_"


%{
#include <cstdio>
#include "Graph.h"
#include "cmdline.h"

using std::map;
using std::vector;

#define YYERROR_VERBOSE
#define YYMAXDEPTH 111655350

int currentState = 0;

extern Graph g;

extern gengetopt_args_info args_info;

extern int graph_lex();
extern int graph_error(const char *);

extern FILE *dot_out;
%}

%union {
  unsigned int val;
  char *name;
}

%type <val> NUMBER
%type <name> NAME
%type <val> annotation

%%

states:
  state
| state states
;

state:
  KW_STATE annotation NUMBER { currentState = $3; } prog lowlink scc
  markings transitions
;

annotation:
  /* empty */
    { $$ = 0; }
| STAR
    { $$ = 1; }
| BANG
    { $$ = 2; }
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
  NAME COLON NUMBER { g.addMarking(currentState, $1, $3); }
;

transitions:
  /* empty */
| transitions transition
;

transition:
  NAME ARROW NUMBER
  {
      g.addEdge(currentState, $3, $1);
  }
| NAME ARROW QUESTION
| NAME PATHARROW NUMBER
;
