#ifndef COSTFUNCTION_H
#define COSTFUNCTION_H

#include <vector>
#include <map>
#include "TaraHelpers.h"
#include "DFA.h"

class CostFunction {

  public: 
  
  std::string name;
  virtual int next(int action, std::vector<int>& dfaStates);

};

class AtomicCondition {
  public: 
  virtual bool satisfiedBy(std::vector<int>& dfaStates) = 0;
};

class SimpleAtomicCondition : public AtomicCondition {
  public: 
    int dfa;
    bool acc;
    bool satisfiedBy(std::vector<int>& dfaStates);
};

class StateBasedAtomicCondition : public AtomicCondition {
  public: 
    int dfa;
    int state;
    bool satisfiedBy(std::vector<int>& dfaStates);
};


class Condition {
public:
  std::vector<AtomicCondition*> atoms;
  bool satisfiedBy(std::vector<int>& dfaStates);

};



class StandardCostFunction : public CostFunction {

public:

  std::set<int> automata;
  std::vector<Condition> cases;
  std::map<std::string, int> caseNames;
  std::map<int, std::map<int, int> > values;
  
  int next(int action, std::vector<int> dfaStates);
  
  

};

#endif
