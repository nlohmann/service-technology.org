#include <cassert>

#include "test.h"
#include "../src/pnapi.h"

using pnapi::PetriNet;
using pnapi::Node;


int main(int argc, char * argv[])
{
  PetriNet net1;
  net1.createPlace("p1", Node::OUTPUT);
  net1.createPlace("p2", Node::INPUT);
  net1.createPlace("p3", Node::INPUT);

  PetriNet net2;
  net2.createPlace("p1", Node::INPUT);
  net2.createPlace("p7");

  begin_test("PetriNet::compose()");
  net1 += net2;
  assert(net1.getPlaces().size() == 4);
  assert(net1.getInputPlaces().size() == 2);
  assert(net1.getOutputPlaces().empty());
  end_test();

  return 0;
}
