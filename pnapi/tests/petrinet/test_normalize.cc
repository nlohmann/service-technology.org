#include <fstream>
#include "test.h"

using namespace std;
using namespace pnapi;

int main(int argc, char *argv[])
{
  begin_test("normalize() with normal net");
  PetriNet n1;
  n1.createArc(n1.createPlace("p1"), n1.createTransition("t1"));
  n1.createArc(n1.createPlace("a", Node::INPUT), *n1.findTransition("t1"));
  n1.createArc(*n1.findTransition("t1"), n1.createPlace("p2"));
  n1.createArc(*n1.findPlace("p2"), n1.createTransition("t2"));
  n1.createArc(*n1.findTransition("t2"), n1.createPlace("b", Node::OUTPUT));
  n1.createArc(*n1.findTransition("t2"), n1.createPlace("p3"));
  n1.finalCondition() = (*n1.findPlace("p3")) > 0;
  n1.findPlace("p1")->mark(1);
  ofstream onet1("test_normalize.sequence_normal.owfn");
  onet1 << io::owfn << n1;
  n1.normalize();
  ofstream onet11("test_normalize.sequence_normal_normalized.owfn");
  onet11 << io::owfn << n1;
  end_test();

  begin_test("normalize() with an unnormal net");
  PetriNet n2;
  Place &p1 = n2.createPlace("p1");
  Place &p2 = n2.createPlace("p2");
  Place &p3 = n2.createPlace("p3");
  Place &p4 = n2.createPlace("p4");
  Place &c = n2.createPlace("C", Node::INPUT);
  Place &t = n2.createPlace("T", Node::INPUT);
  Place &m = n2.createPlace("M", Node::INPUT);
  Place &b = n2.createPlace("B", Node::OUTPUT);
  Transition &t1 = n2.createTransition("t1");
  Transition &t2 = n2.createTransition("t2");
  Transition &t3 = n2.createTransition("t3");
  n2.createArc(p1, t1);
  n2.createArc(p1, t2);
  n2.createArc(t, t1);
  n2.createArc(c, t2);
  n2.createArc(t1, p2);
  n2.createArc(p2, t3);
  n2.createArc(t2, p3);
  n2.createArc(p3, t3);
  n2.createArc(t3, p4);
  n2.createArc(t3, b);
  n2.createArc(m, t3);
  ofstream onet2("test_normalize.coffee_unnormal.owfn");
  onet2 << io::owfn << n2;
  n2.normalize();
  ofstream onet21("test_normalize.coffee_unnormal_normalized.owfn");
  onet21 << io::owfn << n2;
  end_test();

  begin_test("normalize() with a strange unnormal net");
  PetriNet n3;
  Place &q1 = n3.createPlace("p1");
  Place &q2 = n3.createPlace("p2");
  Place &q3 = n3.createPlace("p3");
  Place &q4 = n3.createPlace("p4");
  Place &i = n3.createPlace("i", Node::INPUT);
  Place &o1 = n3.createPlace("o1", Node::OUTPUT);
  Place &o2 = n3.createPlace("o2", Node::OUTPUT);
  Transition &u1 = n3.createTransition("t1");
  Transition &u2 = n3.createTransition("t2");
  Transition &u3 = n3.createTransition("t3");
  n3.createArc(q1, u1);
  n3.createArc(i, u1);
  n3.createArc(u1, o1);
  n3.createArc(u1, q2);
  n3.createArc(u1, q3);
  n3.createArc(q2, u2);
  n3.createArc(q3, u3);
  n3.createArc(i, u2);
  n3.createArc(u3, o2);
  n3.createArc(u2, q4);
  n3.createArc(u3, q4);
  ofstream onet3("test_normalize.strange_unnormal.owfn");
  onet3 << io::owfn << n3;
  n3.normalize();
  ofstream onet31("test_normalize.strange_unnormal_normalized.owfn");
  onet31 << io::owfn << n3;
  end_test();

  return 0;
}
