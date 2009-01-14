#include <iostream>

#include "test.h"


void begin_test(const std::string & msg)
{
  std::cout << "Testing " << msg << " ... " << std::flush;
}

void end_test()
{
  std::cout << "Ok" << std::endl;
}

void createExamplePetriNet(PetriNet & net)
{
	PetriNet *n = &net;
	Place *p= NULL;

	// producer
	p = &n->createPlace("ready to produce");
	n->createPlace("ready to deliver");
	n->createTransition("produce");
	n->createTransition("deliver");
	//FIXME: p->mark(1);
	p = NULL;

	n->createArc(*n->findPlace("ready to produce"), *n->findTransition("produce"));
	n->createArc(*n->findTransition("produce"), *n->findPlace("ready to deliver"));
	n->createArc(*n->findPlace("ready to deliver"), *n->findTransition("deliver"));
	n->createArc(*n->findTransition("deliver"), *n->findPlace("ready to produce"));

	// buffer
	n->createPlace("buffer filled");
	p = &n->createPlace("buffer empty");
	//FIXME: p->mark(1);
	p = NULL;

	n->createArc(*n->findTransition("deliver"), *n->findPlace("buffer filled"));
	n->createArc(*n->findPlace("buffer empty"), *n->findTransition("deliver"));

	// consumer
	p = &n->createPlace("ready to remove");
	n->createPlace("ready to consume");
	n->createTransition("remove");
	n->createTransition("consume");
	//FIXME: p->mark(1);
	p = NULL;

	n->createArc(*n->findPlace("ready to remove"), *n->findTransition("remove"));
	n->createArc(*n->findTransition("remove"), *n->findPlace("ready to consume"));
	n->createArc(*n->findTransition("remove"), *n->findPlace("buffer empty"));
	n->createArc(*n->findPlace("buffer filled"), *n->findTransition("remove"));
	n->createArc(*n->findPlace("ready to consume"), *n->findTransition("consume"));
	n->createArc(*n->findTransition("consume"), *n->findPlace("ready to remove"));

	// interface
	p = &n->createPlace("materials ready", Node::INPUT);
	n->createArc(*p, *n->findTransition("produce"));
	p = &n->createPlace("children happy", Node::OUTPUT);
	n->createArc(*n->findTransition("consume"), *p);
}
