/*****************************************************************************\
 * Copyright 2005, 2006 Niels Lohmann, Christian Gierds                      *
 *                                                                           *
 * This file is part of GNU BPEL2oWFN.                                       *
 *                                                                           *
 * GNU BPEL2oWFN is free software; you can redistribute it and/or modify it  *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * GNU BPEL2oWFN is distributed in the hope that it will be useful, but      *
 * WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General  *
 * Public License for more details.                                          *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with GNU BPEL2oWFN; see file COPYING. if not, write to the Free Software  *
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA. *
\*****************************************************************************/

/*!
 * \file    petrinet.cc
 *
 * \brief   Petri Net API: base functions
 *
 * \author  responsible: Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          last changes of: \$Author: gierds $
 *
 * \since   2005-10-18
 *
 * \date    \$Date: 2006/12/05 14:25:29 $
 *
 * \note    This file is part of the tool GNU BPEL2oWFN and was created during
 *          the project Tools4BPEL at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.164 $
 *
 * \ingroup petrinet
 */





/******************************************************************************
 * Headers
 *****************************************************************************/

#include <cassert>
#include <iostream>

#include "petrinet.h"
#include "helpers.h"		// helper functions
#include "debug.h"		// debug functions





/******************************************************************************
 * Implementation of class functions
 *****************************************************************************/

/*!
 * \brief  true if first role contains role
 * \param  role a role of a node
 * \return true, if the node's first history entry contains the given role
 */
bool Node::firstMemberAs(string role) const
{
  string firstEntry = (*history.begin());
  return (firstEntry.find(role, 0) == firstEntry.find_first_of(".") + 1);
}





/*!
 * \brief  true if first role begins with role
 * \param  role a role of a node
 * \return true, if the node's first history entry begins with the given role
 */
bool Node::firstMemberIs(string role) const
{
  string firstEntry = (*history.begin());
  return (firstEntry.find(role, 0) == 0);
}





/*!
 * \brief  true if history contains role
 * \param  role a role of a node
 * \return true, if the node's history contains the given role
 */
bool Node::historyContains(string role) const
{
  for (unsigned int i = 0; i < history.size(); i++)
    if (history[i] == role)
      return true;

  return false;
}





/*****************************************************************************/


/*!
 * \param my_source	the source-node of the arc
 * \param my_target	the target-node of the arc
 * \param my_weight	the weight of the arc (standard: 1)
 *
 * \pre my_source != NULL
 * \pre my_target != NULL
 */
Arc::Arc(Node *my_source, Node *my_target, unsigned int my_weight)
{
  assert(my_source != NULL);
  assert(my_target != NULL);

  source = my_source;
  target = my_target;
  weight = my_weight;
}





/*****************************************************************************/


/*!
 * \param myid	the id of the transition
 * \param role	the first role of the transition
 */
Transition::Transition(unsigned int my_id, string my_role)
{
  id = my_id;
  nodeType = TRANSITION;
  type = INTERNAL;

  if (my_role != "")
    history.push_back(my_role);
}





/*****************************************************************************/


/*!
 * \param myid   the internal id of the place
 * \param role   the first role of the place
 * \param mytype the type of the place (as defined in #communication_type)
 */
Place::Place(unsigned int my_id, string my_role, communication_type my_type)
{
  type = my_type;
  id = my_id;
  nodeType = PLACE;
  tokens = 0;

  if (my_role != "")
    history.push_back(my_role);
}





/*!
 * \param my_tokens	the number of tokens of this place in the inital
 * 			marking
 */
void Place::mark(unsigned int my_tokens)
{
  tokens = my_tokens;
}





/*****************************************************************************/


/*!
 * The constructor.
 */
PetriNet::PetriNet()
{
  nextId = 1;
  format = FORMAT_OWFN;
}





/*!
 * The destructor of the PetriNet class.
 *
 * \post set of places empty
 * \post set of transitions empty
 * \post set of arcs empty
 * \post roleMap empty
 */
PetriNet::~PetriNet()
{
/*
  for (set<Place *>::iterator p = P.begin(); p != P.end(); p++)
    delete *p;

  for (set<Transition *>::iterator t = T.begin(); t != T.end(); t++)
    delete *t;

  for (set<Arc *>::iterator f = F.begin(); f != F.end(); f++)
    delete *f;

  roleMap.clear();
  */
}





/*!
 * \param my_role  the initial role of the place
 * \param my_type  the type of the place (as defined in #communication_type)
 * \return  pointer of the created place
 * \pre No place with the given role is defined.
 */
Place *PetriNet::newPlace(std::string my_role, communication_type my_type)
{
  Place *p = new Place(getId(), my_role, my_type);
  assert(p != NULL);

  // Decide in which set of places the place has to be inserted.
  switch(my_type)
  {
    case (IN):	{ P_in.insert(p);  break; }
    case (OUT):	{ P_out.insert(p); break; }
    default:	{ P.insert(p);     break; }
  }

  // Test if the place is already defined.
  if (my_role != "")
  {
    assert(roleMap[my_role] == NULL);
    roleMap[my_role] = p;
  }

  return p;
}





/*---------------------------------------------------------------------------*/


/*!
 * \param role  the initial role of the transition
 * \return pointer of the created transition
 * \pre No transition with the given role is defined.
 */
Transition *PetriNet::newTransition(std::string my_role)
{
  Transition *t = new Transition(getId(), my_role);
  assert(t != NULL);

  T.insert(t);

  // Test if the transition is already defined.
  if (my_role != "")
  {
    assert(roleMap[my_role] == NULL);
    roleMap[my_role] = t;
  }

  return t;
}





/*---------------------------------------------------------------------------*/


/*!
 * \param source      source node of the arc
 * \param target      target node of the arc
 * \param type        type of the arc (as defined in #arc_type)
 * \param my_weight   weight of the arc (standard: 1)
 * \return pointer of the created arc
 * \pre The types of the nodes have to match, i.e. an arc can only be created
 *      between a place and a transition or a transition and a place, resp.
 * \pre my_target != NULL
 * \pre my_source != NULL
 */
Arc *PetriNet::newArc(Node *my_source, Node *my_target, arc_type my_type, unsigned int my_weight)
{
  assert(my_source != NULL);
  assert(my_target != NULL);

  assert(my_source->nodeType != my_target->nodeType);

  // Tag the involved transition as communicating if it is.
  if (my_source->nodeType == PLACE)
    if (P_in.find(static_cast<Place*>(my_source)) != P_in.end())
      (static_cast<Transition*>(my_target))->type = IN;
    
  if (my_target->nodeType == PLACE)
    if (P_out.find(static_cast<Place*>(my_target)) != P_out.end())
      (static_cast<Transition*>(my_source))->type = OUT;


  // Finally add the arc to the Petri net.
  Arc *f = new Arc(my_source, my_target, my_weight);
  assert(f != NULL);
  F.insert(f);


  // Add a second arc to close a loop if the arc is a read arc.
  if (my_type == READ)
  {
    Arc *f2 = new Arc(my_target, my_source, my_weight);
    assert(f2 != NULL);
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
 * \pre n != NULL
 */
void PetriNet::detachNode(Node *n)
{
  assert(n != NULL);

  vector<Arc*> removeList;

  for (set<Arc*>::iterator f = F.begin(); f != F.end(); f++)
    if (((*f)->source == n) || ((*f)->target == n))
      removeList.push_back(*f);

  for (unsigned int i = 0; i < removeList.size(); i++)
    removeArc(removeList[i]);
}





/*!
 * \param p place to be removed
 * \pre p != NULL
 */
void PetriNet::removePlace(Place *p)
{
  if (p == NULL)
    return;

  detachNode(p);

  // Remove the roles of the place p, i.e. set the mappings to the NULL
  // pointer.  
  for (vector<string>::iterator role = p->history.begin(); role != p->history.end(); role++)
    if (roleMap[*role] == p)
      roleMap[*role] = NULL;

  
  // Decide from which set of places the place has to be removed.
  switch(p->type)
  {
    case(IN):  { P_in.erase(p);  break; }
    case(OUT): { P_out.erase(p); break; }
    default:   { P.erase(p);     break; }
  }

  delete p;
}





/*!
 * \param t transition to be removed
 * \pre t != NULL
 */
void PetriNet::removeTransition(Transition *t)
{
  if (t == NULL)
    return;

  detachNode(t);

  // Remove the roles of the transition  t, i.e. set the mappings to the NULL
  // pointer.  
  for (vector<string>::iterator role = t->history.begin(); role != t->history.end(); role++)
    if (roleMap[*role] == t)
      roleMap[*role] = NULL;

  T.erase(t);
  delete t;
}





/*!
 * \param f arc to be removed
 * \pre f != NULL
 */
void PetriNet::removeArc(Arc *f)
{
  if (f == NULL)
    return;

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
 * \pre t1 != NULL
 * \pre t2 != NULL
 * \post Transitions t1 and t2 removed.
 * \post Transition t12 having the incoming and outgoing arcs of t1 and t2 and
 *       the union of the histories of t1 and t2.
 */
void PetriNet::mergeTransitions(Transition *t1, Transition *t2)
{
  if (t1 == t2)
    return;

  assert(t1 != NULL);
  assert(t2 != NULL);

  Node *t12 = newTransition("");
  assert(t12 != NULL);

  // organize the communication type of the new transition
  if (t1->type == t2->type)
    t12->type = t1->type;

  if ((t1->type == IN && t2->type == INTERNAL) ||
      (t1->type == INTERNAL && t2->type == IN))
    t12->type = IN;

  if ((t1->type == INTERNAL && t2->type == OUT) ||
      (t1->type == OUT && t2->type == INTERNAL))
    t12->type = OUT;

  if ((t1->type == OUT && t2->type == IN) ||
      (t1->type == IN && t2->type == OUT) ||
      (t1->type == INOUT || t2->type == INOUT))
    t12->type = INOUT;


  // copy t1's history to t12
  for (vector<string>::iterator role = t1->history.begin(); role != t1->history.end(); role++)
  {
    roleMap[*role] = t12;
    t12->history.push_back(*role);
  }

  // copy t2's history to t12
  for (vector<string>::iterator role = t2->history.begin(); role != t2->history.end(); role++)
  {
    roleMap[*role] = t12;
    t12->history.push_back(*role);
  }

  // create the arcs for t12
  set<Node *> pre12 = setUnion(preset(t1), preset(t2));
  set<Node *> post12 = setUnion(postset(t1), postset(t2));

  for (set<Node *>::iterator n = pre12.begin(); n != pre12.end(); n++)
    newArc(static_cast<Place*>(*n), t12);

  for (set<Node *>::iterator n = post12.begin(); n != post12.end(); n++)
    newArc(t12, static_cast<Place*>(*n));

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
 * \pre p1 != NULL
 * \pre p2 != NULL
 * \pre p1 and p2 are internal places.
 * \post Places p1 and p2 removed.
 * \post Place p12 having the incoming and outgoing arcs of p1 and p2 and the
 *       union of the histories of t1 and t2.
 *
 * \todo
 *       -(nlohmann) Make use of P_in and P_out
 */
void PetriNet::mergePlaces(Place *p1, Place *p2)
{
  if (p1 == p2)
    return;

  if (p1 == NULL || p2 == NULL)
    return;

  assert(p1 != NULL);
  assert(p2 != NULL);
  assert(p1->type == INTERNAL);
  assert(p2->type == INTERNAL);

  Place *p12 = newPlace("");
  assert(p12 != NULL);

  // p12 is marked with the maximum number of tokens of p1 and p2
  if (p1->tokens > p2->tokens)
    p12->tokens = p1->tokens;
  else
    p12->tokens = p2->tokens;
  
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
 * Merges two places given two identifiers and the roles of the places. The
 * identifiers are used to complete the role-string and pass the search
 * request.
 *
 * \param id1   identifier of the activity represented by the first place
 * \param role1 string describing the role of the first place (beginning with a
 *              period: .empty
 * \param id2   identifier of the activity represented by the second place
 * \param role2 string describing the role of the second place (beginning with a
 *              period: .empty
 */
void PetriNet::mergePlaces(unsigned int id1, string role1, unsigned int id2, string role2)
{
  mergePlaces(toString(id1) + role1, toString(id2) + role2);
}





/*!
 * \param old_name  the name of the role to be renamed
 * \param new_name  the new name of that role
 * \pre  p is a place of the Petri net.
 */
void PetriNet::renamePlace(string old_name, string new_name)
{
  Place *p = findPlace(old_name);
  assert(p != NULL);

  bool found = false;
  for (unsigned int i = 0; i < p->history.size(); i++)
  {
    if (p->history[i] == old_name)
    {
      p->history[i] = new_name;
      found = true;
      break;
    }
  }

  assert(found == true);

  roleMap[old_name] = NULL;
  roleMap[new_name] = p;
}





/*!
 * \param  my_source  the source node of the arc to be checked
 * \param  my_target  the target node of the arc to be checked
 * \result the weight of the arc
 * \pre  the arc [my_source, my_target] was found in F
 */
unsigned int PetriNet::arc_weight(Node *my_source, Node *my_target) const
{
  for (set<Arc *>::iterator f = F.begin(); f != F.end(); f++)
  {
    if (((*f)->source == my_source) && ((*f)->target == my_target))
      return (*f)->weight;
  }  

  assert(false);
  return 1;
}





/*---------------------------------------------------------------------------*/


/*!
 * \param n a node of the Petri net
 * \result the preset of node n
 * \pre n != NULL
 */
set<Node *> PetriNet::preset(Node *n) const
{
  assert(n != NULL);

  set<Node *> result;

  for (set<Arc *>::iterator f = F.begin(); f != F.end(); f++)
    if ((*f)->target == n)
      result.insert((*f)->source);

  return result;
}





/*!
 * \param n a node of the Petri net
 * \result the postset of node n
 * \pre n != NULL
 */
set<Node *> PetriNet::postset(Node *n) const
{
  assert(n != NULL);

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
  Place *result = static_cast<Place*>(roleMap[role]);

  if (result == NULL)
    trace(TRACE_DEBUG, "[PN]\tPlace with role \"" + role + "\" not found.\n");

  return result;
}





/*!
 * Finds a place of the Petri net given an id and a role.
 *
 * \param  id an identifier
 * \param  role the demanded role
 * \return a pointer to the place or a NULL pointer if the place was not found.
 */
Place *PetriNet::findPlace(unsigned int id, string role)
{
  return findPlace(toString(id) + role);
}





/*!
 * Finds a place given the identifiers of two transitions: one transition in
 * the preset of the place (id1) and one transition in the postset of the place
 * (id2). The result should be unique after structural reduction.
 *
 * \param  id1 an identifier of a transition
 * \param  id2 an identifier of a transition
 * \return a pointer to the place
 * \pre    The place exists.
 */
Place *PetriNet::findPlace(unsigned id1, unsigned id2)
{
  Transition *t1 = NULL;
  Transition *t2 = NULL;

  for (set<Transition*>::iterator t = T.begin(); t != T.end(); t++)
  {
    if ( (*t)->id == id1 )
      t1 = *t;
    if ( (*t)->id == id2 )
      t2 = *t;
  }

  assert(t1 != NULL);
  assert(t2 != NULL);
  assert(t1 != t2); // this only holds for acyclic nets

  set<Node*> temp = setIntersection(postset(t1), preset(t2));

  if (temp.size() > 1)
    cerr << "WARNING" << endl;

  Place *result = static_cast<Place*>(*(temp.begin()));

  assert(result != NULL);

  return result;
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
  Transition *result = static_cast<Transition*>(roleMap[role]);

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
 *  Adds a prefix to all nodes of the Petri net
 *
 *  \param prefix The prefix to add.
 */
void PetriNet::addPrefix(string prefix)
{
  for (set< Place * >::iterator place = P.begin(); place != P.end(); place ++)
  {
    (*place)->prefix = prefix;

    for(vector< string >::iterator name = (*place)->history.begin(); name != (*place)->history.end(); name++)
    {
      if (*place == NULL)
	roleMap[(prefix + *name)] = *place;
    }
  }

  for (set< Transition * >::iterator transition = T.begin(); transition != T.end(); transition ++)
  {
    (*transition)->prefix = prefix;

    for(vector< string >::iterator name = (*transition)->history.begin(); name != (*transition)->history.end(); name++)
      roleMap[prefix + *name] = *transition;
  }
}





/*!
 * \todo gierds: comment me!
 */
void PetriNet::connectNet(PetriNet * net)
{
  for (set< Place * >::iterator place = net->P.begin(); place != net->P.end(); place ++)
  {
    (*place)->id = getId();
    P.insert(*place);

    for(vector< string >::iterator name = (*place)->history.begin(); name != (*place)->history.end(); name++)
      roleMap[((*place)->prefix + *name)] = *place;
  }

  for (set< Transition * >::iterator transition = net->T.begin(); transition != net->T.end(); transition ++)
  {
    (*transition)->id = getId();
    T.insert(*transition);

    for(vector< string >::iterator name = (*transition)->history.begin(); name != (*transition)->history.end(); name++)
      roleMap[(*transition)->prefix + *name] = *transition;
  }

  for (set< Arc * >::iterator arc = net->F.begin(); arc != net->F.end(); arc ++)
    F.insert(*arc);

  set< Place * > additionalP_in;
  set< Place * > additionalP_out;

  for (set< Place * >::iterator place = net->P_in.begin(); place != net->P_in.end(); place ++)
  {
    (*place)->id = getId();
    for(vector< string >::iterator name = (*place)->history.begin(); name != (*place)->history.end(); name++)
    {
      roleMap[(*place)->prefix + *name] = *place;
    }

    set< Place * >::iterator oPlace = P_out.begin();
    
    bool finished = false;
    while ( ! finished && oPlace != P_out.end())
    {
        
      if ((*oPlace)->nodeName().erase(0,4) != (*place)->nodeShortName().erase(0,3))
	oPlace++;
      else
	finished = true;
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
      additionalP_in.insert(*place);
  }

  for (set< Place * >::iterator place = net->P_out.begin(); place != net->P_out.end(); place ++)
  {
    (*place)->id = getId();
    for(vector< string >::iterator name = (*place)->history.begin(); name != (*place)->history.end(); name++)
    {
      roleMap[(*place)->prefix + *name] = *place;
    }
   
    set< Place * >::iterator iPlace = P_in.begin();
    
    bool finished = false;
    while (!finished && (iPlace != P_in.end()))
    {
      if ((*iPlace)->nodeShortName().erase(0,3) != (*place)->nodeShortName().erase(0,4))
	iPlace++;
      else
	finished = true;
    }
    if (iPlace != P_in.end())
    {
      (*place)->type = INTERNAL;
      (*place)->history[0] = (*place)->nodeName();
      (*iPlace)->type = INTERNAL;
      (*iPlace)->history[0] = (*iPlace)->nodeName();
      mergePlaces(*place,*iPlace);
      P_in.erase(*iPlace);
    }
    else
      additionalP_out.insert(*place);
  }

  P_in = setUnion(P_in, additionalP_in);
  P_out = setUnion(P_out, additionalP_out); 
}





/*!
 * Converts input and output places (channels) to internal places.
 */
void PetriNet::makeChannelsInternal()
{
  // copy input and output places to the set of internal places
  for (set<Place *>::iterator place = P_in.begin(); place != P_in.end(); place ++)
    P.insert(*place);

  for (set<Place *>::iterator place = P_out.begin(); place != P_out.end(); place ++)
    P.insert(*place);

  // empty the set of input and output places
  P_in.clear(); P_in = set< Place * >();
  P_out.clear(); P_out = set< Place * >();
}





/*!
 * Re-enumerates the nodes of the Petri net to have places numbered p1, p2, ...
 * and transitions t1, t2, ... .
 */
void PetriNet::reenumerate()
{
  int currentId = 1;

  for (set<Place *>::iterator p = P.begin(); p != P.end(); p++)
    (*p)->id = currentId++;
  for (set<Place *>::iterator p = P_in.begin(); p != P_in.end(); p++)
    (*p)->id = currentId++;
  for (set<Place *>::iterator p = P_out.begin(); p != P_out.end(); p++)
    (*p)->id = currentId++;

  currentId = 1;
  for (set<Transition *>::iterator t = T.begin(); t != T.end(); t++)
    (*t)->id = currentId++;
}
