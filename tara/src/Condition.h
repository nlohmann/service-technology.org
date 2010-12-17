#ifndef CONDITION_H
#define CONDITION_H

#include <map>
#include "ReachabilityGraph.h"

class ReachabilityGraph;

class Condition {
  
  
public: 
  bool satisfies(int state, ReachabilityGraph& rg);
  std::map<int, std::pair<int, int> > constraints;

};

#endif
