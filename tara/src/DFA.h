#ifndef DFA_H
#define DFA_H

#include <map>
#include <set>
#include <vector>
#include <string>

class DFA {
public:
  std::map<std::string, int> stateNames;
  std::vector<std::string> states;
  std::map<std::string, int> actions;
  int initialState;  
  std::set<int> finalStates;
  
  std::map<int, std::map<int,int> > flow;
  
  int getNewState(int oldState, int action);
  
  bool isFinal(int state);
  bool accepts(std::vector<int> word);
 
  
  DFA(){}
  DFA(std::string filename);
  
  std::string name;
};

#endif
