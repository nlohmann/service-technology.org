#include "CostFunction.h" 

int CostFunction::next(int action, std::vector<int>& dfaStates) {
  return 0;
}

bool SimpleAtomicCondition::satisfiedBy(std::vector<int>& dfaStates) {
  return (acc && TaraHelpers::getDFAByID(dfa)->isFinal(dfaStates[dfa])) || (!acc && !TaraHelpers::getDFAByID(dfa)->isFinal(dfaStates[dfa]));
}

bool StateBasedAtomicCondition::satisfiedBy(std::vector<int>& dfaStates) {
  return (dfaStates[dfa] == state);
}

bool Condition::satisfiedBy(std::vector<int>& dfaStates) {
  for (int i = 0; i < atoms.size(); ++i) {
    if (!atoms[i]->satisfiedBy(dfaStates)) {
      return false;
    }
  }
  return true;
}

int StandardCostFunction::next(int action, std::vector<int> dfaStates) {

  for (int i = 0; i < cases.size(); ++i) {
    if (cases[i].satisfiedBy(dfaStates) && ((values[action]).find(i) != (values[action]).end())) {
      return (values[action])[i];
    }
  }
  
  return 0;

}

