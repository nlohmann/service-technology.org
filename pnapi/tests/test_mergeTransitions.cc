#include <iostream>
#include <cassert>
#include "net_gen.h"
#include "../src/pnapi.h"

using std::cout;
using std::endl;
using namespace PNapi;

int main(int argc, char *argv[])
{
	cout << "Testing mergeTransitions()-methods ...";

	PetriNet *net = createProducerConsumerExample();
	Transition *t1 = net->findTransition("produce");
	Transition *t2 = net->findTransition("consume");
	net->mergeTransitions(t1, t2);

	Transition *t12 = net->findTransition("produce");
	Transition *t22 = net->findTransition("consume");
	assert(t12 == t22);
	assert(t12 != NULL);

	cout << " granted!" << endl;
	
	return 0;
}
