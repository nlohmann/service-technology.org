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
#include "cmdline.h"

std::deque<innerState *> innerGraph;

extern unsigned int sumOfLocalMaxCosts;
extern unsigned int cost(pnapi::Transition*);
extern unsigned int getTaraState (unsigned int);
extern gengetopt_args_info args_info;


/* this is the Stack of the nodes in the graph of lola output */
/* this stack is used for DFS */
std::deque<int> nodeStack;

unsigned int maxCost(pnapi::PetriNet* net) {
    status("LoLA returned %d states.", innerGraph.size());
    
    bool USE_SIMPLE = args_info.heuristics_given && args_info.heuristics_arg == heuristics_arg_simple;
    bool USE_MAXOUT = args_info.heuristics_given && args_info.heuristics_arg == heuristics_arg_maxout;

    if (USE_SIMPLE) {
    	status("Optimization enabled: simple.");

        int maxTransCost = 0;

        //iterate over all transitions of the net
        std::set<pnapi::Transition*> allTransitions=net->getTransitions();
        for(std::set<pnapi::Transition*>::iterator it=allTransitions.begin();it!=allTransitions.end();it++) {
          // the cost of that transition
          int curCost=cost(*it);

           // remember the most expensive transition
           maxTransCost=maxTransCost>curCost? maxTransCost: curCost;
         }
        
        unsigned int val = innerGraph.size() * maxTransCost;
        
        status("Using simple upper bound: %d", val);        
        
        return val;
    }
	
    if (USE_MAXOUT) {
    	status("Optimization enabled: maxout.");
        status("Using maxout upper bound: %d", sumOfLocalMaxCosts);        
        return sumOfLocalMaxCosts;
    }


   // assuming innerGraph[0] is start state

   unsigned int initialState = getTaraState(0);

   innerGraph[initialState]->curCost=0;
   innerGraph[initialState]->inStack=true;
   innerGraph[initialState]->curTransition = innerGraph[initialState]->transitions.begin();
   nodeStack.push_back(initialState);
    
   unsigned int maxCost=0;
   unsigned int curCost=0;

   while(!nodeStack.empty()) {
       int tos(nodeStack.back()); /* tos = Top Of Stack */
     
       // if accepting state, update MaxCost
       if(innerGraph[tos]->final) {
           maxCost=maxCost>curCost ? maxCost:curCost;
        }
    
       /* if all childs are visited, remove from stack and reset properties */
       if(innerGraph[tos]->curTransition == innerGraph[tos]->transitions.end()) {
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
          innerGraph[next]->curTransition = innerGraph[next]->transitions.begin();
          
          // get costs for that transition
          unsigned int transitionCost= innerGraph[tos]->curTransition->costs;

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
