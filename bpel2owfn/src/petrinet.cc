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
 *          - 2005-11-10 (nlohmann) Improved #set_union, #PetriNet::simplify.
 *            Respected #dot_output for #drawDot function. Finished commenting.
 *          
 * \todo
 *          - move helper functions (#set_union) into another file
 */






#include "petrinet.h"


extern string intToString(int i); // defined in bpel-unparse.k
extern int debug_level;           // defined in debug.cc
extern std::ostream *dot_output;  // defined in main.c





/*****************************************************************************/
/* helper function */

/*!
 * \brief Returns the union of two sets of type T.
 * \param a set of type T
 * \param b set of type T
 * \return union of the sets a and b
 */
template <typename T> set<T> set_union(set<T> a, set<T> b)
{
  set<T> result;
  insert_iterator<set<T, less<T> > > res_ins(result, result.begin());
  set_union(a.begin(), a.end(), b.begin(), b.end(), res_ins);
  
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
  (*dot_output) << "digraph N {" << endl;

  // list the places
  (*dot_output) << " node [shape=circle fixedsize];" << endl;
  for (vector<Place *>::iterator p = P.begin(); p != P.end(); p++)
    (*dot_output) << " " << (*p)->name << ";" << endl;

  // list the transitions
  (*dot_output) << endl << " node [shape=box fixedsize];" << endl;
  for (vector<Transition *>::iterator t = T.begin(); t != T.end(); t++)
  {
    if ((*t)->guard != "")
    {
      (*dot_output) << " " << (*t)->name << " [";
      (*dot_output) << " shape=record label=\"{" << (*t)->name << "|{"
	<< (*t)->guard << "}}\"]" << endl;
    }
    else
      (*dot_output) << " " << (*t)->name << endl;
  }

  // list the arcs
  for (vector<Arc *>::iterator f = F.begin(); f != F.end(); f++)
  {
    (*dot_output) << " " << (*f)->source->name << " -> "
      << (*f)->target->name << " [";
    
    if ((*f)->type == RESET)
      (*dot_output) << " arrowtail=odot";
    else if ((*f)->type == READ)
      (*dot_output) << " arrowhead=diamond arrowtail=tee";

    if ((*f)->inscription != "")
      (*dot_output) << " label=\"" << (*f)->inscription << "\"";

    (*dot_output) << "];" << endl;
  }

  (*dot_output) << "}" << endl;
}





/*---------------------------------------------------------------------------*/


/*!
 * Merges two transitions. Given transitions t1 and t2:
 *
 * -# a new transition t12 with empty history is created
 * -# this transition gets the union of the histories of transition t1 and t2
 * -# the presets and postsets of t1 and t2 are calculated and united
 * -# t12 is connected with all the places in the preset and postset
 * -# the transitions t1 and t2 are removed
 * 
 * \param t1 first transition
 * \param t2 second transition
 * 
 * \todo
 *       - take care of read and reset arcs
 *       - call an error-function to signal error 
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
  
  for (set<Node *>::iterator n = pre12.begin(); n != pre12.end(); n++)
    newArc(*n, t12);

  for (set<Node *>::iterator n = post12.begin(); n != post12.end(); n++)
    newArc(t12, *n);
  
  removeTransition(t1);
  removeTransition(t2);
}





/*!
 * The actual function to merge two places. Given places p1 and p2:
 *
 * -# a new place p12 with empty history is created
 * -# this place gets the union of the histories of place p1 and p2
 * -# the presets and postsets of p1 and p2 are calculated and united
 * -# p12 is connected with all the transitions in the preset and postset
 * -# the places p1 and p2 are removed
 * 
 * \param p1 first place
 * \param p2 second place
 * 
 * \todo
 *       - take care of read and reset arcs
 *       - take care of markings
 *       - call an error-function to signal error 
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
  
  for (set<Node *>::iterator n = pre12.begin(); n != pre12.end(); n++)
    newArc(*n, p12);

  for (set<Node *>::iterator n = post12.begin(); n != post12.end(); n++)
    newArc(p12, *n);
  
  removePlace(p1);
  removePlace(p2);
}





/*!
 * \param role1 string describing the role of the first place
 * \param role2 string describing the role of the second place
 */
void PetriNet::mergePlaces(string role1, string role2)
{
  mergePlaces(findPlaceRole(role1), findPlaceRole(role2));
}





/*!
 * Merges two places given two activity of the abstract syntax tree and the
 * roles of the places. The activities are used to complete the role-string
 * and pass the search request.
 * 
 * \param act1  activity of the AST represented by the first place
 * \param role1 string describing the role of the first place
 * \param act2  activity of the AST represented by the second place
 * \param role2 string describing the role of the second place
 */
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


/*!
 * Finds a place of the Petri net given the internal name of the place.
 *
 * \param  name the demanded name
 * \return a pointer to the place or NULL if the place was not found.
 *
 * \todo
 *       - call an error-function to signal error
 */
Place *PetriNet::findPlace(string name)
{
  for (vector<Place *>::iterator p = P.begin(); p != P.end(); p++)
    if ( (*p)->name == name)
      return *p;

  cerr << "Node '" << name << "' not found!" << endl;  
  return NULL;
}





/*!
 * Finds a place of the Petri net given a role the place fills or filled.
 *
 * \param  role the demanded role
 * \return a pointer to the place or NULL if the place was not found.
 *
 * \todo
 *       - call an error-function to signal error
 */
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


/*!
 * Implements some simple structural reduction rules for Petri nets:
 * 
 * - If two transitions t1 and t2 have the same preset and postset, one of them
 *   can safely be removed.
 *
 * \todo
 *       - improve performance
 *       - implement more reduction rules
 *       - does the transitions have to be merged or rather be removed?
 *
 */
void PetriNet::simplify()
{
  if (debug_level > TRACE_INFORMATION)
  {
    trace(TRACE_DEBUG, "[PN]\tPetri net size before simplification: ");
    information();
  }
  trace(TRACE_INFORMATION, "Simplifying Petri net...\n");

  
  // a pair to store transitions to be merged
  vector <pair <Transition *, Transition *> > transitionPairs;

  // find transitions with same preset and postset  
  for (vector<Transition *>::iterator t1 = T.begin(); t1 != T.end(); t1++)
    for (vector<Transition *>::iterator t2 = t1; t2 != T.end(); t2++)
      if (*t1 != *t2)
	if ( (preset(*t1) == preset(*t2)) && (postset(*t1) == postset(*t2)) )
	  transitionPairs.push_back(std::pair<Transition *, Transition *>(*t1, *t2));

  // merge the found transitions
  for (unsigned int i = 0; i < transitionPairs.size(); i++)
    mergeTransitions(transitionPairs[i].first, transitionPairs[i].second);
  

  trace(TRACE_INFORMATION, "Simplifying complete.\n");
  if (debug_level > TRACE_INFORMATION)
  {
    trace(TRACE_DEBUG, "[PN]\tPetri net size after simplification: ");
    information();
  }
}
