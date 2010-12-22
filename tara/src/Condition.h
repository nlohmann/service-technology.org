#ifndef CONDITION_H
#define CONDITION_H

#include <map>

class CostGraph;

class MarkingCondition {
  
  
public: 
  virtual bool satisfies(int state, CostGraph* cg);
  std::map<int, std::pair<int, int> > constraints;

};

class PermanentCondition : public MarkingCondition {
  virtual bool satisfies(int state, CostGraph* cg) {return true;}
};


#endif
