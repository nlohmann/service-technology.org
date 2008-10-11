#include "su.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include "../src/pnapi.h"

using std::cout;
using std::endl;
using std::set;
using std::string;
using namespace PNapi;

// DONE: Place* findPlace(string role) const;
/* POSTPONED: Place* findPlace(unsigned int id, string role) const;
 * Derzeit existiert keine Möglichkeit, an eine ID zu kommen
 */
// DONE: Transition* findTransition(string role) const;

// DONE: set< Place * > getFinalPlaces() const;
// DONE: set< Place * > getInterfacePlaces() const;

// POSTPONED: void set_format(output_format my_format, bool standard = true);

/* POSTPONED: void setPlacePort(Place *place, string port);
 * Derzeit existiert keine Möglichkeit, an ports zu kommen
 */


// POSTPONED: unsigned int push_forEach_suffix(string suffix);
// POSTPONED: unsigned int pop_forEach_suffix();

// DONE: PetriNet(const PetriNet &);
// TODO: PetriNet & operator=(const PetriNet &);

// DONE: void renamePlace(string old_name, string new_name);
// TODO: unsigned int reduce(unsigned int reduction_level = 5); Um Bug kümmern

// TODO: void reenumerate();

// TODO: void calculate_max_occurrences();
// TODO: void compose(const PetriNet &net, unsigned int capacityOnInterface = 0);
// DONE: void makeChannelsInternal();

/* Test 1: Ein Petrinetz mit genau einer Transition 
 * und genau einem Place für jeden communication_type
 */ 
int test1()
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
	
	/* 
	// Ausgabe in eine Datei um myNet zu zeichnen
	// Vor der Ausführung von makeChannelsInternal()
	myNet->set_format(FORMAT_DOT);
	std::ofstream f;
	f.open("sdot/test1_1.dot");
	f << (*myNet);
	f.close();
	//*/
	
	// Eigentliche Tests
	// findPlace(role)
	Place* testplace = myNet->findPlace("P0");
	assert(testplace == NULL);
	testplace = myNet->findPlace("P1");
    assert(testplace == p1);
    testplace = myNet->findPlace("P2");
    assert(testplace == p2);
    
    // findTransition()
    Transition* testtransition = myNet->findTransition("T0");
    assert(testtransition == NULL);
    testtransition = myNet->findTransition("T1");
    assert(testtransition == t1);
    
    // getFinalPlaces()
    set<Place*> placeset = myNet->getFinalPlaces();
    assert(placeset.size() == 2);
    assert(placeset.find(p2) != placeset.end());
    assert(placeset.find(p3) != placeset.end());
    
    // getInterfacePlaces()
	placeset = myNet->getInterfacePlaces();
	assert(placeset.size() == 2);
	assert(placeset.find(p1) != placeset.end());
	assert(placeset.find(p4) != placeset.end());
	
	// renamePlace()
	myNet->renamePlace("P3","P23");
	testplace = myNet->findPlace("P3");
	assert(testplace == NULL);
	testplace = myNet->findPlace("P23");
	assert(testplace == p3);
	
	// Copyconstructor
	PetriNet* n2 = new PetriNet(*myNet);
	
	Place* n2p1 = n2->findPlace("P1");
	assert(n2p1 != NULL);
	assert(n2p1->type == IN);
	
	Place* n2p2 = n2->findPlace("P2");
	assert(n2p2 != NULL);
	assert(n2p2->type == INTERNAL);
	assert(n2p2->isFinal == true);
	
	Place* n2p23 = n2->findPlace("P23");
	assert(n2p23 != NULL);
	assert(n2p23->type == INOUT);
	assert(n2p23->isFinal == true);
	
	Place* n2p4 = n2->findPlace("P4");
	assert(n2p4 != NULL);
	assert(n2p4->type == OUT);
	
	Transition* n2t = n2->findTransition("T1");
	
	set<Node*> n2pre = n2->preset(n2t);
	assert(n2pre.find(n2p1) != n2pre.end());
	assert(n2pre.find(n2p2) != n2pre.end());
	assert(n2pre.find(n2p23) != n2pre.end());
	
	set<Node*> n2post = n2->postset(n2t);
	assert(n2post.find(n2p2) != n2post.end());
	assert(n2post.find(n2p23) != n2post.end());
	assert(n2post.find(n2p4) != n2post.end());
	
	// makeChannelsInternal()
	myNet->makeChannelsInternal();
	placeset = myNet->getInterfacePlaces();
	assert(placeset.size() == 0);
    
	/* 
	// Ausgabe in eine Datei um myNet zu zeichnen
    // Nach der Ausführung von makeChannelsInternal()
	f.open("sdot/test1_2.dot");
	f << (*myNet);
	f.close();
	//*/
	
	return 0;
}
/* Test 2: Test aller 6 Reduction Rules aus Muratas Paper.
 */
int test2(){
	// Petrinetz für Reduktion a (Series Places)
	PetriNet *n1 = new PetriNet();
	
	Place *n1pi1 = n1->newPlace("Pi1",IN);
	Place *n1pi2 = n1->newPlace("Pi2",IN);
	Place *n1pi3 = n1->newPlace("Pi3",IN);
	Place *n1pi4 = n1->newPlace("Pi4",IN);
	Place *n1po1 = n1->newPlace("Po1",OUT);
	Place *n1po2 = n1->newPlace("Po2",OUT);
	Place *n1p1 = n1->newPlace("P1");
	Place *n1p2 = n1->newPlace("P2");
	
	Transition *n1t = n1->newTransition("T");
	Transition *n1ti1 = n1->newTransition("Ti1");
	Transition *n1ti2 = n1->newTransition("Ti2");
	Transition *n1ti3 = n1->newTransition("Ti3");
	Transition *n1ti4 = n1->newTransition("Ti4");
	Transition *n1to1 = n1->newTransition("To1");
	Transition *n1to2 = n1->newTransition("To2");
	
	n1->newArc(n1pi1,n1ti1);
	n1->newArc(n1ti1,n1p1);
	
	n1->newArc(n1pi2,n1ti2);
	n1->newArc(n1ti2,n1p1);
	
	n1->newArc(n1pi3,n1ti3);
	n1->newArc(n1ti3,n1p2);
	
	n1->newArc(n1pi4,n1ti4);
	n1->newArc(n1ti4,n1p2);
	
	n1->newArc(n1p1,n1t);
	n1->newArc(n1t,n1p2);
	
	n1->newArc(n1p2,n1to1);
	n1->newArc(n1to1,n1po1);
	
	n1->newArc(n1p2,n1to2);
	n1->newArc(n1to2,n1po2);
	
	// Pre- und Postsets für späteren Test
	set<Node*> *n1pre1 = new set<Node*>();
	n1pre1->insert(n1ti1);
	n1pre1->insert(n1ti2);
	n1pre1->insert(n1ti3);
	n1pre1->insert(n1ti4);
	
	set<Node*> *n1post1 = new set<Node*>();
	n1post1->insert(n1to1);
	n1post1->insert(n1to2);
	
	// Petrinetz für Reduktion b (Series Transitions)
	PetriNet *n2 = new PetriNet();
	
	Place *n2pi1 = n2->newPlace("Pi1",IN);
	Place *n2pi2 = n2->newPlace("Pi2",IN);
	Place *n2po1 = n2->newPlace("Po1",OUT);
	Place *n2po2 = n2->newPlace("Po2",OUT);
	Place *n2po3 = n2->newPlace("Po3",OUT);
	Place *n2po4 = n2->newPlace("Po4",OUT);
	Place *n2p = n2->newPlace("P");
	
	Transition *n2t1 = n2->newTransition("T1");
	Transition *n2t2 = n2->newTransition("T2");
	
	n2->newArc(n2pi1,n2t1);
	n2->newArc(n2pi2,n2t1);
	n2->newArc(n2t1,n2po1);
	n2->newArc(n2t1,n2po2);
	n2->newArc(n2t1,n2p);
	
	n2->newArc(n2p,n2t2);
	n2->newArc(n2t2,n2po3);
	n2->newArc(n2t2,n2po4);
	
	// Pre- und Postsets für späteren Test
	set<Node*> *n2pre1 = new set<Node*>();
	n2pre1->insert(n2pi1);
	n2pre1->insert(n2pi2);
	
	set<Node*> *n2post1 = new set<Node*>();
	n2post1->insert(n2po1);
	n2post1->insert(n2po2);
	n2post1->insert(n2po3);
	n2post1->insert(n2po4);
	
	// Petrinetz für Reduktion c (Parallel Transitions)
	PetriNet *n3 = new PetriNet();
	
	Place *n3pi1 = n3->newPlace("Pi1",IN);
	Place *n3pi2 = n3->newPlace("Pi2",IN);
	Place *n3pi3 = n3->newPlace("Pi3",IN);
	Place *n3pi4 = n3->newPlace("Pi4",IN);
	Place *n3po1 = n3->newPlace("Po1",OUT);
	Place *n3po2 = n3->newPlace("Po2",OUT);
	Place *n3po3 = n3->newPlace("Po3",OUT);
	Place *n3po4 = n3->newPlace("Po4",OUT);
	Place *n3p1 = n3->newPlace("P1");
	Place *n3p2 = n3->newPlace("P2");
	
	Transition *n3ti1 = n3->newTransition("Ti1");
	Transition *n3ti2 = n3->newTransition("Ti2");
	Transition *n3ti3 = n3->newTransition("Ti3");
	Transition *n3ti4 = n3->newTransition("Ti4");
	Transition *n3to1 = n3->newTransition("To1");
	Transition *n3to2 = n3->newTransition("To2");
	Transition *n3to3 = n3->newTransition("To3");
	Transition *n3to4 = n3->newTransition("To4");
	Transition *n3t1 = n3->newTransition("T1");
	Transition *n3t2 = n3->newTransition("T2");
	
	n3->newArc(n3pi1,n3ti1);
	n3->newArc(n3ti1,n3p1);
	
	n3->newArc(n3pi2,n3ti2);
	n3->newArc(n3ti2,n3p1);
	
	n3->newArc(n3pi3,n3ti3);
	n3->newArc(n3ti3,n3p2);
	
	n3->newArc(n3pi4,n3ti4);
	n3->newArc(n3ti4,n3p2);

	n3->newArc(n3p1,n3t1);
	n3->newArc(n3t1,n3p2);
	
	n3->newArc(n3p1,n3t2);
	n3->newArc(n3t2,n3p2);
	
	n3->newArc(n3p1,n3to1);
	n3->newArc(n3to1,n3po1);
	
	n3->newArc(n3p1,n3to2);
	n3->newArc(n3to2,n3po2);
	
	n3->newArc(n3p2,n3to3);
	n3->newArc(n3to3,n3po3);
	
	n3->newArc(n3p2,n3to4);
	n3->newArc(n3to4,n3po4);
	
	// Pre- und Postsets für späteren Test
	set<Node*> n3pre1_1 = n3->preset(n3p1);
	
	set<Node*> *n3pre1_2 = new set<Node*>();
	n3pre1_2->insert(n3ti3);
	n3pre1_2->insert(n3ti4);
	
	set<Node*> *n3post1_1 = new set<Node*>();
	n3post1_1->insert(n3to1);
	n3post1_1->insert(n3to2);
	
	set<Node*> n3post1_2 = n3->postset(n3p2);
	
	// Petrinetz für Reduktion d (Parallel Places)
	PetriNet *n4 = new PetriNet();
	
	Place *n4pi1 = n4->newPlace("Pi1",IN);
	Place *n4pi2 = n4->newPlace("Pi2",IN);
	Place *n4pi3 = n4->newPlace("Pi3",IN);
	Place *n4pi4 = n4->newPlace("Pi4",IN);
	Place *n4po1 = n4->newPlace("Po1",OUT);
	Place *n4po2 = n4->newPlace("Po2",OUT);
	Place *n4po3 = n4->newPlace("Po3",OUT);
	Place *n4po4 = n4->newPlace("Po4",OUT);
	Place *n4p1 = n4->newPlace("P1");
	Place *n4p2 = n4->newPlace("P2");
	
	Transition *n4t1 = n4->newTransition("T1");
	Transition *n4t2 = n4->newTransition("T2");
	
	n4->newArc(n4pi1,n4t1);
	n4->newArc(n4pi2,n4t1);
	n4->newArc(n4t1,n4p1);
	n4->newArc(n4t1,n4p2);
	n4->newArc(n4t1,n4po1);
	n4->newArc(n4t1,n4po2);
		
	n4->newArc(n4pi3,n4t2);
	n4->newArc(n4pi4,n4t2);
	n4->newArc(n4p1,n4t2);
	n4->newArc(n4p2,n4t2);
	n4->newArc(n4t2,n4po3);
	n4->newArc(n4t2,n4po4);
	
	// Pre- und Postsets für späteren Test 
	set<Node*> n4pre1_1 = n4->preset(n4t1);
	
	set<Node*> *n4pre1_2 = new set<Node*>();
	n4pre1_2->insert(n4pi3);
	n4pre1_2->insert(n4pi4);
	
	set<Node*> *n4post1_1 = new set<Node*>();
	n4post1_1->insert(n4po1);
	n4post1_1->insert(n4po2);
	
	set<Node*> n4post1_2 = n4->postset(n4t2);
	
	// Petrinetz für Reduktion e (Selfloop Places)
	PetriNet *n5 = new PetriNet();
	
	Place *n5pi1 = n5->newPlace("Pi1",IN);
	Place *n5pi2 = n5->newPlace("Pi2",IN);
	Place *n5po1 = n5->newPlace("Po1",OUT);
	Place *n5po2 = n5->newPlace("Po2",OUT);
	Place *n5p = n5->newPlace("P");
	n5p->mark(1);
	
	Transition* n5t = n5->newTransition("T");
	
	n5->newArc(n5pi1,n5t);
	n5->newArc(n5pi2,n5t);
	n5->newArc(n5t,n5po1);
	n5->newArc(n5t,n5po2);
	n5->newArc(n5t,n5p);
	n5->newArc(n5p,n5t);
	
	// Pre- und Postsets für späteren Test
	set<Node*> *n5pre1 = new set<Node*>();
	n5pre1->insert(n5pi1);
	n5pre1->insert(n5pi2);
	
	set<Node*> *n5post1 = new set<Node*>();
	n5post1->insert(n5po1);
	n5post1->insert(n5po2);
	
	// Petrinetz für Reduktion f (Selfloop Transitions)
	PetriNet *n6 = new PetriNet();
	
	Place *n6pi1 = n6->newPlace("Pi1",IN);
	Place *n6pi2 = n6->newPlace("Pi2",IN);
	Place *n6po1 = n6->newPlace("Po1",OUT);
	Place *n6po2 = n6->newPlace("Po2",OUT);
	Place *n6p = n6->newPlace("P");
	
	Transition *n6ti1 = n6->newTransition("Ti1");
	Transition *n6ti2 = n6->newTransition("Ti2");
	Transition *n6to1 = n6->newTransition("To1");
	Transition *n6to2 = n6->newTransition("To2");
	Transition *n6t = n6->newTransition("T");
	
	n6->newArc(n6pi1,n6ti1);
	n6->newArc(n6ti1,n6p);
	
	n6->newArc(n6ti2,n6p);
	n6->newArc(n6pi2,n6ti2);
	
	n6->newArc(n6p,n6to1);
	n6->newArc(n6to1,n6po1);
	
	n6->newArc(n6p,n6to2);
	n6->newArc(n6to2,n6po2);
	
	n6->newArc(n6p,n6t);
	n6->newArc(n6t,n6p);
	
	// Pre- und Postsets für späteren Test
	set<Node*> *n6pre1 = new set<Node*>();
	n6pre1->insert(n6ti1);
	n6pre1->insert(n6ti2);
	
	set<Node*> *n6post1 = new set<Node*>();
	n6post1->insert(n6to1);
	n6post1->insert(n6to2);
	
	/*
	// Ausgabe in eine Datei um Netze zu zeichnen
	// Vor Ausführung der Reduktion
	n1->set_format(FORMAT_DOT);
	n2->set_format(FORMAT_DOT);
	n3->set_format(FORMAT_DOT);
	n4->set_format(FORMAT_DOT);
	n5->set_format(FORMAT_DOT);
	n6->set_format(FORMAT_DOT);
	
	std::ofstream f;
	f.open("sdot/test2_a1.dot");
	f << (*n1);
	f.close();
	f.open("sdot/test2_b1.dot");
	f << (*n2);
	f.close();
	f.open("sdot/test2_c1.dot");
	f << (*n3);
	f.close();
	f.open("sdot/test2_d1.dot");
	f << (*n4);
	f.close();
	f.open("sdot/test2_e1.dot");
	f << (*n5);
	f.close();
	f.open("sdot/test2_f1.dot");
	f << (*n6);
	f.close();
	//*/
	
	n1->reduce();
	n2->reduce();
	n3->reduce();
	n4->reduce();
	n5->reduce();
	n6->reduce();
	
	/* 
	// Ausgabe in eine Datei um Netze zu zeichnen
	// Nach Ausführung der Reduktion
	f.open("sdot/test2_a2.dot");
	f << (*n1);
	f.close();
	f.open("sdot/test2_b2.dot");
	f << (*n2);
	f.close();
	f.open("sdot/test2_c2.dot");
	f << (*n3);
	f.close();
	f.open("sdot/test2_d2.dot");
	f << (*n4);
	f.close();
	f.open("sdot/test2_e2.dot");
	f << (*n5);
	f.close();
	f.open("sdot/test2_f2.dot");
	f << (*n6);
	f.close();
	//*/
	
	// Tests
	// Reduktion a
	Place* tmpplace = n1->findPlace("P1");
	assert(tmpplace == n1->findPlace("P2")); // P1 ud P2 sind zusammengelegt
	assert(n1->findTransition("T") == NULL); // T wurde gelöscht
	
	set<Node*> n1pre2 = n1->preset(tmpplace);
	assert((*n1pre1) == n1pre2); // Preset-check
	
	set<Node*> n1post2 = n1->postset(tmpplace);
	assert((*n1post1) == n1post2); // Postset-check
	
	// Reduktion b
	Transition* tmptransition = n2->findTransition("T1");
	assert(tmptransition == n2->findTransition("T2")); // T1 und T2 sind zusammengelegt
	assert(n2->findPlace("P") == NULL); // P wurde gelöscht
	
	set<Node*> n2pre2 = n2->preset(tmptransition);
	assert((*n2pre1) == n2pre2);
	
	set<Node*> n2post2 = n2->postset(tmptransition);
	assert((*n2post1) == n2post2);
	
	// Reduktion c
	tmptransition = n3->findTransition("T1");
	assert(tmptransition == n3->findTransition("T2")); // T1 und T2 sind zusammengelegt
	
	n3pre1_2->insert(tmptransition);
	
	n3post1_1->insert(tmptransition);
	
	set<Node*> n3pre2_1 = n3->preset(n3p1);
	assert(n3pre1_1 == n3pre2_1);
	
	set<Node*> n3pre2_2 = n3->preset(n3p2);
	assert((*n3pre1_2) == n3pre2_2);
	
	set<Node*> n3post2_1 = n3->postset(n3p1);
	assert((*n3post1_1) == n3post2_1);
	
	set<Node*> n3post2_2 = n3->postset(n3p2);
	assert(n3post1_2 == n3post2_2);
	
	// Reduktion d
	tmpplace = n4->findPlace("P1");
	assert(tmpplace == n4->findPlace("P2")); // P1 und P2 sind zusammengelegt
	
	n4pre1_2->insert(tmpplace);
	n4post1_1->insert(tmpplace);
	
	set<Node*> n4pre2_1 = n4->preset(n4t1);
	assert(n4pre1_1 == n4pre2_1);
	
	set<Node*> n4pre2_2 = n4->preset(n4t2);
	assert((*n4pre1_2) == n4pre2_2);
	
	set<Node*> n4post2_1 = n4->postset(n4t1);
	assert((*n4post1_1) == n4post2_1);
	
	set<Node*> n4post2_2 = n4->postset(n4t2);
	assert(n4post1_2 == n4post2_2);
	
	// Reduktion e
	assert(n5->findPlace("P") == NULL);
	
	set<Node*> n5pre2 = n5->preset(n5t);
	assert((*n5pre1) == n5pre2);
	
	set<Node*> n5post2 = n5->postset(n5t);
	assert((*n5post1) == n5post2);
	
	// Reduktion f
	assert(n6->findTransition("T") == NULL);
	
	set<Node*> n6pre2 = n6->preset(n6p);
	assert((*n6pre1) == n6pre2);
	
	set<Node*> n6post2 = n6->postset(n6p);
	assert((*n6post1) == n6post2);
	
	return 0;
}

/* Test 3: Ein Testfall der Reduktion in einem Netz mit 
 * Transitionen ohne Pre- und Postset
 */
int test3()
{
	// Test für Reduktion mit Transitions ohne Postset
	// Petrinetz für Reduktion a (Series Places)
	PetriNet *n1 = new PetriNet();
	
	Place *n1p1 = n1->newPlace("P1");
	Place *n1p2 = n1->newPlace("P2");
	
	Transition *n1t = n1->newTransition("T");
	Transition *n1ti1 = n1->newTransition("Ti1");
	Transition *n1ti2 = n1->newTransition("Ti2");
	Transition *n1ti3 = n1->newTransition("Ti3");
	Transition *n1ti4 = n1->newTransition("Ti4");
	Transition *n1to1 = n1->newTransition("To1");
	Transition *n1to2 = n1->newTransition("To2");
	
	n1->newArc(n1ti1,n1p1);
	
	n1->newArc(n1ti2,n1p1);
	
	n1->newArc(n1ti3,n1p2);
	
	n1->newArc(n1ti4,n1p2);
	
	n1->newArc(n1p1,n1t);
	n1->newArc(n1t,n1p2);
	
	n1->newArc(n1p2,n1to1);
	
	n1->newArc(n1p2,n1to2);
		
	/* 
	// Ausgabe in eine Datei um Netze zu zeichnen
	// Vor Ausführung der Reduktion
	n1->set_format(FORMAT_DOT);
	std::ofstream f;
	f.open("sdot/test3_1.dot");
	f << (*n1);
	f.close();
	//*/
	
	n1->reduce();
	
	/* 
	// Ausgabe in eine Datei um Netze zu zeichnen
	// Nach Ausführung der Reduktion
	f.open("sdot/test3_2.dot");
	f << (*n1);
	f.close();
	//*/
		
	return 0;
}

int su_test()
{
	cout << "Running su-tests..." << endl;
	test1();
	test2();
	//test3();
	cout << "Finished su-tests." << endl;
	return 0;
}
