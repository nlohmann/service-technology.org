/*
 * First test net taken from [Pil08] Picture 4.15.
 */

#include <fstream>

#include "test.h"

using std::fstream;
using pnapi::io::owfn;
using pnapi::io::InputError;

int main(int argc, char * argv[])
{
  // read net
  PetriNet net;
  fstream stream;
  
  begin_test("PetriNet::reduce_series_transitions()");
  
  stream.open("test_series_transitions.owfn", std::ios_base::in);
  
  if(!stream)
  {
    cout << endl << "Couldn't read test_series_transitions.owfn" << endl;
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
  assert(net.getTransitions().size() == 2);
  
  // apply reduction
  net.reduce(PetriNet::SERIES_TRANSITIONS);  
  
  // check result
  assert(net.getPlaces().size() == 4);
  assert(net.getTransitions().size() == 1);
  
  end_test();
  
  
  
  begin_test("PetriNet::reduce_series_transitions() [Don't keep normal]");
  
  stream.open("test_series_transitions_keepnormal.owfn", std::ios_base::in);
  
  if(!stream)
  {
    cout << endl << "Couldn't read test_series_transitions_keepnormal.owfn" << endl;
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
  assert(net.getTransitions().size() == 2);
  assert(net.isNormal());
  
  // apply reduction
  net.reduce(PetriNet::SERIES_TRANSITIONS);  
  
  // check result
  assert(net.getPlaces().size() == 4);
  assert(net.getTransitions().size() == 1);
  assert(!(net.isNormal()));
  
  end_test();
  
  
  
  begin_test("PetriNet::reduce_series_transitions() [Keep normal]");
    
  stream.open("test_series_transitions_keepnormal.owfn", std::ios_base::in);
  
  if(!stream)
  {
    cout << endl << "Couldn't read test_series_transitions_keepnormal.owfn" << endl;
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
  assert(net.getTransitions().size() == 2);
  assert(net.isNormal());
  
  // apply reduction
  net.reduce(PetriNet::SERIES_TRANSITIONS | PetriNet::KEEP_NORMAL);  
  
  // check result
  assert(net.getPlaces().size() == 5);
  assert(net.getTransitions().size() == 2);
  assert(net.isNormal());
  
  end_test();

  return 0;
}
