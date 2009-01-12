#include <iostream>
#include <cassert>
#include "net_gen.h"
#include "../src/pnapi.h"

using std::cout;
using std::endl;
using namespace pnapi;

int main(int argc, char *argv[])
{
	cout << "Testing mirror()-method ...";
	PetriNet *net = createProducerConsumerExample();
	net->mirror();
	Place *p = net->findPlace("materials ready");
	assert(p->type == OUT);
	p = net->findPlace("children happy");
	assert(p->type == IN);
	cout << " granted!" << endl;
	
	return 0;
}
