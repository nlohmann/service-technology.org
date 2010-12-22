#ifndef SITUATION_H
#define SITUATION_H

#include <iostream>
#include "Multiset.h"

class CostGraph;

class Situation {

public:
  // A situation is a state of a cost graph plus a buffer for incoming and outgoing messages

  int state; 
  Multiset<int> inputBuffer;
  Multiset<int> outputBuffer;  

  bool operator== (const Situation& other) const {
    //std::cerr << "comparing (==)" << state << " and " << other.state << std::endl;
    return (state == other.state) && (inputBuffer == other.inputBuffer) && (outputBuffer == other.outputBuffer);
  } 

  bool operator< (const Situation& other) const {
    //std::cerr << "comparing (<)" << state << " and " << other.state << std::endl;
    // return (state < other.state) && (inputBuffer < other.inputBuffer) && (outputBuffer < other.outputBuffer);
    // return !(other == *this);
    
    if (state < other.state) {
      return true;
    }
    if (state == other.state) {
      if (inputBuffer < other.inputBuffer) {
        
        return true;
      
      }
      
      if (inputBuffer == other.inputBuffer) {
      
        if (outputBuffer < other.outputBuffer) {
        
          return true;  
          
        }
      
      }
        
    }
    
    return false;
    
  } 

  Situation effect(int l) const;

  void print(CostGraph& cg) const;

};


#endif
