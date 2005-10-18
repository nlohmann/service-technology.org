#include "petrinet.h"


Arc::Arc(Node *mysource, Node* mytarget)
{
  source = mysource;
  target = mytarget;
}

/*****************************************************************************/

Transition::Transition(std::string myname, std::string role, unsigned int mytype)
{
  type = mytype;
  name = myname;
  roles.push_back(role);
}

/*****************************************************************************/

Place::Place(std::string myname, std::string role, unsigned int mytype)
{
  type = mytype;
  name = myname;
  roles.push_back(role);
}

/*****************************************************************************/

PetriNet::PetriNet()
{
  places = 0;
  transitions = 0;
  arcs = 0;
}


Node *PetriNet::newPlace(std::string name, std::string role, unsigned int type)
{
  Place *p = new Place(name, role, type);
  P.push_back(p);
  places++;
  return p;
}


Node *PetriNet::newTransition(std::string name, std::string role, unsigned int type)
{
  Transition *t = new Transition(name, role, type);
  T.push_back(t);
  transitions++;
  return t;
}

Arc *PetriNet::newArc(Node *source, Node *target)
{
  Arc *f = new Arc(source, target);
  F.push_back(f);
  arcs++;
  return f;
}
 

void PetriNet::information()
{
  std::cout << places << " places" << std::endl;
  std::cout << transitions << " transitions" << std::endl;
  std::cout << arcs << " arcs" << std::endl;
}



void PetriNet::drawDot()
{
  std::cout << "digraph N {" << std::endl;
  std::cout << " node [shape=circle fixedsize];" << std::endl;
  for (std::vector<Place *>::iterator p = P.begin(); p != P.end(); p++)
  {
    std::cout << " " << (*p)->name << ";" << std::endl;
  }

  std::cout << std::endl << " node [shape=box fixedsize];" << std::endl;
  for (std::vector<Transition *>::iterator t = T.begin(); t != T.end(); t++)
  {
    std::cout << " " << (*t)->name << ";" << std::endl;
  }

  for (std::vector<Arc *>::iterator f = F.begin(); f != F.end(); f++)
  {
    std::cout << " " << (*f)->source->name << " -> " << (*f)->target->name << ";" << std::endl;
  }
  std::cout << "}" << std::endl;

}

/******************************************************************************

int main()
{
  PetriNet TheNet = PetriNet();
  
  Node *a = TheNet.newPlace("dkdkdkdkd", 0);
  Node *b = TheNet.newTransition("blfffffub", 0);
  TheNet.newArc(a, b);
  TheNet.newArc(b, a);
  
  TheNet.drawDot();
  
  return 1;
}

******************************************************************************/
