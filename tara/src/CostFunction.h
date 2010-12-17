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
    SimpleAtomicCondition(int dfa, bool acc); 
};

class StateBasedAtomicCondition : public AtomicCondition {
  public: 
    int dfa;
    int state;
    bool satisfiedBy(std::vector<int>& dfaStates);
    StateBasedAtomicCondition(int dfa, int state);
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
  
  // action -> case -> value
  std::map<int, std::map<int, int> > values;
  
  int next(int action, std::vector<int> dfaStates);
  
  StandardCostFunction(std::string filename);

};

#endif
