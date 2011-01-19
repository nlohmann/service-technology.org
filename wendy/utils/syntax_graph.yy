%token KW_STATE KW_PROG KW_LOWLINK
%token COLON COMMA ARROW INDENT BANG STAR QUESTION
%token NUMBER NAME MESSAGE PATHARROW

%token_table
%defines
%name-prefix="graph_"


%{
#include <cstdio>
#include "Graph.h"
#include "cmdline-graph2dot.h"

using std::map;
using std::vector;

#define YYERROR_VERBOSE
#define YYMAXDEPTH 65535

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
  KW_STATE annotation NUMBER { currentState = $3; } prog lowlink
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

markings:
  marking
| markings COMMA marking
;

marking:
  NAME COLON NUMBER
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
