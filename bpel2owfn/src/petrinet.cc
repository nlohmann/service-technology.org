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



Place *PetriNet::newPlace(std::string name, std::string role, unsigned int type)
{
  Place *p = new Place(name, role, type);
  P.push_back(p);
  places++;
  return p;
}



Transition *PetriNet::newTransition(std::string name, std::string role, unsigned int type)
{
  Transition *t = new Transition(name, role, type);
  T.push_back(t);
  transitions++;
  return t;
}



void PetriNet::removePlace(Place *p1)
{
  bool changes;
  
  do
  {
    changes = false;
    for (std::vector<Arc *>::iterator f = F.begin(); f != F.end(); f++)
    {
      if (((*f)->source == p1) || ((*f)->target == p1))
      {
	removeArc(*f);
	changes = true;
	break;
      }	
    }
  } while (changes);

  
  for (std::vector<Place *>::iterator p = P.begin(); p != P.end(); p++)
  {
    if (*p == p1)
    {
      P.erase(p);
      return;
    }
  }
}



void PetriNet::removeArc(Arc *f1)
{
  for (std::vector<Arc *>::iterator f = F.begin(); f != F.end(); f++)
  {
    if (*f == f1)
    {
      F.erase(f);
      return;
    }
  }
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



void PetriNet::mergePlaces(Place *p1, Place *p2)
{
  Node *p12 = newPlace(p1->name + p2->name, p1->name + p2->name, 0);

  std::vector<Node *> pre1 = preset(p1);
  std::vector<Node *> pre2 = preset(p2);
  std::vector<Node *> post1 = postset(p1);
  std::vector<Node *> post2 = postset(p2);

  pre1.insert(pre1.end(), pre2.begin(), pre2.end());
  post1.insert(post1.end(), post2.begin(), post2.end());
  
  for (std::vector<Node *>::iterator n = pre1.begin(); n != pre1.end(); n++)
  {
    newArc(*n, p12);
  }

  for (std::vector<Node *>::iterator n = post1.begin(); n != post1.end(); n++)
  {
    newArc(p12, *n);
  }

  removePlace(p1);
  removePlace(p2);
}



std::vector<Node *> PetriNet::preset(Node *n)
{
  std::vector<Node *> result;

  for (std::vector<Arc *>::iterator f = F.begin(); f != F.end(); f++)
  {
    if ((*f)->target == n)
      result.push_back((*f)->source);
  }

  return result;
}



std::vector<Node *> PetriNet::postset(Node *n)
{
  std::vector<Node *> result;

  for (std::vector<Arc *>::iterator f = F.begin(); f != F.end(); f++)
  {
    if ((*f)->source == n)
      result.push_back((*f)->target);
  }

  return result;
}



/******************************************************************************

int main()
{
  PetriNet TheNet = PetriNet();
  
  Place *p1 = TheNet.newPlace("p1", "p1", 0);
  Place *p2 = TheNet.newPlace("p2", "p2", 0);
  Place *p3 = TheNet.newPlace("p3", "p3", 0);
  Place *p4 = TheNet.newPlace("p4", "p4", 0);
  Place *p5 = TheNet.newPlace("p5", "p5", 0);
  Place *p6 = TheNet.newPlace("p6", "p6", 0);
  
  Transition *t1 = TheNet.newTransition("t1", "t1", 0);
  Transition *t2 = TheNet.newTransition("t2", "t2", 0);
  Transition *t3 = TheNet.newTransition("t3", "t3", 0);

  TheNet.newArc(p1, t1);
  TheNet.newArc(t1, p2);
  TheNet.newArc(p3, t2);
  TheNet.newArc(t2, p4);
  TheNet.newArc(p5, t3);
  TheNet.newArc(t3, p6);
  
  //TheNet.drawDot();

  TheNet.mergePlaces(p2, p3);
  TheNet.mergePlaces(p4, p5);

  TheNet.drawDot();
  
  return 1;
}

******************************************************************************/
