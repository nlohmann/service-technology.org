#include <fstream>
#include <map>
#include "test.h"

#define CHECK std::cerr << "check " << __FILE__ << "@" << __LINE__ << std::endl;

using std::map;
using std::ofstream;
using std::ifstream;

using pnapi::io::owfn;

int main(int argc, char * argv[])
{
  ifstream ifs;

  PetriNet net;
  ifs.open("test_produce.onlineshop.owfn");
  try {
    ifs >> owfn >> net;
  }
  catch (pnapi::io::InputError error) {
    std::cerr << "test_produce:" << error << endl;
    ifs.close();
    exit(EXIT_FAILURE);
  }
  ifs.close();
 
  ifstream ifs2;
  PetriNet constraint;
  ifs2.open("test_produce.onlineshop.constraint.owfn");
  try {
    ifs2 >> owfn >> constraint;
  }
  catch (pnapi::io::InputError error) 
  {
    std::cerr << "test_produce:" << error << endl;
    ifs2.close();
    exit(EXIT_FAILURE);
  }
  ifs2.close();

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
