%token KW_STATE KW_PROG 
%token COLON COMMA ARROW
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
| state states;

state:
  KW_STATE NUMBER KW_PROG NUMBER
    { fprintf(stdout, "s%d ", $2);
      currentState = $2; }
  markings
  transitions
;

markings:
  marking
    {
        if (args_info.emptyStates_given)
            printf("[label=\"\"]\n");
        else {
            printf("[label=\"");
            int printedPlaces = 0;
            for(map<string, int>::iterator m = marking.begin(); m != marking.end(); ++m) {
                if (!args_info.filter_given || filter[m->first]) {
                    if (printedPlaces > 0)
                        printf(", ");

                    if (args_info.columns_given && args_info.columns_arg != 0 && printedPlaces > 0 && (printedPlaces % args_info.columns_arg == 0))
                        printf("\\n");

                    printf("%s", m->first.c_str());
                    
                    if (m->second > 1)
                        printf("(%d)", m->second);
                    
                    printedPlaces++;
                }
            }
            printf("\"];\n");
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
    { fprintf(stdout, "s%d -> s%d [label=%s]\n", currentState, $3, $1); }
;