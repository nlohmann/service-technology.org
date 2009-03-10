#include <fstream>

#include "test.h"

using std::fstream;
using pnapi::io::owfn;
using pnapi::io::InputError;

int main(int argc, char * argv[])
{
  
  
  begin_test("PetriNet::reduce_unused_status_places()");
  skip_test();
  
  /*
  // read net
  PetriNet net;
  fstream stream;
  
  stream.open("test_unused_status_places.owfn", std::ios_base::in);
  
  if(!stream)
  {
    cout << endl << "Couldn't read test_unused_status_places.owfn" << endl;
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
  
  assert(net.getPlaces().size() == 3); 
  
  // apply reduction
  net.reduce(PetriNet::UNUSED_STATUS_PLACES);
  
  // check result
  assert(net.getPlaces().size() == 2);
  
  end_test();
  */

  return 0;
}
