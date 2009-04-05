#include <fstream>
#include <set>
#include "test.h"

int main(int argc, char * argv[])
{
  pnapi::PetriNet net1;
  std::ifstream ifs("test_statemachine.net.owfn");
  
  ifs >> pnapi::io::owfn >> net1;
  
  pnapi::ServiceAutomaton sa(net1);
  
  pnapi::PetriNet net2 = sa.toStateMachine();
  
  assert(net2.getPlaces().size() == 4);
  assert(net2.getTransitions().size() == 4);
  assert(net2.getFinalCondition().concerningPlaces().size() == 1);
  
  int marked = 0;
  for(std::set<pnapi::Place*>::iterator p = net2.getPlaces().begin();
        p != net2.getPlaces().end(); ++p)
  {
    if((*p)->getTokenCout() > 0)
      ++marked;
  }
  
  assert(marked == 1);
  
  return 0;
}
