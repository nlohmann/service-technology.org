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
#include <stack>
#include <map>
#include <stdio.h>
#include <pnapi/pnapi.h>

#include "MaxCost.h"
#include "syntax_graph.h"
#include "syntax_costfunction.h"
#include "verbose.h"

std::deque<innerState *> innerGraph;

extern unsigned int cost(pnapi::Transition*);
extern unsigned int getTaraState (unsigned int);

/* this is the Stack of the nodes in the graph of lola output */
/* this stack is used for DFS */
std::deque<int> nodeStack;

unsigned int maxCost(char ** optimization, unsigned int optLen, pnapi::PetriNet* net) {
    status("LoLA returned %d states.", innerGraph.size());
    
    bool USE_SIMPLE = false;
    
        for (int i = 0; i < optLen; ++i) {
            std::string sOpt = optimization[i];
            if (sOpt.compare("simple") == 0) {
                USE_SIMPLE = true;
		status("Optimization enabled: '%s'.", optimization[i]);
            }        
        }    


    if (USE_SIMPLE) {
    
        int maxTransCost = 0;

        //iterate over all transitions of the net
        std::set<pnapi::Transition*> allTransitions=net->getTransitions();
        for(std::set<pnapi::Transition*>::iterator it=allTransitions.begin();it!=allTransitions.end();it++) {
          // the cost of that transition
          int curCost=cost(*it);

           // remember the most expensive transition
           maxTransCost=maxTransCost>curCost? maxTransCost: curCost;
         }
        
        int val = innerGraph.size() * maxTransCost;
        
        status("Using simple upper bound: %d", val);        
        
        return val;
    }

   // assuming innerGraph[0] is start state

   unsigned int initialState = getTaraState(0);

   innerGraph[initialState]->curCost=0;
   nodeStack.push_back(initialState);
 
   unsigned int maxCost=0;
   unsigned int curCost=0;

   while(!nodeStack.empty()) {
       int tos(nodeStack.back()); /* tos = Top Of Stack */
     
       // if accepting state, update MaxCost
       if(innerGraph[tos]->final) {
           maxCost=maxCost>curCost ? maxCost:curCost;
        //   std::cout << test++ << std::endl;
/*
DEBUG: output all expensive paths

       if(curCost>maxCost) {
          maxCost=curCost;
          for(std::deque<int>::iterator it=nodeStack.begin();it!=nodeStack.end();++it) {
             printf(" %d ", innerGraph[*it]->curCost);
          }
          printf("\n");
       }
*/
}

       /* if all childs are visited, remove from stack and reset properties */
       if(innerGraph[tos]->transitions.empty() ||
              (innerGraph[tos]->curTransition == innerGraph[tos]->transitions.end())) {
              curCost=curCost-innerGraph[tos]->curCost;
              innerGraph[tos]->inStack=false;
              nodeStack.pop_back();
              innerGraph[tos]->curTransition = innerGraph[tos]->transitions.begin();
              continue;
       }

       //check if next node is on Stack, as we dont want to count cycles
       if(innerGraph[innerGraph[tos]->curTransition->successor]->inStack) {
           ++(innerGraph[tos]->curTransition);
           continue;
       }
      
       { 
          //push child
          int next=innerGraph[tos]->curTransition->successor;
          nodeStack.push_back(next);
          innerGraph[next]->inStack=true;
          
          // get costs for that transition (using the  pointer to the net transition)
          int transitionCost=cost(innerGraph[tos]->curTransition->transition);

          // save the cost to that state
          // if this state is removed from stack, the cost will be subtracted
          innerGraph[next]->curCost=transitionCost;
          curCost+=transitionCost;
       }
 
       //for current tos goto next transition
       ++(innerGraph[tos]->curTransition);
   }
 	status("Using upper bound: %d", maxCost);        
  	return maxCost;
   //printf("\n maxCost: %d \n\n", maxCost);
}
