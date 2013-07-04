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
#include "Tara.h"
#include "verbose.h"
#include "cmdline.h"


/* this is the Stack of the nodes in the graph of lola output */
/* this stack is used for DFS */
std::deque<int> nodeStack;

unsigned int maxCost(pnapi::PetriNet* net) {
    status("LoLA returned %d states.", Tara::graph.size());
    
    bool USE_SIMPLE = Tara::args_info.heuristics_given && Tara::args_info.heuristics_arg == heuristics_arg_simple;
    bool USE_MAXOUT = Tara::args_info.heuristics_given && Tara::args_info.heuristics_arg == heuristics_arg_maxout;
    bool USE_LP = Tara::args_info.heuristics_given && Tara::args_info.heuristics_arg == heuristics_arg_lp;

    if (USE_SIMPLE) {
    	status("Optimization enabled: simple.");

        int maxTransCost = 0;

        //iterate over all transitions of the net
        std::set<pnapi::Transition*> allTransitions=net->getTransitions();
        for(std::set<pnapi::Transition*>::iterator it=allTransitions.begin();it!=allTransitions.end();it++) {
          // the cost of that transition
          int curCost=Tara::cost(*it);

           // remember the most expensive transition
           maxTransCost=maxTransCost>curCost? maxTransCost: curCost;
         }
        
        unsigned int val = Tara::graph.size() * maxTransCost;
        
        status("Using simple upper bound: %d", val);        
        
        return val;
    }
	
    if (USE_MAXOUT) {
    	status("Optimization enabled: maxout.");
        status("Using maxout upper bound: %d", Tara::sumOfLocalMaxCosts);        
        return Tara::sumOfLocalMaxCosts;
    }

    if (USE_LP) {
    	status("Optimization enabled: LP.");
        Tara::constructLP();
        int val = Tara::solveLP();        
        status("Using LP upper bound: %d", val);
//        Tara::deleteLP();        
        return val;
    }


   Tara::graph[Tara::initialState]->curCost=0;
   Tara::graph[Tara::initialState]->inStack=true;
   Tara::graph[Tara::initialState]->curTransition = Tara::graph[Tara::initialState]->transitions.begin();
   nodeStack.push_back(Tara::initialState);
    
   unsigned int maxCost = 0;
   int minCost = -1;
   unsigned int curCost = 0;
   unsigned int curLen  = 0;

   while(!nodeStack.empty()) {
       int tos(nodeStack.back()); /* tos = Top Of Stack */
     
       if (maxCost == Tara::sumOfLocalMaxCosts) { status("Found a path which is equal to maxout upper bound."); return maxCost; }
       // if accepting state, update MaxCost
       if(Tara::graph[tos]->final) {
           maxCost = maxCost > curCost ? maxCost : curCost;
           if(minCost == -1) {
               minCost = curCost;
           }
           else {
              minCost = minCost < curCost ? minCost : curCost;
           }
	//   status("Final state found. Current max: %d", maxCost);
        }
    
       /* if all childs are visited, remove from stack and reset properties */
       if(Tara::graph[tos]->curTransition == Tara::graph[tos]->transitions.end()) {
    //          status("Found a path of length %d with costs %d", curLen, curCost);
	      --curLen;
              curCost=curCost-Tara::graph[tos]->curCost;
              Tara::graph[tos]->inStack=false;
              Tara::graph[tos]->curTransition = Tara::graph[tos]->transitions.begin();
	      nodeStack.pop_back();
	      continue;
       }

       //check if next node is on Stack, as we dont want to count cycles
       if(Tara::graph[Tara::graph[tos]->curTransition->successor]->inStack) {
           ++(Tara::graph[tos]->curTransition);
	//   status("Saw an old node: %d", Tara::graph[tos]->curTransition->successor);
           continue;
       }
      
       { 
          //push child
          int next=Tara::graph[tos]->curTransition->successor;
          nodeStack.push_back(next);
          Tara::graph[next]->inStack=true;
          Tara::graph[next]->curTransition = Tara::graph[next]->transitions.begin();
          ++curLen;
          // get costs for that transition
          unsigned int transitionCost= Tara::graph[tos]->curTransition->costs;

          // save the cost to that state
          // if this state is removed from stack, the cost will be subtracted
          Tara::graph[next]->curCost=transitionCost;
          curCost+=transitionCost;
       }
 
       //for current tos goto next transition
       ++(Tara::graph[tos]->curTransition);
   }
 	status("Using upper bound: %d", maxCost);        
 	status("Using lower bound: %d", minCost);        
    Tara::minCosts = minCost;
  	return maxCost;
   //printf("\n maxCost: %d \n\n", maxCost);
}
