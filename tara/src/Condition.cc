#include "Condition.h"
#include "CostGraph.h"

bool MarkingCondition::satisfies(int state, CostGraph* cg) {
  
  for (int i = 0; i < cg->places.size(); ++i) {
    
    
    
    if (constraints.find(i) != constraints.end()) {
      int lower = constraints[i].first;
      int upper = constraints[i].second;
    
      if (cg->getTokens(state,i) > upper || cg->getTokens(state,i) < lower) {return false;}

  }
  }
  return true;
}


