%token KW_STATE KW_PROG
%token COLON COMMA ARROW INDENT BANG STAR QUESTION
%token NUMBER NAME MESSAGE PATHARROW

%token_table
%defines
%name-prefix="graph_"


%{
#include <cstdio>
#include <map>
#include <string>
#include "cmdline-graph2dot.h"

using std::map;
using std::string;

#define YYERROR_VERBOSE
#define YYMAXDEPTH 65535

int currentState = 0;
map<string, int> marking;

extern gengetopt_args_info args_info;
extern map<string,bool> filter;
extern map<string,bool> exclude;

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
  KW_STATE annotation NUMBER prog
  {
      if (!args_info.pathOnly_given || $2 > 0) {
          fprintf(dot_out, "s%d ", $3);
          currentState = $3;
      }
  }
  markings
  {
      if (!args_info.pathOnly_given || $2 > 0) {
          if (args_info.emptyStates_given) {              
              fprintf(dot_out, "[label=\"\"");
              if ($2 == 2) {
                  fprintf(dot_out, " fillcolor=red");
              }
              fprintf(dot_out, "]\n");
          } else {
              fprintf(dot_out, "[label=\"");
              int printedPlaces = 0;
              for(map<string, int>::iterator m = marking.begin(); m != marking.end(); ++m) {
                  if (!args_info.filter_given || filter[m->first]) {
                      if (args_info.exclude_given && exclude[m->first])
                          continue;

                      if (printedPlaces > 0)
                          fprintf(dot_out, ", ");

                      if (args_info.columns_given &&
                          args_info.columns_arg != 0 &&
                          printedPlaces > 0 &&
                          (printedPlaces % args_info.columns_arg == 0))
                          fprintf(dot_out, "\\n");

                      fprintf(dot_out, "%s", m->first.c_str());

                      if (m->second > 1)
                          fprintf(dot_out, "(%d)", m->second);

                      printedPlaces++;
                  }
              }
              fprintf(dot_out, "\"");
              
              if ($2 == 2) {
                  fprintf(dot_out, " fillcolor=red");
              }
              
              fprintf(dot_out, "];\n");
          }

          marking.clear();
      }
  } 
  transitions
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

markings:
  marking
| markings COMMA marking
;

marking:
  NAME COLON NUMBER
    { marking[std::string($1)] = $3; }
;

transitions:
  /* empty */
| transitions transition
;

transition:
  NAME ARROW NUMBER
    { if (!args_info.pathOnly_given)
        fprintf(dot_out, "s%d -> s%d [label=\"%s\"]\n", currentState, $3, $1); }
| NAME ARROW QUESTION
| NAME PATHARROW NUMBER
    { fprintf(dot_out, "s%d -> s%d [label=\"%s\"]\n", currentState, $3, $1); }
;
