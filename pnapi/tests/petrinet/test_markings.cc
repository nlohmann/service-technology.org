#include "test.h"

using namespace pnapi;

int main(int argc, char *argv[])
{
  pnapi::Marking m;
  pnapi::PetriNet net;

  pnapi::Place &p1 = net.createPlace("p1");
  pnapi::Place &p2 = net.createPlace("p2");

  m[*net.findPlace("p1")] = 2;
  //assert(m.getMap().find(&p1) != m.getMap().end());
  m[*net.findPlace("p2")] = 4;
  //assert(m.getMap().find(&p2) != m.getMap().end());

  assert(m[*net.findPlace("p1")] == 2);
  assert(m[*net.findPlace("p2")] == 4);

  net.finalCondition() =
    p1 == 2 && p2 == 4;

  //assert(net.finalCondition().isSatisfied(m));

  return 0;
}
