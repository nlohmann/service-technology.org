#include <iostream>
#include <fstream>
#include <cassert>
#include "../src/pnapi.h"

using namespace PNapi;
using std::cout;
using std::ofstream;

void draw(PetriNet *n, char* name);

int main(int argc, char *argv[])
{
  cout << "Testing isWorkflowNet()-method ...\n";

  // workflow net 1
  PetriNet *n1 = new PetriNet();
  Place *p1 = n1->newPlace("p1");
  Place *p2 = n1->newPlace("p2");
  Transition *t1 = n1->newTransition("t1");
  n1->newArc(p1, t1);
  n1->newArc(t1, p2);

  draw(n1, "nets/simpleWorkflow.dot");

  // a really simple net P->T->P (net 1)
  assert(n1->isWorkflowNet() == true);

  // extending workflow net 1
  Transition *t2 = n1->newTransition("t2");
  Transition *t3 = n1->newTransition("t3");
  Place *p3 = n1->newPlace("p3");
  Place *p4 = n1->newPlace("p4");

  n1->newArc(t2, p3);
  n1->newArc(p3, t3);
  n1->newArc(t3, p4);
  n1->newArc(p4, t2);

  n1->newArc(p1, t2);

  draw(n1, "nets/nonWorkflow1.dot");

  // a net with two strongly connected components (net 2)
  assert(n1->isWorkflowNet() == false);

  n1->newArc(t2, p2);

  draw(n1, "nets/repWorkflow1.dot");

  // net 2 repaired (net 3)
  assert(n1->isWorkflowNet() == true);

  Transition *td = n1->newTransition("dead transition");
  n1->newArc(td, p3);

  draw(n1, "nets/deadWorkflow.dot");

  // net 3 with a dead transition (net 4)
  assert(n1->isWorkflowNet() == false);

  PetriNet *n2 = new PetriNet();
  p1 = n2->newPlace("p1");
  p2 = n2->newPlace("p2");
  p3 = n2->newPlace("p3");
  p4 = n2->newPlace("p4");
  t1 = n2->newTransition("t1");
  t2 = n2->newTransition("t2");
  t3 = n2->newTransition("t3");

  n2->newArc(p1, t1);
  n2->newArc(t1, p2);
  n2->newArc(t2, p2);
  n2->newArc(t2, p3);
  n2->newArc(p3, t3);
  n2->newArc(t3, p4);
  n2->newArc(p4, t2);

  draw(n2, "nets/nonWorkflow2.dot");

  // mirrored net 2 (net 5)
  assert(n2->isWorkflowNet() == false);

  n2->newArc(p1, t2);

  draw(n2, "nets/repWorkflow2.dot");

  // repaired net 5 (net 6)
  assert(n2->isWorkflowNet() == true);

  PetriNet *n3 = new PetriNet();
  p1 = n3->newPlace("p1");
  p2 = n3->newPlace("p2");
  p3 = n3->newPlace("p3");
  p4 = n3->newPlace("p4");
  t1 = n3->newTransition("t1");
  t2 = n3->newTransition("t2");
  t3 = n3->newTransition("t3");

  n3->newArc(p1, t1);
  n3->newArc(t1, p2);
  n3->newArc(t1, p3);
  n3->newArc(p3, t2);
  n3->newArc(t2, p4);
  n3->newArc(p4, t3);
  n3->newArc(t3, p3);

  draw(n3, "nets/nonWorkflow3.dot");

  // net with two strongly connected components
  assert(n3->isWorkflowNet() == false);

  cout << "... Ok\n";

  return 0;
}

void draw(PetriNet *n, char *name)
{
  n->set_format(FORMAT_DOT);
  ofstream f;
  f.open(name);
  f << (*n);
  f.close();
}
