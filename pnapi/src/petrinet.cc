/*****************************************************************************\
  GNU BPEL2oWFN -- Translating BPEL Processes into Petri Net Models

  Copyright (C) 2006, 2007  Niels Lohmann,
                            Christian Gierds, and
                            Martin Znamirowski
  Copyright (C) 2005        Niels Lohmann and
			    Christian Gierds

  GNU BPEL2oWFN is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the Free
  Software Foundation; either version 3 of the License, or (at your option) any
  later version.

  GNU BPEL2oWFN is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
  details.

  You should have received a copy of the GNU General Public License along with
  GNU BPEL2oWFN (see file COPYING); if not, see http://www.gnu.org/licenses
  or write to the Free Software Foundation,Inc., 51 Franklin Street, Fifth
  Floor, Boston, MA 02110-1301  USA.
\*****************************************************************************/

/*!
 * \file    petrinet.cc
 *
 * \brief   Petri Net API: base functions
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          Christian Gierds <gierds@informatik.hu-berlin.de>,
 *          Martin Znamirowski <znamirow@informatik.hu-berlin.de>,
 *          last changes of: \$Author: gierds $
 *
 * \since   2005-10-18
 *
 * \date    \$Date: 2008-06-19 11:42:07 $
 *
 * \note    This file is part of the tool GNU BPEL2oWFN and was created during
 *          the project Tools4BPEL at the Humboldt-Universit�t zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.7 $
 *
 * \ingroup petrinet
 */





/******************************************************************************
 * Headers
 *****************************************************************************/

#include <cassert>
#include <iostream>
#include <utility>
#include <algorithm>

#include "petrinet.h"
#include "helpers.h"		// helper functions (setUnion, setDifference, max, toString)
#include "pnapi.h"

using std::pair;
using std::cerr;
using std::endl;
using std::min;


namespace PNapi
{


/******************************************************************************
 * Implementation of class functions
 *****************************************************************************/

/*!
 * \brief   true if first role contains role
 *
 * \param   role  a role of a node
 * \return  true, if the node's first history entry contains the given role
 */
bool Node::firstMemberAs(string role) const
{
  string firstEntry = (*history.begin());
  return (firstEntry.find(role, 0) == firstEntry.find_first_of(".") + 1);
}





/*!
 * \brief   true if first role begins with role
 *
 * \param   role  a role of a node
 * \return  true, if the node's first history entry begins with the given role
 */
bool Node::firstMemberIs(string role) const
{
  string firstEntry = (*history.begin());
  return (firstEntry.find(role, 0) == 0);
}





/*!
 * \brief   true if history contains role
 *
 * \param   role  a role of a node
 * \return  true, if the node's history contains the given role
 */
bool Node::historyContains(string role) const
{
  for (list<string>::const_iterator iter = history.begin(); iter != history.end(); iter++)
  {
    if ( *iter == role )
    {
      return true;
    }
  }
  return false;
}





/*!
 * \brief  destructor
 */
Node::~Node()
{
}





/*****************************************************************************/


/*!
 * \brief   create a transition and add a first role to the history
 *
 * \param   my_id    the id of the transition
 * \param   my_role  the first role of the transition
 */
Transition::Transition(unsigned int my_id, string my_role)
{
  id = my_id;
  nodeType = TRANSITION;
  type = INTERNAL;

  max_occurrences = 0;

  if (my_role != "")
    history.push_back(my_role);
}





/*!
 * \brief   destructor
 */
Transition::~Transition()
{
}





/*!
 * \brief   add a label to the transition (used for constraint oWFN)
 *
 * \param   new_label  a label to be added to the transition
 */
void Transition::add_label(string new_label)
{
  labels.insert(new_label);
}




/*!
 * \brief   checks if the transition is normal
 *
 * \note    This is a help method for normalize method
 *          in class PetriNet.
 */
bool Transition::isNormal() const
{
  int counter = 0;

  for (set<Node *>::const_iterator p = preset.begin(); p != preset.end(); p++)
    if ((*p)->type != INTERNAL)
      counter++;
  for (set<Node *>::const_iterator p = postset.begin(); p != postset.end(); p++)
    if ((*p)->type != INTERNAL)
      counter++;

  return counter <= 1;
}




/*****************************************************************************/


/*!
 * \brief   create a place and add a first role to the history
 *
 * \param   my_id    the internal id of the place
 * \param   my_role  the first role of the place
 * \param   my_type  the type of the place (as defined in #communication_type)
 */
Place::Place(unsigned int my_id, string my_role, communication_type my_type) :
  tokens(0),
  isFinal(false),
  capacity(0)
{
  id = my_id;
  type = my_type;
  nodeType = PLACE;

  max_occurrences = 0;

  if (my_role != "")
    history.push_back(my_role);
}





/*!
 * \brief   mark the place
 *
 * \param   my_tokens  the number of tokens of this place in the inital
 *                     marking
 */
void Place::mark(unsigned int my_tokens)
{
  tokens = my_tokens;
}




/*!
 * \brief   returns the marking ID of the place
 *
 * \return  unsigned int value for marking_id
 */
unsigned int Place::getMarkingID()
{
  return marking_id;
}




/*!
 * \brief   destructor
 */
Place::~Place()
{
}





/*****************************************************************************/


/*!
 * \brief   create an arc with a given weight
 *
 * \param   my_source  the source-node of the arc
 * \param   my_target  the target-node of the arc
 * \param   my_weight  the weight of the arc (standard: 1)
 *
 * \pre     my_source != NULL
 * \pre     my_target != NULL
 */
Arc::Arc(Node *my_source, Node *my_target, unsigned int my_weight) :
  source(my_source),
  target(my_target),
  weight(my_weight)
{
  assert(my_source != NULL);
  assert(my_target != NULL);

  my_source->postset.insert(my_target);
  my_target->preset.insert(my_source);
}





/*!
 * \brief   destructor
 */
Arc::~Arc()
{
  assert(source != NULL);
  assert(target != NULL);

  source->postset.erase(target);
  target->preset.erase(source);
}





/*!
 * \brief   swaps source and target node of the arc
 *
 * \test    Tested in tests/test_mirror.cc
 */
void Arc::mirror()
{
  Node *old_source = source;
  Node *old_target = target;

  source = old_target;
  target = old_source;

  assert(target != NULL);
  assert(source != NULL);
  source->postset.erase(old_target);
  source->postset.insert(old_source);
  target->preset.erase(old_source);
  target->preset.insert(old_target);
}





/*****************************************************************************/


/*!
 * \brief   constructor
 */
PetriNet::PetriNet() :
  nextId(1),
  format(FORMAT_OWFN),
  invocation_string("PN API"),
  package_string("Petri Net API")
{
}





/*!
 * \brief   copy constructor
 *
 *          The copy constructor with deep copy.
 */
PetriNet::PetriNet(const PetriNet & net)
{
  // cleaning up old net
  for (set<Arc *>::iterator f = F.begin(); f != F.end(); f++)
    delete *f;

  for (set<Place *>::iterator p = P.begin(); p != P.end(); p++)
    delete *p;

  for (set<Place *>::iterator p = P_in.begin(); p != P_in.end(); p++)
    delete *p;

  for (set<Place *>::iterator p = P_out.begin(); p != P_out.end(); p++)
    delete *p;

  for (set<Transition *>::iterator t = T.begin(); t != T.end(); t++)
    delete *t;

  P.clear();
  P_in.clear();
  P_out.clear();
  T.clear();
  F.clear();
  roleMap.clear();
  ports.clear();
  final_set_list.clear();


  nextId = net.nextId;
  format = FORMAT_OWFN;
  invocation_string = net.invocation_string;
  package_string = net.package_string;


  // transfer the the final place set list
  //for (list<set<pair<Place *,unsigned int> > >::const_iterator final_set = net.final_set_list.begin(); final_set != net.final_set_list.end(); final_set++)
  //{
  //  final_set_list.push_back(*final_set);
  //}

  // add all internal places
  for (set< Place * >::iterator place = net.P.begin(); place != net.P.end(); place ++)
  {
    Place * newPlace = new Place( **place );
    newPlace->id = (*place)->id;
    newPlace->preset.clear();
    newPlace->postset.clear();
    P.insert( newPlace );

    roleMap[ newPlace->nodeFullName() ] = newPlace;

    for(list< string >::iterator name = (*place)->history.begin(); name != (*place)->history.end(); name++)
      roleMap[*name] = newPlace;
  }
  // add all input places
  for (set< Place * >::iterator place = net.P_in.begin(); place != net.P_in.end(); place ++)
  {
    Place * newPlace = new Place( **place );
    newPlace->id = (*place)->id;
    newPlace->preset.clear();
    newPlace->postset.clear();
    P_in.insert( newPlace );

    roleMap[ newPlace->nodeFullName() ] = newPlace;

    for(list< string >::iterator name = (*place)->history.begin(); name != (*place)->history.end(); name++)
      roleMap[*name] = newPlace;
  }
  // add all output places
  for (set< Place * >::iterator place = net.P_out.begin(); place != net.P_out.end(); place ++)
  {
    Place * newPlace = new Place( **place );
    newPlace->id = (*place)->id;
    newPlace->preset.clear();
    newPlace->postset.clear();
    P_out.insert( newPlace );

    roleMap[ newPlace->nodeFullName() ] = newPlace;

    for(list< string >::iterator name = (*place)->history.begin(); name != (*place)->history.end(); name++)
      roleMap[*name] = newPlace;
  }
  // recalculate final markings:
  {
    list<set<pair<Place *, unsigned int> > > netFinalList;

    for (list<set<pair<Place *, unsigned int> > >::const_iterator netFinal = net.final_set_list.begin(); netFinal != net.final_set_list.end(); netFinal++)
    {
      set<pair<Place *, unsigned int> > finalMarking;
      for (set<pair<Place *, unsigned int> >::const_iterator p = netFinal->begin(); p != netFinal->end(); p++)
      {
        finalMarking.insert(pair<Place *, unsigned int>(findPlace((p->first)->nodeFullName()), p->second));
      }
      if (! finalMarking.empty())
        netFinalList.push_back(finalMarking);
    }
    final_set_list = netFinalList;
  }
  // add all transitions
  for (set< Transition * >::iterator transition = net.T.begin(); transition != net.T.end(); transition ++)
  {
    Transition * newTransition = new Transition( **transition );
    newTransition->id = (*transition)->id;
    newTransition->preset.clear();
    newTransition->postset.clear();
    T.insert( newTransition );

    roleMap[ newTransition->nodeFullName() ] = newTransition;

    for(list< string >::iterator name = (*transition)->history.begin(); name != (*transition)->history.end(); name++)
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
 * \brief   assignment operator
 *
 *          The "=" operator.
 */
PetriNet & PetriNet::operator=(const PetriNet & net)
{
  // cleaning up old net
  for (set<Arc *>::iterator f = F.begin(); f != F.end(); f++)
    delete *f;

  for (set<Place *>::iterator p = P.begin(); p != P.end(); p++)
    delete *p;

  for (set<Place *>::iterator p = P_in.begin(); p != P_in.end(); p++)
    delete *p;

  for (set<Place *>::iterator p = P_out.begin(); p != P_out.end(); p++)
    delete *p;

  for (set<Transition *>::iterator t = T.begin(); t != T.end(); t++)
    delete *t;

  P.clear();
  P_in.clear();
  P_out.clear();
  T.clear();
  F.clear();
  roleMap.clear();
  ports.clear();
  final_set_list.clear();

  nextId = net.nextId;
  format = FORMAT_OWFN;
  invocation_string = net.invocation_string;
  package_string = net.package_string;

  // add all internal places
  for (set< Place * >::iterator place = net.P.begin(); place != net.P.end(); place ++)
  {
    Place * newPlace = new Place( **place );
    newPlace->id = (*place)->id;
    newPlace->preset.clear();
    newPlace->postset.clear();
    P.insert( newPlace );

    roleMap[ newPlace->nodeFullName() ] = newPlace;

    for(list< string >::iterator name = (*place)->history.begin(); name != (*place)->history.end(); name++)
      roleMap[*name] = newPlace;
  }
  // add all input places
  for (set< Place * >::iterator place = net.P_in.begin(); place != net.P_in.end(); place ++)
  {
    Place * newPlace = new Place( **place );
    newPlace->id = (*place)->id;
    newPlace->preset.clear();
    newPlace->postset.clear();
    P_in.insert( newPlace );

    roleMap[ newPlace->nodeFullName() ] = newPlace;

    for(list< string >::iterator name = (*place)->history.begin(); name != (*place)->history.end(); name++)
      roleMap[*name] = newPlace;
  }
  // add all output places
  for (set< Place * >::iterator place = net.P_out.begin(); place != net.P_out.end(); place ++)
  {
    Place * newPlace = new Place( **place );
    newPlace->id = (*place)->id;
    newPlace->preset.clear();
    newPlace->postset.clear();
    P_out.insert( newPlace );

    roleMap[ newPlace->nodeFullName() ] = newPlace;

    for(list< string >::iterator name = (*place)->history.begin(); name != (*place)->history.end(); name++)
      roleMap[*name] = newPlace;
  }
  // recalculate final markings:
  {
    list<set<pair<Place *, unsigned int> > > netFinalList;

    for (list<set<pair<Place *, unsigned int> > >::const_iterator netFinal = net.final_set_list.begin(); netFinal != net.final_set_list.end(); netFinal++)
    {
      set<pair<Place *, unsigned int> > finalMarking;
      for (set<pair<Place *, unsigned int> >::const_iterator p = netFinal->begin(); p != netFinal->end(); p++)
      {
        finalMarking.insert(pair<Place *, unsigned int>(findPlace((p->first)->nodeFullName()), p->second));
      }
      if (! finalMarking.empty())
        netFinalList.push_back(finalMarking);
    }
    final_set_list = netFinalList;
  }
  // add all transitions
  for (set< Transition * >::iterator transition = net.T.begin(); transition != net.T.end(); transition ++)
  {
    Transition * newTransition = new Transition( **transition );
    newTransition->id = (*transition)->id;
    newTransition->preset.clear();
    newTransition->postset.clear();
    T.insert( newTransition );

    roleMap[ newTransition->nodeFullName() ] = newTransition;

    for(list< string >::iterator name = (*transition)->history.begin(); name != (*transition)->history.end(); name++)
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
 * \brief   destructor
 *
 *          The destructor of the PetriNet class.
 */
PetriNet::~PetriNet()
{
  for (set<Arc *>::iterator f = F.begin(); f != F.end(); f++)
    delete *f;

  for (set<Place *>::iterator p = P.begin(); p != P.end(); p++)
    delete *p;

  for (set<Place *>::iterator p = P_in.begin(); p != P_in.end(); p++)
    delete *p;

  for (set<Place *>::iterator p = P_out.begin(); p != P_out.end(); p++)
    delete *p;

  for (set<Transition *>::iterator t = T.begin(); t != T.end(); t++)
    delete *t;
}





/*!
 * \brief   adds a place with a given role and type
 *
 * \param   my_role  the initial role of the place
 * \param   my_type  the type of the place (as defined in #communication_type)
 * \param   my_port  the port to which this place belongs
 * \return  pointer of the created place
 *
 * \pre     No place with the given role is defined.
 */
Place *PetriNet::newPlace(string my_role, communication_type my_type, unsigned int capacity, string my_port)
{
  string my_role_with_suffix = my_role;

  if (my_role != "" && !forEach_suffix.empty())
    my_role_with_suffix += ("." + forEach_suffix[0]);

  Place *p = new Place(getId(), my_role_with_suffix, my_type);
  assert(p != NULL);
  p->capacity = capacity;

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
    assert(roleMap[my_role_with_suffix] == NULL);
    roleMap[my_role_with_suffix] = p;
  }

  if (my_type != INTERNAL && my_port != "")
  {
    ports[my_port].insert(p);
  }

  return p;
}





/*---------------------------------------------------------------------------*/


/*!
 * \brief   adds a transition with a given role
 *
 * \param   my_role  the initial role of the transition
 * \return  pointer of the created transition
 *
 * \pre     No transition with the given role is defined.
 */
Transition *PetriNet::newTransition(string my_role)
{
  string my_role_with_suffix = my_role;

  if (my_role != "" && !forEach_suffix.empty())
    my_role_with_suffix += ("." + forEach_suffix[0]);

  Transition *t = new Transition(getId(), my_role_with_suffix);
  assert(t != NULL);

  T.insert(t);

  // Test if the transition is already defined.
  if (my_role != "")
  {
    assert(roleMap[my_role_with_suffix] == NULL);
    roleMap[my_role_with_suffix] = t;
  }

  return t;
}





/*---------------------------------------------------------------------------*/


/*!
 * \brief   adds an arc given source and target node, and arc type
 *
 * \param   my_source  source node of the arc
 * \param   my_target  target node of the arc
 * \param   my_type    type of the arc (as defined in #arc_type)
 * \param   my_weight  weight of the arc (standard: 1)
 *
 * \return  pointer of the created arc
 *
 * \pre     The types of the nodes have to match, i.e. an arc can only be
 *          created between a place and a transition or a transition and a
 *          place, resp.
 * \pre     my_target != NULL
 * \pre     my_source != NULL
 */
Arc *PetriNet::newArc(Node *my_source, Node *my_target, arc_type my_type, unsigned int my_weight)
{
  assert(my_source != NULL);
  assert(my_target != NULL);

  assert(my_source->nodeType != my_target->nodeType);

  // Tag the involved transition as communicating if it is.
  if (my_source->nodeType == PLACE)
    if (P_in.find(static_cast<Place*>(my_source)) != P_in.end())
    {
      if ((static_cast<Transition*>(my_target))->type == OUT || (static_cast<Transition*>(my_target))->type == INOUT)
        (static_cast<Transition*>(my_target))->type = INOUT;
      else
        (static_cast<Transition*>(my_target))->type = IN;
    }

  if (my_target->nodeType == PLACE)
    if (P_out.find(static_cast<Place*>(my_target)) != P_out.end())
    {
      if ((static_cast<Transition*>(my_source))->type == IN || (static_cast<Transition*>(my_source))->type == INOUT)
        (static_cast<Transition*>(my_source))->type = INOUT;
      else
        (static_cast<Transition*>(my_source))->type = OUT;
    }


  // Finally add the arc to the Petri net.
  Arc *f = new Arc(my_source, my_target, my_weight);
  assert(f != NULL);
  F.insert(f);

  weight[ pair<Node*, Node*>(my_source, my_target) ] = my_weight;

  // Add a second arc to close a loop if the arc is a read arc.
  if (my_type == READ)
  {
    Arc *f2 = new Arc(my_target, my_source, my_weight);
    assert(f2 != NULL);
    F.insert(f2);
    weight[ pair<Node*, Node*>(my_target, my_source) ] = my_weight;
  }

  return f;
}





/*---------------------------------------------------------------------------*/

/*!
 * \brief   removes all ingoing and outgoing arcs of a node
 *
 *          Removes all ingoing and outgoing arcs of a node, i.e. detatches
 *          the node from the rest of the net.
 *
 * \param   n  node to be detached
 *
 * \pre     n != NULL
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
 * \brief   removes a place from the net
 *
 * \param   p  place to be removed
 *
 * \pre     p != NULL
 *
 * \post	p == NULL
 */
void PetriNet::removePlace(Place *p)
{
  if (p == NULL)
    return;

  // set of transitions in p's neighborhood
  set<Node *> tset;

  switch (p->type)
  {
    case IN:  { tset = p->postset; break; }
    case OUT: { tset = p->preset;  break; }
    default:  { tset.clear();      break; }
  }

  detachNode(p);

  // Remove the roles of the place p, i.e. set the mappings to the NULL
  // pointer.
  for (list<string>::iterator role = p->history.begin(); role != p->history.end(); role++)
  {
    if (roleMap[*role] == p)
    {
      roleMap[*role] = NULL;
    }
  }


  // Decide from which set of places the place has to be removed.
  switch(p->type)
  {
    case(IN):  { P_in.erase(p);  break; }
    case(OUT): { P_out.erase(p); break; }
    default:   { P.erase(p);     break; }
  }

  // reevaluate type of neighbor transitions if necessary
  if (p->type != INTERNAL)
    for (set<Node *>::iterator t = tset.begin(); t != tset.end(); t++)
      reevaluateType(static_cast<Transition *>(*t));

  delete p;
  p = NULL;
}




/*!
 * \brief   Reevaluates the type of transition t
 *
 *          When removing an interface place, the connected
 *          transitions have to get new or same type because
 *          they might be connected to another interface place
 *          or not. In the second case the transition must be
 *          of internal type after the deletion.
 */
void PetriNet::reevaluateType(Transition *t)
{
  if (t->type != INTERNAL)
  {
    set<Node *> pset;

    switch (t->type)
    {
      case IN:
      {
        pset = t->preset;
        t->type = INTERNAL;
        for (set<Node *>::iterator p = pset.begin(); p != pset.end(); p++)
          if ((*p)->type == IN)
          {
            t->type = (*p)->type;
            break;
          }
        break;
      }
      case OUT:
      {
        pset = t->postset;
        t->type = INTERNAL;
        for (set<Node *>::iterator p = pset.begin(); p != pset.end(); p++)
          if ((*p)->type == OUT)
          {
            t->type = (*p)->type;
            break;
          }
        break;
      }
      case INOUT:
      {
        pset = t->preset;
        t->type = INTERNAL;
        for (set<Node *>::iterator p = pset.begin(); p != pset.end(); p++)
          if ((*p)->type == IN)
          {
            t->type = (*p)->type;
            break;
          }

        pset = t->postset;
        for (set<Node *>::iterator p = pset.begin(); p != pset.end(); p++)
          if ((*p)->type == OUT)
          {
            if (t->type == INTERNAL)
              t->type = (*p)->type;
            else
              t->type = INOUT;
            break;
          }
        break;
      }
      default: break;
    }
  }
}





/*!
 * \brief   removes a transition from the net
 *
 * \param   t  transition to be removed
 *
 * \pre     t != NULL
 *
 * \post	t == NULL
 *
 * \test	This method has been tested.
 */
void PetriNet::removeTransition(Transition *t)
{
  if (t == NULL)
    return;

  detachNode(t);

  // Remove the roles of the transition  t, i.e. set the mappings to the NULL
  // pointer.
  for (list<string>::iterator role = t->history.begin(); role != t->history.end(); role++)
    if (roleMap[*role] == t)
      roleMap[*role] = NULL;

  T.erase(t);
  delete t;
  t = NULL;
}





/*!
 * \brief   removes an arc from the net
 *
 * \param   f  arc to be removed
 *
 * \pre     f != NULL
 *
 * \post	f == NULL
 */
void PetriNet::removeArc(Arc *f)
{
  if (f == NULL)
    return;

  weight[ pair<Node*, Node*>(f->source, f->target) ] = 0;

  F.erase(f);

  delete f;
  f = NULL;
}





/*---------------------------------------------------------------------------*/


/*!
 * \brief   merges two sequential transitions
 *
 *          Merges two transitions. Given transitions t1 and t2:
 *          -# a new transition t12 with empty history is created
 *          -# this transition gets the union of the histories of transition
 *             t1 and t2
 *          -# the presets and postsets of t1 and t2 are calculated and united
 *          -# t12 is connected with all the places in the preset and postset
 *          -# the transitions t1 and t2 are removed
 *
 * \param   t1  first transition
 * \param   t2  second transition
 *
 * \pre     t1 != NULL
 * \pre     t2 != NULL
 * \post    Transitions t1 and t2 removed.
 * \post    Transition t12 having the incoming and outgoing arcs of t1 and t2
 *          and the union of the histories of t1 and t2.
 *
 * \todo    overwork the weight setting part according to the appropriate one
 *          of PetriNet::mergePlaces()
 *
 * \test	This method has been tested in tests/test_mergeTransitions.cc.
 */
void PetriNet::mergeTransitions(Transition *t1, Transition *t2)
{
  if (t1 == t2)
    return;

  assert(t1 != NULL);
  assert(t2 != NULL);

  // variable needed for correct arc weights
  bool sametarget = false;
  // container for arcs to be deleted
  set<Arc *>::iterator delArc;
  // the merged transition
  Node *t12 = newTransition("");

  // organize the communication type of the new transition
  if (t1->type == t2->type)
  {
    t12->type = t1->type;
  }
  else if ((t1->type == IN && t2->type == INTERNAL) ||
           (t1->type == INTERNAL && t2->type == IN))
  {
    t12->type = IN;
  }
  else if ((t1->type == INTERNAL && t2->type == OUT) ||
           (t1->type == OUT && t2->type == INTERNAL))
  {
    t12->type = OUT;
  }
  else if ((t1->type == OUT && t2->type == IN) ||
           (t1->type == IN && t2->type == OUT) ||
           (t1->type == INOUT || t2->type == INOUT))
  {
    t12->type = INOUT;
  }
  else assert(false); ///< this should never happen or we have missed a case

  t12->prefix = t1->prefix;

  // copy t1's history to t12
  for (list<string>::iterator role = t1->history.begin(); role != t1->history.end(); role++)
  {
    roleMap[*role] = t12;
    t12->history.push_back(*role);
    if (t1->prefix != "")
    {
        roleMap[t1->prefix + *role] = t12;
        t12->history.push_back(t1->prefix + *role);
    }
  }

  // copy t2's history to t12
  for (list<string>::iterator role = t2->history.begin(); role != t2->history.end(); role++)
  {
    roleMap[*role] = t12;
    t12->history.push_back(*role);
    if (t1->prefix != "" || t2->prefix != "")
    {
        roleMap[t2->prefix + *role] = t12;
        t12->history.push_back(t2->prefix + *role);
    }
  }

  roleMap[t1->nodeFullName()] = t12;
  roleMap[t2->nodeFullName()] = t12;

  // merge pre- and postsets for t12
  t12->preset=setUnion(t1->preset, t2->preset);
  t12->postset=setUnion(t1->postset, t2->postset);

  // create the weighted arcs for t12

  // this is the preset of t12 without the preset of t1. It is needed not generate double arcs if
  // the preset of t1 and t2 were not distinct.
  set<Node *> preset2without1 = setDifference(t12->preset,t1->preset);

  // create new arcs from the t1 preset to t12
  for (set<Node *>::iterator n = t1->preset.begin(); n != t1->preset.end(); n++)
    newArc((*n), t12, STANDARD, arc_weight((*n),t1));

  // create new arcs from the preset of t2 to t12 without those, that have been already covered by the preset of t1
  for (set<Node *>::iterator n = preset2without1.begin(); n != preset2without1.end(); n++)
    newArc((*n), t12, STANDARD, arc_weight((*n),t2));

  // create new arcs from t12 to postset of t1
  for (set<Node *>::iterator n = t1->postset.begin(); n != t1->postset.end(); n++)
    newArc(t12, (*n), STANDARD, arc_weight(t1,(*n)));

  // create new arcs from t12 to postset of t2 and adjust arcweights if a node of postset of t2 is also in postset of t1
  for (set<Node *>::iterator n = t2->postset.begin(); n != t2->postset.end(); n++)
  {
    // Find an arc already created in because of the postset of t1
    for (set<Arc *>::iterator f = F.begin(); f != F.end(); f++)
    {
      if (((*f)->source == t12) && ((*f)->target == (*n)))
      {
        sametarget = true;
        delArc=f;
      }
    }

    // if such an arc was found, save its weight, delete it and add its weight to the arc from t12 to postset of t2
    if (sametarget)
    {
      int weightsave = arc_weight(t12,(*n));
      removeArc(*delArc);
      newArc(t12, (*n), STANDARD, (arc_weight(t2,(*n)) + weightsave));
      sametarget = false;
    }
    // if no such arc was found, simply add an arc from t12 to postset of t2
    else
    {
      newArc(t12, (*n), STANDARD, arc_weight(t2,(*n)));
    }
  }


  // set max_occurrences
  t12->max_occurrences = (t1->max_occurrences > t2->max_occurrences) ?
    t1->max_occurrences :
    t2->max_occurrences;

  removeTransition(t1);
  removeTransition(t2);
}



/*!
 * \brief   merges two places
 *
 *          The actual function to merge two places. Given places p1 and p2:
 *          -# a new place p12 with empty history is created
 *          -# this place gets the union of the histories of place p1 and p2
 *          -# the presets and postsets of p1 and p2 are calculated and united
 *          -# p12 is connected with all the transitions in the preset and postset
 *          -# the places p1 and p2 are removed
 *
 * \param   p1  first place
 * \param   p2  second place
 *
 * \pre     p1 != NULL
 * \pre     p2 != NULL
 * \pre     p1 and p2 are internal places.
 * \post    Places p1 and p2 removed.
 * \post    Place p12 having the incoming and outgoing arcs of p1 and p2 and
 *          the union of the histories of t1 and t2.
 *
 * \test	All methods mergePlaces() has been tested as they are concluding
 * 			to this one method. Test can be found in tests/test_mergePlaces.cc.
 */
void PetriNet::mergePlaces(Place * & p1, Place * & p2)
{
  if (p1 == p2 && p1 != NULL)
    return;

  if (p1 == NULL || p2 == NULL)
  {
    std::cerr << "[PN] At least one parameter of mergePlaces was NULL!" << std::endl;
    assert(false);
    return;
  }

  assert(p1 != NULL);
  assert(p2 != NULL);
  assert(p1->type == INTERNAL);
  assert(p2->type == INTERNAL);

  Place *p12 = newPlace("");

  p12->tokens = max(p1->tokens, p2->tokens);
  p12->isFinal = (p1->isFinal || p2->isFinal);
  p12->wasExternal = p1->wasExternal + p2->wasExternal;
  p12->capacity = max(p1->capacity, p2->capacity);

  p12->history.clear();
  p12->prefix = p1->prefix;

  for (list<string>::iterator role = p1->history.begin(); role != p1->history.end(); role++)
  {
    p12->history.push_back(*role);
    roleMap[*role] = p12;
    if (p1->prefix != "" || p2->prefix != "")
    {
      p12->prefix = p1->prefix;
      roleMap[p1->prefix + *role] = p12;
      p12->history.push_back(p1->prefix + *role);
      roleMap[p2->prefix + *role] = p12;
      p12->history.push_back(p2->prefix + *role);
    }
  }

  for (list<string>::iterator role = p2->history.begin(); role != p2->history.end(); role++)
  {
    p12->history.push_back(*role);
    roleMap[*role] = p12;
    if (p1->prefix != "" || p2->prefix != "")
    {
      roleMap[p1->prefix + *role] = p12;
      p12->history.push_back(p1->prefix + *role);
      roleMap[p2->prefix + *role] = p12;
      p12->history.push_back(p2->prefix + *role);
    }

  }

  list<set<pair<Place *, unsigned int> > > newFinalSetList;
  // change final places in the final sets
  for (list<set<pair<Place *, unsigned int> > >::iterator final_set = final_set_list.begin(); final_set != final_set_list.end(); final_set++)
  {
    set<pair<Place *, unsigned int> > s1;
    set<pair<Place *, unsigned int> > s2;
    for (set<pair<Place *, unsigned int> >::iterator p = final_set->begin(); p != final_set->end(); p++)
    {
      if ((p->first) == p1 || (p->first) == p2)
      {
        p12->isFinal = true;
        s1.insert(pair<Place *, unsigned int>(p12,p->second));
      }
      else
      {
        s2.insert(*p);
      }
    }
    set<pair<Place *, unsigned int> > s1s2 = setUnion(s1,s2);
/*
    set<pair<Place *, unsigned int> > s1s2;
    s1s2.insert(s1.begin(), s1.end());
    s1s2.insert(s2.begin(), s2.end());
*/
    newFinalSetList.push_back(s1s2);
  }
  final_set_list = newFinalSetList;

  // merge pre- and postsets for p12
  p12->preset=setUnion(p1->preset, p2->preset);
  p12->postset=setUnion(p1->postset, p2->postset);

  // create the weighted arcs for p12

  set<Node *> preset1without2 = setDifference(p12->preset,p2->preset);
  set<Node *> preset2without1 = setDifference(p12->preset,p1->preset);
  set<Node *> preset1and2 =     setIntersection(p1->preset,p2->preset);
  set<Node *> postset1without2 = setDifference(p12->postset,p2->postset);
  set<Node *> postset2without1 = setDifference(p12->postset,p1->postset);
  set<Node *> postset1and2 =     setIntersection(p1->postset,p2->postset);

  if ((p1->preset.size() + p1->postset.size()) > 1000)
    std::cerr << (p1->preset.size() + p1->postset.size()) << " arcs to add..." << std::endl;

  for (set<Node *>::iterator n = preset1without2.begin(); n != preset1without2.end(); n++)
    newArc((*n), p12, STANDARD, arc_weight((*n),p1));

  for (set<Node *>::iterator n = preset2without1.begin(); n != preset2without1.end(); n++)
    newArc((*n), p12, STANDARD, arc_weight((*n),p2));

  for (set<Node *>::iterator n = preset1and2.begin(); n != preset1and2.end(); n++)
      newArc((*n), p12, STANDARD, arc_weight((*n),p1) + arc_weight((*n),p2));

  for (set<Node *>::iterator n = postset1without2.begin(); n != postset1without2.end(); n++)
    newArc(p12, (*n), STANDARD, arc_weight(p1,(*n)));

  for (set<Node *>::iterator n = postset2without1.begin(); n != postset2without1.end(); n++)
    newArc(p12, (*n), STANDARD, arc_weight(p2,(*n)));

  for (set<Node *>::iterator n = postset1and2.begin(); n != postset1and2.end(); n++)
      newArc(p12, (*n), STANDARD, arc_weight(p1,(*n)) + arc_weight(p2,(*n)));

  removePlace(p1);
  removePlace(p2);

  p1 = NULL;
  p2 = NULL;
}





/*!
 * \brief   merges two places given two roles
 *
 * \param   role1  string describing the role of the first place
 * \param   role2  string describing the role of the second place
 */
void PetriNet::mergePlaces(string role1, string role2)
{
  Place * p1 = findPlace(role1);
  Place * p2 = findPlace(role2);

  if ( p1 == NULL )
    std::cerr << "p1 has role '" << role1 << "'" << std::endl;

  if ( p2 == NULL )
    std::cerr << "p2 has role '" << role2 << "'" << std::endl;

  mergePlaces( p1, p2 );
}

void PetriNet::mergePlaces(Place * & p1, string role2)
{
  Place * p2 = findPlace(role2);

  if ( p2 == NULL )
    std::cerr << "p2 has role '" << role2 << "'" << std::endl;

  mergePlaces( p1, p2 );
}

void PetriNet::mergePlaces(string role1, Place * & p2)
{
  Place * p1 = findPlace(role1);

  if ( p1 == NULL )
    std::cerr << "p1 has role '" << role1 << "'" << std::endl;

  mergePlaces( p1, p2);
}





/*!
 * \brief   merges two places given two identifiers and roles
 *
 *          Merges two places given two identifiers and the roles of the
 *          places. The identifiers are used to complete the role-string and
 *          pass the search request.
 *
 * \param   id1    identifier of the BPEL activity represented by the first place
 * \param   role1  string describing the role of the first place (beginning
 *                 with a period, e.g. ".initial")
 * \param   id2    identifier of the BPEL activity represented by the second place
 * \param   role2  string describing the role of the second place (beginning
 *                 with a period, e.g. ".initial")
 *
 * \note	This method is used by BPEL2oWFN only.
 */
void PetriNet::mergePlaces(unsigned int id1, string role1, unsigned int id2, string role2)
{
  mergePlaces(toString(id1) + role1, toString(id2) + role2);
}





/*!
 * \brief   rename a node (i.e., rename one role in its history)
 *
 * \param   old_name  the name of the role to be renamed
 * \param   new_name  the new name of that role
 *
 * \pre     p is a place of the Petri net.
 */
void PetriNet::renamePlace(string old_name, string new_name)
{
  Place *p = findPlace(old_name);
  assert(p != NULL);

  bool found = false;

  for( list<string>::iterator iter = p->history.begin(); iter != p->history.end(); iter ++)
  {
    if ( *iter == old_name )
    {
      *iter = new_name;
      found = true;
      break;
    }
  }

  assert(found == true);

  roleMap[old_name] = NULL;
  roleMap[new_name] = p;
}





/*!
 * \brief   returns the arc weight between two nodes
 *
 * \param   my_source  the source node of the arc to be checked
 * \param   my_target  the target node of the arc to be checked
 * \result  the weight of the arc
 *
 * \pre     the arc [my_source, my_target] was found in F
 */
unsigned int PetriNet::arc_weight(Node *my_source, Node *my_target) const
{

  map< pair<Node*, Node*>, int >::const_iterator iter = weight.find(pair<Node*, Node*>(my_source, my_target));

  assert( iter != weight.end() ) ;
  assert( iter->second > 0 );

  return iter->second;

  /*
  for (set<Arc *>::iterator f = F.begin(); f != F.end(); f++)
    if (((*f)->source == my_source) && ((*f)->target == my_target))
      return (*f)->weight;

  assert(false);
  return 1;
  */
}





/*!
 * \brief   returns true if all arcs connecting to n have a weight of 1
 *
 *          Returns true only if the given node's incoming and outgoing arcs
 *          all have the same weight.
 *
 * \param   n  node to be examined
 *
 * \pre     n != NULL
 *
 * \todo    Znamirowski: Set brackets for for-loop. The code is not readable
 *          this way.
 */
bool PetriNet::sameweights(Node *n) const
{
  assert(n != NULL);
  bool first = true;
  unsigned int w = 0;

  for (set<Arc*>::iterator f = F.begin(); f != F.end(); f++)
  {
    if (((*f)->source == n) || ((*f)->target == n) )
    {
      if (first)
      {
        first=false;
        w = (*f)->weight;
      }
      else
      {
        if ( (*f)->weight != w)
        {
          return false;
        }
      }
    }
  }

  return true;
}





/*---------------------------------------------------------------------------*/


/*!
 * \brief   calculates the preset of a node
 *
 * \param   n  a node of the Petri net
 * \result  the preset of node n
 *
 * \pre     n != NULL
 *
 * \deprecated  This function is replaced by the Node class's attribute
 *              #preset and should -- for performance reasons -- not be used
 *              any more.
 */
set<Node *> PetriNet::preset(Node *n) const
{
  std::cerr << "Warning: The method preset(Node *) has been replaced by the Node class's attribute preset!\n";
  assert(n != NULL);

  set<Node *> result;

  for (set<Arc *>::iterator f = F.begin(); f != F.end(); f++)
    if ((*f)->target == n)
      result.insert((*f)->source);

  return result;
}





/*!
 * \brief   calculates the preset of a node
 *
 * \param   n  a node of the Petri net
 * \result  the postset of node n
 *
 * \pre     n != NULL
 *
 * \deprecated  This function is replaced by the Node class's attribute
 *              #postset and should -- for performance reasons -- not be used
 *              any more.
 */
set<Node *> PetriNet::postset(Node *n) const
{
  std::cerr << "Warning: The method postset(Node *) has been replaced by the Node class's attribute postset!\n";
  assert(n != NULL);

  set<Node *> result;

  for (set<Arc *>::iterator f = F.begin(); f != F.end(); f++)
    if ((*f)->source == n)
      result.insert((*f)->target);

  return result;
}





/*---------------------------------------------------------------------------*/


/*!
 * \brief   finds place given a role
 *
 *          Finds a place of the Petri net given a role the place fills or
 *          filled.
 *
 * \param   role  the demanded role
 * \return  a pointer to the place or a NULL pointer if the place was not found.
 */
Place *PetriNet::findPlace(string role) const
{
  map< std::string, Node* >::const_iterator p = roleMap.find(role);

  if ((p != roleMap.end()) && (p->second != NULL) && (p->second->nodeType == PLACE))
    return (static_cast<Place *>(p->second));
  else
  {
    if (!forEach_suffix.empty())
    {
      map< std::string, Node* >::const_iterator p_with_suffix = roleMap.find(role + "." + forEach_suffix[0]);
      if ((p_with_suffix != roleMap.end()) && (p_with_suffix->second->nodeType == PLACE))
        return (static_cast<Place *>(p_with_suffix->second));
    }
  }

  return NULL;
}





/*!
 * \brief   finds place given an id with a role
 *
 *          Finds a place of the Petri net given an id and a role.
 *
 * \param   id    an identifier (BPEL)
 * \param   role  the demanded role
 * \return  a pointer to the place or a NULL pointer if the place was not
 *          found.
 * \note	This is used by BPEL2oWFN only.
 */
Place *PetriNet::findPlace(unsigned int id, string role) const
{
  return findPlace(toString(id) + role);
}





/*!
 * \brief   finds transition given a role
 *
 *          Finds a transition of the Petri net given a role the place fills
 *          or filled.
 *
 * \param   role  the demanded role
 * \return  a pointer to the transition or a NULL pointer if the place was not
 *          found.
 */
Transition *PetriNet::findTransition(string role) const
{
  map< std::string, Node* >::const_iterator t = roleMap.find(role);

  if (t != roleMap.end() && t->second != NULL )
  {
    if (t->second->nodeType == TRANSITION)
    {
      return (dynamic_cast<Transition *>(t->second));

    }
    else
      return NULL;
  }
  else
    return NULL;
}





/*!
 * \brief   returns a set of all final places
 *
 * \return  the set with all final places
 */
set< Place * > PetriNet::getFinalPlaces() const
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

/*!
 * \brief   returns a set of all interface places
 *
 * \return  the set with all interface places
 */
set< Place * > PetriNet::getInterfacePlaces() const
{
  return setUnion(P_in, P_out);
}




void PetriNet::setPlaceSet(set<Place *> Pset)
{
  P = Pset;
}




set<Place *> PetriNet::getPlaceSet() const
{
  return P;
}




void PetriNet::setInputPlaceSet(set<Place *> Pinset)
{
  P_in = Pinset;
}




set<Place *> PetriNet::getInputPlaceSet() const
{
  return P_in;
}




void PetriNet::setOutputPlaceSet(set<Place *> Poutset)
{
  P_out = Poutset;
}




set<Place *> PetriNet::getOutputPlaceSet() const
{
  return P_out;
}




void PetriNet::setTransitionSet(set<Transition *> Tset)
{
  T = Tset;
}




set<Transition *> PetriNet::getTransitionSet() const
{
  return T;
}




void PetriNet::setArcSet(set<Arc *> Fset)
{
  F = Fset;
}




set<Arc *> PetriNet::getArcSet() const
{
  return F;
}




/*---------------------------------------------------------------------------*/


/*!
 * \brief   returns an id for new nodes
 *
 * \return  id for new nodes
 *
 * \post    nextId is incremented
 */
unsigned int PetriNet::getId()
{
  return nextId++;
}





/*!
 * \brief   adds a suffix to the name of all interface places of the net
 *
 * \param   suffix to add to the interface places
 *
 * \note    BPEL2oWFN only method!
 */
void PetriNet::add_interface_suffix(string suffix)
{
  for (set<Place *>::const_iterator place = P_in.begin(); place != P_in.end(); place ++)
    *(*place)->history.begin() += suffix;

  for (set<Place *>::const_iterator place = P_out.begin(); place != P_out.end(); place ++)
    *(*place)->history.begin() += suffix;
}






/*!
 * \brief   adds a prefix to the name of all nodes of the net
 *
 * \param   prefix The prefix to add.
 *
 * \note    BPEL2oWFN only method.
 */
void PetriNet::addPrefix(string prefix, bool renameInterface)
{
  for (set< Place * >::iterator place = P.begin(); place != P.end(); place ++)
  {
    (*place)->prefix = prefix;

    for(list< string >::iterator name = (*place)->history.begin(); name != (*place)->history.end(); name++)
    {
      roleMap[(prefix + *name)] = *place;
    }
  }
  for (set< Place * >::iterator place = P_in.begin(); place != P_in.end(); place ++)
  {
    (*place)->prefix = prefix;

    for(list< string >::iterator name = (*place)->history.begin(); name != (*place)->history.end(); name++)
    {
      roleMap[(prefix + *name)] = *place;
      if (renameInterface)
      {
        *name = prefix + (*name);
        roleMap[*name] = NULL;
      }
    }
  }
  for (set< Place * >::iterator place = P_out.begin(); place != P_out.end(); place ++)
  {
    (*place)->prefix = prefix;

    for(list< string >::iterator name = (*place)->history.begin(); name != (*place)->history.end(); name++)
    {
      roleMap[(prefix + *name)] = *place;
      if (renameInterface)
      {
        *name = prefix + (*name);
        roleMap[*name] = NULL;
      }
    }
  }

  for (set< Transition * >::iterator transition = T.begin(); transition != T.end(); transition ++)
  {
    (*transition)->prefix = prefix;

    for(list< string >::iterator name = (*transition)->history.begin(); name != (*transition)->history.end(); name++)
      roleMap[prefix + *name] = *transition;
  }
}





/*!
 * \brief   composes a second Petri net
 *
 * Given a second Petri net #net, the internal structure is added and input
 * and output places are connected appropriatedly (if an input and an output
 * place name of the two nets match).
 *
 * \param net the net that should be added (connected)
 *
 * \note    If capacity k on input places is given, method creates complementary place
 *          with k+1 token and a deadlock transition
 */
void PetriNet::compose(const PetriNet &net, unsigned int capacityOnInterface)
{
  using namespace std;

  // add all internal places
  for (set< Place *>::iterator place = net.P.begin(); place != net.P.end(); place ++)
        {
            Place * newPlace = new Place( **place );
            newPlace->id = getId();
            newPlace->preset.clear();
            newPlace->postset.clear();
            P.insert( newPlace );

            for(list< string>::iterator name = (*place)->history.begin(); name != (*place)->history.end(); name++)
            {
                roleMap[(newPlace->prefix + *name)] = newPlace;
                roleMap[ newPlace->nodeFullName() ] = newPlace;
            }
        }

  // recalculate final markings:
  {
    list<set<pair<Place *, unsigned int> > > netFinalList;

    // if p was in the final set, the pointer has to be redirected to the new place
    for (list<set<pair<Place *, unsigned int> > >::const_iterator netFinal = net.final_set_list.begin(); netFinal != net.final_set_list.end(); netFinal++)
    {
      set<pair<Place *, unsigned int> > finalMarking;
      for (set<pair<Place *, unsigned int> >::const_iterator p = netFinal->begin(); p != netFinal->end(); p++)
      {
        assert (findPlace((p->first)->nodeFullName()) != NULL);
        finalMarking.insert(pair<Place *, unsigned int>(findPlace((p->first)->nodeFullName()), p->second));
      }
      if (! finalMarking.empty())
      {
        netFinalList.push_back(finalMarking);

          for (set<pair<Place *, unsigned int> >::const_iterator p = finalMarking.begin(); p != finalMarking.end(); p++)
          {
            finalMarking.insert(pair<Place *, unsigned int>(findPlace((p->first)->nodeFullName()), p->second));
          }
      }
    }

    if ( ! netFinalList.empty() && ! final_set_list.empty())
    {
      list<set<pair<Place *,unsigned int> > > newFinalSet;
      // merge the final set lists
      for (list<set<pair<Place *, unsigned int> > >::const_iterator final_setA = netFinalList.begin(); final_setA != netFinalList.end(); final_setA++)
      {
        for (list<set<pair<Place *, unsigned int> > >::const_iterator final_setB = final_set_list.begin(); final_setB != final_set_list.end(); final_setB++)
        {
          set<pair<Place *,unsigned int> > finalMarking = setUnion(*final_setA, *final_setB);
          newFinalSet.push_back(finalMarking);
        }
      }
      final_set_list = newFinalSet;
    }
    else
    {
      final_set_list.merge(netFinalList);
    }
  }

  // add all input places
  for (set< Place * >::iterator place = net.P_in.begin(); place != net.P_in.end(); place ++)
  {
    Place * newPlace = new Place( **place );
    newPlace->id = getId();
    newPlace->preset.clear();
    newPlace->postset.clear();

    // check if input place with same name already exists
    if (roleMap[ newPlace->nodeFullName() ] == NULL )
    {
      // no, all fine
      roleMap[ newPlace->nodeFullName() ] = newPlace;
      P_in.insert( newPlace );
    }
    else if (roleMap[ newPlace->nodeFullName() ]->type == OUT)
    {
      // no, all fine
      roleMap[ newPlace->nodeFullName() ] = newPlace;
      P_in.insert( newPlace );
    }
    // else, we don't need to add the place, it already exist (arcs are connected with the existing place)

    for(list< string >::iterator name = (*place)->history.begin(); name != (*place)->history.end(); name++) {
      roleMap[(newPlace->prefix + *name)] = newPlace;
    }
  }

  // add all output places
  for (set< Place * >::iterator place = net.P_out.begin(); place != net.P_out.end(); place ++)
  {
    Place * newPlace = new Place( **place );
    newPlace->id = getId();
    newPlace->preset.clear();
    newPlace->postset.clear();

    // check if input place with same name already exists
    if (roleMap[ newPlace->nodeFullName() ] == NULL )
    {
      // no, all fine
      roleMap[ newPlace->nodeFullName() ] = newPlace;
      P_out.insert( newPlace );
    }
    else if (roleMap[ newPlace->nodeFullName() ]->type == IN)
    {
      // no, all fine
      roleMap[ newPlace->nodeFullName() ] = newPlace;
      P_out.insert( newPlace );
    }
    // else, we don't need to add the place, it already exist (arcs are connected with the existing place)

    for(list< string >::iterator name = (*place)->history.begin(); name != (*place)->history.end(); name++) {
      roleMap[(newPlace->prefix + *name)] = newPlace;
    }
  }

  // add all transitions
  for (set< Transition * >::iterator transition = net.T.begin(); transition != net.T.end(); transition ++)
  {
    Transition * newTransition = new Transition( **transition );
    newTransition->id = getId();
    newTransition->preset.clear();
    newTransition->postset.clear();
    T.insert( newTransition );

    roleMap[ newTransition->nodeFullName() ] = newTransition;

    for(list< string >::iterator name = (*transition)->history.begin(); name != (*transition)->history.end(); name++)
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

  {
  set< Place * > eraseP_in;
  set< Place * > additionalP_out;
  set<pair<Place *, unsigned int> > finalMarking;

  // merge appropriate input and output places (same name, different prefixes)
  for (set< Place * >::iterator place = P_in.begin(); place != P_in.end(); place ++)
  {

    set< Place * >::iterator pPlace = P_out.begin();
    bool finished = false;


    while ( ! finished && pPlace != P_out.end())
    {

      if ( // (*pPlace)->nodeFullName().erase(0,4) != (*place)->nodeFullName().erase(0,3) &&
           (*pPlace)->nodeFullName() != (*place)->nodeFullName()   )
      {
	pPlace++;
      }
      else
      {
	finished = true;
      }
    }


    // Place * oPlace = findPlace("out." + (*place)->nodeFullName().erase(0,3));
    Place * oPlace = *pPlace;

    if (finished)
    // if ( oPlace != NULL )
    {
      if ( (*place)->prefix != (oPlace)->prefix )
      {
        string name = (*place)->nodeFullName();
        (*place)->type = INTERNAL;
        if ( (*place)->max_occurrences < (oPlace)->max_occurrences )
        {
          (*place)->max_occurrences = (oPlace)->max_occurrences;
        }
        (*place)->history.push_front( (*place)->nodeFullName() );
        roleMap[(*place)->nodeFullName()] = (*place);
        (oPlace)->type = INTERNAL;
        (oPlace)->history.push_front( (oPlace)->nodeFullName() );
        roleMap[(oPlace)->nodeFullName()] = (oPlace);
        (oPlace)->wasExternal = (oPlace)->nodeFullName(); //.substr((oPlace)->nodeFullName().find_first_of(".") + 1);
        P.insert(*place);
        P.insert(oPlace);
        mergePlaces((*place)->nodeFullName(), (oPlace)->nodeFullName());
        eraseP_in.insert(*place);
        P_out.erase(oPlace);
        // if capacity k on input places is given, create complementary place with k+1 token and a deadlock transition
        if (capacityOnInterface > 0)
        {
          // complementary place is part of the initial and final marking
          Place * p = findPlace(name);
          Place * comp_p = newPlace("comp_" + name);
          comp_p->mark(capacityOnInterface);
          finalMarking.insert(pair<Place *, unsigned int>(comp_p, capacityOnInterface));

          // create arcs to the pre- and posttransitions of p
          const set<Node *> preSet = preset(p);
          for (set<Node *>::const_iterator t = preSet.begin(); t != preSet.end(); t++)
          {
              newArc(comp_p, *t);
          }

          const set<Node *> postSet = postset(p);
          for (set<Node *>::const_iterator t = postSet.begin(); t != postSet.end(); t++)
          {
              newArc(*t, comp_p);
          }

          // add deadlock transition
          Transition * t = newTransition("comp_" + name + "_DL");
          newArc(comp_p, t, STANDARD, capacityOnInterface);
        }
      }
    }
  }

  //adding complementary places to final sets
  for (list<set<pair<Place *, unsigned int> > >::iterator final_set = final_set_list.begin(); final_set != final_set_list.end(); final_set++)
  {
    *final_set = setUnion(*final_set, finalMarking);
  }

  // erase input places, that were merged
  for (set< Place * >::iterator place = eraseP_in.begin(); place != eraseP_in.end(); place ++)
  {
    P_in.erase( *place );
  }
  }

  // mark transitions as internal that are no longer communicating
  for (set< Transition * >::iterator transition = T.begin(); transition != T.end(); transition ++)
  {
    if ( (*transition)->type != INTERNAL )
    {
      set< Node * > prePost = setUnion((findTransition((*transition)->nodeFullName()))->preset, (findTransition((*transition)->nodeFullName()))->postset);
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
 * \brief   moves channel places to the list of internal places
 *
 *          Converts input and output places (channels) to internal places.
 *
 * \todo    What means "P_in.clear(); P_in = set< Place * >();"?
 *
 * \deprecated  This method has been replaced by method compose().
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
 * \brief   re-enumerates the nodes
 *
 *          Re-enumerates the nodes of the Petri net to have places numbered
 *          p1, p2, ... and transitions t1, t2, ... .
 */
void PetriNet::reenumerate()
{
  int currentId = 1;

  for (set<Place *>::iterator p = P.begin(); p != P.end(); p++)
  {
    (*p)->id = currentId++;
  }
  for (set<Place *>::iterator p = P_in.begin(); p != P_in.end(); p++)
    (*p)->id = currentId++;
  for (set<Place *>::iterator p = P_out.begin(); p != P_out.end(); p++)
    (*p)->id = currentId++;

  currentId = 1;
  for (set<Transition *>::iterator t = T.begin(); t != T.end(); t++)
    (*t)->id = currentId++;
}





/*!
 * \brief   calculate the maximal occurrences of communication
 *
 *          Calculates the maximal number each communication place can be
 *          used, i.e. the maximal occurrences of transitions in the preset
 *          (postset) of output (input) places. This number is determined by
 *          postprocessing the AST.
 *
 * \todo    use information from the postprocessing/CFG to improved (i.e.
 *          minimize) the calculated values
 *
 * \post    All communication places have max_occurrences values between 0
 *          (initial value) and UINT_MAX (maximal value).
 *
 * \note    BPEL2oWFN only method.
 */
void PetriNet::calculate_max_occurrences()
{
#ifdef USING_BPEL2OWFN

  // set the max_occurrences for the transitions (should make more sense...)
  for (set<Transition *>::iterator transition = T.begin();
       transition != T.end();
       transition++)
  {
    unsigned int activity_identifier = toUInt((*(*transition)->history.begin()).substr(0, (*(*transition)->history.begin()).find_first_of(".")));

    assert(globals::ASTEmap[activity_identifier] != NULL);
    (*transition)->max_occurrences = globals::ASTEmap[activity_identifier]->max_occurrences;
  }



  // process the input places
  for (set<Place *>::iterator p = P_in.begin(); p != P_in.end(); p++)
  {
    set<unsigned int> receiving_activities;

    for (set<Node *>::iterator t = (*p)->postset.begin(); t != (*p)->postset.end(); t++)
      for (unsigned int i = 0; i < (*t)->history.size(); i++)
      {
        // unsigned int transition_activity_id = toUInt((*t)->history[i].substr(0, (*t)->history[i].find_first_of(".")));
        string act_id = (*t)->nodeFullName().substr(0, (*t)->nodeFullName().find_first_of("."));

        act_id = act_id.substr( act_id.find_last_of( "_" ) + 1 );
        unsigned int transition_activity_id = toUInt( act_id );
        assert(globals::ASTEmap[transition_activity_id] != NULL);
        receiving_activities.insert(transition_activity_id);
      }

        for (set<unsigned int>::iterator activity_id = receiving_activities.begin(); activity_id != receiving_activities.end(); activity_id++)
        {
          if (globals::ASTEmap[*activity_id]->max_occurrences == UINT_MAX)
            (*p)->max_occurrences = UINT_MAX;
          else if ((*p)->max_occurrences != UINT_MAX)
          {
            if (globals::ASTEmap[*activity_id]->activityTypeName() == "repeatUntil" ||
                globals::ASTEmap[*activity_id]->activityTypeName() == "while")
              (*p)->max_occurrences += globals::ASTEmap[*activity_id]->max_loops;
            else
              (*p)->max_occurrences += globals::ASTEmap[*activity_id]->max_occurrences;
          }
          //  Debugging Help
          //  cerr << "gehoert auch zu Aktivitaet: " << globals::ASTEmap[*activity_id]->activityTypeName() << "!\n";
        }
  }

    // process the output places
    for (set<Place *>::iterator p = P_out.begin(); p != P_out.end(); p++)
    {
      set<unsigned int> sending_activities;

      for (set<Node *>::iterator t = (*p)->preset.begin(); t != (*p)->preset.end(); t++)
        for (list<string>::iterator iter = (*t)->history.begin(); iter != (*t)->history.end(); iter++)
        {
          unsigned int transition_activity_id = toUInt(iter->substr(0, iter->find_first_of(".")));
          assert(globals::ASTEmap[transition_activity_id] != NULL);
          sending_activities.insert(transition_activity_id);
        }

          for (set<unsigned int>::iterator activity_id = sending_activities.begin(); activity_id != sending_activities.end(); activity_id++)
          {
            if (globals::ASTEmap[*activity_id]->max_occurrences == UINT_MAX)
              (*p)->max_occurrences = UINT_MAX;
            else if ((*p)->max_occurrences != UINT_MAX)
              (*p)->max_occurrences += globals::ASTEmap[*activity_id]->max_occurrences;
          }
    }
#endif
}





/*!
 * \brief   swaps input and output places
 *
 *          Swaps the input and output places, i.e. swaps the sets #P_in and
 *          #P_out and the adjacent arcs.
 *
 *  \test	This method has been tested in tests/test_mirror.cc.
 */
void PetriNet::mirror()
{
  // swap arcs
  for (set<Arc*>::iterator f = F.begin(); f != F.end(); f++)
  {
    if ( P_in.find( findPlace( ((*f)->source)->nodeShortName()) ) != P_in.end() )
      (*f)->mirror();
    else if ( P_out.find( findPlace( ((*f)->target)->nodeShortName()) ) != P_out.end() )
      (*f)->mirror();
  }

  // swap input and output places
  set<Place *> P_in_old = P_in;
  set<Place *> P_out_old = P_out;
  P_in = P_out_old;
  P_out = P_in_old;
  for (set<Place *>::iterator p = P_in.begin(); p != P_in.end(); p++)
    (*p)->type = IN;
  for (set<Place *>::iterator p = P_out.begin(); p != P_out.end(); p++)
    (*p)->type = OUT;

  // swap the transitions
  for (set<Transition *>::iterator t = T.begin(); t != T.end(); t++)
  {
    if ( (*t)->type == OUT )
      (*t)->type = IN;
    else if ( (*t)->type == IN )
      (*t)->type = OUT;
  }
}





/*!
 * \brief   produces a second constraint oWFN
 *
 * \param   a constraint oWFN
 *
 * \test    This method has been tested in tests/test_produce.cc.
 */
void PetriNet::produce(const PetriNet &net)
{
  // the constraint oWFN must have an empty interface
  assert(net.P_in.empty());
  assert(net.P_out.empty());


  // copy the constraint oWFN's places to the oWFN
  for (set<Place *>::iterator p = net.P.begin(); p != net.P.end(); p++)
  {
    Place *p_new = newPlace((*p)->nodeName());

    p_new->isFinal = (*p)->isFinal;
    p_new->tokens = (*p)->tokens;
  }


  // copy the constraint oWFN's unlabeled transitions to the oWFN
  for (set<Transition *>::iterator t = net.T.begin(); t != net.T.end(); t++)
  {
    if ( (*t)->labels.empty())
    {
      Transition *t_new = newTransition((*t)->nodeName());

      // copy the arcs of the constraint oWFN
      for (set< Arc * >::iterator arc = net.F.begin(); arc != net.F.end(); arc ++)
      {
        if ( ( (*arc)->source->nodeType == PLACE) && ( (*arc)->target == static_cast<Node *>(*t) ) )
          newArc( findPlace( (*arc)->source->nodeName() ), t_new, STANDARD, (*arc)->weight );
        if ( ( (*arc)->target->nodeType == PLACE) && ( (*arc)->source == static_cast<Node *>(*t) ) )
          newArc( t_new, findPlace( (*arc)->target->nodeName() ), STANDARD, (*arc)->weight );
      }
    }
  }


  // transitions of the oWFN that are used as labels in the constraint oWFN
  set<string> used_labels;
  set<pair<Transition *, Transition *> > transition_pairs;


  // Traverse the used labels and store pairs of constrainted and labeled
  // transitions. If a label does not identify a transition, look for a plce
  // with the same label instead. According to its communication type, either
  // add the transitions in the preset or the postset to the transition pairs.
  // For example, if "visa" is not found as transition, but there exists an
  // input place with this name, add all transitions in the postset of this
  // place, together with the transition labeled "visa" to the transition
  // pairs.
  for (set<Transition *>::iterator t = net.T.begin(); t != net.T.end(); t++)
  {
    for (set<string>::iterator label = (*t)->labels.begin(); label != (*t)->labels.end(); label++)
    {
      Transition *t_l = findTransition(*label);

      // specified transition not found -- trying places instead
      if (t_l == NULL)
      {
        Place *p = findPlace(*label);
        if (p != NULL)
        {
          set<Node *> transitions_p;

          if (p->type == IN)
            transitions_p = p->postset;
          else
            transitions_p = p->preset;

          for (set<Node *>::iterator pre_transition = transitions_p.begin(); pre_transition != transitions_p.end(); pre_transition++)
          {
            used_labels.insert((*pre_transition)->nodeName());
            transition_pairs.insert(pair<Transition *, Transition *>(static_cast<Transition *>(*pre_transition), *t));
          }
        }
        else
        {
          std::cerr << "label " << *label << " neither describes a transition nor a place" << std::endl;
          assert(false);
        }
      }
      else
      {
        used_labels.insert(*label);
        transition_pairs.insert(pair<Transition *, Transition *>(t_l, *t));
      }
    }
  }


  // create pair transitions with their arcs
  for (set<pair<Transition *, Transition *> >::iterator tp = transition_pairs.begin(); tp != transition_pairs.end(); tp++)
  {
    // I have to comment the next line as Fiona cannot read node names with brackets
    // Transition *t_new = newTransition("(" + tp->first->nodeName() + "," + tp->second->nodeName() + ")");
    Transition *t_new = newTransition(tp->first->nodeName() + "_" + tp->second->nodeName());

    // copy the arcs of the constraint oWFN
    for (set< Arc * >::iterator arc = net.F.begin(); arc != net.F.end(); arc ++)
    {
      if ( ( (*arc)->source->nodeType == PLACE) && ( (*arc)->target == static_cast<Node *>(tp->second) ) )
        newArc( findPlace( (*arc)->source->nodeName() ), t_new, STANDARD, (*arc)->weight );
      if ( ( (*arc)->target->nodeType == PLACE) && ( (*arc)->source == static_cast<Node *>(tp->second) ) )
        newArc( t_new, findPlace( (*arc)->target->nodeName() ), STANDARD, (*arc)->weight );
    }

    // copy the arcs of the oWFN
    for (set< Arc * >::iterator arc = F.begin(); arc != F.end(); arc ++)
    {
      if ( ( (*arc)->source->nodeType == PLACE) && ( (*arc)->target == static_cast<Node *>(tp->first) ) )
        newArc( findPlace( (*arc)->source->nodeName() ), t_new, STANDARD, (*arc)->weight );
      if ( ( (*arc)->target->nodeType == PLACE) && ( (*arc)->source == static_cast<Node *>(tp->first) ) )
        newArc( t_new, findPlace( (*arc)->target->nodeName() ), STANDARD, (*arc)->weight );
    }
  }


  // remove transitions that are used as labels
  for (set<string>::iterator t = used_labels.begin(); t != used_labels.end(); t++)
    removeTransition(findTransition(*t));
}





/*!
 * \brief  adds a transition that has read arcs to all final places
 *
 * \test	This method has been tested in tests/test_loop_final_state.cc.
 *
 * \note    BPEL2oWFN only method.
 */
void PetriNet::loop_final_state()
{
  Transition *t = newTransition("antideadlock");

  for (set<Place *>::iterator place = P.begin(); place != P.end(); place++)
  {
    if ( (*place)->isFinal )
      newArc(t, *place, READ);
  }
}




void PetriNet::setPlacePort(Place *place, string port)
{
  assert (place != NULL);
  assert (place->type != INTERNAL);
  assert (port != "");

  ports[port].insert(place);
}



/*!
 * \brief   Returns the number of interface places in t's neighborhood
 *
 * \return  an unsigned integer which represents the number of interface places
 */
unsigned int PetriNet::neighborInterfacePlaces(Transition *t) const
{
  unsigned int counter = 0;

  for (set<Node *>::iterator ip = t->preset.begin(); ip != t->preset.end(); ip++)
    if ((*ip)->type != INTERNAL)
      counter++;
  for (set<Node *>::iterator ip = t->postset.begin(); ip != t->postset.end(); ip++)
    if ((*ip)->type != INTERNAL)
      counter++;

  return counter;
}




/*!
 * \brief   normalizes the given Petri net
 *
 *          A Petri net (resp. open net) is called normal if
 *            each transition has only one interface place in its neighborhood.
 *
 * \todo    Write test cases.
 */
void PetriNet::normalize()
{
  std::string suffix = "_normalized";
  set<Place *> temp;
  set<Place *> interface = getInterfacePlaces();

  for (set<Place *>::iterator place = interface.begin(); place
      != interface.end(); place++)
  {
    set<Node *> neighbors = setUnion((*place)->preset, (*place)->postset);
    for (set<Node *>::iterator transition = neighbors.begin();
        transition != neighbors.end(); transition++)
    {
      Transition *t = static_cast<Transition *>(*transition);
      while (!t->isNormal())
      {
        // create new internal place from interface place
        Place *newP = newPlace((*place)->nodeFullName() + suffix);
        assert(newP != NULL);

        // create new interface place
        Place *newPin = newPlace("i_" + (*place)->nodeFullName(),
            (*place)->type);
        assert(newPin != NULL);

        // create new transition
        Transition *newT = newTransition("t_" + (*place)->nodeFullName());
        assert(newT != NULL);

        // set arcs (1, 2 & 3)
        if ((*place)->type == IN)
        {
          Arc *f1 = newArc(newPin, newT);
          assert(f1 != NULL);
          Arc *f2 = newArc(newT, newP);
          assert(f2 != NULL);
          for (set<Arc *>::iterator f = F.begin(); f != F.end(); f++)
          {
            if ((*f)->source == (*place))
            {
              Arc *f3 = newArc(newP, (*f)->target);
              assert(f3 != NULL);
            }
          }
        }
        else
        {
          Arc *f1 = newArc(newT, newPin);
          assert(f1 != NULL);
          Arc *f2 = newArc(newP, newT);
          assert(f2 != NULL);
          for (set<Arc *>::iterator f = F.begin(); f != F.end(); f++)
          {
            if ((*f)->target == (*place))
            {
              Arc *f3 = newArc((*f)->source, newP);
              assert(f3 != NULL);
            }
          }
        }

        temp.insert(*place);
      }
    }
  }

  // remove the old interface places
  for (set<Place *>::iterator place = temp.begin(); place != temp.end(); place++)
    removePlace(*place);
}




/*!
 * \brief   sets the final condition
 */
void PetriNet::setFinalCondition(Formula *f)
{
  finalcondition = f;
}




/*!
 * \brief   checks a marking m for final condition
 *
 * \param   Marking m
 *
 * \return  TRUE iff m fulfills the final condition
 */
bool PetriNet::checkFinalCondition(Marking &m) const
{
  return finalcondition->evaluate(m);
}




/*!
 * \brief   sets the invokation string
 */
void PetriNet::setInvocation(std::string invokestr)
{
  invocation_string = invokestr;
}




/*!
 * \brief   returns the invocation string
 */
std::string PetriNet::getInvocation() const
{
  return invocation_string;
}




/*!
 * \brief   sets the package string
 */
void PetriNet::setPackageString(std::string packagestr)
{
  package_string = packagestr;
}




/*!
 * \brief   return the package string
 */
std::string PetriNet::getPackageString() const
{
  return package_string;
}




/*!
 * \brief   deletes all interface places
 */
void PetriNet::makeInnerStructure()
{
    // deletes all IN-places
    for(set<Place *>::iterator place = P_in.begin(); place != P_in.end(); place = P_in.begin())
    {
        removePlace(*place);
    }

    // deletes all OUT-places
    for(set<Place *>::iterator place = P_out.begin(); place != P_out.end(); place = P_out.begin())
    {
        removePlace(*place);
    }
}




/*!
 * \brief   checks whether the Petri net is a workflow net or not
 *
 *          A Petri net N is a workflow net iff
 *            (1) there is exactly one place with empty preset
 *            (2) there is exactly one place with empty postset
 *            (3) all other nodes are situated on a path between the places described in (1) and (2)
 *
 * \return  true iff (1), (2) and (3) are fulfilled
 * \return  false in any other case
 *
 * \test    Tested in tests/test_isWorkflowNet.cc
 */
bool PetriNet::isWorkflowNet()
{
  Place *first = NULL;
  Place *last  = NULL;

  // finding places described in condition (1) & (2)
  for (set<Place *>::const_iterator p = P.begin(); p != P.end(); p++)
  {
    if ((*p)->preset.empty())
    {
      if (first == NULL)
        first = *p;
      else
      {
        std::cerr << "This net is no workflow net because there are more than one place with empty preset!\n";
        return false;
      }
    }
    if ((*p)->postset.empty())
    {
      if (last == NULL)
        last = *p;
      else
      {
        std::cerr << "This net is no workflow net because there are more than one place with empty postset!\n";
        return false;
      }
    }
  }
  if (first == NULL || last == NULL)
  {
    std::cerr << "No workflow net! Either no place with empty preset or no place with empty postset found.\n";
    return false;
  }

  // insert new transition which consumes from last and produces on first to form a cycle
  Transition *tarjan = newTransition("tarjan");
  newArc(last, tarjan);
  newArc(tarjan, first);

  // each 2 nodes \f$x,y \in P \cup T\f$ are in a directed cycle
  // (strongly connected net using tarjan's algorithm)
  unsigned int i = 0; ///< count index
  map<Node *, int> index; ///< index property for nodes
  map<Node *, unsigned int> lowlink; ///< lowlink property for nodes
  set<Node *> stacked; ///< the stack indication for nodes
  stack<Node *> S; ///< stack used by Tarjan's algorithm

  // set all nodes' index values to ``undefined''
  for (set<Place *>::const_iterator p = P.begin(); p != P.end(); p++)
    index[*p] = (-1);
  for (set<Transition *>::const_iterator t = T.begin(); t != T.end(); t++)
    index[*t] = (-1);

  // getting the number of strongly connected components reachable from first
  unsigned int sscCount = dfsTarjan(first, S, stacked, i, index, lowlink);

  std::cout << "\nstrongly connected components: " << sscCount << "\n\n";

  removeTransition(tarjan);

  // check set $P \cup T$
  set<Node *> nodeSet;
  for (set<Place *>::const_iterator p = P.begin(); p != P.end(); p++)
    nodeSet.insert(*p);
  for (set<Transition *>::const_iterator t = T.begin(); t != T.end(); t++)
    nodeSet.insert(*t);

  /* true iff only one strongly connected component found from first and all nodes
   * of $\mathcal{N}$ are members of this component
   */
  if (sscCount == 1 && setDifference(nodeSet, stacked).empty())
    return true;
  else
  {
    cerr << "No workflow net! Some places are not between the the preset-less and the postset-less place.\n";
    return false;
  }
}




/*!
 * \brief   DFS on the net using Tarjan's algorithm.
 *
 * \param   Node n which is to check
 * \param   Stack S is the stack used in the Tarjan algorithm
 * \param   Set stacked which is needed to identify a node which is already stacked
 * \param   int \f$i \in \fmathbb{N}\f$ which is the equivalent to Tarjan's index variable
 * \param   Map index which describes the index property of a node
 * \param   Map lowlink which describes the lowlink property of a node
 *
 * \return  the number of strongly connected components reachable from the start node.
 *
 * \note    Used by method isWorkflowNet() to check condition (3) - only working for this method.
 */
unsigned int PetriNet::dfsTarjan(Node *n, stack<Node *> &S, set<Node *> &stacked, unsigned int &i, map<Node *, int> &index, map<Node *, unsigned int> &lowlink) const
{
  unsigned int retVal = 0;

  index[n] = i;
  lowlink[n] = i;
  i++;
  S.push(n);
  stacked.insert(n);
  std::cout << n->nodeFullName() << " stacked, ";
  for (set<Node *>::const_iterator nn = n->postset.begin(); nn != n->postset.end(); nn++)
  {
    if (index[*nn] < 0)
    {
      retVal += dfsTarjan(*nn, S, stacked, i, index, lowlink);
      lowlink[n] = min(lowlink[n], lowlink[*nn]);
    }
    else
    {
      if (stacked.count(*nn) > 0)
        lowlink[n] = min(lowlink[n], lowlink[*nn]);
    }
  }
  if (static_cast<int>(lowlink[n]) == index[n])
  {
    retVal++;
    std::cout << "\nSCC: ";
    while (!S.empty() && lowlink[S.top()] == lowlink[n])
    {
      std::cout << S.top()->nodeFullName() << ", ";
      S.pop();
    };
  }

  return retVal;
}




/*!
 * \brief   checks whether the Petri net is free choice
 *
 *          A Petri net is free choice iff
 *            all two transitions have either disjoint or equal presets
 *
 * \return  true if the criterion is fulfilled and false if not
 *
 * \todo    Write test cases.
 */
bool PetriNet::isFreeChoice() const
{
  for (set<Transition *>::iterator t = T.begin(); t != T.end(); t++)
    for (set<Transition *>::iterator tt = T.begin(); tt != T.end(); tt++)
    {
      set<Node *> t_pre  = (*t)->preset;
      set<Node *> tt_pre = (*tt)->preset;
      if ((t_pre != tt_pre) && !(setIntersection(t_pre, tt_pre).empty()))
        return false;
    }
  return true;
}




/*!
 * \brief   initializes all Places \f$p \in P\f$ for gathering in a Marking
 *
 * \return  unsigned int with the length of the marking vector
 */
unsigned int PetriNet::initMarking() const
{
  unsigned int c_id = 0;
  for (set<Place *>::const_iterator p = P.begin(); p != P.end(); p++)
  {
    (*p)->marking_id = c_id++;
  }
  for (set<Place *>::const_iterator p_i = P_in.begin(); p_i != P_in.end(); p_i++)
  {
    (*p_i)->marking_id = c_id++;
  }
  for (set<Place *>::const_iterator p_o = P_out.begin(); p_o != P_out.end(); p_o++)
  {
    (*p_o)->marking_id = c_id++;
  }

  return c_id;
}




/*!
 * \brief   calculates the current marking m
 *
 * \return  Marking m which represents the current Marking m
 *          if the net
 *
 * \note    method uses the information in the property token of
 *          each place.
 */
Marking PetriNet::calcCurrentMarking() const
{
  unsigned int placeCount = initMarking(); // initializes the places and returns the number of places
  Marking *m = new Marking(placeCount, 0);

  for (set<Place *>::const_iterator p = P.begin(); p != P.end(); p++)
    (*m)[(*p)->marking_id] = (*p)->tokens;
  for (set<Place *>::const_iterator p = P_in.begin(); p != P_in.end(); p++)
    (*m)[(*p)->marking_id] = (*p)->tokens;
  for (set<Place *>::const_iterator p = P_out.begin(); p != P_out.end(); p++)
    (*m)[(*p)->marking_id] = (*p)->tokens;

  return *m;
}




/*!
 * \brief   recalculates a marking m to its corresponding place tokens
 *
 * An initial markings was described by setting the tokens variable of each place
 * to one specific value. Now, we can describe Markings without using those variables,
 * so probably it is necessary to translate the new markings to the old fashion.
 *
 * \param   Marking m which represents the new markings
 */
void PetriNet::marking2Places(Marking &m)
{
  for (set<Place *>::iterator p = P.begin(); p != P.end(); p++)
    (*p)->tokens = m[(*p)->marking_id];
  for (set<Place *>::iterator p = P_in.begin(); p != P_in.end(); p++)
      (*p)->tokens = m[(*p)->marking_id];
  for (set<Place *>::iterator p = P_out.begin(); p != P_out.end(); p++)
      (*p)->tokens = m[(*p)->marking_id];
}




/*!
 * \brief   calculates the successor marking m' to m under Transition t
 *
 * \param   Marking &m current marking
 * \param   Transition &t defines the scope
 *
 * \return  Marking &m' successor marking
 */
Marking PetriNet::calcSuccessorMarking(Marking &m, Transition *t) const
{
  Marking *mm = new Marking(m);
  set<Arc *> activators; ///< set to gather arcs

  // gather all arcs in F that connect a place p in the preset of t with t
  for (set<Arc *>::const_iterator f = F.begin(); f != F.end(); f++)
    if ((*f)->target == t)
    {
      Place *p = static_cast<Place *>((*f)->source);
      if ((*mm)[p->marking_id] >= (*f)->weight)
        activators.insert(*f);
      else
        break;
    }

  // if m does not activate t then m' = m
  if (t->preset.size() != activators.size())
    return *mm;

  for (set<Arc *>::const_iterator f = F.begin(); f != F.end(); f++)
    if ((*f)->source == t)
    {
      Place *p = static_cast<Place *>((*f)->target);
      (*mm)[p->marking_id] += (*f)->weight;
    }

  for (set<Arc *>::iterator f = activators.begin(); f != activators.end(); f++)
  {
    Place *p = static_cast<Place *>((*f)->source);
    (*mm)[p->marking_id] -= (*f)->weight;
  }

  return *mm;
}




/*!
 * \brief     looks for a living transition under marking m
 *
 * \param     Marking m
 *
 * \return    Transition t if t is living under m and NULL if not
 */
Transition *PetriNet::findLivingTransition(Marking &m) const
{
  Transition *result = NULL;

  for (set<Arc *>::const_iterator f = F.begin(); f != F.end(); f++)
  {
    Node *n = (*f)->source;
    if (n->nodeType == PLACE)
    {
      Place *p = static_cast<Place *>((*f)->source);
      if (m[p->marking_id] >= (*f)->weight)
      {
        result = static_cast<Transition *>((*f)->target);
        break;
      }
    }
  }

  return result;
}




/******************************************************************************
 * Functions to manage <forEach> suffix
 *****************************************************************************/

/*!
 * \note    BPEL2oWFN only method.
 */
unsigned int PetriNet::push_forEach_suffix(string suffix)
{
  forEach_suffix.push_front(suffix);
  return forEach_suffix.size();
}


/*!
 * \note    BPEL2oWFN only method.
 */
unsigned int PetriNet::pop_forEach_suffix()
{
  forEach_suffix.pop_front();
  return forEach_suffix.size();
}

} /* namespace PNapi */
