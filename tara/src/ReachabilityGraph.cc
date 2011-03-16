#include <cstring>
#include <iostream>
#include <sstream>
#include <cassert>
#include "ReachabilityGraph.h"
#include "TaraHelpers.h"

bool ReachabilityGraph::enables(int s, int t) const{
  
  if (delta.find(s) != delta.end()) {
  
  const std::map<int, int>& deltaAtS = (delta.find(s)->second);
  assert(deltaAtS.end() == delta.find(s)->second.end());
  if (deltaAtS.find(t) != deltaAtS.end()) {
    int succ = deltaAtS.find(t)->second;
    return true;
  }
  }

  return false;
}

int ReachabilityGraph::yields(int s, int t) const{
  if (enables(s,t)) {
      return (delta.find(s)->second).find(t)->second;
  } 
  return -1;
}

std::set<int> ReachabilityGraph::enabledTransitions(int s) const{
  
  std::set<int> enabled; 
  for (int i = 0; i < TaraHelpers::transitions.size(); ++i) {
  
  if (enables(s,i)) {
      enabled.insert(i);
    }
  
  }
//  std::cerr << "returning set sized " << enabled.size() << std::endl;
  return enabled;
  
  
}


int ReachabilityGraph::insertPlace(char* t) {

  if (places.size() == 0) root = 0;

  for (int i = 0; i < places.size(); ++i) {
    if (strcmp(t,places[i]) == 0) return i;
  }
  
  places.push_back(t);
  return places.size()-1;
  
}

int ReachabilityGraph::getTokens(int state, int place) {

  if (tokens[state].find(place) != tokens[state].end()) {
    return (tokens[state])[place];
  }
  
  return 0;

}


void ReachabilityGraph::print() {
  std::set<int> visited;
  print_r(root,visited);
}

void ReachabilityGraph::print_r(int s, std::set<int>& visited) {
  // //std::cerr << "hallo" << std::endl;
  if (visited.find(s) == visited.end()) {
    visited.insert(s);
    std::set<int> ens = enabledTransitions(s);
  //  std::cerr << "nr of edges: " << ens.size()<< std::endl;
    for (std::set<int>::iterator it = ens.begin(); it != ens.end(); ++it) {
      std::cerr << s << " (" << markingString(s) << ")" << " | " << TaraHelpers::getTransitionByID(*it) << " > " << yields(s,*it) << " (" << markingString(yields(s,*it)) << ") " << std::endl;
      print_r(yields(s,*it), visited);
    }
  }
}



std::string ReachabilityGraph::markingString(int s) {
  std::string st = " "; 
  for (int i = 0; i < places.size(); ++i) {
    if ((tokens[s])[i] > 0) 
      st += std::string(places[i]) +  ": " + itoa((tokens[s])[i]) + "; "; 
  }  
  return st;
}

void ReachabilityGraph::removeNode(int s) {
  states.erase(s);
  if (root == s) {
    root = -1;
  }
}


std::set<int> ReachabilityGraph::reachableNodes(int state) {
  
  std::set<int> result;
  result.insert(state);
  bool changed = true;
  while (changed) {
    changed = false;
    std::set<int> thisRound;
    
    for (std::set<int>::iterator it = result.begin(); it != result.end(); ++it) {
      int s = *it;
      std::set<int> ens = enabledTransitions(s);
      for (std::set<int>::iterator zit = ens.begin(); zit != ens.end(); ++zit) {
      int sprime = yields(s, *zit);
      if (result.find(sprime) == result.end() && thisRound.find(sprime) == thisRound.end()) {
        thisRound.insert(sprime);
        changed = true;
      }
    }
    
    result.insert(thisRound.begin(), thisRound.end());  
    
    }
  
  } 
  
  return result;

}
/*
void ReachabilityGraph::removeIndifferents(MarkingCondition& condition) {

  std::set<int> redNodes, toRemove;
  for (std::set<int>::iterator it = states.begin(); it != states.end(); ++it) {
    if (!condition.satisfies(*it, this)) {
      redNodes.insert(*it);
      // //std::cerr << "red node " << *it << std::endl;
    }  
  }  

  for (std::set<int>::iterator it = redNodes.begin(); it != redNodes.end(); ++it) {
    std::set<int> reachs = reachableNodes(*it);
    if (subset(reachs,redNodes)) {
      toRemove.insert(*it);
      // //std::cerr << "scheduling " << *it << std::endl;
    }  
  }  
  
  for (std::set<int>::iterator it = toRemove.begin(); it != toRemove.end(); ++it) {
      removeNode(*it);
  }  

  

}

*/

/*

void ReachabilityGraph::removeIndifferents(Condition& condition) {

  std::set<int> toRemove, visited;
  std::set<int> redNodes;
  for (std::set<int>::iterator it = states.begin(); it != states.end(); ++it) {
    if (!condition.satisfies(*it, *this)) {
      redNodes.insert(*it);
    }  
  }
  
  
  

}


void ReachabilityGraph::removeIndifferents(Condition& condition) {

  std::set<int> toRemove, visited;
  
  bool rootgreen = removeIndifferents_r(condition, root, toRemove, visited);
  
  for (std::set<int>::iterator it = toRemove.begin(); it != toRemove.end(); ++it) {
    
    removeState(*it);
  
  }
  if (!rootgreen) {
    
    root = -1;
  
  }
  
  

}

bool ReachabilityGraph::removeIndifferents_r(Condition& condition, int state, std::set<int>& visited, std::set<int>& toRemove) {
  
  if (visited.find(state) == visited.end()) {
    
    visited.insert(state);
    std::set<int> ens = enabledTransitions(state);
    bool satisfactorReachable = false;
    for (std::set<int>::iterator it = ens.begin(); it != ens.end(); ++it) {
      int succ = yields(s,*it);
      satisfactorReachable = satisfactorReachable || removeIndifferents_r(condition, succ, visited, toRemove);
    }
    if (!satisfactorReachable && !condition.satisfies(state, *this)) {
      toRemove.insert(state);
      return false;
    }
    return true;     
  
  } 
  
  return (toRemove.find(state) == toRemove.end());  // rly???
  
   // identify transitions that may fire
   // for each transition call recursively
   // disjunction of results
   // if true, return true
   // if false and does not satisfy, set bit
   
   

}

*/