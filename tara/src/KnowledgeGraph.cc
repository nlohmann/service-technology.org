#include <stack>
#include "KnowledgeGraph.h"

int KnowledgeGraph::insertNode(std::set<Situation> situations) {
  
  for (std::map<int, std::set<Situation> >::iterator it = bubbles.begin(); it != bubbles.end(); ++it) {
   if (it->second == situations) {
      return it->first;
    }
    
  }
  int size = bubbles.size();
  bubbles[size] = situations;
  return size;
}

int KnowledgeGraph::getNode(std::set<Situation> situations) {
  
  for (std::map<int, std::set<Situation> >::iterator it = bubbles.begin(); it != bubbles.end(); ++it) {
   if (it->second == situations) {
      return it->first;
    }
    
  }
  return -1;
}


KnowledgeGraph::KnowledgeGraph(CostGraph* cg) : cg(cg) {
  
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

  bubbleStack.push(initialSet);
  refNodeStack.push(currentRefNode);
  refActionStack.push(currentRefAction);
  
  
  while (bubbleStack.size() != 0) {
    
    std::cerr << "test" << std::endl;
       
    std::set<Situation> currentBubble = bubbleStack.top(); bubbleStack.pop();
    currentNode = getNode(currentBubble);
    currentRefAction = refActionStack.top(); refActionStack.pop();
    currentRefNode = refNodeStack.top(); refNodeStack.pop();
    
    
    if (currentNode == -1) {
      std::cerr << "newnode" << std::endl;
      currentNode = insertNode(currentBubble);
      
      for (int i = 0; i < TaraHelpers::labels.size(); ++i) {
      
        // Get the effect of label on the current bubble
        std::set<Situation> intermediate;
        for (std::set<Situation>::iterator it = currentBubble.begin(); it != currentBubble.end(); ++it) {
          if (!TaraHelpers::getLabelByID(i).incoming || it->inputBuffer.occ(i) > 0) { 
            intermediate.insert(it->effect(i));
          }
        }

        bubbleStack.push(cg->closure(intermediate));
        refNodeStack.push(currentNode);
        refActionStack.push(i);

      }
      
    
    } else {
          std::cerr << "newnode" << std::endl;

    }
      
  (delta[currentRefNode])[currentRefAction] = currentNode;
      
  }
  

}


