#include "test.h"
#include "../../src/automaton.h"
#include "../../src/io.h"
#include <iostream>
#include <fstream>
#include <cstdlib>

using std::ifstream;
using std::fstream;
using std::cout;
using std::endl;
using namespace pnapi;


int main(int argc, char *argv[])
{
  PetriNet net[4];
  
  begin_test("Automaton::Automaton() [construction] with normal net");
  
  ifstream ifstr;
  ifstr.open("test_construction.sequence_normal_net.owfn");
  try
  {
    ifstr >> io::owfn >> net[0];
  }
  catch (io::InputError error)
  {
    std::cerr << error << "\n";
    ifstr.close();
    exit(EXIT_FAILURE);
  }
  ifstr.close();
  Automaton sauto0(net[0]);
  cout << io::sa << sauto0;

  ifstr.clear();
  ifstr.open("test_construction.sequence_unnormal_net.owfn", std::ios_base::in);
  try
  {
    ifstr >> io::owfn >> net[1];
  }
  catch (io::InputError error)
  {
    std::cout << error << "\n";
    ifstr.close();
    exit(EXIT_FAILURE);
  }
  ifstr.close();
  Automaton sauto1(net[1]);
  cout << io::sa << sauto1;

  ifstr.clear();
  ifstr.open("test_construction.strange_net.owfn", std::ios_base::in);
  try
  {
    ifstr >> io::owfn >> net[2];
  }
  catch (io::InputError error)
  {
    std::cerr << error << "\n";
    ifstr.close();
    exit(EXIT_FAILURE);
  }
  ifstr.close();
  pnapi::Automaton sauto2(net[2]);
  cout << io::sa << sauto2;

  end_test();

  return 0;
}
