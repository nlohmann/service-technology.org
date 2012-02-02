/*****************************************************************************\
 Tara-- <<-- Tara -->>

 Copyright (c) <<-- 20XX Author1, Author2, ... -->>

 Tara is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Tara is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Hello.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

#include "DFS_graph.h"
#include "syntax_graph.h"
#include "Requirement.h"
#include <stack>
#include <map>
#include <list>
#include <stdio.h>
#include <pnapi/pnapi.h>
#include "Tara.h"


/// StateInfo (3=relevant AND final, 1=relevant, 0=irrelevant, 2=irrelevant & final)
//define G_STATE_FINAL 1
//define G_STATE_RELEVANT 2
//define G_STATE_VISITED 4
extern std::vector<short> stateInfo;

/* in this stack we put all transitions */
/* TODO: Maybe we dont need this, if we use the Requirement Checker ??? */
std::deque<pnapi::Transition*> transitionStack;
/* this is the Stack of the nodes in the graph of lola output */
/* this stack is used for DFS */
std::deque<int> nodeStack;

void DFS_graph() {

   /* TESTING: create the requirement Check */
   RequirementCheck rc=test_rc();

   /* printf("starting DFS on inner Graph\n"); */

   // assuming Tara::graph[0] is start state
   nodeStack.push_back(0);

   int runs=0; /* for counting the accepted runs */
   while(!nodeStack.empty()) {
       int tos(nodeStack.back()); /* tos= Top Of Stack */
        
       /* check the stateInfo from tos */
       if(stateInfo[tos] & G_STATE_FINAL) {
           /* found a run */
           runs++;
           if(runs % 1000000==0) /* output for large nets */
               printf("Anzahl runs: %d\n", runs);
          /* test output */
          printf("run gefunden. Ende: %d\n", tos);
          printCurrentRun(); /* Ausgabe Transition-Stack */

          /* and aks the Requirement Checkers opinion */
          if(rc.accepts())
              printf("OK\n");
          else 
              printf("nicht OK\n");
       }
       /* check the iterator for the node, if there are any unvisited transitions */
       if(!Tara::graph[tos]->transitions.empty() &&
              (Tara::graph[tos]->curTransition != Tara::graph[tos]->transitions.end())) {

           // update the requirement checker
           // if advancing is not possible, continue...
           if(!rc.advance(Tara::graph[tos]->curTransition->transition)) {
               Tara::graph[tos]->curTransition++;
               continue;
           } /* if requirement check is okay, we get forward in DFS */

           nodeStack.push_back(Tara::graph[tos]->curTransition->successor);

           //add the transition to the transition stack
           transitionStack.push_back(Tara::graph[tos]->curTransition->transition);

           //reset the iterator of the successor (if we visit a state more than one time)
           Tara::graph[Tara::graph[tos]->curTransition->successor]->curTransition=Tara::graph[Tara::graph[tos]->curTransition->successor]->transitions.begin();

           //for current tos goto next transition
           Tara::graph[tos]->curTransition++;
       }
       else { /* node finished */
          rc.abate(); /* abate the requirement checker */
          nodeStack.pop_back(); /* pop the DFS stack */
          transitionStack.pop_back(); /* pop the transition stack */
       }
   }
   printf("Anzahl runs: %d\n", runs);
}

/* output the transition stack */
void printCurrentRun() {
   for(std::deque<pnapi::Transition*>::iterator i=transitionStack.begin();i!=transitionStack.end();i++) {
      printf("%s ", (*i)->getName().c_str());
   }
   printf("\n");
}
