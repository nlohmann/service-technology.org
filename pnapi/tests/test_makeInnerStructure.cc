#include <cassert>
#include <iostream>
#include <set>
#include "../src/pnapi.h"

using std::cout;
using std::endl;
using std::set;
using namespace PNapi;

int main(int argc, char* argv[])
{
	// Netz für Tests generieren
	PetriNet *myNet = new PetriNet();
		
	Place* p1 = myNet->newPlace("P1",IN);
		
	Place* p2 = myNet->newPlace("P2");
	p2->mark(1);
	p2->isFinal = true;
	
	Place* p3 = myNet->newPlace("P3",INOUT);
	p3->isFinal = true;
	
	Place* p4 = myNet->newPlace("P4",OUT);
	
	Place* p5 = myNet->newPlace("P5",IN);
	
	Place* p6 = myNet->newPlace("P6",OUT);
	
	Transition* t1 = myNet->newTransition("T1");
	Transition* t2 = myNet->newTransition("T2");
	
	myNet->newArc(p1,t1);
	myNet->newArc(t1,p4);
	myNet->newArc(p3,t1);
	myNet->newArc(t1,p3);
	myNet->newArc(p2,t1);
	myNet->newArc(t1,p2);
	
	myNet->newArc(p5,t2);
	myNet->newArc(t2,p6);
	
	// makerInnerStructure()
	myNet->makeInnerStructure();
	
	assert(myNet->findPlace("P1")==NULL);
	assert(myNet->findPlace("P4")==NULL);
	assert(myNet->findPlace("P5")==NULL);
	assert(myNet->findPlace("P6")==NULL);
	
	return 0;
}
