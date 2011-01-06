#include <libconfig.h++>

#include "Policy.h"
#include "ReachabilityGraph.h"
#include "CostGraph.h"
using namespace libconfig;

bool Policy::satisfies(CostGraph* cg, int state) {

  if (!condition->satisfies(state, cg)) {
  
    return true;
  
  }
  
  for (std::map<int, std::pair<int,int> >::iterator it = bounds.begin(); it != bounds.end(); ++it) {
    
    int low = it->second.first;
    int up = it->second.second;
    
    if (low > (cg->values[state])[it->first] || up < (cg->values[state])[it->first]) {
    
      return false;
    
    }
    
  
  }
  
  return true;

}

SimplePolicy::SimplePolicy(std::string filename, ReachabilityGraph* rg) {
      Config p;
  p.readFile(filename.c_str());
  
  p.lookupValue("name", name);
  
  // read the condition
  
  MarkingCondition* mc = new MarkingCondition();

  Setting& cCondition = p.lookup("condition");
  
  for (int i = 0; i < cCondition.getLength(); ++i) {
      std::string placeName;
      p.lookupValue(cCondition[i].getPath() + ".place", placeName);
      int place = rg->insertPlace((char*) placeName.c_str());
      Setting& cPlaceBounds = p.lookup(cCondition[i].getPath() + ".bounds");
      mc->constraints[place] = std::pair<int,int>(cPlaceBounds[0],cPlaceBounds[1]);
  }
  
  condition = mc;
  
  // read the required values for the cost functions
  
  Setting& cRequire = p.lookup("require");
  for (int i = 0; i < cRequire.getLength(); ++i) {
  
      std::string cfName; 
      p.lookupValue(cRequire[i].getPath() + ".function", cfName);
      int cf = TaraHelpers::getCostFunctionID(cfName);
      Setting& cFBounds = p.lookup(cRequire[i].getPath() + ".bounds");

      bounds[cf] = std::pair<int,int>(cFBounds[0],cFBounds[1]);
    
  }
  
}
