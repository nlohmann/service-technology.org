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
    // 5th test for reduce() (Selfloop Transitions)
    // generating net
    PetriNet *n6 = new PetriNet();
    
    Place *n6p = n6->newPlace("P");
    
    Transition *n6ti1 = n6->newTransition("Ti1");
    Transition *n6ti2 = n6->newTransition("Ti2");
    Transition *n6to1 = n6->newTransition("To1");
    Transition *n6to2 = n6->newTransition("To2");
    Transition *n6t = n6->newTransition("T");
    
    n6->newArc(n6ti1,n6p);
    
    n6->newArc(n6ti2,n6p);
    
    n6->newArc(n6p,n6to1);
    
    n6->newArc(n6p,n6to2);
    
    n6->newArc(n6p,n6t);
    n6->newArc(n6t,n6p);
    
    // start test
    cout << "testing PetriNet::reduce() (selfloop transitions)... ";
    n6->reduce();
    
    assert(n6->findTransition("T") == NULL);
    
    cout << "Ok" << endl;
    
    return 0;
}
