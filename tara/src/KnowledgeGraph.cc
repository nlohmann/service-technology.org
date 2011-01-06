#include <stack>
#include <iostream>
#include <cassert>
#include "KnowledgeGraph.h"

int KnowledgeGraph::insertNode(std::set<Situation> situations) {
  
  for (std::map<int, std::set<Situation> >::iterator it = bubbles.begin(); it != bubbles.end(); ++it) {
   if (it->second == situations) {
      return it->first;
    }
    
  }
  int size = bubbles.size();
  bubbles[size] = situations;
  flags[size] = 0;
  return size;
}

int KnowledgeGraph::getNode(std::set<Situation> situations) { 
  for (std::set<Situation>::iterator it = situations.begin(); it != situations.end(); ++it) {
    if (it->state == 0) return 0;
  }
  
  for (std::map<int, std::set<Situation> >::iterator it = bubbles.begin(); it != bubbles.end(); ++it) {
   if (it->second == situations) {
      return it->first;
    }
    
  }
  return -1;
}


KnowledgeGraph::KnowledgeGraph(CostGraph* cg) : cg(cg) {
  
  // insert standard bad state
  Situation bad;
  bad.state = 0;
  std::set<Situation> badStateSet;
  badStateSet.insert(bad);
  int badstate = insertNode(badStateSet);
  assert(badstate == 0);
  
  
  std::stack<int> refNodeStack, refActionStack;
  std::stack<std::set<Situation> > bubbleStack;
  
  std::set<Situation> currentBubble;
  int currentNode = 0;
  
  int currentRefNode = -1;
  int currentRefAction = -1;

  Situation initial; 
  initial.state = cg->root;

  std::set<Situation> initialSet;
  initialSet.insert(initial);  
//  std::cerr << "initial size: " << initialSet.size() << std::endl;

  bubbleStack.push(cg->closure(initialSet));
//  std::cerr << "initial size: " << bubbleStack.top().size() << std::endl;
  refNodeStack.push(currentRefNode);
  refActionStack.push(currentRefAction);
  
  
  while (bubbleStack.size() != 0) {
       
    std::set<Situation> currentBubble = bubbleStack.top(); bubbleStack.pop();
    currentNode = getNode(currentBubble);
    currentRefAction = refActionStack.top(); refActionStack.pop();
    currentRefNode = refNodeStack.top(); refNodeStack.pop();
        
    if (currentNode == -1) {
     // std::cerr << "newnode" << std::endl;
      currentNode = insertNode(currentBubble);
      if (currentBubble.size() > 0) {
        for (int i = 0; i < TaraHelpers::labels.size(); ++i) {
      //    std::cerr << "current label: " << i << std::endl;
          bool overflow = false;
          // Get the effect of label on the current bubble
          std::set<Situation> intermediate;
          for (std::set<Situation>::iterator it = currentBubble.begin(); it != currentBubble.end(); ++it) {
    //        std::cerr << "check it out." << std::endl;
            if (TaraHelpers::getLabelByID(i).incoming || it->outputBuffer.occ(i) > 0) { 
              if ((it->effect(i).inputBuffer.occ(i) + it->effect(i).outputBuffer.occ(i)) > 1) overflow = true;
              intermediate.insert(it->effect(i));
            }
          }
          if (!overflow) {
          bubbleStack.push(cg->closure(intermediate));
          refNodeStack.push(currentNode);
          refActionStack.push(i);
          }
        }
      }      
    
    } else {
//          std::cerr << "ex node" << std::endl;

    }
      
  (delta[currentRefNode])[currentRefAction] = currentNode;

  (invDelta[currentNode]).insert(currentRefNode); 
      
  }
  
  std::cerr << bubbles.size() << " nodes." << std::endl;
  for (int i = 0; i < bubbles.size(); ++i) {
    if (i == 0) {
      std::cerr << i << ": badstate" << std::endl;
    } else {
      std::cerr << i << ": " << bubbles[i].size() << " situations: " << std::endl;
    }
    for (std::set<Situation>::iterator it = bubbles[i].begin(); it != bubbles[i].end(); ++it) {
      std::cerr << "\t";
      it->print(*cg);
      std::cerr << std::endl;
    } 
  }

}

void KnowledgeGraph::print() {
  std::set<int> visited;
  print_r(1,visited);
}

  
  void KnowledgeGraph::print_r(int s, std::set<int>& visited) {
  // //std::cerr << "hallo" << std::endl;
  if (visited.find(s) == visited.end()) {
    visited.insert(s);

    if (delta.find(s) != delta.end()) {
    
      std::map<int,int>& enabled = delta.find(s)->second;
      
      for (std::map<int,int>::iterator it = enabled.begin(); it != enabled.end(); ++it) {
        std::cerr << s << " | " << revLabelString(TaraHelpers::getLabelByID(it->first).channel) << " > " << it->second << std::endl;
        print_r(it->second, visited);
      
      }
    
    }
  }
}

void KnowledgeGraph::printToDot() {
  std::set<int> visited;
  
  std::cout << "digraph{" << std::endl;
  //std::cout << "ratio=1.5" << std::endl;
  std::cout << "edge [fontname=Helvetica fontsize=20]" << std::endl;
  std::cout << "node [fontname=Helvetica fontsize=0]" << std::endl;
//  std::cout << "r [label=\"\", width=0, height=0]" << std::endl;
// std::cout << "r -> 1 [arrowsize=0.5]" << std::endl;
  std::cout << "0 [label=\"\", shape=circle, width=0.2,height=0.2, style=filled, fillcolor=red]" << std::endl;
  std::cout << "1 [label=\"\", shape=circle, width=0.2,height=0.2, style=filled, fillcolor=blue]" << std::endl;
  
  printToDot_r(1,visited);
  
  std::cout << "}" << std::endl;
  
}

  
  void KnowledgeGraph::printToDot_r(int s, std::set<int>& visited) {
  // //std::cerr << "hallo" << std::endl;
  if (flags[s] == 1 && visited.find(s) == visited.end()) {
    visited.insert(s);
    if (s > 1) {
      std::string myLabel;
      for (std::set<Situation>::iterator it = bubbles[s].begin(); it != bubbles[s].end(); ++it) {
        myLabel += cg->valueString(it->state);
      }
      // myLabel = itoa(flags[s]);
      std::cout << s << "[width=0.1, height=0.1 ,label=\"\"]" << std::endl ; 
      // << "[label=\"" << myLabel << "\"]" << std::endl;
    }

    if (delta.find(s) != delta.end()) {
    
      std::map<int,int>& enabled = delta.find(s)->second;
      
      for (std::map<int,int>::iterator it = enabled.begin(); it != enabled.end(); ++it) {
        if (bubbles[it->second].size() != 0 && flags[it->second] == 1) {
          std::cout << s << " -> " << it->second << "[arrowsize=0.8, label=\" " << revLabelString(TaraHelpers::getLabelByID(it->first).channel) << "      \"]" << std::endl;
          printToDot_r(it->second, visited);
  }
      }
    
    }
  }
}

void KnowledgeGraph::reflag() {
  // Start with the bad state's predecessors.
  // Flag all predecessors with 1.
  // If a predecessor was already flagged with 1, do not again look at its predecessors.
  // In the end, flag all 0's with 2's.

  // First flag all states with 0
  for (int i = 0; i < bubbles.size(); ++i) {
    
    if (flags[i] == 0) {
      flags[i] = 0;
    }
    
  }
  
  std::stack<int> toFlag;
  
  // push the bad state
  toFlag.push(0);
  
  while (toFlag.size() > 0) {
    
    int curState = toFlag.top(); toFlag.pop();
    if (flags[curState] != 1) { // Flag itself and queue predeccessors!
      flags[curState] = 1;
      if (invDelta.find(curState) != invDelta.end()) { // if there is at least one predecessor
       std::cerr << "found a predecessor" << std::endl;
      std::set<int>& enabled = invDelta.find(curState)->second; // The map comtaining all predecessors.
      for (std::set<int>::iterator it = enabled.begin(); it != enabled.end(); ++it) { // run over all predecessors. push them on the stack!
        if ((*it) != -1) {
        std::cerr << "pushing " << *it << std::endl;
        toFlag.push(*it);
        }
      }
      
    }
    }

      
  }
  
  // Now run over all states and set flags to 2 if they are not yet set to 1.
  for (int i = 0; i < bubbles.size(); ++i) {
    
    if (flags[i] == 0) {
      flags[i] = 2;
    }
    
  }
  
}


