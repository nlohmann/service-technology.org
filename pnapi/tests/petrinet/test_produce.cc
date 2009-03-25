#include <fstream>
#include <map>
#include "test.h"

using std::map;
using std::ofstream;
using std::ifstream;

using pnapi::io::owfn;

int main(int argc, char * argv[])
{
  ifstream ifs;

  PetriNet net;
  ifs.open("test_produce.onlineshop.owfn");
  ifs >> owfn >> net;
  ifs.close();
 
  PetriNet constraint;
  ifs.open("test_produce.onlineshop.constraint.owfn");
  ifs >> owfn >> constraint;
  ifs.close();

  map<Transition *, set<Transition *> > labels;
  labels[constraint.findTransition("t8")].insert(net.findTransition("t1"));
  labels[constraint.findTransition("t8")].insert(net.findTransition("t2"));
  labels[constraint.findTransition("t9")].insert(net.findTransition("t3"));
  labels[constraint.findTransition("t9")].insert(net.findTransition("t4"));

  begin_test("PetriNet::produce() [product with constraint oWFN]");
  net.produce(constraint, labels);
  ofstream ofs("test_produce.onlineshop.result.owfn");
  ofs << owfn << net;
  ofs.close();
  // FIXME: compare result to expected
  //end_test();
  cout << "Incomplete" << endl;

  return 0;
}
