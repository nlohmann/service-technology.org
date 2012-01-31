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
extern unsigned int cost(pnapi::Transition*);
extern pnapi::PetriNet* net;

unsigned int sumOfLocalMaxCosts = 0;

/// current marking of the PN API net; for finding final states
std::map<const pnapi::Place*, unsigned int> currentMarking;

///currentState during parsing
unsigned int currentState;
unsigned int currentTaraState;

// A mapping from the lola state numbers to the actual state numbers
std::map<const unsigned int, unsigned int> lolaToTara;

// The actual inner graph, realized by a deque (quick insertion, quick access)
extern std::deque<innerState *> innerGraph;

unsigned int getTaraState (unsigned int lolaState) {
    // First check whether the lola state has been seen yet
    
    std::map<const unsigned int, unsigned int>::iterator it =  lolaToTara.find(lolaState);
    bool seen = (it != lolaToTara.end());
    
    // If the state has been seen, return the value
    if (seen) {
        return it->second;
    } 
    
    // Otherwise: Create a new state, store the value, return it.
    unsigned int taraState = innerGraph.size();
    innerGraph.push_back(new innerState);
    innerGraph.back()->maxCosts = 0;
    lolaToTara[lolaState] = taraState;
    return taraState;
}

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
	currentTaraState = getTaraState(currentState);
	innerGraph[currentTaraState]->inStack = false;
	innerGraph[currentTaraState]->final = net->getFinalCondition().isSatisfied(pnapi::Marking(currentMarking, net));	
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
       if (currentState != $3) { // We do not need self loops
           unsigned int targetTaraState = getTaraState($3);
           pnapi::Transition *const transition = net->findTransition($1);
           unsigned int trCosts = cost(transition);           
            innerTransition cur= { transition, targetTaraState, trCosts };
           innerGraph[currentTaraState]->transitions.push_back(cur);
           innerGraph[currentTaraState]->curTransition=innerGraph[currentTaraState]->transitions.begin();
           if (trCosts > innerGraph[currentTaraState]->maxCosts) { 
                sumOfLocalMaxCosts += trCosts - innerGraph[currentTaraState]->maxCosts; 
                innerGraph[currentTaraState]->maxCosts = trCosts; 
           }
       }
       free($1); //get rid of those strings

  }
;
