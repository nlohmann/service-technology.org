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
    // 1st test for reduce() (Series Places)
    // generating net
    PetriNet *n1 = new PetriNet();
    
    Place *n1p1 = n1->newPlace("P1");
    Place *n1p2 = n1->newPlace("P2");
    
    Transition *n1t = n1->newTransition("T");
    Transition *n1ti1 = n1->newTransition("Ti1");
    Transition *n1ti2 = n1->newTransition("Ti2");
    Transition *n1ti3 = n1->newTransition("Ti3");
    Transition *n1ti4 = n1->newTransition("Ti4");
    Transition *n1to1 = n1->newTransition("To1");
    Transition *n1to2 = n1->newTransition("To2");
    
    n1->newArc(n1ti1,n1p1);
    
    n1->newArc(n1ti2,n1p1);
    
    n1->newArc(n1ti3,n1p2);
    
    n1->newArc(n1ti4,n1p2);
    
    n1->newArc(n1p1,n1t);
    n1->newArc(n1t,n1p2);
    
    n1->newArc(n1p2,n1to1);
    
    n1->newArc(n1p2,n1to2);
    
    // start test
    cout << "testing PetriNet::reduce() (series places)... ";
    n1->reduce();
    
    Place* tmpplace = n1->findPlace("P1");
    assert(tmpplace == n1->findPlace("P2")); // P1 ud P2 sind zusammengelegt
    assert(n1->findTransition("T") == NULL); // T wurde gelöscht
    
    cout << "Ok" << endl;
    
    return 0;
}
