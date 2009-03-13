#include <fstream>

#include "test.h"

using std::fstream;
using pnapi::io::owfn;
using pnapi::io::InputError;

int main(int argc, char * argv[])
{
  begin_test("PetriNet::reduce_self_loop_transitions()");
  
  // read net
  PetriNet net;
  fstream stream;
  
  stream.open("test_self_loop_transitions.owfn", std::ios_base::in);
  
  if(!stream)
  {
    cout << endl << "Couldn't read test_self_loop_transitions.owfn" << endl;
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
  
  assert(net.getPlaces().size() == 1); 
  assert(net.getTransitions().size() == 3);
  
  // apply reduction
  net.reduce(PetriNet::SELF_LOOP_TRANSITIONS); 
  
  // check result
  assert(net.getPlaces().size() == 1);
  assert(net.getTransitions().size() == 2);
  
  end_test();

  return 0;
}
