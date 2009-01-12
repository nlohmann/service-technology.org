#include <cassert>
#include <iostream>
#include <set>
#include <string>
#include "../src/pnapi.h"

using std::cout;
using std::endl;
using std::set;
using std::string;
using namespace pnapi;

int main(int argc, char* argv[])
{
    // 2nd test for reduce() (Series Transitions)
    // generating net
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
    
    // pre- und postsets for test below
    set<Node*> *n2pre1 = new set<Node*>();
    n2pre1->insert(n2pi1);
    n2pre1->insert(n2pi2);
    
    set<Node*> *n2post1 = new set<Node*>();
    n2post1->insert(n2po1);
    n2post1->insert(n2po2);
    n2post1->insert(n2po3);
    n2post1->insert(n2po4);
    
    // start test
    cout << "testing PetriNet::reduce() (series transitions)... ";
    n2->reduce();
    
    Transition* tmptransition = n2->findTransition("T1");
    assert(tmptransition == n2->findTransition("T2")); // T1 und T2 sind zusammengelegt
    assert(n2->findPlace("P") == NULL); // P wurde gelöscht
    
    set<Node*> n2pre2 = n2->preset(tmptransition);
    assert((*n2pre1) == n2pre2);
    
    set<Node*> n2post2 = n2->postset(tmptransition);
    assert((*n2post1) == n2post2);
    
    cout << "Ok" << endl;
    
    return 0;
}