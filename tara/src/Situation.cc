#include <iostream>
#include "Situation.h"
#include "Label.h"
#include "TaraHelpers.h"
#include "CostGraph.h"

Situation Situation::effect(int l) const {
  if (l == -1) {
    std::cerr << "bad label " << std::endl;
    return *this;  
  } 
  
  Label& label = TaraHelpers::getLabelByID(l);
  Situation result = *this;
  if (label.incoming) {
    result.inputBuffer.insert(l);         
  } else {
    result.outputBuffer.removeOnce(l);         
  }
  //std::cerr << "computed effect, resulting in " << inputBuffer.size() << ", " << outputBuffer.size() << " entries"  << std::endl;
  return result;
}

  void Situation::print(CostGraph& cg) const {
    
    std::cerr << cg.nodeString(state) << " ";
    for (std::set<int>::iterator it = inputBuffer.mCarrier.begin(); it != inputBuffer.mCarrier.end(); ++it) {
      int current = *it;
      std::cerr  << TaraHelpers::getLabelByID(current).channel << ": " << inputBuffer.occ(current) << "; "; 
    }
   for (std::set<int>::iterator it = outputBuffer.mCarrier.begin(); it != outputBuffer.mCarrier.end(); ++it) {
      int current = *it;
      std::cerr  << TaraHelpers::getLabelByID(current).channel << ": " << outputBuffer.occ(current) << "; "; 
    }

  
  }
