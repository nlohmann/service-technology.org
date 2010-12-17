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


};

#endif
