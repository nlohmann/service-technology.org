/*****************************************************************************\
 Wendy -- Calculating Operating Guidelines

 Copyright (C) 2009  Niels Lohmann <niels.lohmann@uni-rostock.de>

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


/*
NOTE: This syntax-scanner does not recognize semantic errors!
Wrong Input causes undefined behaviour and not necessarily an error message.
*/

%token KW_STATE KW_LOWLINK KW_SCC COLON COMMA ARROW NUMBER NAME

%expect 0
%defines
%name-prefix="graph_"

%{
#include <string>
#include <stdio.h>
#include <pnapi/pnapi.h>
#include <map>
#include <set>
#include <list>

extern int graph_lex();
extern int graph_error(const char *);

extern pnapi::PetriNet* net;

/// current marking of the PN API net; for finding final states
std::map<const pnapi::Place*, unsigned int> currentMarking;

///currentState
int currentState;

/// relevante States (1=relevant, 0=irrelevant) 
std::map<const int, int> relStates;


///all relevant runs starting from a state
///eg: state 1 -> [ (t3,t4,t5) , (t3,t5,t6,...), ...]
std::map < const int,std::list < std::list <char*> >* > runs; //TODO: transitions statt C-Strings

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
  KW_STATE NUMBER lowlink
    {
	// printf("current state: %d\n",$2);
        currentState=$2;
        /* current marking is representative of an SCC */
        if ( $2 == $3) {
            relStates[$2]=1;
            /* create new list of all runs starting from this state */
            runs[currentState]= new std::list<std::list<char *> >;
	    // printf("relevanter Zustand: %d\n", $3);
        } else
            relStates[$2]=0;
    }
    scc_markings_transitions
;

scc_markings_transitions:
    scc transitions { /* no marking: do nothing */ }
 |  scc markings
    {
        /* now we read the marking, and can see if we have a final state */
        /*if this is a final state */
        if(relStates[currentState] && net->getFinalCondition().isSatisfied(pnapi::Marking(currentMarking, net)) ) {
             /*DEBUG: printf("(FINAL)\n");*/
             runs[currentState]->push_front(std::list<char*>()); //empty list, because we have final state
        }
        currentMarking.clear();
   } transitions
   {
      //DEBUG: writes for each state all runs to cout
      if(currentState==0 && relStates[currentState]) {
         printf("runs from State %d: %d\n",currentState, runs[currentState]->size());
      

         /*std::list<std::list<char*> >::iterator i;
         for(i=runs[currentState]->begin(); i!=runs[currentState]->end();i++) {
           std::list<char *>::iterator j;
           for(j=i->begin();j!=i->end();j++)
             printf(" %s ",*j);
           printf("\n");
         }*/
         printf("\n");
      }
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
    if(relStates[currentState] && relStates[$3]) {
       //printf("%s -> %d\n",$1,$3);

       /* iterate through all runs of target state and copy
          them to runs of current State with current transition */

       std::list<std::list<char *> >::iterator i;
       for(i=runs[$3]->begin(); i!=runs[$3]->end(); i++) {
         std::list <char*> copy=*i;
         copy.push_front($1); // Do we need strdup ?
         runs[currentState]->push_back(copy);
       }
    }
  }
;
