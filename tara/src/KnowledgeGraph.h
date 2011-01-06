#ifndef KNOWLEDGEGRAPH_H
#define KNOWLEDGEGRAPH_H

#include <map>
#include "Multiset.h"
#include "CostGraph.h"
#include "Situation.h"

class KnowledgeGraph {
public:  
  // The underlying cost graph
  CostGraph* cg;
  
  // A node is annotated with a multiset of situations
  std::map<int, std::set<Situation> > bubbles;
  
  // nodes -> labels -> nodes
  std::map<int, std::map<int, int> > delta;

  // inverse predecessor map
  std::map<int, std::set<int> > invDelta;


  // 1 - bad state reachable, 2 - bad state not reachable, 0 - no idea
  std::map<int, int> flags;

  int getNode(std::set<Situation> situations);    
  
  // Nodes should only be inserted if they do not exist yet
  int insertNode(std::set<Situation> situations);    

  // A knowledge graph is computed from a cost graph
  KnowledgeGraph(CostGraph* cg);
  
  void print_r(int s, std::set<int>& visited);
  void print();
  
  void printToDot_r(int s, std::set<int>& visited);
  void printToDot();
  
  void reflag();
  
};

#endif
