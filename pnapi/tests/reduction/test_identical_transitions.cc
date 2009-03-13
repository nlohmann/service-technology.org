/*
 * Test net taken from [Pil08] Picture 5.2.
 */

#include <fstream>

#include "test.h"

using std::fstream;
using pnapi::io::owfn;
using pnapi::io::InputError;

int main(int argc, char * argv[])
{
  begin_test("PetriNet::reduce_identical_transitions()");
  
  // read net
  PetriNet net;
  fstream stream;
  
  stream.open("test_identical_transitions.owfn", std::ios_base::in);
  
  if(!stream)
  {
    cout << endl << "Couldn't read test_identical_transitions.owfn" << endl;
    assert(false);
  }
  
  try
  {
    stream >> owfn >> net;
  }
  catch (InputError e) 
  { 
    cout << endl << e << endl; 
    assert(false); 
  }
  
  stream.close();
  
  assert(net.getPlaces().size() == 6); 
  assert(net.getTransitions().size() == 8);
  
  // apply reduction
  net.reduce(PetriNet::IDENTICAL_TRANSITIONS);
  
  // check result
  assert(net.getPlaces().size() == 6);
  assert(net.getTransitions().size() == 7);
  
  end_test();

  return 0;
}
