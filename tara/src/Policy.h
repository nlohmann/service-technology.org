#ifndef POLICY_H
#define POLICY_H

#include <vector>
#include "Condition.h"

class CostGraph;

class Policy {
  public:
  MarkingCondition* condition;
  std::vector<std::pair<int,int> > bounds;
  bool satisfies(CostGraph* cg, int state);
};

class PermanentPolicy : public Policy {
public:
  PermanentPolicy(std::vector<std::pair<int,int> > bounds) {
    condition = new PermanentCondition();
    this->bounds = bounds;
  }
};


#endif
