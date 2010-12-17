#ifndef REACHABILITYGRAPH_H
#define REACHABILITYGRAPH_H

#include <map>
#include <vector>
#include <set>
#include <string>
#include "Condition.h"

class MarkingCondition;

class ReachabilityGraph {
  public:
  int root;
  std::map<int, std::map<int, int> > delta;
  std::vector<char*> transitions;
  std::map<int, std::map<int,int> > tokens;
  std::vector<char*> places;
  
  std::set<int> states;
  void removeNode(int s);
  
  int insertTransition(char* t);
  int insertPlace(char* p);
  int getTokens(int state, int place);
    
  std::set<int> enabledTransitions(int s);
  bool enables(int s, int t);
  int yields(int s, int t);  
  
  void print();
  void print_r(int s, std::set<int> visited);

  std::string markingString(int s);

  std::set<int> reachableNodes(int state);

  void removeIndifferents(MarkingCondition& condition);
};

#endif
