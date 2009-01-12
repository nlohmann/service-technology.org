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
    // 3rd test for reduce() (Parallel Transitions)
    // generating net
    PetriNet *n3 = new PetriNet();
    
    Place *n3p1 = n3->newPlace("P1");
    Place *n3p2 = n3->newPlace("P2");
    
    Transition *n3ti1 = n3->newTransition("Ti1");
    Transition *n3ti2 = n3->newTransition("Ti2");
    Transition *n3ti3 = n3->newTransition("Ti3");
    Transition *n3ti4 = n3->newTransition("Ti4");
    Transition *n3to1 = n3->newTransition("To1");
    Transition *n3to2 = n3->newTransition("To2");
    Transition *n3to3 = n3->newTransition("To3");
    Transition *n3to4 = n3->newTransition("To4");
    Transition *n3t1 = n3->newTransition("T1");
    Transition *n3t2 = n3->newTransition("T2");
    
    n3->newArc(n3ti1,n3p1);
    
    n3->newArc(n3ti2,n3p1);
    
    n3->newArc(n3ti3,n3p2);
    
    n3->newArc(n3ti4,n3p2);

    n3->newArc(n3p1,n3t1);
    n3->newArc(n3t1,n3p2);
    
    n3->newArc(n3p1,n3t2);
    n3->newArc(n3t2,n3p2);
    
    n3->newArc(n3p1,n3to1);
    
    n3->newArc(n3p1,n3to2);
    
    n3->newArc(n3p2,n3to3);
    
    n3->newArc(n3p2,n3to4);
    
    // start test
    cout << "testing PetriNet::reduce() (parallel transitions)... ";
    n3->reduce();
    
    Transition* tmptransition = n3->findTransition("T1");
    assert(tmptransition == n3->findTransition("T2")); // T1 und T2 sind zusammengelegt
    
    cout << "Ok" << endl;
    
    return 0;
}
