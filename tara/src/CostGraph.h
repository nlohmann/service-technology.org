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
  
  CostGraph(ReachabilityGraph& rg);
  void print_r(int s, std::set<int>& visited);
  void print();

  std::string stateString(int s);
  std::string valueString(int s);
  
  std::set<int> sit_enabledTransitions(Situation s) const {
    std::set<int> result;
    int aState = s.state;
    std::set<int> inner = enabledTransitions(aState);
    for (std::set<int>::iterator it = inner.begin(); it != inner.end(); ++it) {
      int l = TaraHelpers::getLabel(*it);
      if ((l == -1) || (!TaraHelpers::getLabelByID(l).incoming) || s.inputBuffer.occ(l) > 0) {
        result.insert(*it);
      }
    }
    
    return result;
  
  }
  
  bool sit_enables(Situation s, int t) {
    int l = TaraHelpers::getLabel(t);  
    return (enables(s.state,t) && ((l == -1) || (!TaraHelpers::getLabelByID(l).incoming) || s.inputBuffer.occ(l) > 0)); 
  }
  
  Situation sit_yields(Situation s, int t) {
    Situation sprime;
    sprime.state = yields(s.state, t);
    int l = TaraHelpers::getLabel(t);
    sprime.inputBuffer = s.inputBuffer;
    sprime.outputBuffer = s.outputBuffer;
    if (l == -1) {
      return sprime;
    }
    if (TaraHelpers::getLabelByID(l).incoming) {
      sprime.inputBuffer.removeOnce(l);
    } else {
      sprime.outputBuffer.insert(l);
    }
    return sprime;
     
  }
  
  std::set<Situation> closure(std::set<Situation>& bubble);

};

#endif
