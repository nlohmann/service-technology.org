
#include "DFS_graph.h"
#include "syntax_graph.h"
#include <stack>
#include <map>
#include <list>
#include <stdio.h>
#include <pnapi/pnapi.h>

extern std::map<const int, innerState *const> innerGraph;

/// StateInfo (3=relevant AND final, 1=relevant, 0=irrelevant, 2=irrelevant & final)
//define G_STATE_FINAL 1
//define G_STATE_RELEVANT 2
//define G_STATE_VISITED 4
extern std::vector<short> stateInfo;


//
std::deque<pnapi::Transition*> transitionStack;
std::deque<int> nodeStack;

void DFS_graph() {

   printf("starting DFS on inner Graph\n");

   // lets assume innerGraph[0] is start state
   nodeStack.push_back(0);

   int runs=0;
   int zustand=0;
   while(!nodeStack.empty()) {
       int tos(nodeStack.back());
       if(stateInfo[tos] & G_STATE_FINAL) {
           /* found a run */
           runs++;
           if(runs % 1000000==0)
               printf("Anzahl runs: %d\n", runs);
//          printf("run gefunden. Ende: %d\n", tos);
//           printCurrentRun(); /* Ausgabe Transition-Stack */
       }
       /* check the iterator for the node, if there are any unvisited transitions */
       if(!innerGraph[tos]->transitions.empty() &&
              (innerGraph[tos]->curTransition != innerGraph[tos]->transitions.end())) {
           nodeStack.push_back(innerGraph[tos]->curTransition->successor);
           transitionStack.push_back(innerGraph[tos]->curTransition->transition);

           //reset the iterator of the successor
           innerGraph[innerGraph[tos]->curTransition->successor]->curTransition=innerGraph[innerGraph[tos]->curTransition->successor]->transitions.begin();

           //for current tos goto next transition
           innerGraph[tos]->curTransition++;
       }
       else { /* node finished */
          nodeStack.pop_back();
          transitionStack.pop_back();
       }
   }
   printf("Anzahl runs: %d\n", runs);
}

void printCurrentRun() {
   for(std::deque<pnapi::Transition*>::iterator i=transitionStack.begin();i!=transitionStack.end();i++) {
      printf("%s ", (*i)->getName().c_str());
   }
   printf("\n");
}
