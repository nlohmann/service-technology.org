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
	
	Transition* t1 = myNet->newTransition("T1");
	
	myNet->newArc(p1,t1);
	myNet->newArc(t1,p4);
	myNet->newArc(p3,t1);
	myNet->newArc(t1,p3);
	myNet->newArc(p2,t1);
	myNet->newArc(t1,p2);
	
	// findPlace(role)
	cout << "testing PetriNet::findPlace()... ";
	Place* testplace = myNet->findPlace("P0");
	assert(testplace == NULL);
	testplace = myNet->findPlace("P1");
    assert(testplace == p1);
    testplace = myNet->findPlace("P2");
    assert(testplace == p2);
    cout << "Ok" << endl;
    
    // findTransition()
    cout << "testing PetriNet::findTransition()... ";
    Transition* testtransition = myNet->findTransition("T0");
    assert(testtransition == NULL);
    testtransition = myNet->findTransition("T1");
    assert(testtransition == t1);
    cout << "Ok" << endl;
    
    // getFinalPlaces()
    cout << "testing PetriNet::getFinalPlaces()... ";
    set<Place*> placeset = myNet->getFinalPlaces();
    assert(placeset.size() == 2);
    assert(placeset.find(p2) != placeset.end());
    assert(placeset.find(p3) != placeset.end());
    cout << "Ok" << endl;
    
    // getInterfacePlaces()
    cout << "testing PetriNet::getInterfacePlaces()... ";
	placeset = myNet->getInterfacePlaces();
	assert(placeset.size() == 2);
	assert(placeset.find(p1) != placeset.end());
	assert(placeset.find(p4) != placeset.end());
	cout << "Ok" << endl;
	
	// renamePlace()
	cout << "testing renamePlace()... ";
	myNet->renamePlace("P3","P23");
	testplace = myNet->findPlace("P3");
	assert(testplace == NULL);
	testplace = myNet->findPlace("P23");
	assert(testplace == p3);
	cout << "Ok" << endl;	
	
	return 0;
}
