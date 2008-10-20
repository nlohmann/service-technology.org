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
    // 5th test for reduce() (Selfloop Places)
    // generating net
    PetriNet *n5 = new PetriNet();
    
    Place *n5pi1 = n5->newPlace("Pi1",IN);
    Place *n5pi2 = n5->newPlace("Pi2",IN);
    Place *n5po1 = n5->newPlace("Po1",OUT);
    Place *n5po2 = n5->newPlace("Po2",OUT);
    Place *n5p = n5->newPlace("P");
    n5p->mark(1);
    
    Transition* n5t = n5->newTransition("T");
    
    n5->newArc(n5pi1,n5t);
    n5->newArc(n5pi2,n5t);
    n5->newArc(n5t,n5po1);
    n5->newArc(n5t,n5po2);
    n5->newArc(n5t,n5p);
    n5->newArc(n5p,n5t);
    
    // pre- und postsets for test below
    set<Node*> *n5pre1 = new set<Node*>();
    n5pre1->insert(n5pi1);
    n5pre1->insert(n5pi2);
    
    set<Node*> *n5post1 = new set<Node*>();
    n5post1->insert(n5po1);
    n5post1->insert(n5po2);
    
    // start test
    cout << "testing PetriNet::reduce() (selfloop places)... ";
    n5->reduce();
    
    assert(n5->findPlace("P") == NULL);
        
    set<Node*> n5pre2 = n5->preset(n5t);
    assert((*n5pre1) == n5pre2);
    
    set<Node*> n5post2 = n5->postset(n5t);
    assert((*n5post1) == n5post2);
    
    cout << "Ok" << endl;
    
    return 0;
}