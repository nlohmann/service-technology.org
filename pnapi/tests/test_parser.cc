#include <cassert>
#include <iostream>
#include <sstream>
#include <fstream>

#include "test.h"
#include "../src/parser.h"

using std::cout;
using std::endl;
using std::flush;
using std::stringstream;
using std::ofstream;

using pnapi::PetriNet;

using namespace pnapi::parser;


int main(int argc, char * argv[])
{
  stringstream owfn1;
  owfn1 << "PLACE INTERNAL p1, p2, p3, p4, p5, f1, f2, f3, f4, f5, f6; "
	<< "      INPUT a, b; "
	<< "      OUTPUT c, d; "
	<< "INITIALMARKING p1:   1; "
	<< "FINALCONDITION NOT NOT (f1 = 1 AND f3 <= 2 AND f4 >= 1 AND f5 < 4 "
	<< "                        AND f6 > 2); "
	<< "TRANSITION t1 CONSUME p1: 1, a: 1; "
	<< "              PRODUCE p2: 1, p3: 1, f1: 1, f4: 1; "
	<< "TRANSITION t2 CONSUME p2: 1, f1: 1; "
	<< "              PRODUCE c: 1, f2: 1, p4: 1; "
	<< "TRANSITION t3 CONSUME p3: 1, b: 1; "
	<< "              PRODUCE p5: 1, f3: 2, f5: 1; "
	<< "TRANSITION t4 CONSUME p4: 1, p5: 1; "
	<< "              PRODUCE d: 1, f1: 1, f4: 1, f6: 3;";

  stringstream owfn2;
  owfn2 << "PLACE INPUT c_in, d_in;"
	<< "INITIALMARKING ;"
	<< "FINALCONDITION ;";

  stringstream onwd;
  onwd << "INSTANCE"
       << "  net1: \"test_parser.net1.owfn\","
       << "  net2: \"test_parser.net2.owfn\";"
       << "WIRING"
       << "  net1.c->net2.c_in, net1.d=>net2.d_in;";

  try 
    {
      begin_test("OWFN parser");
      owfn::Parser owfnParser;
      owfn::Visitor owfnVisitor;
      owfnParser.parse(owfn1).visit(owfnVisitor);
      const PetriNet & net = owfnVisitor.getPetriNet();
      assert(net.getPlaces().size() == 11);
      end_test();

      begin_test("ONWD parser");
      ofstream file1("test_parser.net1.owfn");
      ofstream file2("test_parser.net2.owfn");
      file1 << owfn1.str();
      file2 << owfn2.str();
      file1.close();
      file2.close();
      onwd::Parser onwdParser;
      onwd::Visitor onwdVisitor;
      onwdParser.parse(onwd).visit(onwdVisitor);
      //cout << endl << onwdVisitor.getPetriNet() << endl;
      end_test();
    }
  catch (string msg)
    {
      cout << "FAIL" << endl << "ERROR: " << msg << endl;
      return 23;
    }

  return 0;
}
