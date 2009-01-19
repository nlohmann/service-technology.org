#include <iostream>
#include <fstream>
#include "../src/petrinet.h"
#include "../src/automaton.h"

using std::cout;
using std::ofstream;
using namespace pnapi;
using namespace pnapi::formula;

int main(int argc, char *argv[])
{
  PetriNet net;

  Place &p1 = net.createPlace("p1");
  Place &p2 = net.createPlace("p2");
  Place &p3 = net.createPlace("p3");
  Place &p4 = net.createPlace("p4");
  Place &p5 = net.createPlace("p5");
  Place &p6 = net.createPlace("p6");
  p1.mark(2);

  Place &pi = net.createPlace("a", Node::INPUT);
  Place &po = net.createPlace("b", Node::OUTPUT);

  Transition &t1 = net.createTransition("t1");
  Transition &t2 = net.createTransition("t2");
  Transition &t3 = net.createTransition("t3");
  Transition &t4 = net.createTransition("t4");

  net.createArc(p1, t1);
  net.createArc(p1, t2);
  net.createArc(pi, t1);
  net.createArc(t1, p2);
  net.createArc(t2, p2);
  net.createArc(t2, p3);
  net.createArc(p2, t3, 2);
  net.createArc(p3, t3);
  net.createArc(t3, p4);
  net.createArc(t3, p5);
  net.createArc(p4, t4);
  net.createArc(p5, t4);
  net.createArc(t4, po);
  net.createArc(t4, p6);

  Formula &final = net.getFinalCondition();
  Formula *newFinal = new FormulaGreater(&p1, 6);
  final = *newFinal;

  //PetriNet net2(net);

  Automaton sa(net);

  sa.initialize();
  sa.createAutomaton();

  cout << sa;

  return 0;
}
