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
    // 7th test for reduce() (preserve normalisation)
    // generating net
    PetriNet *n7 = new PetriNet();
    
    Place *n7pi = n7->newPlace("Pin",IN);
    Place *n7po = n7->newPlace("Pout",OUT);
    Place *n7p = n7->newPlace("P");
    
    Transition *n7t1 = n7->newTransition("T1");
    Transition *n7t2 = n7->newTransition("T2");
    
    n7->newArc(n7pi,n7t1);
    n7->newArc(n7t1,n7p);
    
    n7->newArc(n7p,n7t2);
    n7->newArc(n7t2,n7po);
        
    // start test
    cout << "testing PetriNet::reduce() (preserve normalisation)... ";
   
    string before = n7->information();
    
    n7->reduce(5,true);
    
    assert(before == n7->information());
    
    cout << "Ok" << endl;
    
    return 0;
}