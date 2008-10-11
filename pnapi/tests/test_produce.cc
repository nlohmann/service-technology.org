#include <iostream>
#include <cassert>
#include "../src/pnapi.h"

using std::cout;
using std::endl;
using namespace PNapi;

int main(int argc, char *argv[])
{
	cout << "Testing produce()-method ...";

	// create online shop service
	PetriNet net;

	Place *p0 = net.newPlace("initial");
	Transition *t7 = net.newTransition("t7");
	Place *po = net.newPlace("order", IN);
	(*p0).mark(1);
	net.newArc(p0, t7);
	net.newArc(po, t7);

	Place *p02 = net.newPlace("p02");
	Transition *t01 = net.newTransition("t01");
	Transition *t02 = net.newTransition("t02");
	Place *ppc = net.newPlace("premium customer", OUT);
	Place *prc = net.newPlace("regular customer", OUT);
	net.newArc(t7, p02);
	net.newArc(p02, t01);
	net.newArc(p02, t02);
	net.newArc(t01, ppc);
	net.newArc(t02, prc);

	Place *p05 = net.newPlace("p05");
	Place *p06 = net.newPlace("p06");
	net.newArc(t01, p05);
	net.newArc(t02, p06);

	Transition *t03 = net.newTransition("t03");
	Transition *t1 = net.newTransition("t1");
	Transition *t04 = net.newTransition("t04");
	Transition *t2 = net.newTransition("t2");
	Place *pcc = net.newPlace("credit card", IN);
	Place *pcod = net.newPlace("cash on delivery", IN);
	net.newArc(p05, t03);
	net.newArc(p05, t1);
	net.newArc(p06, t04);
	net.newArc(p06, t2);
	net.newArc(pcc, t03);
	net.newArc(pcc, t04);
	net.newArc(pcod, t1);
	net.newArc(pcod, t2);

	Place *p10 = net.newPlace("p10");
	Place *p11 = net.newPlace("p11");
	Place *p12 = net.newPlace("p12");
	Place *p13 = net.newPlace("p13");
	Place *pe = net.newPlace("error", OUT);
	net.newArc(t03, p10);
	net.newArc(t1, p11);
	net.newArc(t04, p12);
	net.newArc(t2, p13);

	Transition *t5 = net.newTransition("t5");
	Transition *t3 = net.newTransition("t3");
	Transition *t6 = net.newTransition("t6");
	Transition *t4 = net.newTransition("t4");
	Place *pd = net.newPlace("delivery", OUT);
	Place *ps = net.newPlace("surchage", OUT);
	Place *pf = net.newPlace("final");
	(*pf).isFinal = true;
	net.newArc(p10, t5);
	net.newArc(p11, t3);
	net.newArc(p12, t6);
	net.newArc(p13, t4);
	net.newArc(t5, pf);
	net.newArc(t3, pf);
	net.newArc(t6, pf);
	net.newArc(t4, pf);
	net.newArc(t5, pd);
	net.newArc(t3, ps);
	net.newArc(t4, pd);
	net.newArc(t6, pe);

	// create constraint oWFN
	PetriNet cowfn;
	Place *p0c = cowfn.newPlace("initial_C");
	Place *p1c = cowfn.newPlace("P_C");
	Place *pfc = cowfn.newPlace("final_C");
	Transition *t1c = cowfn.newTransition("t8");
	Transition *t2c = cowfn.newTransition("t9");
	(*p0c).mark(1);
	(*pfc).isFinal = true;
	(*t1c).add_label("t1");
	(*t1c).add_label("t2");
	(*t2c).add_label("t3");
	(*t2c).add_label("t4");
	cowfn.newArc(p0c, t1c);
	cowfn.newArc(t1c, p1c);
	cowfn.newArc(p1c, t2c);
	cowfn.newArc(t2c, pfc);

	// now I know what it does - tests must be implemented

	cout << " granted!" << endl;
	
	return 0;
}
