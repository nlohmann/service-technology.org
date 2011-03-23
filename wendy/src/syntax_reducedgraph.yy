/*****************************************************************************\
 Wendy -- Synthesizing Partners for Services

 Copyright (c) 2009 Niels Lohmann, Christian Sura, and Daniela Weinberg

 Wendy is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Wendy is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Wendy.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

%token KW_STATE KW_PROG KW_LOWLINK KW_SCC
%token COLON COMMA ARROW INDENT QUESTION
%token NUMBER NAME MESSAGE PATHARROW

%token_table
%defines
%name-prefix="reducedgraph_"


%{
#include <map>
#include "InnerMarking.h"
#include "InternalReduction.h"

#define YYERROR_VERBOSE
#define YYMAXDEPTH 111655350

int currentState = 0;

/// a marking of the PN API net
std::map<const pnapi::Place*, unsigned int> __marking;

extern int reducedgraph_lex();
extern int reducedgraph_error(const char *);
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
      const bool final = InnerMarking::net->getFinalCondition().isSatisfied(pnapi::Marking(__marking, InnerMarking::net));
      if (final) {
          Graph::g->addFinal(currentState);
      }
      __marking.clear();
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
  NAME COLON NUMBER { Graph::g->addMarking(currentState, $1, $3); __marking[InnerMarking::net->findPlace($1)] = $3; free($1); }
;

transitions:
  /* empty */
| transitions transition
;

transition:
  NAME ARROW NUMBER { Graph::g->addEdge(currentState, $3, $1); free($1); }
| NAME ARROW QUESTION { free($1); }
| NAME PATHARROW NUMBER { free ($1); }
;
