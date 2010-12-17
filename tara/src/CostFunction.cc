#include <libconfig.h++>
#include <iostream>

#include "CostFunction.h" 
using namespace libconfig;
int CostFunction::next(int action, std::vector<int>& dfaStates) {
  return 0;
}

bool SimpleAtomicCondition::satisfiedBy(std::vector<int>& dfaStates) {
  return (acc && TaraHelpers::getDFAByID(dfa)->isFinal(dfaStates[dfa])) || (!acc && !TaraHelpers::getDFAByID(dfa)->isFinal(dfaStates[dfa]));
}

SimpleAtomicCondition::SimpleAtomicCondition(int dfa, bool acc) :
  dfa(dfa),
  acc(acc) {
}


bool StateBasedAtomicCondition::satisfiedBy(std::vector<int>& dfaStates) {
  return (dfaStates[dfa] == state);
}

StateBasedAtomicCondition::StateBasedAtomicCondition(int dfa, int state) :
  dfa(dfa),
  state(state) {
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
  if (values.find(action) != values.end()) {
    std::map<int, int>& mapping = values[action];
    for (int i = 0; i < cases.size(); ++i) {
      if (cases[i].satisfiedBy(dfaStates) && mapping.find(i) != mapping.end()) {
        return mapping[i];
      }
    }
  }  
  return 0;

}


StandardCostFunction::StandardCostFunction(std::string filename) {

  Config dfa;
  dfa.readFile(filename.c_str());
  
  dfa.lookupValue("name", name);
  
  // cases 
  Setting& cCases = dfa.lookup("cases");
  for (int i = 0; i < cCases.getLength(); ++i) {
    Setting& cCase = cCases[i];
    std::string caseName;
    cCase.lookupValue("name", caseName);
    Setting& cCondition = dfa.lookup(cCase.getPath() + ".condition");
    
    cases.push_back(Condition());
    Condition& currentCase = cases[cases.size()-1];
    caseNames[caseName] = cases.size() -1; 

    for (int j = 0; j < cCondition.getLength(); ++j) {
      Setting& cAtom = cCondition[j];
      std::string cDFA = cAtom[0];
      std::string cState = cAtom[1];
      if (cState == "_ACCEPT_") {
        currentCase.atoms.push_back(new SimpleAtomicCondition(TaraHelpers::getDFAID(cDFA), true));
      } else if (cState == "_REJECT_") { 
        currentCase.atoms.push_back(new SimpleAtomicCondition(TaraHelpers::getDFAID(cDFA), false));              
      } else {
        currentCase.atoms.push_back(new StateBasedAtomicCondition(TaraHelpers::getDFAID(cDFA), TaraHelpers::getDFAByName(cDFA)->stateNames[cState]));                      
      }

    }
  }
  
  // values
  Setting& cValues = dfa.lookup("values");
  for (int i = 0; i < cValues.getLength(); ++i) {
    Setting& cValueTriple = cValues[i];
    std::string action, caseName;
    int value; 
    cValueTriple.lookupValue("action", action);
    cValueTriple.lookupValue("case", caseName);
    cValueTriple.lookupValue("value", value);
    int transitionID = TaraHelpers::insertTransition(action);
    int caseID = caseNames[caseName];
    (values[transitionID])[caseID] = value;
    
  }  


}
