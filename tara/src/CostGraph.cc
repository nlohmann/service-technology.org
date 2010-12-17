#include "CostGraph.h"

CostGraph::CostGraph(ReachabilityGraph& rg) {
  
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
    //std::cerr << "push: " << TaraHelpers::getDFAByID(i)->initialState << std::endl;
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
    // //std::cerr << "A state to handle" << std::endl;
    
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
          // //std::cerr << "inserting edge to existing node: " << currentReferrer << "->" << i << std::endl;
        }
        
      }
      if (found) break;
    }
  
    if (!found) {
      states.insert(currentState);
      dfaStates[currentState] = currentDFAStates;
      values[currentState] = currentValues;
      tokens[currentState] = currentMarking;    
      // //std::cerr << "inserting new state: " << currentState << " : " << markingString(currentState) << std::endl;
      if (currentReferrer != -1) {
        (delta[currentReferrer])[currentAction] = currentState;
          // //std::cerr << "delta has entries for " << delta.size() << " states. current: " << delta[currentReferrer].size() << std::endl;
        // //std::cerr << "inserting edge to new node: " << currentReferrer << "->" << currentState << std::endl;
      }
      
      ++currentState;
      
      int srcState; 
      
      if (currentSrcReferrer == -1) {
        srcState = rg.root;
      } else {
        srcState = rg.yields(currentSrcReferrer, currentAction);
        // //std::cerr << "new src state: " << srcState << std::endl;
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
        
        // //std::cerr << "pushing: " << rg.markingString(rg.yields(srcState,*it)) << " ref: " << srcState <<  std::endl;        
                
        referrerStack.push(currentState-1);
        srcReferrerStack.push(srcState);
        actionStack.push(*it);
        dfaStatesStack.push(currentDFAStatesN);
        valuesStack.push(currentValuesN);
        markingStack.push(std::map<int,int>(rg.tokens[rg.yields(srcState,*it)]));
              
      }
    
    }

       // //std::cerr << "root hat " << delta[root].size() << " eintraege" << std::endl;  
  
  }
  
    places = rg.places;
 
    
  }
  
  
  std::string CostGraph::stateString(int s) {
    //std::cerr << "go1 " << s << std::endl;
    std::string result = " ";
    std::vector<int>& thestates = dfaStates[s];
    for (int i = 0; i < thestates.size(); ++i) {
      DFA* dfa = TaraHelpers::getDFAByID(i);
      //std::cerr << "mittendrin1 " << s << std::endl;
      
      int x = thestates[i];
      //std::cerr << "mittendrin1 " << s << std::endl;

      result += dfa->name + ": "; 

      //std::cerr << "mittendrin1 " << s << ", " << x << std::endl;
      result += dfa->states[x] + "; ";
      //std::cerr << "mittendrin1 " << s << std::endl;

    }
    //std::cerr << "gone1 " << s << std::endl;
    return result;
  }
  
  std::string CostGraph::valueString(int s) {
    //std::cerr << "go2 " << s << std::endl;
    std::string result = " ";
    std::vector<int>& vals = values[s];
    for (int i = 0; i < vals.size(); ++i) {
      CostFunction* cf = TaraHelpers::getCostFunctionByID(i);
      result += cf->name  + ": " + itoa(vals[i]) + "; ";
    }
    //std::cerr << "gone2 " << s << std::endl;
    return result;
  }
  
  
  void CostGraph::print_r(int s, std::set<int> visited) {
  // //std::cerr << "hallo" << std::endl;
  if (visited.find(s) == visited.end()) {
    visited.insert(s);
    std::set<int> ens = enabledTransitions(s);
    // //std::cerr << "nr of edges: " << ens.size()<< std::endl;
    for (std::set<int>::iterator it = ens.begin(); it != ens.end(); ++it) {
      std::cerr << s << " (" << markingString(s) << stateString(s) << valueString(s) << ")" << " | " << TaraHelpers::getTransitionByID(*it) << " > " << yields(s,*it) << " (" << markingString(yields(s,*it)) << stateString(yields(s,*it)) << valueString(yields(s,*it)) << ") " << std::endl;
      print_r(yields(s,*it), visited);
    }
  }
}
  
