#include <iostream>
#include <cassert>

#include "../src/pnapi.h"

using std::cout;
using std::endl;
using namespace pnapi;

int main(int argc, char *argv[])
{
  // workflow net 1
  PetriNet *n1 = new PetriNet();
  Place *p1 = n1->newPlace("p1");
  Place *p2 = n1->newPlace("p2");
  Transition *t1 = n1->newTransition("t1");
  n1->newArc(p1, t1);
  n1->newArc(t1, p2);

  // initialize marking
  Marking m(n1->initMarking(), 0);

  // set token to the places
  m[p1->getMarkingID()] = 3;
  m[p2->getMarkingID()] = 0;

  // calculate successor
  Marking m2 = n1->calcSuccessorMarking(m, t1);

  // successor has to be p1 = 2 and p2 = 1
  assert(m2 != m);

  int i = 0;
  Transition * t;

  // deadlocks the net calculates all markings until no transition can be activated
  while ((t = n1->findLivingTransition(m2)) != NULL)
  {
    cout << i++ << ". Das Netz lebt noch!\n";
    m2 = n1->calcSuccessorMarking(m2, t);
  }

  cout << i << ". Das Netz lebt nun nicht mehr!\n";

  assert(i == 2);

  return 0;
}
