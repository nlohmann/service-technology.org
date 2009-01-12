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
	
	// Assignment Operator
	cout << "testing assignment operator... ";
	PetriNet* n2 = new PetriNet();
	(*n2) = (*myNet);
	
	Place* n2p1 = n2->findPlace("P1");
	assert(n2p1 != NULL);
	assert(n2p1->type == IN);
	
	Place* n2p2 = n2->findPlace("P2");
	assert(n2p2 != NULL);
	assert(n2p2->type == INTERNAL);
	assert(n2p2->isFinal == true);
	
	Place* n2p3 = n2->findPlace("P3");
	assert(n2p3 != NULL);
	assert(n2p3->type == INOUT);
	assert(n2p3->isFinal == true);
	
	Place* n2p4 = n2->findPlace("P4");
	assert(n2p4 != NULL);
	assert(n2p4->type == OUT);
	
	Transition* n2t = n2->findTransition("T1");
	
	set<Node*> n2pre = n2->preset(n2t);
	assert(n2pre.find(n2p1) != n2pre.end());
	assert(n2pre.find(n2p2) != n2pre.end());
	assert(n2pre.find(n2p3) != n2pre.end());
	
	set<Node*> n2post = n2->postset(n2t);
	assert(n2post.find(n2p2) != n2post.end());
	assert(n2post.find(n2p3) != n2post.end());
	assert(n2post.find(n2p4) != n2post.end());
	
	cout << "Ok" << endl;
	
	return 0;
}
