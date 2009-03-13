/*
 * Test net taken from [Pil08] Picture 3.3.
 */

#include <fstream>

#include "test.h"

using std::fstream;
using pnapi::io::owfn;
using pnapi::io::InputError;

int main(int argc, char * argv[])
{
  begin_test("PetriNet::reduce_identical_places()");
  
  // read net
  PetriNet net;
  fstream stream;
  
  stream.open("test_identical_places.owfn", std::ios_base::in);
  
  if(!stream)
  {
    cout << endl << "Couldn't read test_identical_places.owfn" << endl;
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
  
  assert(net.getPlaces().size() == 12); 
  assert(net.getTransitions().size() == 6);
  
  // apply reduction
  net.reduce(PetriNet::IDENTICAL_PLACES);
  
  // check result
  assert(net.getPlaces().size() == 11);
  assert(net.getTransitions().size() == 6);
  
  end_test();

  return 0;
}
