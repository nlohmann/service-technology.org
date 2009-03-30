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

  begin_test("PetriNet::produce() [product with constraint oWFN]");
  net.produce(constraint);
  //cout << owfn << net;
  ofstream ofs("test_produce.onlineshop.result.owfn");
  ofs << owfn << net;
  ofs.close();
  // FIXME: compare result to expected
  //end_test();
  cout << "Incomplete" << endl;

  return 0;
}
