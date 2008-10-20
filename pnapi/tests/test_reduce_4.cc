#include <cassert>
#include <iostream>
#include <set>
#include <string>
#include "../src/pnapi.h"

using std::cout;
using std::endl;
using std::set;
using std::string;
using namespace PNapi;

int main(int argc, char* argv[])
{
    // 4th test for reduce() (Parallel Places)
    // generating net
    PetriNet *n4 = new PetriNet();
    
    Place *n4pi1 = n4->newPlace("Pi1",IN);
    Place *n4pi2 = n4->newPlace("Pi2",IN);
    Place *n4pi3 = n4->newPlace("Pi3",IN);
    Place *n4pi4 = n4->newPlace("Pi4",IN);
    Place *n4po1 = n4->newPlace("Po1",OUT);
    Place *n4po2 = n4->newPlace("Po2",OUT);
    Place *n4po3 = n4->newPlace("Po3",OUT);
    Place *n4po4 = n4->newPlace("Po4",OUT);
    Place *n4p1 = n4->newPlace("P1");
    Place *n4p2 = n4->newPlace("P2");
    
    Transition *n4t1 = n4->newTransition("T1");
    Transition *n4t2 = n4->newTransition("T2");
    
    n4->newArc(n4pi1,n4t1);
    n4->newArc(n4pi2,n4t1);
    n4->newArc(n4t1,n4p1);
    n4->newArc(n4t1,n4p2);
    n4->newArc(n4t1,n4po1);
    n4->newArc(n4t1,n4po2);
        
    n4->newArc(n4pi3,n4t2);
    n4->newArc(n4pi4,n4t2);
    n4->newArc(n4p1,n4t2);
    n4->newArc(n4p2,n4t2);
    n4->newArc(n4t2,n4po3);
    n4->newArc(n4t2,n4po4);
    
    // pre- und postsets for test below 
    set<Node*> n4pre1_1 = n4->preset(n4t1);
    
    set<Node*> *n4pre1_2 = new set<Node*>();
    n4pre1_2->insert(n4pi3);
    n4pre1_2->insert(n4pi4);
    
    set<Node*> *n4post1_1 = new set<Node*>();
    n4post1_1->insert(n4po1);
    n4post1_1->insert(n4po2);
    
    set<Node*> n4post1_2 = n4->postset(n4t2);
    
    // start test
    cout << "testing PetriNet::reduce() (parallel places)... ";
    n4->reduce();
    
    Place* tmpplace = n4->findPlace("P1");
    assert(tmpplace == n4->findPlace("P2")); // P1 und P2 sind zusammengelegt
    
    n4pre1_2->insert(tmpplace);
    n4post1_1->insert(tmpplace);
    
    set<Node*> n4pre2_1 = n4->preset(n4t1);
    assert(n4pre1_1 == n4pre2_1);
    
    set<Node*> n4pre2_2 = n4->preset(n4t2);
    assert((*n4pre1_2) == n4pre2_2);
    
    set<Node*> n4post2_1 = n4->postset(n4t1);
    assert((*n4post1_1) == n4post2_1);
    
    set<Node*> n4post2_2 = n4->postset(n4t2);
    assert(n4post1_2 == n4post2_2);
    
    cout << "Ok" << endl;
    
    return 0;
}