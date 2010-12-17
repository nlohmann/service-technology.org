#ifndef COSTGRAPH_H
#define COSTGRAPH_H

#include <set>
#include <map>
#include <vector>
#include <stack>
#include <iostream>

#include "ReachabilityGraph.h"
#include "DFA.h"
#include "CostFunction.h"
#include "TaraHelpers.h"

class CostGraph : public ReachabilityGraph {
public:  
  
  std::map<int, std::vector<int> > dfaStates;
  std::map<int, std::vector<int> > values;
  
  CostGraph(ReachabilityGraph& rg) {
  
  int currentState = 0;
  root = 0;
  
  std::stack<std::vector<int> > dfaStatesStack;
  std::stack<std::vector<int> > valuesStack;
  std::stack<std::map<int, int> > markingStack;
  std::stack<int> referrerStack;
  std::stack<int> srcReferrerStack;
  std::stack<int> actionStack;
  
  int currentReferrer;
  int currentAction;
  int currentSrcReferrer;

  std::vector<int> currentDFAStates;
  std::vector<int> currentValues;
  std::map<int, int> currentMarking;
  
  for (int i = 0; i < TaraHelpers::automata.size(); ++i) {
    currentDFAStates.push_back(TaraHelpers::getDFAByID(i)->initialState);
  }

  for (int i = 0; i < TaraHelpers::costFunctions.size(); ++i) {
    currentValues.push_back(0);
  }

  dfaStatesStack.push(currentDFAStates);
  valuesStack.push(currentValues);
  markingStack.push(std::map<int,int>(rg.tokens[rg.root]));
  srcReferrerStack.push(-1);
  actionStack.push(-1);
  referrerStack.push(-1);
  
  while (markingStack.size() != 0) {
    // std::cerr << "A state to handle" << std::endl;
    
    bool found = false;
    
    currentDFAStates = dfaStatesStack.top(); dfaStatesStack.pop();
    currentValues = valuesStack.top(); valuesStack.pop();
    currentMarking = markingStack.top(); markingStack.pop();
    currentReferrer = referrerStack.top(); referrerStack.pop();
    currentAction = actionStack.top(); actionStack.pop();
    currentSrcReferrer = srcReferrerStack.top(); srcReferrerStack.pop();
    
    for (int i = 0; i < currentState; ++i) {
      if (dfaStates[i] == currentDFAStates && values[i] == currentValues && tokens[i] == currentMarking) {
        found = true;
        
        if (currentReferrer != -1) {
          (delta[currentReferrer])[currentAction] = i;
          // std::cerr << "inserting edge to existing node: " << currentReferrer << "->" << i << std::endl;
        }
        
      }
      if (found) break;
    }
  
    if (!found) {
      states.insert(currentState);
      dfaStates[currentState] = currentDFAStates;
      values[currentState] = currentValues;
      tokens[currentState] = currentMarking;    
      // std::cerr << "inserting new state: " << currentState << " : " << markingString(currentState) << std::endl;
      if (currentReferrer != -1) {
        (delta[currentReferrer])[currentAction] = currentState;
          // std::cerr << "delta has entries for " << delta.size() << " states. current: " << delta[currentReferrer].size() << std::endl;
        // std::cerr << "inserting edge to new node: " << currentReferrer << "->" << currentState << std::endl;
      }
      
      ++currentState;
      
      int srcState; 
      
      if (currentSrcReferrer == -1) {
        srcState = rg.root;
      } else {
        srcState = rg.yields(currentSrcReferrer, currentAction);
        // std::cerr << "new src state: " << srcState << std::endl;
      }
      
      std::set<int> enabled = rg.enabledTransitions(srcState);
            
      for (std::set<int>::iterator it = enabled.begin(); it != enabled.end(); ++it) {
        
        std::vector<int> currentValuesN;
        std::vector<int> currentDFAStatesN;
        
        
        for (int i = 0; i < currentValues.size(); ++i) {
          currentValuesN.push_back(currentValues[i] + TaraHelpers::getCostFunctionByID(i)->next(*it, currentDFAStates));
        } 

        for (int i = 0; i < currentDFAStates.size(); ++i) {
          currentDFAStatesN.push_back(TaraHelpers::getDFAByID(i)->getNewState(currentDFAStates[i], *it));
        } 
        
        // std::cerr << "pushing: " << rg.markingString(rg.yields(srcState,*it)) << " ref: " << srcState <<  std::endl;        
                
        referrerStack.push(currentState-1);
        srcReferrerStack.push(srcState);
        actionStack.push(*it);
        dfaStatesStack.push(currentDFAStatesN);
        valuesStack.push(currentValuesN);
        markingStack.push(std::map<int,int>(rg.tokens[rg.yields(srcState,*it)]));
              
      }
    
    }
    transitions = rg.transitions;
    places = rg.places;
    // std::cerr << "root hat " << delta[root].size() << " eintraege" << std::endl;  
  
  }
  
    
    
  }


};

#endif
