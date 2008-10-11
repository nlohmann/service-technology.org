#include <iostream>
#include <cassert>
#include "net_gen.h"
#include "../src/pnapi.h"

using std::cout;
using std::endl;
using namespace PNapi;

int main(int argc, char *argv[])
{
	cout << "Testing removeTransition()-method ...";

	PetriNet *net = createProducerConsumerExample();
	Transition *t = net->findTransition("produce");

	net->removeTransition(t);

	t = net->findTransition("produce");
	assert(t == NULL);

	cout << " granted!" << endl;
	
	return 0;
}
