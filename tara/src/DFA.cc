#include <libconfig.h++>
#include "DFA.h"
#include "TaraHelpers.h"
using namespace libconfig;

int DFA::getNewState(int oldState, int action) {

  if ((flow[oldState]).find(action) == (flow[oldState]).end()) {
    return oldState;
  }
  
  return (flow[oldState])[action];

}

bool DFA::accepts(std::vector<int> word) {

  int currentState = initialState;
  
  for (int i = 0; i < word.size(); ++i) {
    
    currentState = getNewState(currentState, word[i]);
  
  }
  
  return isFinal(currentState);

}

bool DFA::isFinal(int state) {
  
  return (finalStates.find(state) != finalStates.end());

}

DFA::DFA(std::string filename) {

  Config dfa;
  dfa.readFile(filename.c_str());
  
  dfa.lookupValue("name", name);
  
  Setting& cStates = dfa.lookup("states");
  for (int i = 0; i < cStates.getLength(); ++i) {
    stateNames[cStates[i]] = states.size();
    states.push_back(cStates[i]);     
  }
  
  dfa.lookupValue("initial", name);

  initialState = stateNames[name];

  Setting& cFinal = dfa.lookup("final");
  for (int i = 0; i < cFinal.getLength(); ++i) {
      finalStates.insert(stateNames[cFinal[i]]);
  }


/*  Setting& cActions = dfa.lookup("actions");
  for (int i = 0; i < cActions.getLength(); ++i) {
      actions[cActions[i]] = i;
  }
*/
 
  Setting& cTransitions = dfa.lookup("transitions");
  for (int i = 0; i < cTransitions.getLength(); ++i) {
      std::string s1 = cTransitions[i][0];
      std::string a = cTransitions[i][1];
      std::string s2 = cTransitions[i][2];
      (flow[stateNames[s1]])[TaraHelpers::insertTransition(a)] = stateNames[s2];
  }
  
    
}



