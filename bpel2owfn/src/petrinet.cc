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
 *          - last changed: \$Date: 2006/06/14 11:26:31 $
 *
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.124 $
 */





#include "petrinet.h"
#include "debug.h"		// debugging help
#include "exception.h"		// exception class
#include "helpers.h"		// helper functions





/******************************************************************************
 * External variables
 *****************************************************************************/

//extern SymbolManager symMan;	// defined in bpel-syntax.yy





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
 * \param role a role of a node
 * \return true, if the node's history contains the given role
 */
bool Node::historyContains(string role)
{
  for (int i = 0; i < history.size(); i++)
  {
    if (history[i] == role)
      return true;
  }

  return false;
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





/*****************************************************************************/


/*!
 * \param myid    the id of the transition
 * \param role    the first role of the transition
 */
Transition::Transition(unsigned int myid, string role)
{
  id = myid;
  nodeType = TRANSITION;
  type = INTERNAL;

  if (role != "")
    history.push_back(role);
}









/*****************************************************************************/


/*!
 * \param myid   the internal id of the place
 * \param role   the first role of the place
 * \param mytype the type of the place (as defined in #communication_type)
 */
Place::Place(unsigned int myid, string role, communication_type mytype)
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
 * \param mytype the type of the place (as defined in #communication_type)
 * \return pointer of the created place
 */
Place *PetriNet::newPlace(string role, communication_type mytype)
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
  if (p == NULL)
    return;

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
  if (t == NULL)
    return;

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
  if (f == NULL)
    return;

  trace(TRACE_VERY_DEBUG, "[PN]\tRemoving arc (" + intToString(f->source->id) + "," + intToString(f->target->id) + ")...\n");

  F.erase(f);
  delete f;
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
 */
void PetriNet::mergeTransitions(Transition * t1, Transition * t2)
{
  if (t1 == NULL || t2 == NULL)
    throw Exception(MERGING_ERROR, "One of the transitions is null!\n", pos(__FILE__, __LINE__, __FUNCTION__));

  trace(TRACE_VERY_DEBUG, "[PN]\tMerging transitions " + intToString(t1->id) + " and " + intToString(t2->id) + "...\n");


//  if (t1->type != INTERNAL && t2->type != INTERNAL)
//    throw Exception(MERGING_ERROR, "Cannot merge communication transition!\n", pos(__FILE__, __LINE__, __FUNCTION__));
  

  Node *t12 = newTransition();
  if (t1->type != INTERNAL)
    t12->type = t1->type;

  if (t2->type != INTERNAL)
    t12->type = t2->type;


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

  if (p1->marked || p2->marked)
    ((Place*)p12)->mark();
  
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

/*!
 *  Adds a prefix to all nodes of the Petri net
 *
 *  \param prefix The prefix to add.
 *
 */
void PetriNet::addPrefix(string prefix)
{

  for (set< Place * >::iterator place = P.begin(); place != P.end(); place ++)
  {
    for(vector< string >::iterator name = (*place)->history.begin(); name != (*place)->history.end(); name++)
    {
      *name = prefix + *name;
    }
  }
  for (set< Transition * >::iterator transition = T.begin(); transition != T.end(); transition ++)
  {
    for(vector< string >::iterator name = (*transition)->history.begin(); name != (*transition)->history.end(); name++)
    {
      *name = prefix + *name;
    }
  }
}

void PetriNet::connectNet(PetriNet * net)
{

  for (set< Place * >::iterator place = net->P.begin(); place != net->P.end(); place ++)
  {
    (*place)->id = getId();
    P.insert(*place);
  }
  for (set< Transition * >::iterator transition = net->T.begin(); transition != net->T.end(); transition ++)
  {
    (*transition)->id = getId();
    T.insert(*transition);
  }
  for (set< Arc * >::iterator arc = net->F.begin(); arc != net->F.end(); arc ++)
  {
    F.insert(*arc);
  }
  for (set< Place * >::iterator place = net->P_in.begin(); place != net->P_in.end(); place ++)
  {
    (*place)->id = getId();
    P_in.insert(*place);
  }
  for (set< Place * >::iterator place = net->P_out.begin(); place != net->P_out.end(); place ++)
  {
    (*place)->id = getId();
    P_out.insert(*place);
  }
  // merge input and output places
  set< Place * > newP_in;
  for (set< Place * >::iterator place = P_in.begin(); place != P_in.end(); place ++)
  {
    set< Place * >::iterator oPlace = P_out.begin();
    bool finished = false;
    while ( ! finished && oPlace != P_out.end())
    {
      if ((*oPlace)->nodeName() != (*place)->nodeName())
      {
	oPlace++;
      }
      else
      {
	finished = true;
      }
    }
    if (oPlace != P_out.end())
    {
      (*place)->type = INTERNAL;
      (*place)->history[0] = (*place)->nodeName();
      (*oPlace)->type = INTERNAL;
      (*oPlace)->history[0] = (*oPlace)->nodeName();
      mergePlaces(*place,*oPlace);
      P_out.erase(*oPlace);
    }
    else
    {
      newP_in.insert(*place);
    }
  }
  P_in = newP_in;
  
}

