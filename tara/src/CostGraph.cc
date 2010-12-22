#include <iostream>
#include "CostGraph.h"
#include "Policy.h"

CostGraph::CostGraph(ReachabilityGraph& rg, std::vector<Policy*>& policies) {
  
  states.insert(0); // bad state!
  
  int currentState = 1;
  root = 1;
  places = rg.places;
   
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
//    std::cerr << "push: " << TaraHelpers::getDFAByID(i)->initialState << std::endl;
  }

  for (int i = 0; i < TaraHelpers::costFunctions.size(); ++i) {
    currentValues.push_back(0);
  }

  dfaStatesStack.push(currentDFAStates);
  valuesStack.push(currentValues);
  markingStack.push(rg.tokens[rg.root]);
 // std::cerr << rg.root << rg.markingString(rg.root) << std::endl;
  srcReferrerStack.push(-1);
  actionStack.push(-1);
  referrerStack.push(-1);
 
  while (markingStack.size() != 0) {

    
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
//          std::cerr << "inserting edge to existing node: " << currentReferrer << "->" << i << std::endl;
        }
        
      }
      if (found) break;
    }
  
    if (!found) {

      states.insert(currentState);
      dfaStates[currentState] = currentDFAStates;
      values[currentState] = currentValues;
      tokens[currentState] = currentMarking;    
      
      // determine if the state is good. if yes, keep it, otherwise, use bad state 0
      bool good = true;
      for (int pc = 0; pc < policies.size(); ++pc) {
      
        if (!policies[pc]->satisfies(this, currentState)) {
          good = false; break;
        }
      
      } // end for
      
      if (good) {

        if (currentReferrer != -1) {
          (delta[currentReferrer])[currentAction] = currentState;
            // //std::cerr << "delta has entries for " << delta.size() << " states. current: " << delta[currentReferrer].size() << std::endl;
          // //std::cerr << "inserting edge to new node: " << currentReferrer << "->" << currentState << std::endl;
        } // end if
        
        ++currentState;
        int srcState; 
      
        if (currentSrcReferrer == -1) {
          srcState = rg.root;
        } else {
          srcState = rg.yields(currentSrcReferrer, currentAction);
        // //std::cerr << "new src state: " << srcState << std::endl;
        } // end if
      
        std::set<int> enabled = rg.enabledTransitions(srcState);
        //std::cerr << enabled.size() << std::endl;      
        for (std::set<int>::iterator it = enabled.begin(); it != enabled.end(); ++it) {
        
          std::vector<int> currentValuesN;
          std::vector<int> currentDFAStatesN;
        
        
          for (int i = 0; i < currentValues.size(); ++i) {
              currentValuesN.push_back((int) currentValues[i] + (int) (TaraHelpers::getCostFunctionByID(i)->next(*it, currentDFAStates)));
          } // end for

          for (int i = 0; i < currentDFAStates.size(); ++i) {
            currentDFAStatesN.push_back(TaraHelpers::getDFAByID(i)->getNewState(currentDFAStates[i], *it));
          } // end for
        
          // //std::cerr << "pushing: " << rg.markingString(rg.yields(srcState,*it)) << " ref: " << srcState <<  std::endl;        
                  
          referrerStack.push(currentState-1);
          srcReferrerStack.push(srcState);
          actionStack.push(*it);
          dfaStatesStack.push(currentDFAStatesN);
          valuesStack.push(currentValuesN);
          markingStack.push(std::map<int,int>(rg.tokens[rg.yields(srcState,*it)]));
             } 
      } else {
      
        if (currentReferrer != -1) {
          (delta[currentReferrer])[currentAction] = 0;
            // //std::cerr << "delta has entries for " << delta.size() << " states. current: " << delta[currentReferrer].size() << std::endl;
          // //std::cerr << "inserting edge to new node: " << currentReferrer << "->" << currentState << std::endl;
        }
      
      
      }


      }
    
    
      
    
    }

       // //std::cerr << "root hat " << delta[root].size() << " eintraege" << std::endl;  
  
  }
  
    
  
  
  
  std::string CostGraph::stateString(int s) {
   // std::cerr << "go1 " << s << std::endl;
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
  
void CostGraph::print() {
  std::set<int> visited;
  print_r(root,visited);
}

  
  void CostGraph::print_r(int s, std::set<int>& visited) {
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

std::set<Situation> CostGraph::closure(std::set<Situation>& bubble) {
  bool change = true;
  std::set<Situation> result;
  std::stack<Situation> sitStack;
  
  for (std::set<Situation>::iterator it = bubble.begin(); it != bubble.end(); ++it) {
    sitStack.push(*it);    
  //  std::cerr << "pushing " << it->state << std::endl;
  }
  
  while (sitStack.size() != 0) {
  
    Situation s = sitStack.top(); sitStack.pop();
//    bool found = result.find(s) != result.end();
    bool found = result.count(s) == 1;
  //  std::cerr << "situation: " << s.state << " was found? " << found << std::endl;
    if (!found) {
      result.insert(s);
      std::set<int> ens = sit_enabledTransitions(s);
    //  std::cerr << "nr of enabled transitions: " << ens.size() << std::endl;
      for (std::set<int>::iterator it = ens.begin(); it != ens.end(); ++it) {
    //    std::cerr << "push " << sit_yields(s, *it).state << std::endl;
        sitStack.push(sit_yields(s, *it));
      
      }
    
    }
  
  
  }
  //std::cerr << "returning a closure of size " << result.size() << std::endl;
  return result;
}


