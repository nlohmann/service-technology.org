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
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          Christian Gierds <gierds@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nielslohmann $
 *
 * \since   2005-10-18
 *
 * \date    \$Date: 2006/12/14 17:35:16 $
 *
 * \note    This file is part of the tool GNU BPEL2oWFN and was created during
 *          the project Tools4BPEL at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.172 $
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

#ifdef USING_BPEL2OWFN
#include "ast-details.h"
#endif





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
 * \param my_id    the id of the transition
 * \param my_role  the first role of the transition
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
 * \param my_id   the internal id of the place
 * \param my_role the first role of the place
 * \param my_type the type of the place (as defined in #communication_type)
 */
Place::Place(unsigned int my_id, string my_role, communication_type my_type) :
  tokens(0),
  max_occurrences(0),
  isFinal(false)
{
  id = my_id;
  type = my_type;
  nodeType = PLACE;

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
 * The copy constructor with deep copy.
 */
PetriNet::PetriNet(const PetriNet & net)
{
  nextId = 1;
  format = FORMAT_OWFN;

  // add all internal places
  for (set< Place * >::iterator place = net.P.begin(); place != net.P.end(); place ++)
  {
    Place * newPlace = new Place( **place );
    newPlace->id = getId();
    P.insert( newPlace );

    roleMap[ newPlace->nodeFullName() ] = newPlace;

    for(vector< string >::iterator name = (*place)->history.begin(); name != (*place)->history.end(); name++)
      roleMap[*name] = newPlace;
  }
  // add all input places
  for (set< Place * >::iterator place = net.P_in.begin(); place != net.P_in.end(); place ++)
  {
    Place * newPlace = new Place( **place );
    newPlace->id = getId();
    P_in.insert( newPlace );

    roleMap[ newPlace->nodeFullName() ] = newPlace;

    for(vector< string >::iterator name = (*place)->history.begin(); name != (*place)->history.end(); name++)
      roleMap[*name] = newPlace;
  }
  // add all output places
  for (set< Place * >::iterator place = net.P_out.begin(); place != net.P_out.end(); place ++)
  {
    Place * newPlace = new Place( **place );
    newPlace->id = getId();
    P_out.insert( newPlace );

    roleMap[ newPlace->nodeFullName() ] = newPlace;

    for(vector< string >::iterator name = (*place)->history.begin(); name != (*place)->history.end(); name++)
      roleMap[*name] = newPlace;
  }
  // add all transitions
  for (set< Transition * >::iterator transition = net.T.begin(); transition != net.T.end(); transition ++)
  {
    Transition * newTransition = new Transition( **transition );
    newTransition->id = getId();
    T.insert( newTransition );
    
    roleMap[ newTransition->nodeFullName() ] = newTransition;

    for(vector< string >::iterator name = (*transition)->history.begin(); name != (*transition)->history.end(); name++)
      roleMap[ *name ] = newTransition;
  }
  // create arcs according to the given "net"
  for (set< Arc * >::iterator arc = net.F.begin(); arc != net.F.end(); arc ++)
  {
    if ( (*arc)->source->nodeType == PLACE)
    {
      newArc( findPlace( (*arc)->source->nodeFullName() ), findTransition( (*arc)->target->nodeFullName() ), STANDARD, (*arc)->weight );
    }
    else
    {
      newArc( findTransition( (*arc)->source->nodeFullName() ), findPlace( (*arc)->target->nodeFullName() ), STANDARD, (*arc)->weight );
    }
  }
}





/*!
 * The "=" operator.
 */
PetriNet & PetriNet::operator=(const PetriNet & net)
{
  // cleaning up old net
  for (set<Place *>::iterator p = P.begin(); p != P.end(); p++)
    delete *p;

  for (set<Place *>::iterator p = P_in.begin(); p != P_in.end(); p++)
    delete *p;
  
  for (set<Place *>::iterator p = P_out.begin(); p != P_out.end(); p++)
    delete *p;

  for (set<Transition *>::iterator t = T.begin(); t != T.end(); t++)
    delete *t;

  for (set<Arc *>::iterator f = F.begin(); f != F.end(); f++)
    delete *f;

  P.clear();
  P_in.clear();
  P_out.clear();
  T.clear();
  F.clear();
  roleMap.clear();

  nextId = 1;
  format = FORMAT_OWFN;

  // add all internal places
  for (set< Place * >::iterator place = net.P.begin(); place != net.P.end(); place ++)
  {
    Place * newPlace = new Place( **place );
    newPlace->id = getId();
    P.insert( newPlace );

    roleMap[ newPlace->nodeFullName() ] = newPlace;

    for(vector< string >::iterator name = (*place)->history.begin(); name != (*place)->history.end(); name++)
      roleMap[*name] = newPlace;
  }
  // add all input places
  for (set< Place * >::iterator place = net.P_in.begin(); place != net.P_in.end(); place ++)
  {
    Place * newPlace = new Place( **place );
    newPlace->id = getId();
    P_in.insert( newPlace );

    roleMap[ newPlace->nodeFullName() ] = newPlace;

    for(vector< string >::iterator name = (*place)->history.begin(); name != (*place)->history.end(); name++)
      roleMap[*name] = newPlace;
  }
  // add all output places
  for (set< Place * >::iterator place = net.P_out.begin(); place != net.P_out.end(); place ++)
  {
    Place * newPlace = new Place( **place );
    newPlace->id = getId();
    P_out.insert( newPlace );

    roleMap[ newPlace->nodeFullName() ] = newPlace;

    for(vector< string >::iterator name = (*place)->history.begin(); name != (*place)->history.end(); name++)
      roleMap[*name] = newPlace;
  }
  // add all transitions
  for (set< Transition * >::iterator transition = net.T.begin(); transition != net.T.end(); transition ++)
  {
    Transition * newTransition = new Transition( **transition );
    newTransition->id = getId();
    T.insert( newTransition );
    
    roleMap[ newTransition->nodeFullName() ] = newTransition;

    for(vector< string >::iterator name = (*transition)->history.begin(); name != (*transition)->history.end(); name++)
      roleMap[ *name ] = newTransition;
  }
  // create arcs according to the given "net"
  for (set< Arc * >::iterator arc = net.F.begin(); arc != net.F.end(); arc ++)
  {
    if ( (*arc)->source->nodeType == PLACE)
    {
      newArc( findPlace( (*arc)->source->nodeFullName() ), findTransition( (*arc)->target->nodeFullName() ), STANDARD, (*arc)->weight );
    }
    else
    {
      newArc( findTransition( (*arc)->source->nodeFullName() ), findPlace( (*arc)->target->nodeFullName() ), STANDARD, (*arc)->weight );
    }
  }
  return *this;
}





/*!
 * The destructor of the PetriNet class.
 */
PetriNet::~PetriNet()
{

  for (set<Place *>::iterator p = P.begin(); p != P.end(); p++)
    delete *p;

  for (set<Place *>::iterator p = P_in.begin(); p != P_in.end(); p++)
    delete *p;
  
  for (set<Place *>::iterator p = P_out.begin(); p != P_out.end(); p++)
    delete *p;

  for (set<Transition *>::iterator t = T.begin(); t != T.end(); t++)
    delete *t;

  for (set<Arc *>::iterator f = F.begin(); f != F.end(); f++)
    delete *f;
}





/*!
 * \param my_role  the initial role of the place
 * \param my_type  the type of the place (as defined in #communication_type)
 * \return  pointer of the created place
 * \pre No place with the given role is defined.
 */
Place *PetriNet::newPlace(string my_role, communication_type my_type)
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
 * \param my_role  the initial role of the transition
 * \return pointer of the created transition
 * \pre No transition with the given role is defined.
 */
Transition *PetriNet::newTransition(string my_role)
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
 * \param my_source   source node of the arc
 * \param my_target   target node of the arc
 * \param my_type     type of the arc (as defined in #arc_type)
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

  p12->tokens = max(p1->tokens, p2->tokens); 
  p12->isFinal = (p1->isFinal || p2->isFinal);

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
 *
 * \todo change "roleMap[old_name] = NULL;" to something with delete?
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
    if (((*f)->source == my_source) && ((*f)->target == my_target))
      return (*f)->weight;

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
 *
 * \todo make this function "const"
 */
Place *PetriNet::findPlace(string role)
{
  return static_cast<Place*>(roleMap[role]);
}





/*!
 * Finds a place of the Petri net given an id and a role.
 *
 * \param  id an identifier
 * \param  role the demanded role
 * \return a pointer to the place or a NULL pointer if the place was not found.
 *
 * \todo make this function "const"
 */
Place *PetriNet::findPlace(unsigned int id, string role)
{
  return static_cast<Place*>(roleMap[toString(id) + role]);
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
  return static_cast<Transition*>(roleMap[role]);
}

/*!
 * Returns a set of all final places
 *
 * \return the set with all final places
 */
set< Place * > PetriNet::getFinalPlaces()
{
  set< Place * > result;

  for ( set< Place * >::iterator place = P.begin(); place != P.end(); place++ )
  {
    if ( (*place)->isFinal )
    {
      result.insert( *place );
    }
  }

  return result;
}




/*---------------------------------------------------------------------------*/


/*!
 * \return id for new nodes
 * \post nextId is incremented
 */
unsigned int PetriNet::getId()
{
  return nextId++;
}





/*!
 * Adds a prefix to all nodes of the Petri net
 *
 * \param prefix The prefix to add.
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
  for (set< Place * >::iterator place = P_in.begin(); place != P_in.end(); place ++)
  {
    (*place)->prefix = prefix;

    for(vector< string >::iterator name = (*place)->history.begin(); name != (*place)->history.end(); name++)
    {
      if (*place == NULL)
	roleMap[(prefix + *name)] = *place;
    }
  }
  for (set< Place * >::iterator place = P_out.begin(); place != P_out.end(); place ++)
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
void PetriNet::compose(PetriNet &net)
{
  // add all internal places
  for (set< Place * >::iterator place = net.P.begin(); place != net.P.end(); place ++)
  {
    Place * newPlace = new Place( **place );
    newPlace->id = getId();
    P.insert( newPlace );

    roleMap[ newPlace->nodeFullName() ] = newPlace;

    for(vector< string >::iterator name = (*place)->history.begin(); name != (*place)->history.end(); name++)
      roleMap[(newPlace->prefix + *name)] = newPlace;
  }
  // add all input places
  for (set< Place * >::iterator place = net.P_in.begin(); place != net.P_in.end(); place ++)
  {
    Place * newPlace = new Place( **place );
    newPlace->id = getId();
    P_in.insert( newPlace );

    roleMap[ newPlace->nodeFullName() ] = newPlace;

    for(vector< string >::iterator name = (*place)->history.begin(); name != (*place)->history.end(); name++)
      roleMap[(newPlace->prefix + *name)] = newPlace;
  }
  // add all output places
  for (set< Place * >::iterator place = net.P_out.begin(); place != net.P_out.end(); place ++)
  {
    Place * newPlace = new Place( **place );
    newPlace->id = getId();
    P_out.insert( newPlace );

    roleMap[ newPlace->nodeFullName() ] = newPlace;

    for(vector< string >::iterator name = (*place)->history.begin(); name != (*place)->history.end(); name++)
      roleMap[(newPlace->prefix + *name)] = newPlace;
  }
  // add all transitions
  for (set< Transition * >::iterator transition = net.T.begin(); transition != net.T.end(); transition ++)
  {
    Transition * newTransition = new Transition( **transition );
    newTransition->id = getId();
    T.insert( newTransition );
    
    roleMap[ newTransition->nodeFullName() ] = newTransition;

    for(vector< string >::iterator name = (*transition)->history.begin(); name != (*transition)->history.end(); name++)
      roleMap[ newTransition->prefix + *name ] = newTransition;
  }
  // create arcs according to the given "net"
  for (set< Arc * >::iterator arc = net.F.begin(); arc != net.F.end(); arc ++)
  {
    if ( (*arc)->source->nodeType == PLACE)
    {
      newArc( findPlace( (*arc)->source->nodeFullName() ), findTransition( (*arc)->target->nodeFullName() ), STANDARD, (*arc)->weight );
    }
    else
    {
      newArc( findTransition( (*arc)->source->nodeFullName() ), findPlace( (*arc)->target->nodeFullName() ), STANDARD, (*arc)->weight );
    }
  }

  set< Place * > eraseP_in;
  set< Place * > additionalP_out;

  // merge appropreate input and output places (same name, different prefixes)
  for (set< Place * >::iterator place = P_in.begin(); place != P_in.end(); place ++)
  {
    set< Place * >::iterator oPlace = P_out.begin();
    
    bool finished = false;
    while ( ! finished && oPlace != P_out.end())
    {
        
      if ((*oPlace)->nodeFullName().erase(0,4) != (*place)->nodeFullName().erase(0,3))
	oPlace++;
      else
	finished = true;
    }

    if (oPlace != P_out.end())
    {
      if ( (*place)->prefix != (*oPlace)->prefix )
      {
        (*place)->type = INTERNAL;
        (*place)->history[0] = (*place)->nodeFullName();
        (*oPlace)->type = INTERNAL;
        (*oPlace)->history[0] = (*oPlace)->nodeFullName();
        P.insert(*place);
        P.insert(*oPlace);
        mergePlaces(*place,*oPlace);
        eraseP_in.insert(*place);
        P_out.erase(*oPlace);
      }
    }
  }
  // erase input places, that were merged
  for (set< Place * >::iterator place = eraseP_in.begin(); place != eraseP_in.end(); place ++)
  {
    P_in.erase( *place );
  }

  // mark transitions as internal that are no longer communicating
  for (set< Transition * >::iterator transition = T.begin(); transition != T.end(); transition ++)
  {
    if ( (*transition)->type != INTERNAL )
    {
      set< Node * > prePost = setUnion( preset( findTransition( (*transition)->nodeFullName() ) ), postset( findTransition( (*transition)->nodeFullName() ) ) );
      bool isCommunicating = false;
      for ( set< Node * >::iterator place = prePost.begin(); place != prePost.end(); place++ )
      {
        if ( (*place)->type != INTERNAL )
        {
          isCommunicating = true;
        }
      }
      if ( ! isCommunicating )
      {
        (*transition)->type = INTERNAL;
      }
    }
  }
}





/*!
 * Converts input and output places (channels) to internal places.
 *
 * \todo What means "P_in.clear(); P_in = set< Place * >();"?
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





/*!
 * Calculates the maximal number each communication place can be used, i.e. the
 * maximal occurrences of transitions in the preset (postset) of output (input)
 * places. This number is determined by postprocessing the AST.
 *
 * \todo use information from the postprocessing/CFG to improved (i.e.
 *       minimize) the calculated values
 *
 * \post All communication places have max_occurrences values between 0
 *       (initial value) and UINT_MAX (maximal value).
 */
void PetriNet::calculate_max_occurrences()
{
#ifdef USING_BPEL2OWFN
  extern map<unsigned int, ASTE*> ASTEmap;

  // process the input places
  for (set<Place *>::iterator p = P_in.begin(); p != P_in.end(); p++)
  {
    set<Node *> receiving_transitions = postset(*p);
    set<unsigned int> receiving_activities;

    for (set<Node *>::iterator t = receiving_transitions.begin(); t != receiving_transitions.end(); t++)
      for (unsigned int i = 0; i < (*t)->history.size(); i++)
      {
	unsigned int transition_activity_id = toUInt((*t)->history[i].substr(0, (*t)->history[i].find_first_of(".")));
	assert(ASTEmap[transition_activity_id] != NULL);
	receiving_activities.insert(transition_activity_id);
      }

    for (set<unsigned int>::iterator activity_id = receiving_activities.begin(); activity_id != receiving_activities.end(); activity_id++)
    {
      if (ASTEmap[*activity_id]->max_occurrences == UINT_MAX)
	(*p)->max_occurrences = UINT_MAX;
      else if ((*p)->max_occurrences != UINT_MAX)
      {
	if (ASTEmap[*activity_id]->activityTypeName() == "repeatUntil" ||
	    ASTEmap[*activity_id]->activityTypeName() == "while")
	  (*p)->max_occurrences += ASTEmap[*activity_id]->max_loops;
	else
	  (*p)->max_occurrences += ASTEmap[*activity_id]->max_occurrences;
      }
    }
  }

  // process the output places
  for (set<Place *>::iterator p = P_out.begin(); p != P_out.end(); p++)
  {
    set<Node *> sending_transitions = preset(*p);
    set<unsigned int> sending_activities;

    for (set<Node *>::iterator t = sending_transitions.begin(); t != sending_transitions.end(); t++)
      for (unsigned int i = 0; i < (*t)->history.size(); i++)
      {
	unsigned int transition_activity_id = toUInt((*t)->history[i].substr(0, (*t)->history[i].find_first_of(".")));
	assert(ASTEmap[transition_activity_id] != NULL);
	sending_activities.insert(transition_activity_id);
      }

    for (set<unsigned int>::iterator activity_id = sending_activities.begin(); activity_id != sending_activities.end(); activity_id++)
    {
      if (ASTEmap[*activity_id]->max_occurrences == UINT_MAX)
	(*p)->max_occurrences = UINT_MAX;
      else if ((*p)->max_occurrences != UINT_MAX)
	(*p)->max_occurrences += ASTEmap[*activity_id]->max_occurrences;
    }
  }
#endif
}
