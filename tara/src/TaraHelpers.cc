#include "TaraHelpers.h"
#include "CostFunction.h"

  static int insertDFA(std::string name, DFA* dfa);
  static int getDFAID(std::string name);
  static DFA* getDFAByName(std::string name);
  static DFA* getDFAByID(int i);

  static int insertCostFunction(std::string name, DFA* costFunction);
  static int getCostFunctionID(std::string name);
  static CostFunction* getCostFunctionByName(std::string name);
  static CostFunction* getCostFunctionByID(int i);


int TaraHelpers::insertDFA(DFA* dfa) {
    int old = getDFAID(dfa->name);
    if (old == -1) {
      int size = automata.size();
      automata.push_back(dfa);
      automataNames[dfa->name] = size;
      return size;
    }
    return old;
}

int TaraHelpers::getDFAID(std::string name) {
  if (automataNames.find(name) == automataNames.end()) {
    return -1;
  }
  return automataNames[name];
}

DFA* TaraHelpers::getDFAByName(std::string name) {
  return getDFAByID(automataNames[name]);
}

DFA* TaraHelpers::getDFAByID(int i) {
  return automata[i];
}

int TaraHelpers::insertCostFunction(CostFunction* costFunction) {
    int old = getCostFunctionID(costFunction->name);
    if (old == -1) {
      int size = costFunctions.size();
      costFunctions.push_back(costFunction);
      costFunctionNames[costFunction->name] = size;
      return size;
    }
    return old;
}

int TaraHelpers::getCostFunctionID(std::string name) {
  if (costFunctionNames.find(name) == costFunctionNames.end()) {
    return -1;
  }
  return costFunctionNames[name];
}

CostFunction* TaraHelpers::getCostFunctionByName(std::string name) {
  return getCostFunctionByID(costFunctionNames[name]);
}

CostFunction* TaraHelpers::getCostFunctionByID(int i) {
  return costFunctions[i];
}


std::vector<DFA*> TaraHelpers::automata;
std::map<std::string, int> TaraHelpers::automataNames;
std::vector<CostFunction*> TaraHelpers::costFunctions;
std::map<std::string, int> TaraHelpers::costFunctionNames;

