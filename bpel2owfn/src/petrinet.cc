/*!
 * \file petrinet.c
 *
 * \brief Functions for Petri nets (implementation)
 *
 * This file implements the classes and functions defined in petrinet.h.
 * 
 * \author  
 *          - Niels Lohmann <nlohmann@informatik.hu-berlin.de>
 *          
 * \date    2005-11-09
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \version
 *          - 2005-11-09 (nlohmann) Added debug output and doxygen comments.
 *          
 * \todo
 *          - finish commenting
 *          - move helper functions (#set_union) into another file
 */






#include "petrinet.h"


extern string intToString(int i); // defined in bpel-unparse.k
extern int debug_level;           // defined in debug.cc






/*****************************************************************************/
/* helper functions */

/*!
 * \brief Returns the union of two sets of strings.
 * \param a set of strings
 * \param b set of strings
 * \return union of the sets a and b
 * \todo
 *       - improve performance (is there a STL-solution?)
 */
set<string> set_union(set<string> a, set<string> b)
{
  set<string> result = a;

  for (set<string>::iterator it = b.begin(); it != b.end(); it++)
    result.insert(*it);

  return result;
}





/*!
 * \brief Returns the union of two sets of Node pointers.
 * \param a set of Node pointers
 * \param b set of Node pointers
 * \return union of the sets a and b
 * \todo
 *       - improve performance (is there a STL-solution?)
 */
set<Node *> set_union(set<Node *> a, set<Node *> b)
{
  set<Node *> result = a;

  for (set<Node *>::iterator it = b.begin(); it != b.end(); it++)
    result.insert(*it);

  return result;
}





/*****************************************************************************/


/*!
 * \param mysource      the source-node of the arc
 * \param mytarget      the target-node of the arc
 * \param mytype        the type of the arc (as defined in #arc_type)
 * \param myinscription the inscription of the arc (empty string for none)
 */
Arc::Arc(Node *mysource, Node* mytarget, arc_type mytype, string myinscription)
{
  source = mysource;
  target = mytarget;
  type = mytype;
  inscription = myinscription;
}





/*****************************************************************************/


/*!
 * \param myname  the internal name of the transition
 * \param role    the first role of the transition
 * \param myguard the guard of the transition
 */
Transition::Transition(string myname, string role, string myguard)
{
  guard = myguard;
  name = myname;
  history.insert(role);
}





/*****************************************************************************/


/*!
 * \param myname the internal name of the place
 * \param role   the first role of the place
 * \param mytype the type of the place (as defined in #place_type)
 */
Place::Place(string myname, string role, place_type mytype)
{
  type = mytype;
  name = myname;
  history.insert(role);
}





/*****************************************************************************/


/*!
 * \bug
 *      - debug text is not printed
 */
PetriNet::PetriNet()
{
  trace(TRACE_DEBUG, "[PN]\tCreated a Petri net object.\n");
}





/*!
 * Creates a low-level place.
 * \param name the internal name of the place
 * \param role the first role of the place
 * \return pointer of the created place
 */
Place *PetriNet::newPlace(string name, string role)
{
  return newPlace(name, role, LOW);
}





/*!
 * \param name   the internal name of the place
 * \param role   the first role of the place
 * \param mytype the type of the place (as defined in #place_type)
 * \return pointer of the created place
 */
Place *PetriNet::newPlace(string name, string role, place_type mytype)
{
  Place *p = new Place(name, role, mytype);
  P.push_back(p);
  return p;
}





/*!
 * Creates an unguarded transition.
 * \param name the internal name of the transition
 * \param role the first role of the transition
 * \return pointer of the created transition
 */
Transition *PetriNet::newTransition(string name, string role)
{
  return newTransition(name, role, "");
}





/*!
 * \param name  the internal name of the transition
 * \param role  the first role of the transition
 * \param guard guard of the transition
 * \return pointer of the created transition
 */
Transition *PetriNet::newTransition(string name, string role, string guard)
{
  Transition *t = new Transition(name, role, guard);
  T.push_back(t);
  return t;
}





/*!
 * Creates a (uninscripted) standard arc.
 * \param source source node of the arc
 * \param target target node of the arc
 * \return pointer of the created arc
 */
Arc *PetriNet::newArc(Node *source, Node *target)
{
  return newArc(source, target, STANDARD, "");
}





/*!
 * Creates an inscripted standard arc.
 * \param source      source node of the arc
 * \param target      target node of the arc
 * \param inscription inscription of the arc
 * \return pointer of the created arc
 */
Arc *PetriNet::newArc(Node *source, Node *target, string inscription)
{
  return newArc(source, target, STANDARD, inscription);
}





/*!
 * Creates an uninscripted arc.
 * \param source source node of the arc
 * \param target target node of the arc
 * \param type   type of the arc (as defined in #arc_type)
 * \return pointer of the created arc
 */
Arc *PetriNet::newArc(Node *source, Node *target, arc_type type)
{
  return newArc(source, target, type, "");
}





/*!
 * Creates an arc with arbitrary inscription and type.
 * \param source      source node of the arc
 * \param target      target node of the arc
 * \param type        type of the arc (as defined in #arc_type)
 * \param inscription inscription of the arc 
 * \return pointer of the created arc
 */
Arc *PetriNet::newArc(Node *source, Node *target, arc_type type, string inscription)
{
  Arc *f = new Arc(source, target, type, inscription);
  F.push_back(f);
  return f;
}





/*---------------------------------------------------------------------------*/

/*!
 * Removes a place and all arcs connected with it.
 * \param p1 place to be removed
 * \todo
 *        - improve performance by avoiding the fixed point operation
 *        - improve performance by avoiding a linear search in P
 */
void PetriNet::removePlace(Place *p1)
{
  // fixed-point operation: remove all in- and outgoing arcs
  bool changes;
  do
  {
    changes = false;
    for (vector<Arc *>::iterator f = F.begin(); f != F.end(); f++)
    {
      if (((*f)->source == p1) || ((*f)->target == p1))
      {
	removeArc(*f);
	changes = true;
	break;
      }	
    }
  } while (changes);

  
  // find the place and delete it
  for (vector<Place *>::iterator p = P.begin(); p != P.end(); p++)
  {
    if (*p == p1)
    {
      P.erase(p);
      return;
    }
  }
}





/*!
 * Removes a transition and all arcs connected with it.
 * \param t1 transition to be removed
 * \todo
 *        - improve performance by avoiding the fixed point operation
 *        - improve performance by avoiding a linear search in T
 */
void PetriNet::removeTransition(Transition *t1)
{
  // fixed-point operation: remove all in- and outgoing arcs
  bool changes;
  do
  {
    changes = false;
    for (vector<Arc *>::iterator f = F.begin(); f != F.end(); f++)
    {
      if (((*f)->source == t1) || ((*f)->target == t1))
      {
	removeArc(*f);
	changes = true;
	break;
      }	
    }
  } while (changes);

  // find the transition and delete it
  for (vector<Transition *>::iterator t = T.begin(); t != T.end(); t++)
  {
    if (*t == t1)
    {
      T.erase(t);
      return;
    }
  }
}






/*!
 * \param f1 arc to be removed
 * \todo
 *       - improve performance by avoiding a linear search in F
 */
void PetriNet::removeArc(Arc *f1)
{
  for (vector<Arc *>::iterator f = F.begin(); f != F.end(); f++)
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
  cerr << P.size() << " places, ";
  cerr << T.size() << " transitions, ";
  cerr << F.size() << " arcs" << endl;
}





/*!
 * Creates a DOT output (see http://www.graphviz.org) of the Petri net. It uses
 * the digraph-statement and adds labels to transitions, places and arcs if
 * neccessary. It also distinguishes the three arc types of #arc_type.
 *
 * \todo
 *       - treatment of input and output places
 */
void PetriNet::drawDot()
{
  cout << "digraph N {" << endl;

  // list the places
  cout << " node [shape=circle fixedsize];" << endl;
  for (vector<Place *>::iterator p = P.begin(); p != P.end(); p++)
    cout << " " << (*p)->name << ";" << endl;

  // list the transitions
  cout << endl << " node [shape=box fixedsize];" << endl;
  for (vector<Transition *>::iterator t = T.begin(); t != T.end(); t++)
  {
    if ((*t)->guard != "")
    {
      cout << " " << (*t)->name << " [";
      cout << " shape=record label=\"{" << (*t)->name << "|{" << (*t)->guard
	<< "}}\"]" << endl;
    }
    else
      cout << " " << (*t)->name << endl;
  }

  // list the arcs
  for (vector<Arc *>::iterator f = F.begin(); f != F.end(); f++)
  {
    cout << " " << (*f)->source->name << " -> " << (*f)->target->name << " [";
    if ((*f)->type == RESET)
      cout << " arrowtail=odot";
    else if ((*f)->type == READ)
      cout << " arrowhead=diamond arrowtail=tee";

    if ((*f)->inscription != "")
      cout << " label=\"" << (*f)->inscription << "\"";

    cout << "];" << endl;
  }

  cout << "}" << endl;
}





/*---------------------------------------------------------------------------*/


/*!
 * \todo
 *       - take care of read and reset arcs
 */
void PetriNet::mergeTransitions(Transition *t1, Transition *t2)
{
  trace(TRACE_DEBUG, "[PN]\tMerging transitions '" + t1->name + "' and '" + t2->name + "'.\n");
  
  if (t1->guard != "" || t2->guard != "")
  {
    cerr << "[PN]\tMerging of guarded transition not yet supported!" << endl;
    return;
  }
	
  Node *t12 = newTransition(t1->name + t2->name, t1->name + t2->name, "");

  t12->history.clear();
  
  for (set<string>::iterator role = t1->history.begin(); role != t1->history.end(); role++)
    t12->history.insert(*role);
  
  for (set<string>::iterator role = t2->history.begin(); role != t2->history.end(); role++)
    t12->history.insert(*role);
  
  set<Node *> pre12 = set_union(preset(t1), preset(t2));
  set<Node *> post12 = set_union(postset(t1), postset(t2));
  
  // TODO we have to take care of reset and read arcs!
  
  for (set<Node *>::iterator n = pre12.begin(); n != pre12.end(); n++)
    newArc(*n, t12);

  for (set<Node *>::iterator n = post12.begin(); n != post12.end(); n++)
    newArc(t12, *n);
  
  removeTransition(t1);
  removeTransition(t2);
}





/*!
 * \todo
 *       - take care of read and reset arcs
 */
void PetriNet::mergePlaces(Place *p1, Place *p2)
{
  trace(TRACE_DEBUG, "[PN]\tMerging places '" + p1->name + "' and '" + p2->name + "'.\n");
  
  if(p1->type != LOW || p2->type != LOW)
  {
    cerr << "[PN]\tMerging of high-level places not yet supported!" << endl;
    return;
  }
  
  Node *p12 = newPlace(p1->name + p2->name, p1->name + p2->name, LOW);

  p12->history.clear();
  
  for (set<string>::iterator role = p1->history.begin(); role != p1->history.end(); role++)
    p12->history.insert(*role);
  
  for (set<string>::iterator role = p2->history.begin(); role != p2->history.end(); role++)
    p12->history.insert(*role);

  set<Node *> pre12 = set_union(preset(p1), preset(p2));
  set<Node *> post12 = set_union(postset(p1), postset(p2));
  
  // TODO we have to take care of reset and read arcs!
  
  for (set<Node *>::iterator n = pre12.begin(); n != pre12.end(); n++)
    newArc(*n, p12);

  for (set<Node *>::iterator n = post12.begin(); n != post12.end(); n++)
    newArc(p12, *n);
  
  removePlace(p1);
  removePlace(p2);
}



void PetriNet::mergePlaces(string role1, string role2)
{
  mergePlaces(findPlaceRole(role1), findPlaceRole(role2));
}



void PetriNet::mergePlaces(kc::impl_activity* act1, string role1, kc::impl_activity* act2, string role2)
{
  mergePlaces(intToString(act1->id->value) + role1, intToString(act2->id->value) + role2);
}





/*---------------------------------------------------------------------------*/


/*!
 * \param n a node of the Petri net
 * \result the preset of node n
 */
set<Node *> PetriNet::preset(Node *n)
{
  set<Node *> result;

  for (vector<Arc *>::iterator f = F.begin(); f != F.end(); f++)
    if ((*f)->target == n)
      result.insert((*f)->source);

  return result;
}





/*!
 * \param n a node of the Petri net
 * \result the postset of node n
 */
set<Node *> PetriNet::postset(Node *n)
{
  set<Node *> result;

  for (vector<Arc *>::iterator f = F.begin(); f != F.end(); f++)
    if ((*f)->source == n)
      result.insert((*f)->target);

  return result;
}


/*---------------------------------------------------------------------------*/


Place *PetriNet::findPlace(string name)
{
  for (vector<Place *>::iterator p = P.begin(); p != P.end(); p++)
    if ( (*p)->name == name)
      return *p;

  cerr << "Node '" << name << "' not found!" << endl;  
  return NULL;
}



Place *PetriNet::findPlaceRole(string role)
{
  for (vector<Place *>::iterator p = P.begin(); p != P.end(); p++)
    for (set<string>::iterator r = (*p)->history.begin(); r != (*p)->history.end(); r++)
      if ( (*r).substr((*r).find_first_of(".")+1) == role )
	return *p;

  cerr << "Node with role '" << role << "' not found!" << endl;  
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

  vector<Transition *> left;
  vector<Transition *> right;
  
  for (vector<Transition *>::iterator t1 = T.begin(); t1 != T.end(); t1++)
  {
    for (vector<Transition *>::iterator t2 = t1; t2 != T.end(); t2++)
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
