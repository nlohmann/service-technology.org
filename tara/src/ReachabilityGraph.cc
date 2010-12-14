#include <cstring>
#include <iostream>
#include "ReachabilityGraph.h"

bool ReachabilityGraph::enables(int s, int t) {
  return (delta.find(std::pair<int,int>(s,t)) != delta.end());
}

int ReachabilityGraph::yields(int s, int t) {
  if (enables(s,t)) {
      return delta[std::pair<int,int>(s,t)];
  } 
  return -1;
}

std::set<int> ReachabilityGraph::enabledTransitions(int s) {
  
  std::set<int> enabled; 
  for (int i = 0; i < transitions.size(); ++i) {
  
    if (enables(s,i)) {
      enabled.insert(i);
    }
  
  }
  return enabled;
  
  
}

int ReachabilityGraph::insertTransition(char* t) {

  if (transitions.size() == 0) root = 0;

  for (int i = 0; i < transitions.size(); ++i) {
    if (strcmp(t,transitions[i]) == 0) return i;
  }
  
  transitions.push_back(t);
  return transitions.size()-1;
  
}

void ReachabilityGraph::print() {
  std::set<int> visited;
  print_r(root,visited);
}

void ReachabilityGraph::print_r(int s, std::set<int> visited) {
  if (visited.find(s) == visited.end()) {
    visited.insert(s);
    std::set<int> ens = enabledTransitions(s);
    for (std::set<int>::iterator it = ens.begin(); it != ens.end(); ++it) {
      std::cerr << s << "|" << transitions[(*it)] << ">" << yields(s,*it) << std::endl;
      print_r(yields(s,*it), visited);
    }
  }
}
