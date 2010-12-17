#ifndef TARAHELPERS_H
#define TARAHELPERS_H

#include <map>
#include <string>

#include "DFA.h"

class CostFunction;

class TaraHelpers {
public:

  static std::vector<DFA*> automata;
  static std::map<std::string, int> automataNames;
  static std::vector<CostFunction*> costFunctions;
  static std::map<std::string, int> costFunctionNames;

  static int insertDFA(DFA* dfa);
  static int getDFAID(std::string name);
  static DFA* getDFAByName(std::string name);
  static DFA* getDFAByID(int i);

  static int insertCostFunction(CostFunction* costFunction);
  static int getCostFunctionID(std::string name);
  static CostFunction* getCostFunctionByName(std::string name);
  static CostFunction* getCostFunctionByID(int i);


};

#endif
