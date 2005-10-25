#include "petrinet.h"

extern std::string intToString(int i);


/* helpers */

std::set<std::string> set_union(std::set<std::string> a, std::set<std::string> b)
{
  std::set<std::string> result = a;

  for (std::set<std::string>::iterator it = b.begin(); it != b.end(); it++)
  {
    result.insert(*it);
  }

  return result;
}

std::set<Node *> set_union(std::set<Node *> a, std::set<Node *> b)
{
  std::set<Node *> result = a;

  for (std::set<Node *>::iterator it = b.begin(); it != b.end(); it++)
  {
    result.insert(*it);
  }

  return result;
}





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
  roles.insert(role);
}

/*****************************************************************************/

Place::Place(std::string myname, std::string role, unsigned int mytype)
{
  type = mytype;
  name = myname;
  roles.insert(role);
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


void PetriNet::removeTransition(Transition *t1)
{
  bool changes;
  
  do
  {
    changes = false;
    for (std::vector<Arc *>::iterator f = F.begin(); f != F.end(); f++)
    {
      if (((*f)->source == t1) || ((*f)->target == t1))
      {
	removeArc(*f);
	changes = true;
	break;
      }	
    }
  } while (changes);

  
  for (std::vector<Transition *>::iterator t = T.begin(); t != T.end(); t++)
  {
    if (*t == t1)
    {
      T.erase(t);
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



void PetriNet::mergeTransitions(Transition *t1, Transition *t2)
{
  Node *t12 = newTransition(t1->name + t2->name, t1->name + t2->name, 0);

  t12->roles.clear();
  
  for (std::set<std::string>::iterator role = t1->roles.begin(); role != t1->roles.end(); role++)
    t12->roles.insert(*role);
  
  for (std::set<std::string>::iterator role = t2->roles.begin(); role != t2->roles.end(); role++)
    t12->roles.insert(*role);
  
  std::set<Node *> pre12 = set_union(preset(t1), preset(t2));
  std::set<Node *> post12 = set_union(postset(t1), postset(t2));
  
  for (std::set<Node *>::iterator n = pre12.begin(); n != pre12.end(); n++)
    newArc(*n, t12);

  for (std::set<Node *>::iterator n = post12.begin(); n != post12.end(); n++)
    newArc(t12, *n);
  
  removeTransition(t1);
  removeTransition(t2);
}




void PetriNet::mergePlaces(Place *p1, Place *p2)
{
  Node *p12 = newPlace(p1->name + p2->name, p1->name + p2->name, 0);

  p12->roles.clear();
  
  for (std::set<std::string>::iterator role = p1->roles.begin(); role != p1->roles.end(); role++)
    p12->roles.insert(*role);
  
  for (std::set<std::string>::iterator role = p2->roles.begin(); role != p2->roles.end(); role++)
    p12->roles.insert(*role);

  std::set<Node *> pre12 = set_union(preset(p1), preset(p2));
  std::set<Node *> post12 = set_union(postset(p1), postset(p2));
  
  for (std::set<Node *>::iterator n = pre12.begin(); n != pre12.end(); n++)
    newArc(*n, p12);

  for (std::set<Node *>::iterator n = post12.begin(); n != post12.end(); n++)
    newArc(p12, *n);
  
  removePlace(p1);
  removePlace(p2);
}



void PetriNet::mergePlaces(std::string role1, std::string role2)
{
  mergePlaces(findPlaceRole(role1), findPlaceRole(role2));
}


void PetriNet::mergePlaces(kc::impl_activity* act1, std::string role1, kc::impl_activity* act2, std::string role2)
{
  mergePlaces(intToString(act1->id->value) + role1, intToString(act2->id->value) + role2);
}


std::set<Node *> PetriNet::preset(Node *n)
{
  std::set<Node *> result;

  for (std::vector<Arc *>::iterator f = F.begin(); f != F.end(); f++)
  {
    if ((*f)->target == n)
      result.insert((*f)->source);
  }

  return result;
}



std::set<Node *> PetriNet::postset(Node *n)
{
  std::set<Node *> result;

  for (std::vector<Arc *>::iterator f = F.begin(); f != F.end(); f++)
  {
    if ((*f)->source == n)
      result.insert((*f)->target);
  }

  return result;
}



Place *PetriNet::findPlace(std::string name)
{
  for (std::vector<Place *>::iterator p = P.begin(); p != P.end(); p++)
  {
    if ( (*p)->name == name)
      return *p;
  }

  std::cerr << "Node '" << name << "' not found!" << std::endl;  
  return NULL;
}



Place *PetriNet::findPlaceRole(std::string role)
{
  for (std::vector<Place *>::iterator p = P.begin(); p != P.end(); p++)
    for (std::set<std::string>::iterator r = (*p)->roles.begin(); r != (*p)->roles.end(); r++)
      if ( (*r).substr((*r).find_first_of(".")+1) == role )
	return *p;

  std::cerr << "Node with role '" << role << "' not found!" << std::endl;  
  return NULL;
}





void PetriNet::simplify()
{
  std::vector<Transition *> left;
  std::vector<Transition *> right;
  
  for (std::vector<Transition *>::iterator t1 = T.begin(); t1 != T.end(); t1++)
  {
    for (std::vector<Transition *>::iterator t2 = t1; t2 != T.end(); t2++)
    {
      if (*t1 != *t2)
      {
	if ( (preset(*t1) == preset(*t2)) && (postset(*t1) == postset(*t2)) )
	{
	  left.push_back(*t1);
	  right.push_back(*t2);
	}
      }
    }
  }

  for (int i = 0; i < left.size(); i++)
    mergeTransitions(left[i], right[i]);
  
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
