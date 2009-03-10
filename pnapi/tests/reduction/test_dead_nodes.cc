#include <fstream>

#include "test.h"

using std::fstream;
using pnapi::io::owfn;
using pnapi::io::InputError;

int main(int argc, char * argv[])
{
  begin_test("PetriNet::reduce_dead_nodes()");
  
  // read net
  PetriNet net;
  fstream stream;
  
  stream.open("test_dead_nodes.owfn", std::ios_base::in);
  
  if(!stream)
  {
    cout << endl << "Couldn't read test_dead_nodes.owfn" << endl;
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
  net.reduce(PetriNet::DEAD_NODES);
  
  // check result
  assert(net.getPlaces().size() == 2);
  
  end_test();

  return 0;
}
