#ifndef POLICY_H
#define POLICY_H

#include <vector>
#include "Condition.h"

class CostGraph;
class ReachabilityGraph;

class Policy {
  public:
  MarkingCondition* condition;
  std::map<int, std::pair<int,int> > bounds;
  bool satisfies(CostGraph* cg, int state);
  std::string name;
};

class SimplePolicy : public Policy {
  public: 
  SimplePolicy(std::string filename, ReachabilityGraph* rg);
};

class PermanentPolicy : public Policy {
public:
  PermanentPolicy(std::map<int, std::pair<int,int> > bounds) {
    condition = new PermanentCondition();
    this->bounds = bounds;
  }
};


#endif
