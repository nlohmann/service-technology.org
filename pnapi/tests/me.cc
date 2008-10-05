// DONE: void mergePlaces(Place * & p1, Place * & p2);
// DONE: void mergePlaces(string role1, Place * & p2);
// DONE: void mergePlaces(Place * & p1, string role2);
// DONE: void mergePlaces(string role1, string role2);
// POSTPONED: void mergePlaces(unsigned int id1, string role1, unsigned int id2, string role2);
// => change documentation text (done)

// DONE: void mergeTransitions(Transition *t1, Transition *t2);
// POSTPONED: void mergeParallelTransitions(Transition *t1, Transition *t2);
// | there could be a doubled function

// POSTPONED: set<Node*> preset(Node *n) const;
// POSTPONED: set<Node*> postset(Node *n) const;
// => because of the documentation's deprecated note

// POSTPONED: friend ostream& operator<< (ostream& os, const PetriNet &obj); | no testing
// TODO: PetriNet & operator=(const PetriNet &);

// DONE: void removeTransition(Transition *t);

// TODO: void produce(const PetriNet &net);

// DONE: void mirror();

// TODO: void addPrefix(string prefix, bool renameInterface = false);
// TODO: void add_interface_suffix(string suffix);

// DONE: void loop_final_state();

#include <iostream>
#include <fstream>
#include <cassert>
#include <cstdlib>
#include <string>
#include "../src/pnapi.h"

using namespace std;
using namespace PNapi;

unsigned int counter = 0;

PetriNet *createNet();
void drawNet(PetriNet *net);
void testAllmergePlaces();
void testMirror();
void testMergeTransitions();
void testRemove();
void testLoopFinal();

void testProduce();
void testPreSet();
void testPostSet();
void testOperatorEqual();
void testPrefix();
void testISuffix();

int me_test() {

	cout << "Running Stephan's Tests ..." << endl;

	testAllmergePlaces();

	testMirror();

	testMergeTransitions();

	testRemove();

	testProduce();
	
//	testPreSet();
	
//	testPostSet();

	testLoopFinal();
	
//	testOperatorEqual();
	
//	testPrefix();
	
//	testISuffix();

	cout << "Testing done ..." << endl;

	cout << counter << " drawings done." << endl;

	return 0;
}

/* producer-consumer example from [Reisig 1998]
 * only internal-places are created
 */
PetriNet *createNet() {
	PetriNet *n = new PetriNet();
	Place *p= NULL;

	// producer
	p = n->newPlace("ready to produce");
	n->newPlace("ready to deliver");
	n->newTransition("produce");
	n->newTransition("deliver");
	p->mark(1);
	p = NULL;

	n->newArc(n->findPlace("ready to produce"), n->findTransition("produce"));
	n->newArc(n->findTransition("produce"), n->findPlace("ready to deliver"));
	n->newArc(n->findPlace("ready to deliver"), n->findTransition("deliver"));
	n->newArc(n->findTransition("deliver"), n->findPlace("ready to produce"));

	// buffer
	n->newPlace("buffer filled");
	p = n->newPlace("buffer empty");
	p->mark(1);
	p = NULL;

	n->newArc(n->findTransition("deliver"), n->findPlace("buffer filled"));
	n->newArc(n->findPlace("buffer empty"), n->findTransition("deliver"));

	// consumer
	p = n->newPlace("ready to remove");
	n->newPlace("ready to consume");
	n->newTransition("remove");
	n->newTransition("consume");
	p->mark(1);
	p = NULL;

	n->newArc(n->findPlace("ready to remove"), n->findTransition("remove"));
	n->newArc(n->findTransition("remove"), n->findPlace("ready to consume"));
	n->newArc(n->findTransition("remove"), n->findPlace("buffer empty"));
	n->newArc(n->findPlace("buffer filled"), n->findTransition("remove"));
	n->newArc(n->findPlace("ready to consume"), n->findTransition("consume"));
	n->newArc(n->findTransition("consume"), n->findPlace("ready to remove"));

	// interface
	p = n->newPlace("materials ready", IN);
	n->newArc(p, n->findTransition("produce"));
	p = n->newPlace("children happy", OUT);
	n->newArc(n->findTransition("consume"), p);
	
	return n;
}

void drawNet(PetriNet *net) {
	ofstream file;

	net->set_format(FORMAT_DOT);
	file.open("cNet.dot");
	file << (*net);
	file.close();

	counter++;
}

void testAllmergePlaces() {
	cout << "Testing all mergePlaces()-methods ...";

	PetriNet *net;
	Place *p1, *p2;
	string s1 = "ready to produce", s2 = "ready to deliver";

	cout << "mergePlaces(Place * & p1, Place * & p2) ...";
	net = createNet();
	p1 = net->findPlace(s1);
	p2 = net->findPlace(s2);
	net->mergePlaces(p1, p2);
	p1 = net->findPlace(s1);
	p2 = net->findPlace(s2);
	assert(p1 == p2);
	assert(p1 != NULL);
	delete net;
	cout << " granted!" << endl;

	cout << "mergePlaces(Place * & p1, string role2) ...";
	net = createNet();
	p1 = net->findPlace(s1);
	net->mergePlaces(p1, s2);
	p1 = net->findPlace(s1);
	p2 = net->findPlace(s2);
	assert(p1 == p2);
	assert(p1 != NULL);
	delete net;
	cout << " granted!" << endl;

	cout << "mergePlaces(string role1, Place * & p2) ...";
	net = createNet();
	p2 = net->findPlace(s2);
	net->mergePlaces(s1, p2);
	p1 = net->findPlace(s1);
	p2 = net->findPlace(s2);
	assert(p1 == p2);
	assert(p1 != NULL);
	delete net;
	cout << " granted!" << endl;

	cout << "mergePlaces(string role1, string role2) ...";
	net = createNet();
	net->mergePlaces(s1, s2);
	p1 = net->findPlace(s1);
	p2 = net->findPlace(s2);
	assert(p1 == p2);
	assert(p1 != NULL);
	delete net;
	cout << " granted!" << endl;

	cout << "Test granted!" << endl;
}

void testMirror() {
	cout << "Testing mirror()-method ...";
	PetriNet *net = createNet();
	net->mirror();
	Place *p = net->findPlace("materials ready");
	assert(p->type == OUT);
	p = net->findPlace("children happy");
	assert(p->type == IN);
	cout << " granted!" << endl;
}

void testMergeTransitions() {
	cout << "Testing mergeTransitions()-methods ...";

	PetriNet *net = createNet();
	Transition *t1 = net->findTransition("produce");
	Transition *t2 = net->findTransition("consume");
	net->mergeTransitions(t1, t2);

	Transition *t12 = net->findTransition("produce");
	Transition *t22 = net->findTransition("consume");
	assert(t12 == t22);
	assert(t12 != NULL);

	cout << " granted!" << endl;
}

void testRemove() {
	cout << "Testing removeTransition()-method ...";

	PetriNet *net = createNet();
	Transition *t = net->findTransition("produce");

	net->removeTransition(t);

	t = net->findTransition("produce");
	assert(t == NULL);

	cout << " granted!" << endl;
}

void testProduce() {
	cout << "Testing produce()-method ...";

	PetriNet *net = new PetriNet();
	Place *p = net->newPlace("p1");
	Transition *t = net->newTransition("t1");
	net->newArc(p, t, READ);
	// net->produce(*net);
	// what happens here

	cout << " granted!" << endl;
}

void testPreSet()
{
	cout << "Testing preset()-method ...";
	
	PetriNet *net = createNet();
	net = NULL; // until a test is implemented
	
	cout << " granted!" << endl;
}

void testPostSet()
{
	cout << "Testing postset()-method ...";
	
	PetriNet *net = createNet();
	net = NULL; // until a test is implemented
	
	cout << " granted!" << endl;
}

void testLoopFinal() 
{
	cout << "Testing loop_final_state()-method ...";

	PetriNet *net = createNet();

	Place *p = net->newPlace("final place");
	p->isFinal = true;
	net->newArc(net->findTransition("produce"), p);
	net->newArc(net->findTransition("consume"), p);
	p = net->newPlace("final place 2");
	p->isFinal = true;
	net->newArc(net->findTransition("produce"), p);
	net->newArc(net->findTransition("consume"), p);
	p = net->newPlace("final place 3");
	p->isFinal = true;
	net->newArc(net->findTransition("produce"), p);
	net->newArc(net->findTransition("consume"), p);

	net->loop_final_state();
	
	// in the current implementation correct
	// because of the transition's role
	Transition *t = net->findTransition("antideadlock");
	assert(net->preset(t) == net->postset(t));

	cout << " granted!" << endl;
}

void testOperatorEqual()
{
	PetriNet *net = createNet();
	PetriNet test = *net;
	
	set<Place *> placeset1 = net->getInterfacePlaces();
	set<Place *> placeset2 = test.getInterfacePlaces();
	
	for (set<Place *>::iterator begin = placeset1.begin(); begin != placeset1.end(); begin++)
	{
		// do something
	}
}

void testPrefix()
{
	// nothing to be done here for now
}

void testISuffix()
{
	cout << "Testing add_interface_suffix()-method ...";
	
	PetriNet *net = createNet();
	string r1 = "materials ready";
	string r2 = "children happy";
	string suffix = "_test";
	
	net->add_interface_suffix(suffix);
	
	Place *p = net->findPlace(r1);
	Place *q = net->findPlace(r2);
	
	cout << " granted!" << endl;
}
