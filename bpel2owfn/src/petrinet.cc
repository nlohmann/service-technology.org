/*
 * petrinet.c
 *
 * Change log:
 * 
 * date       | author        | changes
 * ---------------------------------------------------------------------
 * 2005-11-09 | Lohmann       | - added debug output
 *
 */

#include "petrinet.h"

extern std::string intToString(int i); // defined in bpel-unparse.k
extern int debug_level;                // defined in debug.cc


/*****************************************************************************/
/* helper functions */

/// return the union of two string sets
std::set<std::string> set_union(std::set<std::string> a, std::set<std::string> b)
{
  std::set<std::string> result = a;

  for (std::set<std::string>::iterator it = b.begin(); it != b.end(); it++)
    result.insert(*it);

  return result;
}

/// return the union of two Node sets
std::set<Node *> set_union(std::set<Node *> a, std::set<Node *> b)
{
  std::set<Node *> result = a;

  for (std::set<Node *>::iterator it = b.begin(); it != b.end(); it++)
    result.insert(*it);

  return result;
}



/*****************************************************************************/

Arc::Arc(Node *mysource, Node* mytarget, arc_type mytype, std::string myinscription)
{
  source = mysource;
  target = mytarget;
  type = mytype;
  inscription = myinscription;
}



/*****************************************************************************/

Transition::Transition(std::string myname, std::string role, std::string myguard)
{
  guard = myguard;
  name = myname;
  history.insert(role);
}



/*****************************************************************************/

Place::Place(std::string myname, std::string role, place_type mytype)
{
  type = mytype;
  name = myname;
  history.insert(role);
}



/*****************************************************************************/

PetriNet::PetriNet()
{
}



Place *PetriNet::newPlace(std::string name, std::string role)
{
  return newPlace(name, role, LOW);
}


Place *PetriNet::newPlace(std::string name, std::string role, place_type mytype)
{
  Place *p = new Place(name, role, mytype);
  P.push_back(p);
  return p;
}


Transition *PetriNet::newTransition(std::string name, std::string role)
{
  return newTransition(name, role, "");
}


Transition *PetriNet::newTransition(std::string name, std::string role, std::string guard)
{
  Transition *t = new Transition(name, role, guard);
  T.push_back(t);
  return t;
}


Arc *PetriNet::newArc(Node *source, Node *target)
{
  return newArc(source, target, STANDARD, "");
}


Arc *PetriNet::newArc(Node *source, Node *target, std::string inscription)
{
  return newArc(source, target, STANDARD, inscription);
}


Arc *PetriNet::newArc(Node *source, Node *target, arc_type type)
{
  return newArc(source, target, type, "");
}


Arc *PetriNet::newArc(Node *source, Node *target, arc_type type, std::string inscription)
{
  Arc *f = new Arc(source, target, type, inscription);
  F.push_back(f);
  return f;
}

/*---------------------------------------------------------------------------*/

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

/*---------------------------------------------------------------------------*/

void PetriNet::information()
{
  std::cout << P.size() << " places, ";
  std::cout << T.size() << " transitions, ";
  std::cout << F.size() << " arcs" << std::endl;
}



void PetriNet::drawDot()
{
  std::cout << "digraph N {" << std::endl;

  // list the places
  std::cout << " node [shape=circle fixedsize];" << std::endl;
  for (std::vector<Place *>::iterator p = P.begin(); p != P.end(); p++)
    std::cout << " " << (*p)->name << ";" << std::endl;

  // list the transitions
  std::cout << std::endl << " node [shape=box fixedsize];" << std::endl;
  for (std::vector<Transition *>::iterator t = T.begin(); t != T.end(); t++)
  {
    if ((*t)->guard != "")
    {
      std::cout << " " << (*t)->name << " [";
      std::cout << " shape=record label=\"{" << (*t)->name << "|{" << (*t)->guard << "}}\"]" << std::endl;
    }
    else
      std::cout << " " << (*t)->name << std::endl;
  }

  // list the arcs
  for (std::vector<Arc *>::iterator f = F.begin(); f != F.end(); f++)
  {
    std::cout << " " << (*f)->source->name << " -> " << (*f)->target->name << " [";
    if ((*f)->type == RESET)
      std::cout << " arrowtail=odot";
    else if ((*f)->type == READ)
      std::cout << " arrowhead=diamond arrowtail=tee";

    if ((*f)->inscription != "")
      std::cout << " label=\"" << (*f)->inscription << "\"";

    std::cout << "];" << std::endl;
  }

  std::cout << "}" << std::endl;
}


/*---------------------------------------------------------------------------*/


void PetriNet::mergeTransitions(Transition *t1, Transition *t2)
{
  trace(TRACE_DEBUG, "[PN]\tMerging transitions '" + t1->name + "' and '" + t2->name + "'.\n");
  
  if (t1->guard != "" || t2->guard != "")
  {
    std::cerr << "[PN]\tMerging of guarded transition not yet supported!" << std::endl;
    return;
  }
	
  Node *t12 = newTransition(t1->name + t2->name, t1->name + t2->name, "");

  t12->history.clear();
  
  for (std::set<std::string>::iterator role = t1->history.begin(); role != t1->history.end(); role++)
    t12->history.insert(*role);
  
  for (std::set<std::string>::iterator role = t2->history.begin(); role != t2->history.end(); role++)
    t12->history.insert(*role);
  
  std::set<Node *> pre12 = set_union(preset(t1), preset(t2));
  std::set<Node *> post12 = set_union(postset(t1), postset(t2));
  
  // TODO we have to take care of reset and read arcs!
  
  for (std::set<Node *>::iterator n = pre12.begin(); n != pre12.end(); n++)
    newArc(*n, t12);

  for (std::set<Node *>::iterator n = post12.begin(); n != post12.end(); n++)
    newArc(t12, *n);
  
  removeTransition(t1);
  removeTransition(t2);
}




void PetriNet::mergePlaces(Place *p1, Place *p2)
{
  trace(TRACE_DEBUG, "[PN]\tMerging places '" + p1->name + "' and '" + p2->name + "'.\n");
  
  if(p1->type != LOW || p2->type != LOW)
  {
    std::cerr << "[PN]\tMerging of high-level places not yet supported!" << std::endl;
    return;
  }
  
  Node *p12 = newPlace(p1->name + p2->name, p1->name + p2->name, LOW);

  p12->history.clear();
  
  for (std::set<std::string>::iterator role = p1->history.begin(); role != p1->history.end(); role++)
    p12->history.insert(*role);
  
  for (std::set<std::string>::iterator role = p2->history.begin(); role != p2->history.end(); role++)
    p12->history.insert(*role);

  std::set<Node *> pre12 = set_union(preset(p1), preset(p2));
  std::set<Node *> post12 = set_union(postset(p1), postset(p2));
  
  // TODO we have to take care of reset and read arcs!
  
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


/*---------------------------------------------------------------------------*/


std::set<Node *> PetriNet::preset(Node *n)
{
  std::set<Node *> result;

  for (std::vector<Arc *>::iterator f = F.begin(); f != F.end(); f++)
    if ((*f)->target == n)
      result.insert((*f)->source);

  return result;
}



std::set<Node *> PetriNet::postset(Node *n)
{
  std::set<Node *> result;

  for (std::vector<Arc *>::iterator f = F.begin(); f != F.end(); f++)
    if ((*f)->source == n)
      result.insert((*f)->target);

  return result;
}


/*---------------------------------------------------------------------------*/


Place *PetriNet::findPlace(std::string name)
{
  for (std::vector<Place *>::iterator p = P.begin(); p != P.end(); p++)
    if ( (*p)->name == name)
      return *p;

  std::cerr << "Node '" << name << "' not found!" << std::endl;  
  return NULL;
}



Place *PetriNet::findPlaceRole(std::string role)
{
  for (std::vector<Place *>::iterator p = P.begin(); p != P.end(); p++)
    for (std::set<std::string>::iterator r = (*p)->history.begin(); r != (*p)->history.end(); r++)
      if ( (*r).substr((*r).find_first_of(".")+1) == role )
	return *p;

  std::cerr << "Node with role '" << role << "' not found!" << std::endl;  
  return NULL;
}


/*---------------------------------------------------------------------------*/


void PetriNet::simplify()
{
  if (debug_level > TRACE_INFORMATION)
  {
    trace(TRACE_DEBUG, "[PN]\tPetri net size before simplification: ");
    information();
  }
  
  trace(TRACE_INFORMATION, "Simplifying Petri net...\n");

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

  trace(TRACE_INFORMATION, "Simplifying complete.\n");

  if (debug_level > TRACE_INFORMATION)
  {
    trace(TRACE_DEBUG, "[PN]\tPetri net size after simplification: ");
    information();
  }
  
}
