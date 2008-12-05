#include <cassert>
#include <iostream>
#include <sstream>

#include "../src/parser.h"

using std::cout;
using std::endl;
using std::flush;
using std::stringstream;

using PNapi::parser::owfn::Parser;
using PNapi::parser::owfn::Visitor;
using PNapi::PetriNet;


void begin_test(const string & msg)
{
  cout << "Testing " << msg << " ... " << flush;
}

void end_test()
{
  cout << "Ok" << endl;
}


int main(int argc, char * argv[])
{
  begin_test("OWFN parser");
  stringstream file("PLACE INTERNAL p1, p2, p3, p4, p5, f1, f2, f3, f4, f5, f6; INPUT a, b; OUTPUT c, d; INITIALMARKING p1:   1; FINALCONDITION NOT NOT (f1 = 1 AND f3 <= 2 AND f4 >= 1 AND f5 < 4 AND f6 > 2); TRANSITION t1 CONSUME p1: 1, a: 1; PRODUCE p2: 1, p3: 1, f1: 1, f4: 1; TRANSITION t2 CONSUME p2: 1, f1: 1; PRODUCE c: 1, f2: 1, p4: 1; TRANSITION t3 CONSUME p3: 1, b: 1; PRODUCE p5: 1, f3: 2, f5: 1; TRANSITION t4 CONSUME p4: 1, p5: 1; PRODUCE d: 1, f1: 1, f4: 1, f6: 3;");
  Parser parser;
  Visitor visitor;
  parser.parse(file).visit(visitor);
  const PetriNet & net = visitor.getPetriNet();
  assert(net.getPlaceSet().size() == 11);
  end_test();

  return 0;
}
