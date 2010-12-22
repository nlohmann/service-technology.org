#include "Policy.h"
#include "CostGraph.h"

bool Policy::satisfies(CostGraph* cg, int state) {

  if (!condition->satisfies(state, cg)) {
  
    return true;
  
  }
  
  for (int i = 0; i < bounds.size(); ++i) {
    
    int low = bounds[i].first;
    int up = bounds[i].second;
    
    if (low > (cg->values[state])[i] || up < (cg->values[state])[i]) {
    
      return false;
    
    }
    
  
  }
  
  return true;

}
