#include "test.h"
#include "../../src/automaton.h"
#include "../../src/io.h"

int main(int argc, char *argv[])
{
  PetriNet net;

  Place &p1 = net.createPlace();
  p1.mark(1);
  Place &p2 = net.createPlace();
  Place &p3 = net.createPlace();

  Place &a = net.createPlace("a", Node::INPUT);
  Place &x = net.createPlace("x", Node::OUTPUT);

  Transition &t1 = net.createTransition();
  Transition &t2 = net.createTransition();

  net.createArc(p1, t1);
  net.createArc(a, t1);
  net.createArc(t1, p2);
  net.createArc(p2, t2);
  net.createArc(t2, x);
  net.createArc(t2, p3);

  net.finalCondition() =
    p1 == 0 && p2 == 0 && p3 > 0;


  // automaton construction from PetriNet instance
  begin_test("Automaton::Automaton() [construction]");
  pnapi::ServiceAutomaton sauto(net);
  cout << pnapi::io::sa << sauto;
  end_test();

  return 0;
}
