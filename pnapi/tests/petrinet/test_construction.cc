#include "test.h"

int main(int argc, char * argv[])
{
  {
    // constructor
    begin_test("PetriNet::PetriNet() [standard constructor]");
    PetriNet net;
    end_test();

    
    // create*()
    begin_test("PetriNet::create*() [component construction]");
    Place & p1 = net.createPlace();
    assert(!p1.getName().empty());
    Place & p2 = net.createPlace("testPlace1");
    Place & p3 = net.createPlace("testPlace2", Place::INPUT);
    Place & p4 = net.createPlace("testPlace3", Place::OUTPUT);

    Transition & t1 = net.createTransition();
    assert(!t1.getName().empty());
    Transition & t2 = net.createTransition("testTrans");

    Arc & arc1 = net.createArc(p1, t1);
    Arc & arc2 = net.createArc(t2, p2);
    Arc & arc3 = net.createArc(p3, t1);
    end_test();

    
    // get[Component]*()
    begin_test("PetriNet::get*() [component querying]");
    assert(net.getNodes().size() == 6);
    assert(net.getTransitions().size() == 2);
    assert(net.getPlaces().size() == 4);
    assert(net.getInterfacePlaces().size() == 2);
    assert(net.getInputPlaces().size() == 1);
    assert(net.getOutputPlaces().size() == 1);
    end_test();


    // find*()
    begin_test("PetriNet::contains/find*() [component querying]");
    assert(net.containsNode(p2));
    assert(net.containsNode("testTrans"));
    assert(!net.containsNode("nonexistingNode"));
    assert(net.findArc(p3, t1) == &arc3);
    assert(net.findArc(p1, t2) == NULL);
    assert(net.findNode(p1.getName()) == &p1);
    assert(net.findNode("testPlace3") == &p4);
    assert(net.findNode("nonexistingNode") == NULL);
    assert(net.findPlace("testPlace2") == &p3);
    assert(net.findPlace(t1.getName()) == NULL);
    assert(net.findTransition("testTrans") == &t2);
    assert(net.findTransition(p4.getName()) == NULL);
    end_test();

    
    // destructor
    begin_test("PetriNet::~PetriNet() [destructor]");
  }
  end_test();

  return 0;
}
