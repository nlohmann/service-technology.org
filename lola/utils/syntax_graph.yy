%token KW_STATE KW_PROG KW_PLACES KW_TRANSITIONS KW_STATES KW_EDGES KW_HASH
%token COLON COMMA ARROW INDENT
%token NUMBER NAME 

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

%%

graph:
  prefix states suffix
;

prefix:
  /* empty */
| NUMBER KW_PLACES NUMBER KW_TRANSITIONS
;

suffix:
  /* empty */
| INDENT NUMBER KW_STATES COMMA NUMBER KW_EDGES COMMA NUMBER KW_HASH
;

states:
  state
| state states
;

state:
  KW_STATE NUMBER prog
    { fprintf(dot_out, "s%d ", $2);
      currentState = $2; }
  markings transitions
;

prog:
  /* empty */
| KW_PROG NUMBER
;

markings:
  marking
    {
        if (args_info.emptyStates_given)
            fprintf(dot_out, "[label=\"\"]\n");
        else {
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
            fprintf(dot_out, "\"];\n");
        }
    
        marking.clear();
    }
| marking COMMA markings
;

marking:
  NAME COLON NUMBER
    { marking[std::string($1)] = $3; }
;

transitions:
  /* empty */
| NAME ARROW NUMBER transitions
    { fprintf(dot_out, "s%d -> s%d [label=\"%s\"]\n", currentState, $3, $1); }
;
