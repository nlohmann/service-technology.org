#include "test.h"

int main(int argc, char * argv[])
{
  PetriNet net;

  Place & p1 = net.createPlace();
  Place & p2 = net.createPlace();

  net.finalCondition() = p1 == 1 || p2 > 3 && p1 >= 2 || p1 < 4 && p2 <= 7;
}
