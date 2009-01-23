#include "test.h"

int main(int argc, char * argv[])
{
  {
    PetriNet net = createExamplePetriNet();
    int nNodes = net.getNodes().size();

    begin_test("PetriNet::PetriNet() [copy constructor]");
    PetriNet copy = net;
    assert(copy.getNodes().size() == nNodes);
    end_test();
   
    begin_test("PetriNet::operator=() [assignment operator]");
    PetriNet assign = createExamplePetriNet();
    assign = net;
    assert(assign.getNodes().size() == nNodes);
    end_test();

    begin_test("PetriNet::~PetriNet() [destruction after copy]");
  }
  end_test();
}
