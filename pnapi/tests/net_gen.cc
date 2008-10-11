#include "net_gen.h"
#include "../src/pnapi.h"

using namespace std;

PetriNet *createProducerConsumerExample()
{
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
