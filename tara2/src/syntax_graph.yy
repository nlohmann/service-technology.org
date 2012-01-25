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

/// StateInfo (3=relevant AND final, 1=relevant, 0=irrelevant, 2=irrelevant & final)
//define G_STATE_FINAL 1
//define G_STATE_RELEVANT 2
//define G_STATE_VISITED 4
//std::vector<short> stateInfo(5); //starting with 5

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
	//printf("current state: %d\n",$2);
        currentState=$2;

        //check if we have to resize the stateInfo vector
 //rmv       if(stateInfo.capacity()<$2) {
 //rmv           stateInfo.resize(2*$2,0);
  //      }

        /* current marking is representative of an SCC */
 //rmv        if($2 == $3) {
 //rmv           stateInfo[$2] |= G_STATE_RELEVANT;
            innerGraph.insert(std::pair<int, innerState *const>($2,new innerState));
            innerGraph[$2]->inStack=false;
            innerGraph[$2]->final=net->getFinalCondition().isSatisfied(pnapi::Marking(currentMarking, net));
 //       }
        /*else {
            stateInfo[$2]=0;
            stateInfo[$3]=0;
       }*/

       // if this state is final, then the lowlink representative is final
  //     if() {
           //printf("FINAL: %d\n",$3);
  //         stateInfo[$3] |= G_STATE_FINAL;
  //     }
       currentMarking.clear();
    }
    transitions
   {
      //DEBUG: ausgangsgrad
/*      if(stateInfo[currentState] & G_STATE_RELEVANT)
	      printf("Ausgangsgrad von %d: %d\n", currentState, innerGraph[currentState]->transitions.size());
*/
   }
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
    /* look if have an interesting transition */
    /*DEBUG:
    if(stateInfo[currentState]>3 | stateInfo[$3]>3)
	printf("problemo\n");
    */
//rmv    if(stateInfo[currentState] & stateInfo[$3] & G_STATE_RELEVANT ) {
       //printf("%s -> %d\n",$1,$3);

       innerTransition cur= { net->findTransition($1), $3 };
       free($1); //get rid of those strings

       innerGraph[currentState]->transitions.push_back(cur);
       innerGraph[currentState]->curTransition=innerGraph[currentState]->transitions.begin();
       /* iterate through all runs of target state and copy
          them to runs of current State with current transition */
//rmv    }
  }
;
