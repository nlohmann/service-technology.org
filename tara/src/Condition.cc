#include "Condition.h"
#include "ReachabilityGraph.h"

bool MarkingCondition::satisfies(int state, ReachabilityGraph& rg) {
  
  for (int i = 0; i < rg.places.size(); ++i) {
    
    
    
    if (constraints.find(i) != constraints.end()) {
      int lower = constraints[i].first;
      int upper = constraints[i].second;
    
      if (rg.getTokens(state,i) > upper || rg.getTokens(state,i) < lower) {return false;}

  }
  }
  return true;
}


