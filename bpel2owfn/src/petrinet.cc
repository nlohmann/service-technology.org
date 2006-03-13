/*****************************************************************************\
 * Copyright 2005, 2006 Niels Lohmann, Christian Gierds, Dennis Reinert      *
 *                                                                           *
 * This file is part of BPEL2oWFN.                                           *
 *                                                                           *
 * BPEL2oWFN is free software; you can redistribute it and/or modify it      *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or(at your    *
 * option) any later version.                                                *
 *                                                                           *
 * BPEL2oWFN is distributed in the hope that it will be useful, but WITHOUT  *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
 * more details.                                                             *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with BPEL2oWFN; if not, write to the Free Software Foundation, Inc., 51   *
 * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.                      *
\*****************************************************************************/

/*!
 * \file petrinet.cc
 *
 * \brief Functions for Petri nets(implementation)
 *
 * This file implements the classes and functions defined in petrinet.h.
 *
 * \author
 *          - responsible: Niels Lohmann <nlohmann@informatik.hu-berlin.de>
 *          - last changes of: \$Author: nlohmann $
 *
 * \date
 *          - created: 2005-10-18
 *          - last changed: \$Date: 2006/03/13 16:05:09 $
 *
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.110 $
 */





#include "petrinet.h"





/******************************************************************************
 * External variables
 *****************************************************************************/

extern SymbolManager symMan;	// defined in bpel-syntax.yy





/******************************************************************************
 * Implementation of class functions
 *****************************************************************************/

/*!
 * \param role a role of a node
 * \return true, if the node's first history entry contains the given role
 */
bool Node::firstMemberAs(string role)
{
  string firstEntry =(*history.begin());
  return (firstEntry.find(role, 0) == firstEntry.find_first_of(".") + 1);
}





/*!
 * \param role a role of a node
 * \return true, if the node's first history entry begins with the given role
 */
bool Node::firstMemberIs(string role)
{
  string firstEntry =(*history.begin());
  return (firstEntry.find(role, 0) == 0);
}





/*!
 * \return the name of the node type
 */
string Node::nodeTypeName()
{
  switch(nodeType)
  {
    case (PLACE):
      return "place";
    case (TRANSITION):
      return "transition";
    default:
      return "uninitialized node";	// should never happen
  }
}





/*!
 * \return the short name of the node, e.g. for LoLA output
 */
string Node::nodeShortName()
{
  if (nodeType == PLACE)
    return ("p" + intToString(id));
  if (nodeType == TRANSITION)
    return ("t" + intToString(id));
  else
    return NULL;
}




/*****************************************************************************/


/*!
 * \param mysource      the source-node of the arc
 * \param mytarget      the target-node of the arc
 */
Arc::Arc(Node * mysource, Node * mytarget)
{
  source = mysource;
  target = mytarget;
}





/*!
 * DOT-output of the arc.
*/
string Arc::dotOut()
{
  return " " + intToString(source->id) + " -> " + intToString(target->id) + ";\n";
}





/*****************************************************************************/


/*!
 * \param myid    the id of the transition
 * \param role    the first role of the transition
 */
Transition::Transition(unsigned int myid, string role)
{
  id = myid;
  nodeType = TRANSITION;

  if (role != "")
    history.push_back(role);
}





/*!
 * DOT-output of the transition. Transitions are colored corresponding to their
 * initial role.
*/
string Transition::dotOut()
{
  string result;
  result += " " + intToString(id) + "\t[label=\"" + nodeShortName() + "\"";

  if (firstMemberAs("internal.eventHandler."))
    result += " style=filled fillcolor=plum";
  else if (firstMemberAs("internal.compensationHandler."))
    result += " style=filled fillcolor=aquamarine";
  else if (firstMemberAs("internal.stop."))
    result += " style=filled fillcolor=lightskyblue2";
  else if (firstMemberAs("internal.faultHandler."))
    result += " style=filled fillcolor=pink";

  result += "];\n";
  return result;
}





/*****************************************************************************/


/*!
 * \param myid   the internal id of the place
 * \param role   the first role of the place
 * \param mytype the type of the place(as defined in #place_type)
 */
Place::Place(unsigned int myid, string role, place_type mytype)
{
  type = mytype;
  id = myid;
  nodeType = PLACE;
  marked = false;

  if (role != "")
    history.push_back(role);
}





/*
 * Initially mark the place.
 */
void Place::mark()
{
  marked = true;
}





/*!
 * DOT-output of the place. Places are colored corresponding to their initial
 * role.
*/
string Place::dotOut()
{
  string
    result;
  result += " " + intToString(id) + "\t[label=\"" + nodeShortName() + "\"";

  if (firstMemberAs("eventHandler."))
    result += " style=filled fillcolor=plum";
  else if (firstMemberAs("internal.compensationHandler."))
    result += " style=filled fillcolor=aquamarine";
  else if (firstMemberAs("internal.stop."))
    result += " style=filled fillcolor=lightskyblue2";
  else if (firstMemberAs("internal.faultHandler."))
    result += " style=filled fillcolor=pink";
  else if (firstMemberIs("link.") || firstMemberIs("!link."))
    result += " style=filled fillcolor=yellow";
  else if (firstMemberIs("variable."))
    result += " style=filled fillcolor=cyan shape=ellipse";
  else if (firstMemberIs("in.") || firstMemberIs("out."))
    result += " style=filled fillcolor=gold shape=ellipse";
  else if (firstMemberAs("internal.Active")
	   || firstMemberAs("internal.!Active"))
    result += " style=filled fillcolor=yellowgreen";
  else if (firstMemberAs("internal.Completed")
	   || firstMemberAs("internal.!Completed"))
    result += " style=filled fillcolor=yellowgreen ";
  else if (firstMemberAs("internal.Compensated")
	   || firstMemberAs("internal.!Compensated"))
    result += " style=filled fillcolor=yellowgreen";
  else if (firstMemberAs("internal.Ended")
	   || firstMemberAs("internal.!Ended"))
    result += " style=filled fillcolor=yellowgreen";
  else if (firstMemberAs("internal.Faulted")
	   || firstMemberAs("internal.!Faulted"))
    result += " style=filled fillcolor=yellowgreen";
  else if (firstMemberAs("internal.Terminated")
	   || firstMemberAs("internal.!Terminated"))
    result += " style=filled fillcolor=yellowgreen";
  else if (firstMemberIs("1.internal.initial")
	   || firstMemberIs("1.internal.final"))
    result += " style=filled fillcolor=green";
  else if (firstMemberIs("1.internal.clock"))
    result += " style=filled fillcolor=seagreen";

  result += "];\n";
  return result;
}





/*****************************************************************************/


PetriNet::PetriNet()
{
  hasNoInterface = false;
  nextId = 1;
}





/*!
 * Creates a low-level place without an initial role.
 * \return pointer of the created place
 */
Place *PetriNet::newPlace()
{
  return newPlace("", INTERNAL);
}





/*!
 * The actual newPlace function called by all other overloaded newPlace
 * functions.
 *
 * \param role   the initial role of the place
 * \param mytype the type of the place(as defined in #place_type)
 * \return pointer of the created place
 */
Place *PetriNet::newPlace(string role, place_type mytype)
{
  trace(TRACE_VERY_DEBUG, "[PN]\tCreating place p" + intToString(id()) + " (" + role + ") ...\n");

  Place *p = new Place(getId(), role, mytype);

  // Decide in which set of places the place has to be inserted.
  switch(mytype)
  {
    case (IN):	{ P_in.insert(p); break; }
    case (OUT):	{ P_out.insert(p); break; }
    default:	{ P.insert(p); break; }
  }

  // Test if the place is already defined.
  if (role != "")
  {
    if (roleMap[role] != NULL)
    {
      throw Exception(DOUBLE_NODE, "Place with role '" + role + "' already defined.\n", pos(__FILE__, __LINE__, __FUNCTION__));
    }
    else
      roleMap[role] = p;
  }

  return p;
}





/*---------------------------------------------------------------------------*/


/*!
 * Creates a transition without an initial role.
 * \return pointer of the created transition
 */
Transition *PetriNet::newTransition()
{
  return newTransition("");
}





/*!
 * The actual newTransition function called by all other overloaded
 * newTransition functions.
 *
 * \param role  the initial role of the transition
 * \return pointer of the created transition
 */
Transition *PetriNet::newTransition(string role)
{
  trace(TRACE_VERY_DEBUG, "[PN]\tCreating transition t" + intToString(id()) + " (" + role + ") ...\n");

  Transition *t = new Transition(getId(), role);
  T.insert(t);

  // Test if the transition is already defined.
  if (role != "")
  {
    if (roleMap[role] != NULL)
    {
      throw Exception(DOUBLE_NODE, "Transition with role '" + role + "' already defined.\n", pos(__FILE__, __LINE__, __FUNCTION__));
    }
    else
      roleMap[role] = t;
  }

  return t;
}





/*---------------------------------------------------------------------------*/


/*!
 * Creates an arc with arbitrary type.
 * \param source      source node of the arc
 * \param target      target node of the arc
 * \param type        type of the arc(as defined in #arc_type)
 * \return pointer of the created arc
 */
Arc *PetriNet::newArc(Node * source, Node * target, arc_type type)
{

  // Tests if the source node is a NULL pointer, i.e. the node was not found
  // and the arc cannot be added.
  if ((Place *) source == NULL ||(Transition *) source == NULL)
  {
    string name = "unknown";
    string role = "unknown";
    if ((Place *) target != NULL)
    {
      name = target->nodeTypeName() + " " + target->nodeShortName();
      role = *(target->history.begin());
    }

    if ((Transition *) target != NULL)
    {
      name = target->nodeTypeName() + " " + target->nodeShortName();
      role = *(target->history.begin());
    }

    throw Exception(ARC_ERROR, "Source of arc to " + name + " (" + role + ") not found!\n", pos(__FILE__, __LINE__, __FUNCTION__));
  }

  // Tests if the target node is a NULL pointer, i.e. the node was not found
  // and the arc cannot be added.
  if ((Place *) target == NULL ||(Transition *) target == NULL)
  {
    string name = "unknown";
    string role = "unknown";
    if ((Place *) source != NULL)
    {
      name = source->nodeTypeName() + " " + source->nodeShortName();
      role = *(source->history.begin());
    }
    if ((Transition *) source != NULL)
    {
      name = source->nodeTypeName() + " " + source->nodeShortName();
      role = *(source->history.begin());
    }

    throw Exception(ARC_ERROR, "Target of arc from " + name + " (" + role + ") not found!\n", pos(__FILE__, __LINE__, __FUNCTION__));
  }

  // Now we can be sure both nodes exist.
  trace(TRACE_VERY_DEBUG, "[PN]\tCreating arc (" + intToString(source->id) + "," + intToString(target->id) + ")...\n");

  // Tests if the two nodes are of different types, i.e. we only draw arcs
  // between places and transitions.
  if (source->nodeType == target->nodeType)
    throw Exception(ARC_ERROR, "Arc between two " + source->nodeTypeName() +
	"s!\n" + *((source->history).begin()) + " and " + *((target->history).begin()), pos(__FILE__, __LINE__, __FUNCTION__));

  // Verbose output every 1000 arcs.
  if ((!F.empty()) &&(F.size() % 1000 == 0))
    trace(TRACE_INFORMATION, "[PN]\t" + information() + "\n");

  // Finally add the arc to the Petri net.
  Arc *f = new Arc(source, target);
  F.insert(f);

  // Add a second arc to close a loop if the arc is a read arc.
  if (type == READ)
  {
    Arc *f2 = new Arc(target, source);
    F.insert(f2);
  }

  return f;
}





/*---------------------------------------------------------------------------*/

/*!
 * Removes all ingoing and outgoing arcs of a node, i.e. detatches the node
 * from the rest of the net.
 *
 * \param n node to be detached
 */
void PetriNet::detachNode(Node * n)
{
  trace(TRACE_VERY_DEBUG, "[PN]\tDetaching node " + intToString(n->id) + "...\n");

  vector<Arc *> removeList;

  for (set<Arc *>::iterator f = F.begin(); f != F.end(); f++)
    if (((*f)->source == n) ||((*f)->target == n))
      removeList.push_back(*f);

  for (unsigned int i = 0; i < removeList.size(); i++)
    removeArc(removeList[i]);
}





/*!
 * Removes a place and all arcs connected with it.
 * \param p place to be removed
 */
void PetriNet::removePlace(Place * p)
{
  trace(TRACE_VERY_DEBUG, "[PN]\tRemoving place " + intToString(p->id) + "...\n");
  
  
  detachNode(p);


  // Remove the roles of the place p, i.e. set the mappings to the NULL
  // pointer.  
  for (vector<string>::iterator role = p->history.begin(); role != p->history.end(); role++)
    if (roleMap[*role] == p)
      roleMap[*role] = NULL;

  
  // Decide from which set of places the place has to be removed.
  switch(p->type)
  {
    case(IN):  { P_in.erase(p); break; }
    case(OUT): { P_out.erase(p); break; }
    default:   { P.erase(p); break; }
  }

  delete p;
}





/*!
 * Removes a transition and all arcs connected with it.
 * \param t transition to be removed
 */
void PetriNet::removeTransition(Transition * t)
{
  trace(TRACE_VERY_DEBUG, "[PN]\tRemoving transition " + intToString(t->id) + "...\n");

  detachNode(t);
  T.erase(t);
  delete t;
}





/*!
 * \param f arc to be removed
 */
void PetriNet::removeArc(Arc * f)
{
  trace(TRACE_VERY_DEBUG, "[PN]\tRemoving arc (" + intToString(f->source->id) + "," + intToString(f->target->id) + ")...\n");

  F.erase(f);
  delete f;
}





/*---------------------------------------------------------------------------*/


/*!
 * \return string containing information about the net
 */
string PetriNet::information()
{
  string result = intToString(P.size()) + " places, " + intToString(T.size()) + " transitions, " + intToString(F.size()) + " arcs";
  return result;
}





/*!
 * Prints information about the generated Petri net. In particular, for each
 * place and transition all roles of the history are printed to understand
 * the net and maybe LoLA's witness pathes later.
 *
 * \todo put this to the nodes
 */
void PetriNet::printInformation()
{
  // the places
  (*output) << "PLACES:\nID\tTYPE\t\tROLES\n";

  // the internal places
  for (set<Place *>::iterator p = P.begin(); p != P.end(); p++)
  {
    (*output) <<(*p)->nodeShortName() << "\tinternal";

    for (vector<string>::iterator role =(*p)->history.begin(); role !=(*p)->history.end(); role++)
      if (role ==(*p)->history.begin())
	(*output) << "\t" + *role + "\n";
      else
	(*output) << "\t\t\t" + *role + "\n";
  }

  // the input places
  for (set<Place *>::iterator p = P_in.begin(); p != P_in.end(); p++)
  {
    (*output) <<(*p)->nodeShortName() << "\tinput   ";

    for (vector<string>::iterator role =(*p)->history.begin(); role !=(*p)->history.end(); role++)
      if (role ==(*p)->history.begin())
	(*output) << "\t" + *role + "\n";
      else
	(*output) << "\t\t\t" + *role + "\n";
  }

  // the output places
  for (set<Place *>::iterator p = P_out.begin(); p != P_out.end(); p++)
  {
    (*output) <<(*p)->nodeShortName() << "\toutput  ";

    for (vector<string>::iterator role =(*p)->history.begin(); role !=(*p)->history.end(); role++)
      if (role ==(*p)->history.begin())
	(*output) << "\t" + *role + "\n";
      else
	(*output) << "\t\t\t" + *role + "\n";
  }

  // the transitions
  (*output) << "\nTRANSITIONS:\n";
  (*output) << "ID\tROLES\n";

  for (set<Transition *>::iterator t = T.begin(); t != T.end(); t++)
  {
    (*output) << (*t)->nodeShortName() + "\t";

    for (vector<string>::iterator role =(*t)->history.begin(); role !=(*t)->history.end(); role++)
      if (role == (*t)->history.begin())
	(*output) << *role + "\n";
      else
	(*output) << "\t" + *role + "\n";
  }
}





/*---------------------------------------------------------------------------*/


/*!
 * Creates a DOT output(see http://www.graphviz.org) of the Petri net. It uses
 * the digraph-statement and adds labels to transitions, places and arcs if
 * neccessary. It also distinguishes the three arc types of #arc_type.
 */
void PetriNet::dotOut()
{
  trace(TRACE_DEBUG, "[PN]\tCreating DOT-output.\n");

  (*output) << "digraph N {" << endl;
  (*output) << " graph [fontname=\"Helvetica-Oblique\", label=\"";

  if (parameters[P_SIMPLIFY])
    (*output) << "structural simplified ";

  (*output) << "Petri net generated from " << filename << "\"];" << endl;
  (*output) <<
    " node [fontname=\"Helvetica-Oblique\" fontsize=10 fixedsize];" << endl;
  (*output) << " edge [fontname=\"Helvetica-Oblique\" fontsize=10];" <<
    endl << endl;

  // list the places
  (*output) << endl << " node [shape=circle];" << endl;
  for (set<Place *>::iterator p = P.begin(); p != P.end(); p++)
    (*output) <<(*p)->dotOut();

  if (!hasNoInterface)
  {
    for (set<Place *>::iterator p = P_in.begin(); p != P_in.end(); p++)
      (*output) <<(*p)->dotOut();
    for (set<Place *>::iterator p = P_out.begin(); p != P_out.end(); p++)
      (*output) <<(*p)->dotOut();
  }

  // list the transitions
  (*output) << endl << " node [shape=box regular=true];" << endl;
  for (set<Transition *>::iterator t = T.begin(); t != T.end(); t++)
    (*output) <<(*t)->dotOut();

  // list the arcs
  for (set<Arc *>::iterator f = F.begin(); f != F.end(); f++)
    (*output) <<(*f)->dotOut();

  (*output) << "}" << endl;
}




/*!
 * Collect all input and output places and remove(i.e. detach) them.
 */
void PetriNet::removeInterface()
{
  trace(TRACE_DEBUG, "[PN]\tRemoving interface places.\n");

  list<Place *> killList;

  for (set<Place *>::iterator p = P_in.begin(); p != P_in.end(); p++)
    killList.push_back(*p);

  for (set<Place *>::iterator p = P_out.begin(); p != P_out.end(); p++)
    killList.push_back(*p);

  for (list<Place *>::iterator it = killList.begin(); it != killList.end(); it++)
    removePlace(*it);

  hasNoInterface = true;
}





/*****************************************************************************
 * Output formats
 *****************************************************************************/

/*!
 * Outputs the net in PNML (Petri Net Markup Language).
 */
void PetriNet::pnmlOut()
{
  trace(TRACE_DEBUG, "[PN]\tCreating PNML-output.\n");

  // remove interface since we do not create an open workflow net
  removeInterface();

  (*output) << "<!-- Petri net created by " << PACKAGE_STRING << " reading file " << filename << " -->" << endl << endl;

  (*output) << "<pnml>" << endl;
  (*output) << "  <net id=\"" << filename << "\" type=\"\">" << endl << endl;

  // places(only internal)
  for (set<Place *>::iterator p = P.begin(); p != P.end(); p++)
  {
    (*output) << "    <place id=\"" <<(*p)->nodeShortName() << "\">" << endl;
    (*output) << "      <name>" << endl;
    (*output) << "        <text>" <<(*(*p)->history.begin()) << "</text>" << endl;
    (*output) << "      </name>" << endl;
    if ((*p)->marked)
    {
      (*output) << "      <initialMarking>" << endl;
      (*output) << "        <text>1</text>" << endl;
      (*output) << "      </initialMarking>" << endl;
    }
    (*output) << "    </place>" << endl << endl;
  }

  // transitions
  for (set<Transition *>::iterator t = T.begin(); t != T.end(); t++)
  {
    (*output) << "    <transition id=\"" <<(*t)->nodeShortName() << "\">" << endl;
    (*output) << "      <name>" << endl;
    (*output) << "        <text>" <<(*(*t)->history.begin()) << "</text>" << endl;
    (*output) << "      </name>" << endl;
    (*output) << "    </transition>" << endl << endl;
  }
  (*output) << endl;

  // arcs
  int arcNumber = 1;
  for (set<Arc *>::iterator f = F.begin(); f != F.end(); f++, arcNumber++)
  {
    (*output) << "    <arc id=\"a" << arcNumber << "\" ";
    (*output) << "source=\"" <<(*f)->source->nodeShortName() << "\" ";
    (*output) << "target=\"" <<(*f)->source->nodeShortName() << "\" />" << endl;
  }
  (*output) << endl;
  (*output) << "  </net>" << endl;
  (*output) << "</pnml>" << endl;
  (*output) << endl << "<!-- END OF FILE -->" << endl;
}





/*!
 * Outputs the net in low-level PEP notation.
 */
void PetriNet::pepOut()
{
  trace(TRACE_DEBUG, "[PN]\tCreating PEP-output.\n");

  // remove interface since we do not create an open workflow net
  removeInterface();

  // header
  (*output) << "PEP" << endl << "PTNet" << endl << "FORMAT_N" << endl;

  // places(only internal)
  (*output) << "PL" << endl;
  for (set<Place *>::iterator p = P.begin(); p != P.end(); p++)
  {
    (*output) <<(*p)->id << "\"" <<(*p)->nodeShortName() << "\"80@40";
    if ((*p)->marked)
      (*output) << "M1";
    (*output) << "k1" << endl;
  }

  // transitions
  (*output) << "TR" << endl;
  for (set<Transition *>::iterator t = T.begin(); t != T.end(); t++)
    (*output) <<(*t)->id << "\"" <<(*t)->nodeShortName() << "\"80@40" << endl;

  // arcs from transitions to places
  (*output) << "TP" << endl;
  for (set<Arc *>::iterator f = F.begin(); f != F.end(); f++)
    if (((*f)->source->nodeType) == TRANSITION)
      (*output) <<(*f)->source->id << "<" <<(*f)->target->id << endl;

  // arcs from places to transitions
  (*output) << "PT" << endl;
  for (set<Arc *>::iterator f = F.begin(); f != F.end(); f++)
    if (((*f)->source->nodeType) == PLACE)
      (*output) <<(*f)->source->id << ">" <<(*f)->target->id << endl;
}





/*!
 * Outputs the net in APNN (Abstract Petri Net Notation).
 */
void PetriNet::apnnOut()
{
  trace(TRACE_DEBUG, "[PN]\tCreating APNN-output.\n");

  // remove interface since we do not create an open workflow net
  removeInterface();

  (*output) << "\\beginnet{" << filename << "}" << endl << endl;

  // places(only internal)
  for (set<Place *>::iterator p = P.begin(); p != P.end(); p++)
  {
    (*output) << "  \\place{" <<(*p)->nodeShortName() << "}{";
    if ((*p)->marked)
      (*output) << "\\init{1}";
    (*output) << "}" << endl;
  }
  (*output) << endl;

  // transitions
  for (set<Transition *>::iterator t = T.begin(); t != T.end(); t++)
  {
    (*output) << "  \\transition{" <<(*t)->nodeShortName() << "}{}" << endl;
  }
  (*output) << endl;

  // arcs
  int arcNumber = 1;
  for (set<Arc *>::iterator f = F.begin(); f != F.end(); f++, arcNumber++)
  {
    (*output) << "  \\arc{a" << arcNumber << "}{ ";
    (*output) << "\\from{" <<(*f)->source->nodeShortName() << "} ";
    (*output) << "\\to{" <<(*f)->target->nodeShortName() << "} }" << endl;
  }
  (*output) << endl;

  (*output) << "\\endnet" << endl;
}





/*!
 * Outputs the net in LoLA-format.
 */
void PetriNet::lolaOut()
{
  trace(TRACE_DEBUG, "[PN]\tCreating LoLA-output.\n");

  // remove interface since we do not create an open workflow net
  removeInterface();

  (*output) << "{ Petri net created by " << PACKAGE_STRING << " reading " << filename << " }" << endl << endl;

  
  // places (only internal)
  (*output) << "PLACE" << endl;
  unsigned int count = 1;
  for (set<Place *>::iterator p = P.begin(); p != P.end(); count++, p++)
  {
    (*output) << "  " <<(*p)->nodeShortName();
    
    if (count < P.size())
      (*output) << "," << endl;
  }
  (*output) << endl << ";" << endl << endl << endl;


  // initial marking
  (*output) << "MARKING" << endl;
  count = 1;
  for (set<Place *>::iterator p = P.begin(); p != P.end(); p++)
  {
    if ((*p)->marked)
    {
      if (count++ != 1)
	(*output) << "," << endl;
      
      (*output) << "  " << (*p)->nodeShortName() << ":\t1";
    }
  }
  (*output) << endl << ";" << endl << endl << endl;


  // transitions
  for (set<Transition *>::iterator t = T.begin(); t != T.end(); t++)
  {
    (*output) << "TRANSITION " << (*t)->nodeShortName() << endl;
    set<Node *> consume = preset(*t);
    set<Node *> produce = postset(*t);
    
    (*output) << "CONSUME" << endl;
    count = 1;
    for (set<Node *>::iterator pre = consume.begin(); pre != consume.end(); count++, pre++)
    {
      (*output) << "  " << (*pre)->nodeShortName() << ":\t1";
      
      if (count < consume.size())
	(*output) << "," << endl;
    }
    (*output) << ";" << endl;
    
    (*output) << "PRODUCE" << endl;
    count = 1;
    for (set<Node *>::iterator post = produce.begin(); post != produce.end(); count++, post++)
    {
      (*output) << "  " << (*post)->nodeShortName() << ":\t1";
      
      if (count < produce.size())
	(*output) << "," << endl;
    }
    
    (*output) << ";" << endl << endl;
  }  
  (*output) << "{ END OF FILE }" << endl;
}





/*!
 * Outputs the net in oWFN-format.
 */
void PetriNet::owfnOut()
{
  trace(TRACE_DEBUG, "[PN]\tCreating oWFN-output.\n");

  (*output) << "{ oWFN created by " << PACKAGE_STRING << " reading " << filename << " }" << endl << endl;

  // places
  (*output) << "PLACE" << endl;

  // internal places
  (*output) << "  INTERNAL" << endl;
  unsigned int count = 1;
  for (set<Place *>::iterator p = P.begin(); p != P.end(); count++, p++)
  {
    (*output) << "    " <<(*p)->nodeShortName();
    
    if (count < P.size())
      (*output) << "," << endl;
  }
  (*output) << endl << ";" << endl << endl;


  // input places
  (*output) << "  INPUT" << endl;
  count = 1;
  for (set<Place *>::iterator p = P_in.begin(); p != P_in.end(); count++, p++)
  {
    (*output) << "    " <<(*p)->nodeShortName();
    
    if (count < P_in.size())
      (*output) << "," << endl;
  }
  (*output) << endl << ";" << endl << endl;


  // output places
  (*output) << "  OUTPUT" << endl;
  count = 1;
  for (set<Place *>::iterator p = P_out.begin(); p != P_out.end(); count++, p++)
  {
    (*output) << "    " <<(*p)->nodeShortName();
    
    if (count < P_out.size())
      (*output) << "," << endl;
  }
  (*output) << endl << ";" << endl << endl;
  

  // initial marking
  (*output) << "INITIALMARKING" << endl;
  count = 1;
  for (set<Place *>::iterator p = P.begin(); p != P.end(); p++)
  {
    if ((*p)->marked)
    {
      if (count++ != 1)
	(*output) << "," << endl;
      
      (*output) << "  " << (*p)->nodeShortName() << ":\t1";
    }
  }
  (*output) << endl << ";" << endl << endl;  

/*
  // final condition
  (*output) << "FINALCONDITION" << endl << "  (";
  
  for (set<Place*>::iterator p_in = P_in.begin();
      p_in != P_in.end();
      p_in++)
  {
    (*output) << "(" << (*p_in)->nodeShortName() << " = 0) AND ";
  }

  (*output) << endl << "   ";
  
  for (set<Place*>::iterator p_out = P_out.begin();
      p_out != P_out.end();
      p_out++)
  {
    (*output) << "(" << (*p_out)->nodeShortName() << " = 0) AND ";
  }

  (*output) << endl << "   (" << findPlace("1.internal.final")->nodeShortName() << " > 0));" << endl;
  (*output) << endl << endl << endl;
*/

  // final marking
  (*output) << "FINALMARKING" << endl;
  (*output) << "  " << findPlace("1.internal.final")->nodeShortName() << ":\t1" << endl;
  (*output) << ";" << endl << endl << endl;


  // transitions
  for (set<Transition *>::iterator t = T.begin(); t != T.end(); t++)
  {
    (*output) << "TRANSITION " << (*t)->nodeShortName() << endl;
    set<Node *> consume = preset(*t);
    set<Node *> produce = postset(*t);
    
    (*output) << "CONSUME" << endl;
    count = 1;
    for (set<Node *>::iterator pre = consume.begin(); pre != consume.end(); count++, pre++)
    {
      (*output) << "  " << (*pre)->nodeShortName() << ":\t1";
      
      if (count < consume.size())
	(*output) << "," << endl;
    }
    (*output) << ";" << endl;
    
    (*output) << "PRODUCE" << endl;
    count = 1;
    for (set<Node *>::iterator post = produce.begin(); post != produce.end(); count++, post++)
    {
      (*output) << "  " << (*post)->nodeShortName() << ":\t1";
      
      if (count < produce.size())
	(*output) << "," << endl;
    }
    
    (*output) << ";" << endl << endl;
  }  
  (*output) << "{ END OF FILE }" << endl;
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
void PetriNet::mergeTransitions(Transition * t1, Transition * t2)
{
  if (t1 == NULL || t2 == NULL)
    throw Exception(MERGING_ERROR, "One of the transitions is null!\n", pos(__FILE__, __LINE__, __FUNCTION__));

  trace(TRACE_VERY_DEBUG, "[PN]\tMerging transitions " + intToString(t1->id) + " and " + intToString(t2->id) + "...\n");

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

  set<Node *> pre12 = setUnion(preset(t1), preset(t2));
  set<Node *> post12 = setUnion(postset(t1), postset(t2));

  for (set<Node *>::iterator n = pre12.begin(); n != pre12.end(); n++)
    newArc((*n), t12);

  for (set<Node *>::iterator n = post12.begin(); n != post12.end(); n++)
    newArc(t12,(*n));

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
 *       -(nlohmann) Make use of P_in and P_out
 */
void PetriNet::mergePlaces(Place * p1, Place * p2)
{
  if (p1 == p2)
    return;

  if (p1 == NULL || p2 == NULL)
    return;

  if (p1->type != INTERNAL || p2->type != INTERNAL)
    throw Exception(MERGING_ERROR, (string)"Merging of interface places not supported!\n" + "place " +
	p1->nodeShortName() + "(type " + intToString(p2->type) + ") and " +
	p2->nodeShortName() + "(type " + intToString(p2->type) + ")", pos(__FILE__, __LINE__, __FUNCTION__));

  trace(TRACE_VERY_DEBUG, "[PN]\tMerging places " + intToString(p1->id) + " and " + intToString(p2->id) + "...\n");

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

  set<Node *> pre12 = setUnion(preset(p1), preset(p2));
  set<Node *> post12 = setUnion(postset(p1), postset(p2));

  for (set<Node *>::iterator n = pre12.begin(); n != pre12.end(); n++)
    newArc((*n), p12);

  for (set<Node *>::iterator n = post12.begin(); n != post12.end(); n++)
    newArc(p12,(*n));

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
 * \param role1 string describing the role of the first place(beginning with a
 *              period: .empty
 * \param act2  activity of the AST represented by the second place(beginning
 *              with a period: .empty
 * \param role2 string describing the role of the second place
 */
void PetriNet::mergePlaces(kc::impl_activity * act1, string role1,
                           kc::impl_activity * act2, string role2)
{
  mergePlaces(intToString(act1->id->value) + role1,
	      intToString(act2->id->value) + role2);
}





/*---------------------------------------------------------------------------*/


/*!
 * \param n a node of the Petri net
 * \result the preset of node n
 */
set<Node *> PetriNet::preset(Node * n)
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
set<Node *> PetriNet::postset(Node * n)
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
 * \return a pointer to the place or a NULL pointer if the place was not found.
 */
Place *PetriNet::findPlace(string role)
{
  Place *result =(Place *) roleMap[role];

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
Place *PetriNet::findPlace(kc::impl_activity * activity, string role)
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
  Transition *result =(Transition *) roleMap[role];

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
 *   removed(sequence) and the preset and postset can be merged.
 *
 * \todo
 *       -(nlohmann) improve performance
 *       -(nlohmann) implement more reduction rules
 *
 */
void PetriNet::simplify()
{
  trace(TRACE_DEBUG, "[PN]\tPetri net size before simplification: " + information() + "\n");
  trace(TRACE_INFORMATION, "Simplifying Petri net...\n");



  
  // remove structural dead nodes
  bool done = false;
  while (!done)
  {
    done = true;
  
    list<Place*> deadPlaces;
    list<Transition*> deadTransitions;
    list<Place*> tempPlaces;

    // find unmarked places with empty preset
    for (set<Place*>::iterator p = P.begin(); p != P.end(); p++)
    {
      if (preset(*p).empty() && !(*p)->marked)
      {
	deadPlaces.push_back(*p);
	tempPlaces.push_back(*p);
	trace(TRACE_VERY_DEBUG, "[PN]\tPlace p" + intToString((*p)->id) + " is structurally dead.\n");
	done = false;
      }
    }

    while (!tempPlaces.empty())
    {
      // p is a dead place
      Place* p = tempPlaces.back();
      tempPlaces.pop_back();
      set<Node*> ps = postset(p);

      // transitions in the postset of a dead place are dead
      for (set<Node*>::iterator t = ps.begin(); t != ps.end(); t++)
      {
      	deadTransitions.push_back( (Transition*)(*t) );
	trace(TRACE_VERY_DEBUG, "[PN]\tTransition t" + intToString((*t)->id) + " is structurally dead\n");
	done = false;
      }
    }


    // remove dead nodes
    for (list<Place*>::iterator p = deadPlaces.begin(); p != deadPlaces.end(); p++)
      if (P.find(*p) != P.end())
	removePlace(*p);
    for (list<Transition*>::iterator t = deadTransitions.begin(); t != deadTransitions.end(); t++)
      if (T. find(*t) != T.end())
	removeTransition(*t);
  }


  
  
  
  // a pair to store transitions to be merged
  vector<pair<string, string> > transitionPairs;

  trace(TRACE_VERY_DEBUG, "[PN]\tSearching for transitions with same preset and postset...\n");
  // find transitions with same preset and postset
  for (set<Transition *>::iterator t1 = T.begin(); t1 != T.end(); t1++)
    for (set<Transition *>::iterator t2 = t1; t2 != T.end(); t2++)
      if (*t1 != *t2)
	if ((preset(*t1) == preset(*t2)) &&(postset(*t1) == postset(*t2)))
	  transitionPairs.push_back(pair<string, string>(*((*t1)->history.begin()), *((*t2)->history.begin())));

  trace(TRACE_VERY_DEBUG, "[PN]\tFound " + intToString(transitionPairs.size()) + " transitions with same preset and postset...\n");

  // merge the found transitions
  for (unsigned int i = 0; i < transitionPairs.size(); i++)
  {
    Transition *t1 = findTransition(transitionPairs[i].first);
    Transition *t2 = findTransition(transitionPairs[i].second);

    if ((t1 != NULL) && (t2 != NULL) && (t1 != t2))
      mergeTransitions(t1, t2);
  }

  trace(TRACE_VERY_DEBUG, "[PN]\tnew reduction rule\n");

  // a pair to store places to be merged
  vector<string> sequenceTransitions;
  vector<pair<string, string> >placeMerge;

  // find transitions with singelton preset and postset
  for (set<Transition *>::iterator t = T.begin(); t != T.end(); t++)
    if (preset(*t).size() == 1 && postset(*t).size() == 1)
      {
	string id1 = *((*(preset(*t).begin()))->history.begin());
	string id2 = *((*(postset(*t).begin()))->history.begin());
	placeMerge.push_back(pair < string, string >(id1, id2));
	sequenceTransitions.push_back(*((*t)->history.begin()));
      }

  // merge preset and postset
  for (unsigned int i = 0; i < placeMerge.size(); i++)
    mergePlaces(placeMerge[i].first, placeMerge[i].second);

  // remove "sequence"-transtions
  for (unsigned int i = 0; i < sequenceTransitions.size(); i++)
    {
      Transition *sequenceTransition = findTransition(sequenceTransitions[i]);
      if (sequenceTransition != NULL)
	removeTransition(sequenceTransition);
    }

  trace(TRACE_INFORMATION, "Simplifying complete.\n");
  trace(TRACE_DEBUG, "[PN]\tPetri net size after simplification: " + information() + "\n");
}





void PetriNet::removeIsolatedNodes()
{
  set<Place *> isolatedPlaces;
  
  for (set<Place *>::iterator p = P.begin(); p != P.end(); p++)
    if (preset(*p).empty() && postset(*p).empty())
      isolatedPlaces.insert(*p);

  for (set<Place *>::iterator p = isolatedPlaces.begin(); p != isolatedPlaces.end(); p++)
    removePlace(*p);
}



void PetriNet::removeVariables()
{
  extern SymbolManager symMan;
  
  for (list<string>::iterator variable = symMan.variables.begin(); variable != symMan.variables.end(); variable++)
  {
    removePlace( findPlace("variable." + *variable) );
  }
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
