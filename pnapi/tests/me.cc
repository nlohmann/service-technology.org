// TODO: void mergePlaces(Place * & p1, Place * & p2);
// TODO: void mergePlaces(string role1, Place * & p2);
// TODO: void mergePlaces(Place * & p1, string role2);
// TODO: void mergePlaces(string role1, string role2);
// TODO: void mergePlaces(unsigned int id1, string role1, unsigned int id2, string role2);
// TODO: void mergeTransitions(Transition *t1, Transition *t2);
// TODO: void mergeParallelTransitions(Transition *t1, Transition *t2);

// TODO: set<Node*> preset(Node *n) const;
// TODO: set<Node*> postset(Node *n) const;

// TODO: friend ostream& operator<< (ostream& os, const PetriNet &obj);
// TODO: PetriNet & operator=(const PetriNet &);

// TODO: void removeTransition(Transition *t);
// TODO: void produce(const PetriNet &net);

// TODO: void mirror();

// TODO: void addPrefix(string prefix, bool renameInterface = false);
// TODO: void add_interface_suffix(string suffix);

// TODO: void loop_final_state();

#include <iostream>
#include <set>
#include "../src/pnapi.h"

using std::cout;
using std::endl;
using namespace PNapi;

int error_count;

int main() 
{
	extern int error_count;
	error_count = 0;
	
	cout << "Running me-test..." << endl;
	cout << "error_counter has been initialized..." << endl;
	
	PetriNet *n = new PetriNet();
	PetriNet *m = new PetriNet();
	n->newPlace("test",IN,100,"zweck");
	n->newPlace("foo",OUT,200,"bar");
	n->newTransition("bla");
	cout << n->information() << endl;
	
	(*m) = (*n);
	if (n->information() != m->information())
		error_count++;
	else
		cout << "Operator (=) works well!\n";
	
	return error_count;
}


