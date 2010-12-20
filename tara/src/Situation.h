#ifndef SITUATION_H
#define SITUATION_H

#include "Multiset.h"

class Situation {

public:
  // A situation is a state of a cost graph plus a buffer for incoming and outgoing messages

  int state; 
  Multiset<int> inputBuffer;
  Multiset<int> outputBuffer;  

  bool operator== (const Situation& other) const {
    return (state == other.state) && (inputBuffer == other.inputBuffer) && (outputBuffer == other.outputBuffer);
  } 

  bool operator< (const Situation& other) const {
    return (state == other.state) && (inputBuffer < other.inputBuffer) && (outputBuffer < other.outputBuffer);
  } 

  Situation effect(int l) const;


};


#endif
