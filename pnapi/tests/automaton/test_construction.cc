#include "test.h"
#include "../../src/automaton.h"
#include "../../src/io.h"
#include <iostream>
#include <fstream>

using std::ifstream;
using namespace pnapi;

int main(int argc, char *argv[])
{
  PetriNet net[3];
  ifstream ifstr;

  begin_test("Automaton::Automaton() [construction] with normal net");
  ifstr.open("sequence_normal.owfn", std::ios_base::in);
  try
  {
  ifstr >> io::owfn >> net[0];
  ifstr.close();
  }
  catch (io::InputError error)
  {
    std::cout << error << "\n";
  }
//  cout << io::owfn << net[0];
  ifstr.open("sequence_unnormal.owfn", std::ios_base::in);
  try
  {
  ifstr >> io::owfn >> net[1];
  ifstr.close();
  }
  catch (io::InputError error)
  {
    std::cout << error << "\n";
  }

/*  ifstr.open("strange_net.owfn", std::ios_base::in);
  try
  {
  ifstr >> io::owfn >> net[2];
  ifstr.close();
  }
  catch (io::InputError error)
  {
    std::cout << error << "\n";
  }*/
  // automaton construction from PetriNet instance
  pnapi::ServiceAutomaton sauto0(net[0]);
  pnapi::ServiceAutomaton sauto1(net[1]);
//  pnapi::ServiceAutomaton sauto2(net[2]);
  cout << io::sa << sauto0;
  cout << io::sa << sauto1;
//  cout << io::sa << sauto2;
  end_test();

  return 0;
}
