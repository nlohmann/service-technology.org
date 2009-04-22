#include <cassert>
#include <fstream>
#include <iostream>
#include "test.h"


using std::cerr;
using std::ifstream;

int main()
{
  ifstream ifstr1("test_criteria.workflownet.owfn");
  PetriNet n1;
  ifstr1 >> pnapi::io::owfn >> n1;
  if (!n1.isWorkflow())
  {
    cerr << "n1 is not a workflow net!\n";
    assert(false);
  }
  else
    cerr << "n1 is a workflow net!\n";
  ifstr1.close();

  ifstream ifstr2("test_criteria.noworkflownet.owfn");
  PetriNet n2;
  ifstr2 >> pnapi::io::owfn >> n2;
  if (n2.isWorkflow())
  {
    cerr << "n2 is a workflow net!\n";
    assert(false);
  }
  else
    cerr << "n2 is not a workflow net!\n";
  ifstr2.close();
  
  ifstream ifstr3("test_criteria.freechoice.owfn");
  PetriNet n3;
  ifstr3 >> pnapi::io::owfn >> n3;
  if (!n3.isFreeChoice())
  {
    cerr << "n3 is not free-choice!\n";
    assert(false);
  }
  else
    cerr << "n3 is free-choice!\n";
  ifstr3.close();
  
  ifstream ifstr4("test_criteria.nofreechoice.owfn");
  PetriNet n4;
  ifstr4 >> pnapi::io::owfn >> n4;
  if (n4.isFreeChoice())
  {
    cerr << "n4 is free-choice!\n";
    assert(false);
  }
  else
    cerr << "n4 is not free-choice!\n";
  ifstr4.close();

  return 0;
}
