/*!
 * \file petrinet.cc
 *
 * \brief Functions for Petri nets (implementation)
 *
 * This file implements the classes and functions defined in petrinet.h.
 * 
 * \author  
 *          - responsible: Niels Lohmann <nlohmann@informatik.hu-berlin.de>
 *          - last changes of: \$Author: nlohmann $
 *          
 * \date
 *          - created: 2005/10/18
 *          - last changed: \$Date: 2005/11/16 16:10:07 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \version \$Revision: 1.30 $
 *          - 2005-11-09 (nlohmann) Added debug output and doxygen comments.
 *          - 2005-11-10 (nlohmann) Improved #set_union, #PetriNet::simplify.
 *            Respected #dot_output for #drawDot function. Finished commenting.
 *          - 2005-11-11 (nlohmann) Changed intenal name (string) to an id
 *            (unsigned int). Improved functions that use #P, #T, #F that are
 *            sets now. Added function #PetriNet::detachNode.
 *          - 2005-11-13 (nlohmann) Added function #PetriNet::id. Explicitly
 *            call destructors of #Arc, #Place and #Transition. Added function
 *            #PetriNet::makeLowLevel. Added CVS-tags.
 *          - 2005-11-14 (nlohmann) Added new reduction rule. Added functions
 *            #PetriNet::longInformation() and #PetriNet::lolaOut(). Use
 *            #Exception-class to signal errors.
 *          - 2005-11-16 (nlohmann) Added a mapping for faster access to Nodes
 *            given a role.
 *
 */





#include "petrinet.h"

/// Mapping of roles to nodes of the Petri net.
map <string, Node*> roleMap;



extern int debug_level;           // defined in debug.cc
extern ostream *dot_output;       // defined in main.c
extern ostream *lola_output;      // defined in main.c
extern ostream *info_output;      // defined in main.c
extern string filename;           // defined in main.c





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
 * \param myid    the id of the transition
 * \param role    the first role of the transition
 * \param myguard the guard of the transition
 */
Transition::Transition(unsigned int myid, string role, string myguard)
{
  guard = myguard;
  id = myid;
  
  if (role != "")
    history.insert(role);
}





/*****************************************************************************/


/*!
 * \param myid   the internal id of the place
 * \param role   the first role of the place
 * \param mytype the type of the place (as defined in #place_type)
 */
Place::Place(unsigned int myid, string role, place_type mytype)
{
  type = mytype;
  id = myid;

  if (role != "")
    history.insert(role);
}





/*****************************************************************************/


/*!
 * \bug
 *      - debug text is not printed
 */
PetriNet::PetriNet()
{
  trace(TRACE_DEBUG, "[PN]\tCreating a Petri net object.\n");
  lowLevel = false;
  nextId = 0;
}





/*---------------------------------------------------------------------------*/


/*!
 * Creates a low-level place without an initial role.
 * \return pointer of the created place
 */
Place *PetriNet::newPlace()
{
  return newPlace("", LOW);
}





/*!
 * Creates a low-level place.
 * \param role the initial role of the place
 * \return pointer of the created place
 */
Place *PetriNet::newPlace(string role)
{
  return newPlace(role, LOW);
}





/*!
 * The actual newPlace function called by all other overloaded newPlace
 * functions.
 *
 * \param role   the initial role of the place
 * \param mytype the type of the place (as defined in #place_type)
 * \return pointer of the created place
 */
Place *PetriNet::newPlace(string role, place_type mytype)
{
  trace(TRACE_VERY_DEBUG, "[PN]\tCreating place " + intToString(id()) +
      "...\n");
  
  Place *p = new Place(getId(), role, mytype);
  P.insert(p);

  if (role != "")
    roleMap[role] = p;

  if (P.size() % 100 == 0)
    trace(TRACE_INFORMATION, "[PN]\t" + information() + "\n");
  
  return p;
}





void PetriNet::markPlace(Place *p)
{
}





/*---------------------------------------------------------------------------*/


/*!
 * Creates an unguarded transition without an initial role.
 * \return pointer of the created transition
 */
Transition *PetriNet::newTransition()
{
  return newTransition("", "");
}





/*!
 * Creates an unguarded transition.
 * \param role the initial role of the transition
 * \return pointer of the created transition
 */
Transition *PetriNet::newTransition(string role)
{
  return newTransition(role, "");
}





/*!
 * The actual newTransition function called by all other overloaded
 * newTransition functions.
 *
 * \param role  the initial role of the transition
 * \param guard guard of the transition
 * \return pointer of the created transition
 */
Transition *PetriNet::newTransition(string role, string guard)
{
  trace(TRACE_VERY_DEBUG, "[PN]\tCreating transition " + intToString(id()) +
      "...\n");
  
  Transition *t = new Transition(getId(), role, guard);
  T.insert(t);

  if (T.size() % 100 == 0)
    trace(TRACE_INFORMATION, "[PN]\t" + information() + "\n");
  
  return t;
}





/*---------------------------------------------------------------------------*/


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
  trace(TRACE_VERY_DEBUG, "[PN]\tCreating arc (" + intToString(source->id) +
      "," + intToString(target->id) + ")...\n");

  Arc *f = new Arc(source, target, type, inscription);
  F.insert(f);

  return f;
}





/*---------------------------------------------------------------------------*/

/*!
 * Removes all ingoing and outgoing arcs of a node, i.e. detatches the node
 * from the rest of the net.
 * 
 * \param n node to be detached
 */
void PetriNet::detachNode(Node *n)
{
  trace(TRACE_VERY_DEBUG, "[PN]\tDetaching node " + intToString(n->id) +
      "...\n");
  
  vector<Arc *> removeList;

  for (set<Arc *>::iterator f = F.begin(); f != F.end(); f++)
    if (((*f)->source == n) || ((*f)->target == n))
      removeList.push_back(*f);

  for (unsigned int i=0; i<removeList.size(); i++)
    removeArc(removeList[i]);
}



/*!
 * Removes a place and all arcs connected with it.
 * \param p place to be removed
 */
void PetriNet::removePlace(Place *p)
{
  trace(TRACE_VERY_DEBUG, "[PN]\tRemoving place " + intToString(p->id) +
      "...\n");
  
  detachNode(p);
  P.erase(p);
  delete p;
}





/*!
 * Removes a transition and all arcs connected with it.
 * \param t transition to be removed
 */
void PetriNet::removeTransition(Transition *t)
{
  trace(TRACE_VERY_DEBUG, "[PN]\tRemoving transition " + intToString(t->id) +
      "...\n");
  
  detachNode(t);
  T.erase(t);
  delete t;
}






/*!
 * \param f arc to be removed
 */
void PetriNet::removeArc(Arc *f)
{
  trace(TRACE_VERY_DEBUG, "[PN]\tRemoving arc (" + intToString(f->source->id) +
      "," + intToString(f->target->id) + ")...\n");
  
  F.erase(f);
  delete f;
}






/*---------------------------------------------------------------------------*/


/*!
 * \return string containing information about the net
 */
string PetriNet::information()
{
  string result;
  result += intToString(P.size()) + " places, " + intToString(T.size()) +
    " transitions, " +  intToString(F.size()) + " arcs";
  return result;
}



/*!
 * Prints information about the generated Petri net. In particular, for each
 * place and transition all roles of the history are printed to understand
 * the net and maybe LoLA's witness pathes later.
 * 
 * \todo
 *       - add command-line parameter -i / --information
 *       - write information to file
 */
void PetriNet::printInformation()
{
  // the places
  (*info_output) << "PLACES:\n";
  (*info_output) << "ID\tTYPE\t\tROLES\n";
  for (set<Place *>::iterator p = P.begin(); p != P.end(); p++)
  {
    (*info_output) << "p" << ((*p)->id) << "\t";

    switch ((*p)->type)
    {
      case(LOW):      { (*info_output) << "low-level"; break; }
      case(TIME):     { (*info_output) << "time"; break; }
      case(PROPERTY): { (*info_output) << "property"; break; }
      case(MESSAGE):  { (*info_output) << "message"; break; }
      case(DATA):     { (*info_output) << "data"; break; }
      default:        { (*info_output) << "other"; }
    }

    for (set<string>::iterator role = (*p)->history.begin(); role != (*p)->history.end(); role++)
    {
      if (role == (*p)->history.begin())
	(*info_output) << "\t" + *role + "\n";
      else
	(*info_output) << "\t\t\t" + *role + "\n";
    }
  }
  
  
  // the transitions
  (*info_output) << "\nTRANSITIONS:\n";
  (*info_output) << "ID\tGUARD\t\tROLES\n";
  for (set<Transition *>::iterator t = T.begin(); t != T.end(); t++)
  {
    (*info_output) << "t" + intToString((*t)->id) + "\t";
    
    if ((*t)->guard != "")
      (*info_output) << "{" + (*t)->guard + "} ";
    else
      (*info_output) << "\t";
    
    for (set<string>::iterator role = (*t)->history.begin(); role != (*t)->history.end(); role++)
    {
      if (role == (*t)->history.begin())
	(*info_output) << "\t" + *role + "\n";
      else
	(*info_output) << "\t\t\t" + *role + "\n";
    }
  }
}





/*---------------------------------------------------------------------------*/


/*!
 * Creates a DOT output (see http://www.graphviz.org) of the Petri net. It uses
 * the digraph-statement and adds labels to transitions, places and arcs if
 * neccessary. It also distinguishes the three arc types of #arc_type.
 *
 * \todo
 *       - treatment of input and output places
 *       - treatment of markings
 */
void PetriNet::drawDot()
{
  trace(TRACE_DEBUG, "[PN]\tCreating DOT-output.\n");
  
  (*dot_output) << "digraph N {" << endl;
  (*dot_output) << " graph [fontname=\"Helvetica-Oblique\", label=\"" << filename << "\"];" << endl;
  (*dot_output) << " node [fontname=\"Helvetica-Oblique\"];" << endl;
  (*dot_output) << " edge [fontname=\"Helvetica-Oblique\"];" << endl << endl;
  
  // list the places
  (*dot_output) << " node [shape=circle, fixedsize];" << endl;
  for (set<Place *>::iterator p = P.begin(); p != P.end(); p++)
  {
    (*dot_output) << " " << (*p)->id << "\t[label=\"p" << (*p)->id << "\"";

    // color high-level places
    switch ((*p)->type)
    {
      case (DATA): { (*dot_output) << " style=filled fillcolor=green"; break; }
      default:;
    }
      
    (*dot_output) << "];" << endl;
  }

  // list the transitions
  (*dot_output) << endl << " node [shape=box, fixedsize];" << endl;
  for (set<Transition *>::iterator t = T.begin(); t != T.end(); t++)
  {
    if ((*t)->guard != "")
    {
      (*dot_output) << " " << (*t)->id << "\t[";
      (*dot_output) << " shape=record label=\"{t" << (*t)->id << "|{"
	<< (*t)->guard << "}}\" style=filled fillcolor=green];" << endl;
    }
    else
      (*dot_output) << " " << (*t)->id << "\t[label=\"t" << (*t)->id << "\"];" << endl;
  }

  // list the arcs
  for (set<Arc *>::iterator f = F.begin(); f != F.end(); f++)
  {
    (*dot_output) << " " << (*f)->source->id << " -> "
      << (*f)->target->id << "\t[";
    
    if ((*f)->type == RESET)
      (*dot_output) << "arrowtail=odot";
    else if ((*f)->type == READ)
      (*dot_output) << "arrowhead=diamond arrowtail=tee";

    if ((*f)->inscription != "")
      (*dot_output) << " label=\"" << (*f)->inscription << "\"";

    (*dot_output) << "];" << endl;
  }

  (*dot_output) << "}" << endl;
}





/*!
 * Outputs the net in LoLA-format.
 *
 * \todo
 *       - add support for reset-arcs
 *       - add support for high-level constructs
 *       - add markings
 */
void PetriNet::lolaOut()
{
  trace(TRACE_DEBUG, "[PN]\tCreating LoLA-output.\n");

  // we can only create low-level nets for LoLA right now
  if (!lowLevel)
    makeLowLevel();

  (*lola_output) << "{ Petri net created by BPEL2oWFN reading file '" << filename << "' }" << endl << endl;
  
  // places
  (*lola_output) << "PLACE" << endl;
  unsigned int count = 1;
  for (set<Place *>::iterator p = P.begin(); p != P.end(); count++,p++)
  {
    (*lola_output) << "  p" << (*p)->id;
    if (count < P.size())
      (*lola_output) << ",";
    else
      (*lola_output) << ";";
    (*lola_output) << "\t\t { " << (*(*p)->history.begin()) << " }" << endl;
  }
  (*lola_output) << endl << endl;
  
  (*lola_output) << "MARKING" << endl << ";" << endl << endl;

  // transitions
  count = 1;
  for (set<Transition *>::iterator t = T.begin(); t != T.end(); count++,t++)
  {
    (*lola_output) << "TRANSITION t" << (*t)->id << "\t { " << (*(*t)->history.begin()) << " }" << endl;
    set<Node *> consume = preset(*t);
    set<Node *> produce = postset(*t);

    (*lola_output) << "CONSUME" << endl;
    unsigned int count2 = 1;
    for (set<Node *>::iterator pre = consume.begin(); pre != consume.end(); count2++, pre++)
    {
      (*lola_output) << "  p" << (*pre)->id << ": 1";
      if (count2 < consume.size())
	(*lola_output) << ",";
      else
	(*lola_output) << ";";
      (*lola_output) << "\t { " << (*(*pre)->history.begin()) << " }" << endl;
    }

    (*lola_output) << "PRODUCE" << endl;
    count2 = 1;
    for (set<Node *>::iterator post = produce.begin(); post != produce.end(); count2++, post++)
    {
      (*lola_output) << "  p" << (*post)->id << ": 1";
      if (count2 < produce.size())
	(*lola_output) << ",";
      else
	(*lola_output) << ";";
      (*lola_output) << "\t { " << (*(*post)->history.begin()) << " }" << endl;
    }
    (*lola_output) << endl;
  }
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
 */
void PetriNet::mergeTransitions(Transition *t1, Transition *t2)
{
  trace(TRACE_VERY_DEBUG, "[PN]\tMerging transitions " + intToString(t1->id) +
      " and " + intToString(t2->id) + "...\n");

  if (t1->guard != "" || t2->guard != "")
    throw Exception(MERGING_ERROR, "Merging of guarded transition not yet supported!\n", typeid(this).name());
	
  Node *t12 = newTransition();
  
  for (set<string>::iterator role = t1->history.begin(); role != t1->history.end(); role++)
  {
    roleMap[*role] = t12;
    t12->history.insert(*role);
  }
  
  for (set<string>::iterator role = t2->history.begin(); role != t2->history.end(); role++)
  {
    roleMap[*role] = t12;
    t12->history.insert(*role);
  }
  
  set<Node *> pre12 = setUnion(preset(t1), preset(t2));
  set<Node *> post12 = setUnion(postset(t1), postset(t2));
  
  for (set<Node *>::iterator n = pre12.begin(); n != pre12.end(); n++)
    newArc(*n, t12);

  for (set<Node *>::iterator n = post12.begin(); n != post12.end(); n++)
    newArc(t12, *n);
  
  removeTransition(t1);
  removeTransition(t2);

  trace(TRACE_VERY_DEBUG, "[PN]\tMerging done.\n");
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
 */
void PetriNet::mergePlaces(Place *p1, Place *p2)
{
  trace(TRACE_VERY_DEBUG, "[PN]\tMerging places " + intToString(p1->id) +
      " and " + intToString(p2->id) + "...\n");
  
  if (p1 == p2)
    return;
    //throw Exception(MERGING_ERROR, "Merging of same places undefined!\n", typeid(this).name());

  if(p1->type != LOW || p2->type != LOW)
    throw Exception(MERGING_ERROR, "Merging of high-level places not yet supported!\n", typeid(this).name());
  
  Node *p12 = newPlace();
  
  for (set<string>::iterator role = p1->history.begin(); role != p1->history.end(); role++)
  {
    p12->history.insert(*role);
    roleMap[*role] = p12;
  }
  
  for (set<string>::iterator role = p2->history.begin(); role != p2->history.end(); role++)
  {
    p12->history.insert(*role);
    roleMap[*role] = p12;
  }

  set<Node *> pre12 = setUnion(preset(p1), preset(p2));
  set<Node *> post12 = setUnion(postset(p1), postset(p2));
  
  for (set<Node *>::iterator n = pre12.begin(); n != pre12.end(); n++)
    newArc(*n, p12);

  for (set<Node *>::iterator n = post12.begin(); n != post12.end(); n++)
    newArc(p12, *n);
  
  removePlace(p1);
  removePlace(p2);
  trace(TRACE_VERY_DEBUG, "[PN]\tMerging done.\n");
}





/*!
 * \param role1 string describing the role of the first place
 * \param role2 string describing the role of the second place
 */
void PetriNet::mergePlaces(string role1, string role2)
{
  mergePlaces(findPlace(role1), findPlace(role2));
}





/*!
 * Merges two places given two activity of the abstract syntax tree and the
 * roles of the places. The activities are used to complete the role-string
 * and pass the search request.
 * 
 * \param act1  activity of the AST represented by the first place
 * \param role1 string describing the role of the first place (beginning with a
 *              period: .empty
 * \param act2  activity of the AST represented by the second place (beginning
 *              with a period: .empty
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

  for (set<Arc *>::iterator f = F.begin(); f != F.end(); f++)
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

  for (set<Arc *>::iterator f = F.begin(); f != F.end(); f++)
    if ((*f)->source == n)
      result.insert((*f)->target);

  return result;
}





/*---------------------------------------------------------------------------*/


/*!
 * Finds a place of the Petri net given a role the place fills or filled.
 *
 * \param  role the demanded role
 * \return a pointer to the place or throw an exception if the place was not
 *         found.
 */
Place *PetriNet::findPlace(string role)
{
  Place *result = (Place *)roleMap[role];

  if (result == NULL)
    throw Exception(NODE_NOT_FOUND, "Node with role '" + role + "' not found!\n", typeid(this).name());
  else
    return result;

  /*
  for (set<Place *>::iterator p = P.begin(); p != P.end(); p++)
    for (set<string>::iterator r = (*p)->history.begin(); r != (*p)->history.end(); r++)
      if ((*r).substr((*r).find_first_of(".")+1) == role)
	return *p;

  throw Exception(NODE_NOT_FOUND, "Node with role '" + role + "' not found!\n", typeid(this).name());
  */
}





/*---------------------------------------------------------------------------*/


/*!
 * Implements some simple structural reduction rules for Petri nets:
 * 
 * - If two transitions t1 and t2 have the same preset and postset, one of them
 *   can safely be removed.
 * - If a transition has a singleton preset and postset, the transition can be
 *   removed (sequence) and the preset and postset can be merged.
 *
 * \todo
 *       - improve performance
 *       - implement more reduction rules
 *
 * \bug repeated excecution fails
 *
 */
void PetriNet::simplify()
{
  trace(TRACE_DEBUG, "[PN]\tPetri net size before simplification: " +
      information() + "\n");
  trace(TRACE_INFORMATION, "Simplifying Petri net...\n");

  
  // a pair to store transitions to be merged
  vector <pair <Transition *, Transition *> > transitionPairs;

  // find transitions with same preset and postset  
  for (set<Transition *>::iterator t1 = T.begin(); t1 != T.end(); t1++)
    for (set<Transition *>::iterator t2 = t1; t2 != T.end(); t2++)
      if (*t1 != *t2)
	if ( (preset(*t1) == preset(*t2)) && (postset(*t1) == postset(*t2)) )
	  transitionPairs.push_back(pair<Transition *, Transition *>(*t1, *t2));

  // merge the found transitions
  for (unsigned int i = 0; i < transitionPairs.size(); i++)
    mergeTransitions(transitionPairs[i].first, transitionPairs[i].second);

  
  trace(TRACE_DEBUG, "[PN]\tnew reduction rule\n");

  // a pair to store places to be merged
  vector <Transition *> sequenceTransition;
  vector <pair <string, string> > placeMerge;
  
  // find transitions with singelton preset and postset
  for (set<Transition *>::iterator t = T.begin(); t != T.end(); t++)
    if (preset(*t).size() == 1 && postset(*t).size() == 1)
    {
      string id1 = *((*(preset(*t).begin()))->history.begin());
      string id2 = *((*(postset(*t).begin()))->history.begin());
      placeMerge.push_back(pair<string, string>(id1, id2));
      sequenceTransition.push_back(*t);
    }
    
  // merge preset and postset
  for (unsigned int i = 0; i < placeMerge.size(); i++)
    mergePlaces(placeMerge[i].first, placeMerge[i].second);

  // remove "sequence"-transtions
  for (unsigned int i = 0; i < sequenceTransition.size(); i++)
    removeTransition(sequenceTransition[i]);


  trace(TRACE_INFORMATION, "Simplifying complete.\n");
  trace(TRACE_DEBUG, "[PN]\tPetri net size after simplification: " +
      information() + "\n");
}





/*!
 * Converts the created Petri net to low-level representation. Therefore the
 * following steps are taken:
 *
 * - Convert all places to low-level places.
 * - Remove all transition guards -- decisions are now taken
 *   non-deterministically.
 * - Convert places of type #DATA, #TIME and #PROPERTY to #LOW. (todo)
 * - Convert places of type #MESSAGE to #IN resp. #OUT. (todo)
 * - Remove arc inscriptions -- all places are low-level places anyway.
 * - Convert read arcs to loops.
 *
 * Only reset arcs remain as high-level constructs. They will be converted
 * (i.e. unfolded) in a later stage since this unfolding is target-format
 * specific.
 * 
 * \todo
 *       - take care of markings
 *       - take care of places (#TIME, #PROPERTY, #IN, #OUT)
 */
void PetriNet::makeLowLevel()
{
  trace(TRACE_INFORMATION, "Converting Petri net to low-level...\n");

  // convert places to low-level places
  for (set<Place *>::iterator p = P.begin(); p != P.end(); p++)
    (*p)->type = LOW;

  
  // remove transition guards
  for (set<Transition *>::iterator t = T.begin(); t != T.end(); t++)
    (*t)->guard = "";

  
  // convert read arcs, remove inscriptions
  set<Arc *> readArcs;

  for (set<Arc *>::iterator f = F.begin(); f != F.end(); f++)
  {
    (*f)->inscription = "";
    if ((*f)->type == READ)
    {
      (*f)->type = STANDARD;
      readArcs.insert(*f);
    }
  }

  for (set<Arc *>::iterator f = readArcs.begin(); f != readArcs.end(); f++)
    newArc( (*f)->target, (*f)->source );

  lowLevel = true;
}





/*---------------------------------------------------------------------------*/


/*!
 * \return id for new nodes
 */
unsigned int PetriNet::getId()
{
  return nextId++;
}






/*!
 * \return id for last added node
 */
unsigned int PetriNet::id()
{
  return nextId;
}
