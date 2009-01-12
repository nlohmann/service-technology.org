#include <cassert>
#include <iostream>
#include <set>
#include "../src/pnapi.h"

using std::cout;
using std::endl;
using std::set;
using namespace pnapi;

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
	
	Transition* t1 = myNet->newTransition("T1");
	
	myNet->newArc(p1,t1);
	myNet->newArc(t1,p4);
	myNet->newArc(p3,t1);
	myNet->newArc(t1,p3);
	myNet->newArc(p2,t1);
	myNet->newArc(t1,p2);
	
	// makeChannelsInternal()
	myNet->makeChannelsInternal();
	set<Place*> placeset = myNet->getInterfacePlaces();
	assert(placeset.size() == 0);
	
	return 0;
}
