#include "test.h"

int main(int argc, char * argv[])
{
  PetriNet net = createExamplePetriNet();

  /*
  // merge places
  begin_test("Place::merge() [place merging]");
  string s1 = "ready to produce", s2 = "ready to deliver";
  Place * p1 = net.findPlace(s1);
  Place * p2 = net.findPlace(s2);
  p1->merge(*p2);
  p1 = net.findPlace(s1);
  p2 = net.findPlace(s2);
  assert(p1 != NULL);
  assert(p2 != NULL);
  assert(p1 == p2);
  end_test();  
  */


  /*
  // merge transitions
  begin_test("Transition::merge() [transition merging]");
  Transition * t1 = net.findTransition("produce");
  Transition * t2 = net.findTransition("consume");
  t1->merge(*t2);
  t1 = net.findTransition("produce");
  t2 = net.findTransition("consume");
  assert(t1 != NULL);
  assert(t2 != NULL);
  assert(t1 == t2);
  end_test();
  */


  // composition
  PetriNet net1;
  net1.createPlace("p1", Node::OUTPUT);
  net1.createPlace("p2", Node::INPUT);
  net1.createPlace("p3", Node::INPUT);

  PetriNet net2;
  net2.createPlace("p1", Node::INPUT);
  net2.createPlace("p7");

  begin_test("PetriNet::compose() [composition]");
  net1.compose(net2);
  //cout << pnapi::io::owfn << net1;
  assert(net1.getPlaces().size() == 6);
  assert(net1.getInputPlaces().size() == 2);
  assert(net1.getOutputPlaces().empty());
  end_test();

  return 0;
}
