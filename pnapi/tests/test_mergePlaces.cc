#include <iostream>
#include <cassert>
#include "../src/pnapi.h"
#include "net_gen.h"

using std::cout;
using std::endl;
using namespace pnapi;

int main(int argc, char *argv[])
{
	cout << "Testing all mergePlaces()-methods ...";

	PetriNet *net;
	Place *p1, *p2;
	string s1 = "ready to produce", s2 = "ready to deliver";

	cout << "mergePlaces(Place * & p1, Place * & p2) ...";
	net = createProducerConsumerExample();
	p1 = net->findPlace(s1);
	p2 = net->findPlace(s2);
	net->mergePlaces(*p1, *p2);
	p1 = net->findPlace(s1);
	p2 = net->findPlace(s2);
	assert(p1 != NULL);
	assert(p2 != NULL);
	assert(p1 == p2);
	delete net;
	cout << " granted!" << endl;

	cout << "mergePlaces(Place * & p1, string role2) ...";
	net = createProducerConsumerExample();
	p1 = net->findPlace(s1);
	net->mergePlaces(*p1, s2);
	p1 = net->findPlace(s1);
	p2 = net->findPlace(s2);
	assert(p1 == p2);
	assert(p1 != NULL);
	delete net;
	cout << " granted!" << endl;

	cout << "mergePlaces(string role1, Place * & p2) ...";
	net = createProducerConsumerExample();
	p2 = net->findPlace(s2);
	net->mergePlaces(s1, *p2);
	p1 = net->findPlace(s1);
	p2 = net->findPlace(s2);
	assert(p1 == p2);
	assert(p1 != NULL);
	delete net;
	cout << " granted!" << endl;

	cout << "mergePlaces(string role1, string role2) ...";
	net = createProducerConsumerExample();
	net->mergePlaces(s1, s2);
	p1 = net->findPlace(s1);
	p2 = net->findPlace(s2);
	assert(p1 == p2);
	assert(p1 != NULL);
	delete net;
	cout << " granted!" << endl;

	cout << "Test granted!" << endl;
	
	return 0;
}
