/*****************************************************************************\
 Tara -- 

 Copyright (C) 2009  Niels Lohmann <niels.lohmann@uni-rostock.de>

 Tara is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Tara is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Tara.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/


/*
NOTE: This syntax-scanner does not recognize semantic errors!
Wrong Input causes undefined behaviour and not necessarily an error message.
*/

%token KW_STATE KW_LOWLINK KW_SCC COLON COMMA ARROW NUMBER NAME

%expect 0
%defines
%name-prefix="graph_"

%{
#include "MaxCost.h"
#include <string>
#include <stdio.h>
#include <pnapi/pnapi.h>
#include <map>
#include <set>
#include <list>
#include <utility>

extern int graph_lex();
extern int graph_error(const char *);

extern pnapi::PetriNet* net;

/// current marking of the PN API net; for finding final states
std::map<const pnapi::Place*, unsigned int> currentMarking;

///currentState during parsing
unsigned int currentState;

// the inner Graph that gets constructed
std::map<const unsigned int, innerState *const> innerGraph;

%}

%union {
  unsigned int val;
  char* str;
}

%type <val> NUMBER
%type <str> NAME
%type <val> lowlink

%%

states:
  state
| states state

;

state:
  KW_STATE NUMBER lowlink scc markings
    {
        currentState=$2;
        innerGraph.insert(std::pair<int, innerState *const>($2,new innerState));
        innerGraph[$2]->inStack=false;
        innerGraph[$2]->final=net->getFinalCondition().isSatisfied(pnapi::Marking(currentMarking, net));

        currentMarking.clear();
    }
    transitions
    { /* do nothing */  }
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

/* do something with Tarjan's lowlink value (needed for generating
   livelock free partners or reduction rule smart sending event) */
lowlink:
  KW_LOWLINK NUMBER
    { $$ = $2; }
;

markings:
markingList
;

markingList:
 marking
| markingList COMMA marking
;

marking:
  NAME COLON NUMBER
  { currentMarking[net->findPlace($1)] = $3; free($1); }
  /* calculate current marking to find final states*/
;

transitions:
  /* empty */
| transitionList
;

transitionList:
  transition
| transitionList transition
;

transition:
  NAME ARROW NUMBER 
  {
       innerTransition cur= { net->findTransition($1), $3 };
       free($1); //get rid of those strings

       innerGraph[currentState]->transitions.push_back(cur);
       innerGraph[currentState]->curTransition=innerGraph[currentState]->transitions.begin();
  }
;
