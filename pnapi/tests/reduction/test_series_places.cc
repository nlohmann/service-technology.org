/*
 * Test net taken from [Pil08] Picture 4.21.
 */

#include <fstream>

#include "test.h"

using std::fstream;
using pnapi::io::owfn;
using pnapi::io::InputError;

int main(int argc, char * argv[])
{
  begin_test("PetriNet::reduce_series_places()");
  
  // read net
  PetriNet net;
  fstream stream;
  
  stream.open("test_series_places.owfn", std::ios_base::in);
  
  if(!stream)
  {
    cout << endl << "Couldn't read test_series_places.owfn" << endl;
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
  
  assert(net.getPlaces().size() == 5); 
  assert(net.getTransitions().size() == 7);
  
  // apply reduction
  net.reduce(PetriNet::SERIES_PLACES); 
  
  // check result
  assert(net.getPlaces().size() == 4);
  assert(net.getTransitions().size() == 6);
  
  end_test();

  return 0;
}
