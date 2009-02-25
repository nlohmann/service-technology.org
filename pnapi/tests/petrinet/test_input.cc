#include <sstream>
#include <fstream>
#include "test.h"
#include "parser.h"

using std::stringstream;

using pnapi::io::owfn;
using pnapi::io::meta;
using pnapi::io::InputError;
using pnapi::io::CREATOR;
using pnapi::io::INPUTFILE;
using pnapi::io::OUTPUTFILE;

int main(int argc, char * argv[])
{
  PetriNet net;
  Place & in = net.createPlace("in", Place::INPUT);
  Place & out = net.createPlace("out", Place::OUTPUT);
  Place & p1 = net.createPlace("", Place::INTERNAL, 4, 8);
  Place & p2 = net.createPlace();
  Transition & t1 = net.createTransition();
  Transition & t2 = net.createTransition();
  net.createArc(in, t1, 3);
  net.createArc(t1, p1);
  net.createArc(p1, t2);
  net.createArc(t2, out);
  net.finalCondition() = p1 == 2 && p2 == 4 || p1 > 1;

  /*
  // the test doesn't work reliably because of set ordering
  begin_test("io::operator>>() [Petri net OWFN input]");
  stringstream stream, test;
  stream << owfn << net;
  //cout << owfn << net;
  try { stream >> owfn >> net; }
  catch (InputError e) { cout << endl << e << endl; assert(false); }
  test << owfn << net;
  //cout << owfn << net;
  assert(stream.str() == test.str());
  end_test();
  */

  /*
  begin_test("io::operator>>() [Petri net OWFN finalmarking]");
  PetriNet net2;
  stringstream fmdata;
  fmdata
    << "PLACE" << endl
    << "  INTERNAL" << endl
    << "    SAFE 8 : p1; p2;" << endl << endl
    << "  INPUT" << endl
    << "    in;" << endl << endl
    << "  OUTPUT" << endl
    << "    out;" << endl << endl
    << "PORTS" << endl
    << "  port1: out;" << endl << endl
    << "INITIALMARKING" << endl
    << "  p1: 4;" << endl << endl
    << "FINALCONDITION" << endl
    << "  p1 = 1 AND ALL_OTHER_PLACES_EMPTY;" << endl
    << "TRANSITION t1" << endl
    << "  CONSUME in:3;" << endl
    << "  PRODUCE p1;" << endl
    << "TRANSITION t2" << endl
    << "  CONSUME p1;" << endl
    << "  PRODUCE out;";
  try { fmdata >> owfn >> net2; }
  catch (InputError e) { cout << endl << e << endl; assert(false); }
  cout << owfn << net2;
  end_test();
  */

  return 0;
}
