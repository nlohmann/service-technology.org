#include <iostream>
#include <cassert>
#include "net_gen.h"
#include "../src/pnapi.h"

using std::cout;
using std::endl;
using namespace PNapi;

int main(int argc, char *argv[])
{
	cout << "Testing loop_final_state()-method ...";

	PetriNet *net = createProducerConsumerExample();

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
	// because of the transition's role "antideadlock"
	Transition *t = net->findTransition("antideadlock");
	assert(net->preset(t) == net->postset(t));

	cout << " granted!" << endl;
	
	return 0;
}
