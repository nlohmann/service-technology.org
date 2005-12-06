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
 *          - created: 2005-10-18
 *          - last changed: \$Date: 2005/12/06 12:32:15 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \version \$Revision: 1.58 $
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
 *            #PetriNet::printInformation() and #PetriNet::lolaOut(). Use
 *            #Exception-class to signal errors.
 *          - 2005-11-16 (nlohmann) Added a mapping for faster access to Nodes
 *            given a role. Pimped DOT-output. Added several overloadings for
 *            easier pattern definition.
 *          - 2005-11-18 (nlohmann) Added a simple test to avoid arcs between
 *            two places resp. two transitions.
 *          - 2005-11-20 (nlohmann) Added support for high-level arcs for
 *            merging.
 *          - 2005-11-29 (nlohmann) Roles are now organized in a vector.
 *
 */





#include "petrinet.h"





extern int debug_level;              // defined in debug.cc
extern ostream *dot_output;          // defined in main.c
extern ostream *lola_output;         // defined in main.c
extern ostream *info_output;         // defined in main.c
extern string filename;              // defined in main.c
extern bool mode_simplify_petri_net; // defined in main.c





/*!
 * \todo
 *       - (nlohmann) comment me!
 *       - (nlohmann) put me into helpers.cc
 */
set<pair<Node *, arc_type> > setUnion(set<pair<Node *, arc_type> > a, set<pair<Node *, arc_type> > b)
{
  set<pair<Node *, arc_type> > result;
  insert_iterator<set< pair<Node *, arc_type>, less< pair<Node *, arc_type> > > > res_ins(result, result.begin());
  set_union(a.begin(), a.end(), b.begin(), b.end(), res_ins);
  
  return result;
}



/*****************************************************************************/


/*!
 * \param role a role of a node
 * \return true, if the node's first history entry begins with the given role
 */
bool Node::firstMemberOf(string role)
{
  string firstEntry = (*history.begin());
  return (firstEntry.find(role, 0) == firstEntry.find_first_of(".")+1);
}



/*!
 * \todo (nlohmann) comment me
 */
bool Node::firstMemberIs(string role)
{
  string firstEntry = (*history.begin());
  return (firstEntry.find(role, 0) == 0);
}




/*
 * \return the name of the node type
 */
string Node::nodeTypeName()
{
  switch(nodeType)
  {
    case(PLACE):
      return "place";
    case(TRANSITION):
      return "transition";
    default:
      return "uninitialized node";
  }
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
 * \param myid    the id of the transition
 * \param role    the first role of the transition
 * \param myguard the guard of the transition
 */
Transition::Transition(unsigned int myid, string role, string myguard)
{
  guard = myguard;
  id = myid;
  nodeType = TRANSITION;
  
  if (role != "")
    history.push_back(role);
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
  nodeType = PLACE;

  if (role != "")
    history.push_back(role);
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
  trace(TRACE_VERY_DEBUG, "[PN]\tCreating place p" + intToString(id()) +
      " (" + role + ") ...\n");
  
  Place *p = new Place(getId(), role, mytype);
  P.insert(p);

  if (role != "")
    roleMap[role] = p;

  if (P.size() % 100 == 0)
    trace(TRACE_INFORMATION, "[PN]\t" + information() + "\n");
  
  return p;
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
  trace(TRACE_VERY_DEBUG, "[PN]\tCreating transition t" + intToString(id()) +
      " (" + role + ") ...\n");
  
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
 * Creates an inscripted standard arc.
 * \param source      source node of the arc
 * \param target      target node of the arc
 * \param inscription inscription of the arc
 * \return pointer of the created arc
 */
Arc *PetriNet::newArc(Node *source, Node *target, kc::casestring inscription)
{
  return newArc(source, target, STANDARD, inscription->name);
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
 *
 * \todo
 *       - (nlohmann) add test to check whether one of the pointers is NULL
 */
Arc *PetriNet::newArc(Node *source, Node *target, arc_type type, string inscription)
{
  trace(TRACE_VERY_DEBUG, "[PN]\tCreating arc (" + intToString(source->id) +
      "," + intToString(target->id) + ")...\n");

  if (source->nodeType == target->nodeType)
    throw Exception(ARC_ERROR, "Arc between two " + source->nodeTypeName() + "s!\n",
	*((source->history).begin()) + " and " + *((target->history).begin()));
  
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
 */
void PetriNet::printInformation()
{
  // the places
  (*info_output) << "PLACES:\nID\tTYPE\t\tROLES\n";
  for (set<Place *>::iterator p = P.begin(); p != P.end(); p++)
  {
    (*info_output) << "p" << ((*p)->id) << "\t";

    switch ((*p)->type)
    {
      case(LOW):      { (*info_output) << "low-level"; break; }
      case(DATA):     { (*info_output) << "data     "; break; }
      default:        { (*info_output) << "other    "; }
    }

    for (vector<string>::iterator role = (*p)->history.begin(); role != (*p)->history.end(); role++)
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
    
    for (vector<string>::iterator role = (*t)->history.begin(); role != (*t)->history.end(); role++)
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
 *       - (nlohmann) treatment of input and output places
 *       - (nlohmann) escape labels
 */
void PetriNet::drawDot()
{
  trace(TRACE_DEBUG, "[PN]\tCreating DOT-output.\n");
  
  (*dot_output) << "digraph N {" << endl;
  (*dot_output) << " graph [fontname=\"Helvetica-Oblique\", label=\"";
  
  if (mode_simplify_petri_net)
    (*dot_output) << "structural simplified ";
  if (lowLevel)
    (*dot_output) << "low-level ";
  
  (*dot_output) << "Petri net generated from " << filename << "\"];" << endl;
  (*dot_output) << " node [fontname=\"Helvetica-Oblique\" fontsize=10];" << endl;
  (*dot_output) << " edge [fontname=\"Helvetica-Oblique\" fontsize=10];" << endl << endl;

  
  // list the places
  (*dot_output) << " node [shape=circle, fixedsize];" << endl;
  for (set<Place *>::iterator p = P.begin(); p != P.end(); p++)
  {
    (*dot_output) << " " << (*p)->id << "\t[label=\"p" << (*p)->id << "\"";

    if ((*p)->firstMemberOf("eventHandler."))
      (*dot_output) << " style=filled fillcolor=plum";
    else if ((*p)->firstMemberOf("compensationHandler."))
      (*dot_output) << " style=filled fillcolor=aquamarine";
    else if ((*p)->firstMemberOf("stop."))
      (*dot_output) << " style=filled fillcolor=lightskyblue2";
    else if ((*p)->firstMemberOf("faulthandler."))
      (*dot_output) << " style=filled fillcolor=pink";
    else if ((*p)->firstMemberIs("link.") || (*p)->firstMemberIs("!link."))
      (*dot_output) << " style=filled fillcolor=yellow";
    else if ((*p)->firstMemberOf("Active") || (*p)->firstMemberOf("!Active"))
      (*dot_output) << " style=filled fillcolor=yellowgreen ";
    else if ((*p)->firstMemberOf("Completed") || (*p)->firstMemberOf("!Completed"))
      (*dot_output) << " style=filled fillcolor=yellowgreen ";
    else if ((*p)->firstMemberOf("Compensated") || (*p)->firstMemberOf("!Compensated"))
      (*dot_output) << " style=filled fillcolor=yellowgreen ";
    else if ((*p)->firstMemberOf("Ended") || (*p)->firstMemberOf("!Ended"))
      (*dot_output) << " style=filled fillcolor=yellowgreen ";
    else if ((*p)->firstMemberOf("Faulted") || (*p)->firstMemberOf("!Faulted"))
      (*dot_output) << " style=filled fillcolor=yellowgreen ";
    else if ((*p)->firstMemberOf("Terminated") || (*p)->firstMemberOf("!Terminated"))
      (*dot_output) << " style=filled fillcolor=yellowgreen ";
    else if ((*p)->firstMemberIs("1.initial") || (*p)->firstMemberIs("1.final"))
      (*dot_output) << " style=filled fillcolor=green";

    (*dot_output) << "];" << endl;
  }


  // list the transitions
  (*dot_output) << endl << " node [shape=box, fixedsize];" << endl;
  for (set<Transition *>::iterator t = T.begin(); t != T.end(); t++)
  {
    (*dot_output) << " " << (*t)->id << "\t[label=\"t" << (*t)->id << "\"";
    
    if ((*t)->guard != "")
      (*dot_output) << " shape=record label=\"{t" << (*t)->id << "|{" << (*t)->guard << "}}\"";

    if ((*t)->firstMemberOf("eventHandler."))
      (*dot_output) << " style=filled fillcolor=plum";
    else if ((*t)->firstMemberOf("compensationHandler."))
      (*dot_output) << " style=filled fillcolor=aquamarine";
    else if ((*t)->firstMemberOf("stop."))
      (*dot_output) << " style=filled fillcolor=lightskyblue2";
    else if ((*t)->firstMemberOf("faulthandler."))
      (*dot_output) << " style=filled fillcolor=pink";
    
    (*dot_output) << "];" << endl;
  }
  (*dot_output) << endl;

  
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
 */
void PetriNet::lolaOut()
{
  trace(TRACE_DEBUG, "[PN]\tCreating LoLA-output.\n");

  // we can only create low-level nets for LoLA right now
  if (!lowLevel)
    makeLowLevel();

  (*lola_output) << "{ Petri net created by BPEL2oWFN reading " << filename << " }" << endl << endl;

  /*
  // the reset-arc
  unsigned maxErrors = 3;
  Node *fault_place = findPlace("1.fault_in");
  unsigned int fault_id = fault_place->id;

  set<unsigned int> fault_t_in;
  set<pair<Node *, arc_type> > fault_preset = preset(fault_place);
  for (set<pair<Node *, arc_type> >::iterator it = fault_preset.begin(); it != fault_preset.end(); it++)
  {
    fault_t_in.insert( (*it).first->id );
    cerr << "in:  " << (*it).first->id << endl;
  }
  
  set<unsigned int> fault_t_out;  
  set<pair<Node *, arc_type> > fault_postset = postset(fault_place);
  for (set<pair<Node *, arc_type> >::iterator it = fault_postset.begin(); it != fault_postset.end(); it++)
  {
    fault_t_out.insert( (*it).first->id );
    cerr << "out: " << (*it).first->id << endl;
  }


  // the neccessary functions of the reset-arc
  (*lola_output) << "SORT" << endl;
  (*lola_output) << "  1.maxErrors = [ 0 , " << maxErrors << " ] ;" << endl << endl;
  (*lola_output) << "FUNCTION inc_p" << fault_id << "(x:1.maxErrors):1.maxErrors" << endl;
  (*lola_output) << "BEGIN" << endl << "  RETURN x + 1" << endl << "END" << endl << endl;

  (*lola_output) << "FUNCTION dec_p" << fault_id << "(x:1.maxErrors):1.maxErrors" << endl;
  (*lola_output) << "BEGIN" << endl << "  RETURN x - 1" << endl << "END" << endl << endl;

  (*lola_output) << "FUNCTION reset_p" << fault_id << "(x:1.maxErrors):1.maxErrors" << endl;
  (*lola_output) << "BEGIN" << endl << "  RETURN 0" << endl << "END" << endl << endl;

  (*lola_output) << "FUNCTION zero_p" << fault_id << "():1.maxErrors" << endl;
  (*lola_output) << "BEGIN" << endl << "  RETURN 0" << endl << "END" << endl << endl;
  */
  
  // places
  (*lola_output) << "PLACE" << endl;
  unsigned int count = 1;
  for (set<Place *>::iterator p = P.begin(); p != P.end(); count++,p++)
  {
    (*lola_output) << "  p" << (*p)->id;
    
    /*
    // the error-place
    if ((*p)->id == fault_id)
      (*lola_output) << " : 1.maxErrors";

    */
      
    if (count < P.size())
      (*lola_output) << ",";
    else
      (*lola_output) << ";";
    (*lola_output) << "\t\t { " << (*(*p)->history.begin()) << " }" << endl;
  }
  (*lola_output) << endl << endl;
  
  
  // initial marking
  (*lola_output) << "MARKING" << endl;
  (*lola_output) << "  p" << findPlace("1.initial")->id << ":\t1,\t { initial marking of the process }" << endl;
  (*lola_output) << "  p" << findPlace("1.clock")->id << ":\t1\t { initial marking of the clock }" << endl;
  //(*lola_output) << "  p" << fault_id << ":\tzero_p" << fault_id << "()\t { initial marking of the fault place }" << endl;
  (*lola_output)   << ";" << endl << endl << endl;


  // transitions
  count = 1;
  for (set<Transition *>::iterator t = T.begin(); t != T.end(); count++,t++)
  {
    (*lola_output) << "TRANSITION t" << (*t)->id << "\t { " << (*(*t)->history.begin()) << " }" << endl;
    set<pair<Node *, arc_type> > consume = preset(*t);
    set<pair<Node *, arc_type> > produce = postset(*t);

    /*
    if ( fault_t_in.find( (*t)->id ) != fault_t_in.end() || fault_t_out.find( (*t)->id ) != fault_t_out.end() )
    {
      (*lola_output) << "VAR x : 1.maxErrors;" << endl;
      (*lola_output) << "GUARD x > 0" << endl;
    }
    */
    

    (*lola_output) << "CONSUME" << endl;
    if (consume.empty())
      (*lola_output) << ";" << endl;
    
    unsigned int count2 = 1;
    for (set<pair<Node *, arc_type> >::iterator pre = consume.begin(); pre != consume.end(); count2++, pre++)
    {
      //if( (*pre).first->id != fault_id )
      (*lola_output) << "  p" << (*pre).first->id << ":\t1";
      //else
	//(*lola_output) << "  p" << (*pre).first->id << ":\tx";

      if (count2 < consume.size())
	(*lola_output) << ",";
      else
	(*lola_output) << ";";
      (*lola_output) << "\t { " << (*(*pre).first->history.begin()) << " }" << endl;
    }

    
    (*lola_output) << "PRODUCE" << endl;
    if (produce.empty())
      (*lola_output) << ";" << endl;
    
    /*
    // decrement the fault place (since it was in the preset)
    if (fault_t_out.find( (*t)->id ) != fault_t_out.end() )
      (*lola_output) << "  p" << fault_id << ":\tdec_p" << fault_id << "(x)," << endl;
    */
      
    count2 = 1;
    for (set<pair<Node *, arc_type> >::iterator post = produce.begin(); post != produce.end(); count2++, post++)
    {
      //if( (*post).first->id != fault_id )
      (*lola_output) << "  p" << (*post).first->id << ":\t1";
      //else
	//(*lola_output) << "  p" << (*post).first->id << ":\tinc_p" << fault_id << "(x)";

      if (count2 < produce.size())
	(*lola_output) << ",";
      else
	(*lola_output) << ";";
      (*lola_output) << "\t { " << (*(*post).first->history.begin()) << " }" << endl;
    }
    (*lola_output) << endl;
    (*lola_output) << "{ END OF FILE }" << endl;
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
 */
void PetriNet::mergeTransitions(Transition *t1, Transition *t2)
{
  trace(TRACE_VERY_DEBUG, "[PN]\tMerging transitions " + intToString(t1->id) +
      " and " + intToString(t2->id) + "...\n");

  if (t1->guard != "" || t2->guard != "")
    throw Exception(MERGING_ERROR, "Merging of guarded transition not yet supported!\n", typeid(this).name());
	
  Node *t12 = newTransition();
  
  for (vector<string>::iterator role = t1->history.begin(); role != t1->history.end(); role++)
  {
    roleMap[*role] = t12;
    t12->history.push_back(*role);
  }
  
  for (vector<string>::iterator role = t2->history.begin(); role != t2->history.end(); role++)
  {
    roleMap[*role] = t12;
    t12->history.push_back(*role);
  }
  
  set<pair<Node *, arc_type> > pre12 = setUnion(preset(t1), preset(t2));
  set<pair<Node *, arc_type> > post12 = setUnion(postset(t1), postset(t2));
  
  for (set<pair<Node *, arc_type> >::iterator n = pre12.begin(); n != pre12.end(); n++)
    newArc((*n).first, t12, (*n).second);

  for (set<pair<Node *, arc_type> >::iterator n = post12.begin(); n != post12.end(); n++)
    newArc(t12, (*n).first, (*n).second);
  
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
 *       - (nlohmann) Take care of arc inscriptions.
 */
void PetriNet::mergePlaces(Place *p1, Place *p2)
{  
  if (p1 == p2)
    return;

  if (p1 == NULL || p2 == NULL)
    return;

  if(p1->type != LOW || p2->type != LOW)
    throw Exception(MERGING_ERROR, "Merging of high-level places not yet supported!\n",
	"place p" + intToString(p1->id) + " (type " + intToString(p2->type) + ") and p" + intToString(p2->id) + " (type " + intToString(p2->type) + ")");
 
  trace(TRACE_VERY_DEBUG, "[PN]\tMerging places " + intToString(p1->id) +
      " and " + intToString(p2->id) + "...\n");
  
  Node *p12 = newPlace();
  
  for (vector<string>::iterator role = p1->history.begin(); role != p1->history.end(); role++)
  {
    p12->history.push_back(*role);
    roleMap[*role] = p12;
  }
  
  for (vector<string>::iterator role = p2->history.begin(); role != p2->history.end(); role++)
  {
    p12->history.push_back(*role);
    roleMap[*role] = p12;
  }

  set<pair<Node *, arc_type> > pre12 = setUnion(preset(p1), preset(p2));
  set<pair<Node *, arc_type> > post12 = setUnion(postset(p1), postset(p2));
  
  for (set<pair<Node *, arc_type> >::iterator n = pre12.begin(); n != pre12.end(); n++)
    newArc((*n).first, p12, (*n).second);

  for (set<pair<Node *, arc_type> >::iterator n = post12.begin(); n != post12.end(); n++)
    newArc(p12, (*n).first, (*n).second);
  
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
set<pair<Node *, arc_type> > PetriNet::preset(Node *n)
{
  set<pair<Node *, arc_type> > result;

  for (set<Arc *>::iterator f = F.begin(); f != F.end(); f++)
    if ((*f)->target == n)
    {
      pair<Node *, arc_type> element = pair<Node *, arc_type>((*f)->source, (*f)->type);
      result.insert(element);
    }

  return result;
}





/*!
 * \param n a node of the Petri net
 * \result the postset of node n
 */
set<pair<Node *, arc_type> > PetriNet::postset(Node *n)
{
  set<pair<Node *, arc_type> > result;

  for (set<Arc *>::iterator f = F.begin(); f != F.end(); f++)
    if ((*f)->source == n)
    {
      pair<Node *, arc_type> element = pair<Node *, arc_type>((*f)->target, (*f)->type);
      result.insert(element);
    }

  return result;
}





/*---------------------------------------------------------------------------*/


/*!
 * Finds a place of the Petri net given a role the place fills or filled.
 *
 * \param  role the demanded role
 * \return a pointer to the place or a NULL pointer if the place was not found.
 */
Place *PetriNet::findPlace(string role)
{
  Place *result = (Place *)roleMap[role];

  if (result == NULL)
    trace(TRACE_DEBUG, "[PN]\tPlace with role \"" + role + "\" not found.\n");

  return result;
}





/*
 * Finds a place of the Petri net given an activity and a role.
 *
 * \param  activity an activity of which the id is taken
 * \param  role the demanded role
 * \return a pointer to the place or a NULL pointer if the place was not found.
 */
Place *PetriNet::findPlace(kc::impl_activity* activity, string role)
{
  return findPlace(intToString(activity->id->value) + role);
}





/*!
 * Finds a transition of the Petri net given a role the place fills or filled.
 *
 * \param  role the demanded role
 * \return a pointer to the transition or a NULL pointer if the place was not
 *         found.
 */
Transition *PetriNet::findTransition(string role)
{
  Transition *result = (Transition *)roleMap[role];

  if (result == NULL)
    trace(TRACE_DEBUG, "[PN]\tTransition with role \"" + role + "\" not found.\n");

  return result;
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
 *       - (nlohmann) improve performance
 *       - (nlohmann) implement more reduction rules
 *
 * \bug
 *      - repeated excecution fails
 *      - reduction chrashes sometimes during merging of places
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
      string id1 = *((*(preset(*t).begin())).first->history.begin());
      string id2 = *((*(postset(*t).begin())).first->history.begin());
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
 * - Convert all places to type #LOW.
 * - Remove arc inscriptions -- all places are low-level places anyway.
 * - Convert read arcs to loops.
 *
 * Only reset arcs remain as high-level constructs. They will be converted
 * (i.e. unfolded) in a later stage since this unfolding is target-format
 * specific.
 * 
 * \todo
 *       - (nlohmann) take care of interface places (#IN, #OUT)
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
