#include <sstream>
#include <fstream>
#include <map>
#include "test.h"
#include "parser.h"

using std::map;
using std::ofstream;
using std::ifstream;
using std::stringstream;

using pnapi::io::owfn;
using pnapi::io::lola;
using pnapi::io::dot;
using pnapi::io::meta;
using pnapi::io::nets;
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

  begin_test("io::operator>>() [Petri net OWFN input]");
  ifstream ifs("test_input.net.owfn");
  try { ifs >> owfn >> net; }
  catch (InputError e) { cout << endl << e << endl; assert(false); }
  //cout << dot << net;
  
  // check transition costs
  assert(net.findTransition("t1")->getCost() == 0);
  assert(net.findTransition("t2")->getCost() == 0);
  assert(net.findTransition("t3")->getCost() == 2);
  
  end_test();

  /*
  // the test doesn't work reliably because of set ordering
  begin_test("io::operator>>() [Petri net LOLA input]");
  extern int pnapi_lola_debug;
  //pnapi_lola_debug = 1;
  stringstream stream, test;
  //stream << lola << net;
  stream << "PLACE p1, p2; MARKING p1:4; TRANSITION t1 CONSUME ; PRODUCE p1:1; TRANSITION t2 CONSUME p1:1; PRODUCE ;";
  cout << lola << net;
  try { stream >> lola >> net; }
  catch (InputError e) { cout << endl << e << endl; assert(false); }
  //test << lola << net;
  cout << lola << net;
  //assert(stream.str() == test.str());
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

  stringstream owfn1;
  owfn1 << "PLACE INTERNAL p1, p2, p3, p4, p5, f1, f2, f3, f4, f5, f6; "
	<< "      INPUT a, b; "
	<< "      OUTPUT c1, c2, d; "
	<< "INITIALMARKING p1:   1; "
	<< "FINALCONDITION p2 = 1 AND ALL_OTHER_PLACES_EMPTY; "
	<< "TRANSITION t1 CONSUME p1: 1, a: 1; "
	<< "              PRODUCE p2: 1, p3: 1, f1: 1, f4: 1; "
	<< "TRANSITION t2 CONSUME p2: 1, f1: 1; "
	<< "              PRODUCE c1: 1, f2: 1, p4: 1; "
	<< "TRANSITION t3 CONSUME p3: 1, b: 1; "
	<< "              PRODUCE p5: 1, f3: 2, f5: 1; "
	<< "TRANSITION t4 CONSUME p4: 1, p5: 1; "
	<< "              PRODUCE d: 1, f1: 1, f4: 1, f6: 3;";

  stringstream owfn2;
  owfn2 << "PLACE INTERNAL; INPUT c_in, d_in1, d_in2; OUTPUT;"
	<< "INITIALMARKING ;"
	<< "FINALCONDITION ;";

  stringstream onwd;
  onwd << "INSTANCE"
       << "  net1, net2;"
       << "WIRING"
       << "  net1.c1->net2.c_in, net1.c2->net2.c_in, "
       << "  net1.d=>net2.d_in1, net1.d=>net2.d_in2;";

  ofstream file1("test_input.net1.owfn");
  ofstream file2("test_input.net2.owfn");
  file1 << owfn1.str();
  file2 << owfn2.str();
  file1.close();
  file2.close();

  /*
  begin_test("io::operator>>() [Petri net ONWD input]");
  try 
    { 
      PetriNet net1;
      owfn1 >> owfn >> net1;
      map<string, PetriNet *> netsByName;
      netsByName["net1"] = &net1;
      onwd >> meta(pnapi::io::INPUTFILE, "<onwd>")
	   >> nets(netsByName)
	   >> pnapi::io::onwd >> net; 

      ifstream ifs;
      map<string, PetriNet *> netsByName;

      PetriNet airline;
      ifs.open("airline.owfn");
      ifs >> owfn >> airline;
      ifs.close();
      netsByName["airline"] = &airline;

      PetriNet agency_traveler;
      ifs.open("agency_traveler.owfn");
      ifs >> owfn >> agency_traveler;
      ifs.close();
      netsByName["agency_traveler"] = &agency_traveler;

      ifs.open("booking.onwd");
      ifs >> onwd >> nets(netsByName) >> net;
      ifs.close();
    }
  catch (InputError e) { cout << endl << e << endl; assert(false); }
  //cout << owfn << net;
  end_test();
  */

  return 0;
}
