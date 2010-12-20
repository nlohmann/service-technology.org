#include "Situation.h"
#include "Label.h"
#include "TaraHelpers.h"

Situation Situation::effect(int l) const {
  if (l == -1) {
    return *this;  
  } 
  
  Label& label = TaraHelpers::getLabelByID(l);
  Situation result = *this;
  if (label.incoming) {
    result.inputBuffer.insert(l);         
  } else {
    result.outputBuffer.removeOnce(l);         
  }
  return result;
}

