/*!
 * \file    petrinet.cc
 *
 * \brief   Petri net basics
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          Christian Gierds <gierds@informatik.hu-berlin.de>,
 *          Martin Znamirowski <znamirow@informatik.hu-berlin.de>,
 *          Robert Waltemath <robert.waltemath@uni-rostock.de>,
 *          last changes of: $Author$
 *
 * \since   2005-10-18
 *
 * \date    $Date$
 *
 * \version $Revision$
 */

#include <cassert>
#include <iostream>
#include <algorithm>

#include "util.h"
#include "parser.h"
#include "petrinode.h"
#include "petrinet.h"

using std::cerr;
using std::min;


namespace pnapi
{

  /*!
   * Reads a Petri net from a stream (in most cases backed by a file). The 
   * format
   * of the stream data is not determined automatically. You have to set it 
   * explicitly using PetriNet::setFormat().
   */
  istream & operator>>(istream & is, PetriNet & net)
  {
    switch (net.format_)
      {
      case PetriNet::FORMAT_OWFN:
	{
	  parser::owfn::Parser parser;
	  parser::owfn::Visitor visitor;
	  parser.parse(is).visit(visitor);
	  net = visitor.getPetriNet();
	  break;
	}
	
      default:
	assert(false);  // unsupported input format
      }
    
    return is;
  }


  /*!
   */
  PetriNet::PetriNet() :
    format_(FORMAT_OWFN)
  {
  }


  /*!
   * The copy constructor with deep copy.
   */
  PetriNet::PetriNet(const PetriNet & net)
  {
    *this += net;
  }

  
  /*!
   * The destructor of the PetriNet class.
   */
  PetriNet::~PetriNet()
  {
    // delete all places
    for (set<Place *>::iterator it = places_.begin(); it != places_.end(); ++it)
      deletePlace(**it);

    // delete all transitions
    for (set<Transition *>::iterator it = transitions_.begin(); 
	 it != transitions_.end(); ++it)
      deleteTransition(**it);
  }


  /*!
   * The "=" operator.
   */
  PetriNet & PetriNet::operator=(const PetriNet & net)
  {
    assert(this != &net);

    this->~PetriNet();
    return *new (this) PetriNet(net);
  }


  /*!
   */
  void PetriNet::compose(const PetriNet & net, const string & prefix, 
			 const string & netPrefix)
  {
    assert(prefix != netPrefix);

    PetriNet tmpNet = net;  // copy the net to prefix it

    // find the interface places to be merged
    vector<pair<Place *, Place *> > mergePlaces;
    const set<Place *> & interface = tmpNet.getInterfacePlaces();
    for (set<Place *>::iterator it = interface.begin(); it != interface.end();
	 ++it)
      {
	Place * place = findPlace((*it)->getName());
	if (place != NULL && place->isComplementType(**it))
	  mergePlaces.push_back(pair<Place *, Place *>(place, *it));
      }

    // prefix and combine the nets
    prefixNodeNames(prefix);
    tmpNet.prefixNodeNames(netPrefix);
    *this += tmpNet;

    // translate references and merge places
    for (vector<pair<Place *, Place *> >::iterator it = mergePlaces.begin();
	 it != mergePlaces.end(); ++it)
      {
	if (&it->first->getPetriNet() != this)
	  it->first = findPlace(it->first->getName());
	else if (&it->second->getPetriNet() != this)
	  it->second = findPlace(it->second->getName());
	it->first->merge(*it->second);
      }
  }


  /*!
   * \param   source  the source Node
   * \param   target  the target Node
   * \param   weight  weight of the Arc
   * \return  the newly created Arc
   */
  Arc & PetriNet::createArc(Node & source, Node & target, int weight)
  {
    return *new Arc(*this, source, target, weight);
  }


  /*!
   * If an empty name is given, one is generated using getUniqueNodeName().
   *
   * \param   name  the (initial) name of the place
   * \param   type  communication type of the place (internal or interface)
   * \return  the newly created place
   */
  Place & PetriNet::createPlace(const string & name, Node::Type type)
  {
    return *new Place(*this, name.empty() ? getUniqueNodeName("p") : name,
		      type);
  }


  /*!
   * If an empty name is given, one is generated using getUniqueNodeName().
   */
  Transition & PetriNet::createTransition(const string & name)
  {
    return *new Transition(*this, name.empty() ? getUniqueNodeName("t") : name);
  }


  /*!
   */
  bool PetriNet::containsNode(Node & node) const
  {
    return nodes_.find(&node) != nodes_.end();
  }


  /*!
   */
  bool PetriNet::containsNode(const string & name) const
  {
    return findNode(name) != NULL;
  }


  /*!
   */
  Node * PetriNet::findNode(const string & name) const
  {
    map<string, Node *>::const_iterator result = nodesByName_.find(name);

    if (result != nodesByName_.end())
      return result->second;

    return NULL;
  }


  /*!
   * \return  a pointer to the place or a NULL pointer if the place was not 
   *          found.
   */
  Place * PetriNet::findPlace(const string & name) const
  {
    // if we had a map placesByName we could avoid the dynamic cast here
    return dynamic_cast<Place *>(findNode(name));
  }


  /*!
   * \return  a pointer to the transition or a NULL pointer if the transition 
   *          was not found.
   */
  Transition * PetriNet::findTransition(const string & name) const
  {
    // if we had a map transitionsByName we could avoid the dynamic cast here
    return dynamic_cast<Transition *>(findNode(name));
  }


  /*!
   */
  Arc * PetriNet::findArc(const Node & source, const Node & target) const
  {
    for (set<Arc *>::iterator it = arcs_.begin(); it != arcs_.end(); ++it)
      if (&(*it)->getSourceNode() == &source && 
	  &(*it)->getTargetNode() == &target)
	return *it;

    return NULL;
  }


  /*!
   */
  const set<Node *> & PetriNet::getNodes() const 
  {
    return nodes_;
  }


  /*!
   */
  const set<Place *> & PetriNet::getPlaces() const
  {
    return places_;
  }


  /*!
   */
  const set<Place *> & PetriNet::getInputPlaces() const
  {
    return inputPlaces_;
  }


  /*!
   */
  const set<Place *> & PetriNet::getOutputPlaces() const
  {
    return outputPlaces_;
  }


  /*!
   */
  const set<Place *> & PetriNet::getInterfacePlaces() const
  {
    return interfacePlaces_;
  }


  /*!
   */
  const set<Transition *> & PetriNet::getTransitions() const
  {
    return transitions_;
  }


  /*!
   * The actual function to merge two places. Given places p1 and p2:
   * -# a new place p12 with empty history is created
   * -# this place gets the union of the histories of place p1 and p2
   * -# the presets and postsets of p1 and p2 are calculated and united
   * -# p12 is connected with all the transitions in the preset and postset
   * -# the places p1 and p2 are removed
   *
   * \param   p1  first place
   * \param   p2  second place
   *
   * \pre     p1 and p2 are internal places.
   * \post    Places p1 and p2 removed.
   * \post    Place p12 having the incoming and outgoing arcs of p1 and p2 and
   *          the union of the histories of t1 and t2.
   */
  void PetriNet::mergePlaces(Place & p1, Place & p2)
  {
    assert(&p1.getPetriNet() == &p2.getPetriNet());
    assert(p1.getType() == Node::INTERNAL);
    assert(p2.getType() == Node::INTERNAL);

    mergeArcs(p1, p2, p1.getPreset(), p2.getPreset(), false);
    mergeArcs(p1, p2, p1.getPostset(), p2.getPostset(), true);

    // FIXME: what about the final condition formula?

    p1.merge(p2);
  }


  void PetriNet::mergeArcs(Place & p1, Place & p2, 
			   const set<Node *> & set1, const set<Node *> & set2, 
			   bool isPostset)
  {
    for (set<Node *>::iterator it = set2.begin(); it != set2.end(); ++it)
      {
	Node & p1Source = isPostset ? p1   : **it;
	Node & p2Source = isPostset ? p2   : **it;
	Node & p1Target = isPostset ? **it : p1;
	Node & p2Target = isPostset ? **it : p2;

	Arc * arc1 = findArc(p1Source, p1Target);
	Arc * arc2 = findArc(p2Source, p2Target);

	assert(arc2 != NULL);
	if (arc1 == NULL)
	  createArc(p1Source, p1Target, arc2->getWeight());
	else
	  arc1->setWeight(arc1->getWeight() + arc2->getWeight());
      }
  }


  /*!
   * Given a second Petri net #net, the internal structure is added and input
   * and output places are connected appropriatly (if an input and an output
   * place name of the two nets match).
   */
  PetriNet & PetriNet::operator+=(const PetriNet & net)
  {
    Place * p;

    // add all transitions of the net
    for (set<Transition *>::iterator it = net.transitions_.begin(); 
	 it != net.transitions_.end(); ++it)
      new Transition(*this, **it);

    // add all non-existing places and merge existing ones
    for (set<Place *>::iterator it = net.places_.begin(); 
	 it != net.places_.end(); ++it)
      {
	p = findPlace((*it)->getName());
	if (p == NULL)
	  new Place(*this, **it);
	else
	  {
	    assert((p->getType() == Place::INPUT && 
		    (*it)->getType() == Place::OUTPUT) ||
		   (p->getType() == Place::OUTPUT &&
		    (*it)->getType() == Place::INPUT));
	    p->merge(**it);
	    p->internalize();
	  }
      }

    // create arcs according to the arcs in the net
    for (set<Arc *>::iterator it = net.arcs_.begin(); it != net.arcs_.end(); 
	 ++it)
      new Arc(*this, **it);

    // FIXME: calculate FINAL stuff

    return *this;
  }


  /*!
   * \param   base  base name
   * \return  a string to be used as a name for a new node
   */
  string PetriNet::getUniqueNodeName(const string & base) const
  {
    int i = 0;
    string name = base;

    // use a "mutable" cache to make this more efficient
    while (nodesByName_.find(name) != nodesByName_.end())
      name = base + util::toString(++i);

    return name;
  }


  /*!
   */
  void PetriNet::updateNodeNameHistory(Node & node, 
				       const deque<string> & oldHistory)
  {
    assert(containsNode(node));
    
    finalizeNodeNameHistory(node, oldHistory);
    initializeNodeNameHistory(node);
  }


  /*!
   */
  void PetriNet::updatePlaceType(Place & place, Node::Type type)
  {
    assert(containsNode(place));

    finalizePlaceType(place, type);
    initializePlaceType(place);
  }



  /****************************************************************************
   *** Private PetriNet Function Definitions
   ***************************************************************************/


  /*!
   * All Places and Transitions of the net are prefixed.
   *
   * \param   prefix  the prefix to be added
   */
  PetriNet & PetriNet::prefixNodeNames(const string & prefix)
  {
    for (set<Node *>::iterator it = nodes_.begin(); it != nodes_.end(); ++it)
      (*it)->prefixNameHistory(prefix);
    return *this;
  }


  void PetriNet::updateArcs(Arc & arc)
  {
    assert(&arc.getPetriNet() == this);
    assert(arcs_.find(&arc) == arcs_.end());
    assert(findArc(arc.getSourceNode(), arc.getTargetNode()) == NULL);

    arcs_.insert(&arc);
  }


  void PetriNet::updatePlaces(Place & place)
  {
    updateNodes(place);
    places_.insert(&place);
    initializePlaceType(place);
  }

  
  void PetriNet::updateTransitions(Transition & trans)
  {
    updateNodes(trans);
    transitions_.insert(&trans);
  }


  void PetriNet::updateNodes(Node & node)
  {
    assert(&node.getPetriNet() == this);
    assert(!containsNode(node));
    assert(nodesByName_.find(node.getName()) == nodesByName_.end());

    nodes_.insert(&node);
    initializeNodeNameHistory(node);
  }


  void PetriNet::deletePlace(Place & place)
  {
    finalizePlaceType(place);
    places_.erase(&place);
    deleteNode(place);
  }


  void PetriNet::deleteTransition(Transition & trans)
  {
    transitions_.erase(&trans);
    deleteNode(trans);
  }


  /*!
   * \note  Never call this method directly! Use deletePlace() or 
   *        deleteTransition() instead!
   */
  void PetriNet::deleteNode(Node & node)
  {
    assert(containsNode(node));
    assert(dynamic_cast<Place *>(&node) == NULL ? true : 
	   places_.find(dynamic_cast<Place *>(&node)) == places_.end());
    assert(dynamic_cast<Transition *>(&node) == NULL ? true : 
	   transitions_.find(dynamic_cast<Transition *>(&node)) == 
	   transitions_.end());

    // FIXME: arcs and pre-/postsets!
    
    finalizeNodeNameHistory(node, node.getNameHistory());
    nodes_.erase(&node);

    delete &node;
  }


  void PetriNet::initializeNodeNameHistory(Node & node)
  {
    deque<string> history = node.getNameHistory();
    for (deque<string>::iterator it = history.begin(); it != history.end(); 
	 ++it)
      {
	assert((nodesByName_.find(*it))->second == &node ||
	       nodesByName_.find(*it) == nodesByName_.end());
	nodesByName_[*it] = &node;
      }
  }


  void PetriNet::finalizeNodeNameHistory(Node & node, 
					 const deque<string> & history)
  {
    for (deque<string>::const_iterator it = history.begin(); 
	 it != history.end(); ++it)
      nodesByName_.erase(*it);
  }


  void PetriNet::initializePlaceType(Place & place)
  {
    switch (place.getType())
      {
      case Node::INTERNAL: 
	internalPlaces_.insert(&place); break;
      case Node::INPUT:    
	inputPlaces_.insert(&place);
	interfacePlaces_.insert(&place);
	break;
      case Node::OUTPUT:   
	outputPlaces_.insert(&place); 
	interfacePlaces_.insert(&place);
	break;
      }
  }


  void PetriNet::finalizePlaceType(Place & place)
  {
    finalizePlaceType(place, place.getType());
  }


  void PetriNet::finalizePlaceType(Place & place, Node::Type type)
  {
    switch (type)
      {
      case Node::INTERNAL: 
	internalPlaces_.erase(&place); break;
      case Node::INPUT:    
	inputPlaces_.erase(&place); 
	interfacePlaces_.erase(&place);
	break; 
      case Node::OUTPUT:   
	outputPlaces_.erase(&place);
	interfacePlaces_.erase(&place);
	break;
      }
  }


  
  /***************************** NOT YET REFACTORED *************************/



/*!
 * \brief   checks if the transition is normal
 *
 * \note    This is a help method for normalize method
 *          in class PetriNet.
 */
bool Transition::isNormal() const
{
  int counter = 0;

  for (set<Node *>::const_iterator p = getPreset().begin(); p != getPreset().end(); p++)
    if ((*p)->getType() != Node::INTERNAL)
      counter++;
  for (set<Node *>::const_iterator p = getPostset().begin(); p != getPostset().end(); p++)
    if ((*p)->getType() != Node::INTERNAL)
      counter++;

  return counter <= 1;
}







/*!
 * \brief   mark the place
 *
 * \param   my_tokens  the number of tokens of this place in the inital
 *                     marking
 */
void Place::mark(unsigned int my_tokens)
{
  tokens_ = my_tokens;
}




/*!
 * \brief   destructor
 */
/*
Arc::~Arc()
{
  assert(source != NULL);
  assert(target != NULL);

  // FIXME
  //source->postset.erase(target);
  //target->preset.erase(source);
}
*/




/*!
 * \brief   swaps source and target node of the arc
 */
void Arc::mirror()
{
  /* FIXME
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
  */
}





/*****************************************************************************/












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
/*
Place *PetriNet::newPlace(string my_role, communication_type my_type, unsigned int capacity, string my_port)
{
  string my_role_with_suffix = my_role;

  if (my_role != "" && !forEach_suffix.empty())
    my_role_with_suffix += ("." + forEach_suffix[0]);

  Place *p = NULL; //new Place(*this, getId(), my_role_with_suffix, my_type);
  assert(p != NULL);
  //FIXME: p->capacity = capacity;

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
*/




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
  /*
  if (t->getType() != Node::INTERNAL)
  {
    set<Node *> pset;

    switch (t->getType())
      {
      case Node::INPUT:
      {
        pset = t->getPreset();
        t->setType(Node::INTERNAL);
        for (set<Node *>::iterator p = pset.begin(); p != pset.end(); p++)
          if ((*p)->getType() == Node::INPUT)
          {
            t->setType((*p)->getType());
            break;
          }
        break;
      }
      case Node::OUTPUT:
      {
        pset = t->getPostset();
        t->setType(Node::INTERNAL);
        for (set<Node *>::iterator p = pset.begin(); p != pset.end(); p++)
          if ((*p)->getType() == Node::OUTPUT)
          {
            t->setType((*p)->getType());
            break;
          }
        break;
      }
      case INOUT:
      {
        pset = t->getPreset();
        t->setType(Node::INTERNAL);
        for (set<Node *>::iterator p = pset.begin(); p != pset.end(); p++)
          if ((*p)->getType() == Node::INPUT)
          {
            t->setType((*p)->getType());
            break;
          }

        pset = t->getPostset();
        for (set<Node *>::iterator p = pset.begin(); p != pset.end(); p++)
          if ((*p)->getType() == Node::OUTPUT)
          {
            if (t->getType() == Node::INTERNAL)
              t->setType((*p)->getType());
            else
              ; // FIXME: t->setType(INOUT);
            break;
          }
        break;
	}
      default: break;
    }
  }
      */
}





/*!
 * \brief   returns the set of all nodes
 */


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
void PetriNet::mergeTransitions(Transition & t1, Transition & t2)
{
  /*
  if (t1 == t2)
    return;

  assert(t1 != NULL);
  assert(t2 != NULL);

  // variable needed for correct arc weights
  //bool sametarget = false;
  // container for arcs to be deleted
  set<Arc *>::iterator delArc;
  // the merged transition
  Node *t12 = newTransition("");

  // organize the communication type of the new transition
  if (t1->getType() == t2->getType())
  {
    t12->setType(t1->getType());
  }
  else if ((t1->getType() == Node::INPUT && t2->getType() == Node::INTERNAL) ||
           (t1->getType() == Node::INTERNAL && t2->getType() == Node::INPUT))
  {
    t12->setType(Node::INPUT);
  }
  else if ((t1->getType() == Node::INTERNAL && t2->getType() == Node::OUTPUT) ||
           (t1->getType() == Node::OUTPUT && t2->getType() == Node::INTERNAL))
  {
    t12->setType(Node::OUTPUT);
  }
  else if ((t1->getType() == Node::OUTPUT && t2->getType() == Node::INPUT) ||
           (t1->getType() == Node::INPUT && t2->getType() == Node::OUTPUT) ||
           (t1->getType() == INOUT || t2->getType() == INOUT))
  {
    ; // FIXME: t12->setType(INOUT);
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

  roleMap[t1->getName()] = t12;
  roleMap[t2->getName()] = t12;

  // merge pre- and postsets for t12
  // FIXME
  //t12->preset=setUnion(t1->getPreset(), t2->getPreset());
  //t12->postset=setUnion(t1->getPostset(), t2->getPostset());

  // create the weighted arcs for t12

  // this is the preset of t12 without the preset of t1. It is needed not generate double arcs if
  // the preset of t1 and t2 were not distinct.
  set<Node *> preset2without1 = util::setDifference(t12->getPreset(),t1->getPreset());


  // create new arcs from the t1 preset to t12
  for (set<Node *>::iterator n = t1->getPreset().begin(); n != t1->getPreset().end(); n++)
    newArc((*n), t12, STANDARD, arc_weight((*n),t1));

  // create new arcs from the preset of t2 to t12 without those, that have been already covered by the preset of t1
  for (set<Node *>::iterator n = preset2without1.begin(); n != preset2without1.end(); n++)
    newArc((*n), t12, STANDARD, arc_weight((*n),t2));

  // create new arcs from t12 to postset of t1
  for (set<Node *>::iterator n = t1->getPostset().begin(); n != t1->getPostset().end(); n++)
    newArc(t12, (*n), STANDARD, arc_weight(t1,(*n)));

  // create new arcs from t12 to postset of t2 and adjust arcweights if a node of postset of t2 is also in postset of t1
  for (set<Node *>::iterator n = t2->getPostset().begin(); n != t2->getPostset().end(); n++)
  {
    // Find an arc already created in because of the postset of t1
    for (set<Arc *>::iterator f = F.begin(); f != F.end(); f++)
    {
      if ((&(*f)->getSourceNode() == t12) && (&(*f)->getTargetNode() == (*n)))
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

  deleteTransition(*t1);
  deleteTransition(*t2);
  */
}











/*!
 * \brief   returns true iff all arcs connecting to n have the same weight
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
  /*
  assert(n != NULL);
  bool first = true;
  unsigned int w = 0;

  for (set<Arc*>::iterator f = F.begin(); f != F.end(); f++)
  {
    if ((&(*f)->getSourceNode() == n) || (&(*f)->getTargetNode() == n) )
    {
      if (first)
      {
        first=false;
        w = (*f)->getWeight();
      }
      else
      {
        if ( (*f)->getWeight() != w)
        {
          return false;
        }
      }
    }
  }
  */
  return true;
}





/*---------------------------------------------------------------------------*/




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
  for (set<Place *>::iterator place = inputPlaces_.begin(); place != inputPlaces_.end(); place ++)
    places_.insert(*place);

  for (set<Place *>::iterator place = outputPlaces_.begin(); place != outputPlaces_.end(); place ++)
    places_.insert(*place);

  // empty the set of input and output places
  inputPlaces_.clear(); inputPlaces_ = set< Place * >();
  outputPlaces_.clear(); outputPlaces_ = set< Place * >();
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
  for (set<Place *>::iterator p = inputPlaces_.begin(); p != inputPlaces_.end(); p++)
  {
    set<unsigned int> receiving_activities;

    for (set<Node *>::iterator t = (*p)->getPostset().begin(); t != (*p)->getPostset().end(); t++)
      for (unsigned int i = 0; i < (*t)->history.size(); i++)
      {
        // unsigned int transition_activity_id = toUInt((*t)->history[i].substr(0, (*t)->history[i].find_first_of(".")));
        string act_id = (*t)->getName().substr(0, (*t)->getName().find_first_of("."));

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
    for (set<Place *>::iterator p = outputPlaces_.begin(); p != outputPlaces_.end(); p++)
    {
      set<unsigned int> sending_activities;

      for (set<Node *>::iterator t = (*p)->getPreset().begin(); t != (*p)->getPreset().end(); t++)
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
 *          Swaps the input and output places, i.e. swaps the sets #inputPlaces_ and
 *          #outputPlaces_ and the adjacent arcs.
 *
 *  \test	This method has been tested in tests/test_mirror.cc.
 */
void PetriNet::mirror()
{
  // swap arcs
  for (set<Arc*>::iterator f = arcs_.begin(); f != arcs_.end(); f++)
  {
    if ( inputPlaces_.find( findPlace( ((*f)->getSourceNode()).getName()) ) != inputPlaces_.end() )
      (*f)->mirror();
    else if ( outputPlaces_.find( findPlace( ((*f)->getTargetNode()).getName()) ) != outputPlaces_.end() )
      (*f)->mirror();
  }

  // swap input and output places
  set<Place *> inputPlaces__old = inputPlaces_;
  set<Place *> outputPlaces__old = outputPlaces_;
  inputPlaces_ = outputPlaces__old;
  outputPlaces_ = inputPlaces__old;
  for (set<Place *>::iterator p = inputPlaces_.begin(); p != inputPlaces_.end(); p++)
    ;//(*p)->setType(Node::INPUT);
  for (set<Place *>::iterator p = outputPlaces_.begin(); p != outputPlaces_.end(); p++)
    ;//(*p)->setType(Node::OUTPUT);

  // swap the transitions
  for (set<Transition *>::iterator t = transitions_.begin(); t != transitions_.end(); t++)
  {
    if ( (*t)->getType() == Node::OUTPUT )
      ;//(*t)->setType(Node::INPUT);
    else if ( (*t)->getType() == Node::INPUT )
      ;//(*t)->setType(Node::OUTPUT);
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
  /* FIXME
  // the constraint oWFN must have an empty interface
  assert(net.inputPlaces_.empty());
  assert(net.outputPlaces_.empty());


  // copy the constraint oWFN's places to the oWFN
  for (set<Place *>::iterator p = net.places_.begin(); p != net.places_.end(); p++)
  {
    Place *p_new = newPlace((*p)->getName());

    p_new->isFinal = (*p)->isFinal;
    p_new->tokens = (*p)->tokens;
  }


  // copy the constraint oWFN's unlabeled transitions to the oWFN
  for (set<Transition *>::iterator t = net.transitions_.begin(); t != net.transitions_.end(); t++)
  {
    if ( (*t)->getLabels().empty())
    {
      //Transition *t_new = newTransition((*t)->getName());

      // copy the arcs of the constraint oWFN
      for (set< Arc * >::iterator arc = net.arcs_.begin(); arc != net.arcs_.end(); arc ++)
      {
        if ( ( (*arc)->getSourceNode()->nodeType == PLACE) && ( (*arc)->getTargetNode() == static_cast<Node *>(*t) ) )
          newArc( findPlace( (*arc)->getSourceNode()->getName() ), t_new, STANDARD, (*arc)->getWeight() );
        if ( ( (*arc)->getTargetNode()->nodeType == PLACE) && ( (*arc)->getSourceNode() == static_cast<Node *>(*t) ) )
          newArc( t_new, findPlace( (*arc)->getTargetNode()->getName() ), STANDARD, (*arc)->getWeight() );
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
  for (set<Transition *>::iterator t = net.transitions_.begin(); t != net.transitions_.end(); t++)
  {
    set<string> labels = (*t)->getLabels();
    for (set<string>::iterator label = labels.begin(); label != labels.end(); label++)
    {
      Transition *t_l = findTransition(*label);

      // specified transition not found -- trying places instead
      if (t_l == NULL)
      {
        Place *p = findPlace(*label);
        if (p != NULL)
        {
          set<Node *> transitions_p;

          if (p->getType() == Node::INPUT)
            transitions_p = p->getPostset();
          else
            transitions_p = p->getPreset();

          for (set<Node *>::iterator pre_transition = transitions_p.begin(); pre_transition != transitions_p.end(); pre_transition++)
          {
            used_labels.insert((*pre_transition)->getName());
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
    // Transition *t_new = newTransition("(" + tp->first->getName() + "," + tp->second->getName() + ")");
    //Transition *t_new = newTransition(tp->first->getName() + "_" + tp->second->getName());

    // copy the arcs of the constraint oWFN
    for (set< Arc * >::iterator arc = net.arcs_.begin(); arc != net.arcs_.end(); arc ++)
    {
      if ( ( (*arc)->getSourceNode()->nodeType == PLACE) && ( (*arc)->getTargetNode() == static_cast<Node *>(tp->second) ) )
        newArc( findPlace( (*arc)->getSourceNode()->getName() ), t_new, STANDARD, (*arc)->getWeight() );
      if ( ( (*arc)->getTargetNode()->nodeType == PLACE) && ( (*arc)->getSourceNode() == static_cast<Node *>(tp->second) ) )
        newArc( t_new, findPlace( (*arc)->getTargetNode()->getName() ), STANDARD, (*arc)->getWeight() );
    }

    // copy the arcs of the oWFN
    for (set< Arc * >::iterator arc = arcs_.begin(); arc != arcs_.end(); arc ++)
    {
      if ( ( (*arc)->getSourceNode()->nodeType == PLACE) && ( (*arc)->getTargetNode() == static_cast<Node *>(tp->first) ) )
        newArc( findPlace( (*arc)->getSourceNode()->getName() ), t_new, STANDARD, (*arc)->getWeight() );
      if ( ( (*arc)->getTargetNode()->nodeType == PLACE) && ( (*arc)->getSourceNode() == static_cast<Node *>(tp->first) ) )
        newArc( t_new, findPlace( (*arc)->getTargetNode()->getName() ), STANDARD, (*arc)->getWeight() );
    }
  }


  // remove transitions that are used as labels
  for (set<string>::iterator t = used_labels.begin(); t != used_labels.end(); t++)
    deleteTransition(*findTransition(*t));
  */
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
  //Transition *t = newTransition("antideadlock");

  for (set<Place *>::iterator place = places_.begin(); place != places_.end(); place++)
  {
    /* FIXME
    if ( (*place)->isFinal )
      newArc(t, *place, READ);
    */
  }
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
	/* FIXME

  std::string suffix = "_normalized";
  set<Place *> temp;
  set<Place *> interface = getInterfacePlaces();

  for (set<Place *>::iterator place = interface.begin(); place
      != interface.end(); place++)
  {
    set<Node *> neighbors = util::setUnion((*place)->getPreset(), (*place)->getPostset());
    for (set<Node *>::iterator transition = neighbors.begin();
        transition != neighbors.end(); transition++)
    {
      Transition *t = static_cast<Transition *>(*transition);
      while (!t->isNormal())
      {
        // create new internal place from interface place
        Place *newP = newPlace((*place)->getName() + suffix);
        assert(newP != NULL);

        // create new interface place
        Place *newPin = 
	  &createPlace("i_" + (*place)->getName(), (*place)->getType());
        assert(newPin != NULL);

        // create new transition
        Transition *newT = newTransition("t_" + (*place)->getName());
        assert(newT != NULL);

        // set arcs (1, 2 & 3)
        if ((*place)->getType() == Node::INPUT)
        {
          Arc *f1 = newArc(newPin, newT);
          assert(f1 != NULL);
          Arc *f2 = newArc(newT, newP);
          assert(f2 != NULL);
          for (set<Arc *>::iterator f = arcs_.begin(); f != arcs_.end(); f++)
          {
            if (&(*f)->getSourceNode() == (*place))
            {
              Arc *f3 = newArc(newP, &(*f)->getTargetNode());
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
          for (set<Arc *>::iterator f = arcs_.begin(); f != arcs_.end(); f++)
          {
            if (&(*f)->getTargetNode() == (*place))
            {
              Arc *f3 = newArc(&(*f)->getSourceNode(), newP);
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
	*/
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
  //FIXME: return finalcondition->evaluate(m);
  return false;
}




/*!
 * \brief   deletes all interface places
 */
void PetriNet::makeInnerStructure()
{
    // deletes all IN-places
    for(set<Place *>::iterator place = inputPlaces_.begin(); place != inputPlaces_.end(); place = inputPlaces_.begin())
    {
        deletePlace(**place);
    }

    // deletes all OUT-places
    for(set<Place *>::iterator place = outputPlaces_.begin(); place != outputPlaces_.end(); place = outputPlaces_.begin())
    {
        deletePlace(**place);
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
  for (set<Place *>::const_iterator p = places_.begin(); p != places_.end(); p++)
  {
    if ((*p)->getPreset().empty())
    {
      if (first == NULL)
        first = *p;
      else
      {
        std::cerr << "This net is no workflow net because there are more than one place with empty preset!\n";
        return false;
      }
    }
    if ((*p)->getPostset().empty())
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
  Transition *tarjan = &createTransition("tarjan");
  //FIXME
  //newArc(last, tarjan);
  //  newArc(tarjan, first);

  // each 2 nodes \f$x,y \in P \cup T\f$ are in a directed cycle
  // (strongly connected net using tarjan's algorithm)
  unsigned int i = 0; ///< count index
  map<Node *, int> index; ///< index property for nodes
  map<Node *, unsigned int> lowlink; ///< lowlink property for nodes
  set<Node *> stacked; ///< the stack indication for nodes
  stack<Node *> S; ///< stack used by Tarjan's algorithm

  // set all nodes' index values to ``undefined''
  for (set<Place *>::const_iterator p = places_.begin(); p != places_.end(); p++)
    index[*p] = (-1);
  for (set<Transition *>::const_iterator t = transitions_.begin(); t != transitions_.end(); t++)
    index[*t] = (-1);

  // getting the number of strongly connected components reachable from first
  unsigned int sscCount = dfsTarjan(first, S, stacked, i, index, lowlink);

  std::cout << "\nstrongly connected components: " << sscCount << "\n\n";

  deleteTransition(*tarjan);

  // check set $P \cup T$
  set<Node *> nodeSet;
  for (set<Place *>::const_iterator p = places_.begin(); p != places_.end(); p++)
    nodeSet.insert(*p);
  for (set<Transition *>::const_iterator t = transitions_.begin(); t != transitions_.end(); t++)
    nodeSet.insert(*t);

  /* true iff only one strongly connected component found from first and all nodes
   * of $\mathcal{N}$ are members of this component
   */
  if (sscCount == 1 && util::setDifference(nodeSet, stacked).empty())
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
  std::cout << n->getName() << " stacked, ";
  for (set<Node *>::const_iterator nn = n->getPostset().begin(); nn != n->getPostset().end(); nn++)
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
      std::cout << S.top()->getName() << ", ";
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
  for (set<Transition *>::iterator t = transitions_.begin(); t != transitions_.end(); t++)
    for (set<Transition *>::iterator tt = transitions_.begin(); tt != transitions_.end(); tt++)
    {
      set<Node *> t_pre  = (*t)->getPreset();
      set<Node *> tt_pre = (*tt)->getPreset();
      if ((t_pre != tt_pre) && !(util::setIntersection(t_pre, tt_pre).empty()))
        return false;
    }
  return true;
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
  /* FIXME
  unsigned int placeCount = initMarking(); // initializes the places and returns the number of places
  Marking *m = new Marking(placeCount, 0);

  for (set<Place *>::const_iterator p = places_.begin(); p != places_.end(); p++)
    (*m)[(*p)->marking_id] = (*p)->tokens;
  for (set<Place *>::const_iterator p = inputPlaces_.begin(); p != inputPlaces_.end(); p++)
    (*m)[(*p)->marking_id] = (*p)->tokens;
  for (set<Place *>::const_iterator p = outputPlaces_.begin(); p != outputPlaces_.end(); p++)
    (*m)[(*p)->marking_id] = (*p)->tokens;

  return *m;
  */

  return *new Marking();
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
  /* FIXME
  for (set<Place *>::iterator p = places_.begin(); p != places_.end(); p++)
    (*p)->tokens = m[(*p)->marking_id];
  for (set<Place *>::iterator p = inputPlaces_.begin(); p != inputPlaces_.end(); p++)
      (*p)->tokens = m[(*p)->marking_id];
  for (set<Place *>::iterator p = outputPlaces_.begin(); p != outputPlaces_.end(); p++)
      (*p)->tokens = m[(*p)->marking_id];
  */
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
  for (set<Arc *>::const_iterator f = arcs_.begin(); f != arcs_.end(); f++)
    if (&(*f)->getTargetNode() == t)
    {
      //Place *p = static_cast<Place *>((*f)->getSourceNode());
      /* FIXME
      if ((*mm)[p->marking_id] >= (*f)->getWeight())
        activators.insert(*f);
      else
        break;
      */
    }

  // if m does not activate t then m' = m
  if (t->getPreset().size() != activators.size())
    return *mm;

  /* FIXME
  for (set<Arc *>::const_iterator f = arcs_.begin(); f != arcs_.end(); f++)
    if ((*f)->getSourceNode() == t)
    {
      Place *p = static_cast<Place *>((*f)->getTargetNode());
      (*mm)[p->marking_id] += (*f)->getWeight();
    }

  for (set<Arc *>::iterator f = activators.begin(); f != activators.end(); f++)
  {
    Place *p = static_cast<Place *>((*f)->getSourceNode());
    (*mm)[p->marking_id] -= (*f)->getWeight();
  }
  */

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

  for (set<Arc *>::const_iterator f = arcs_.begin(); f != arcs_.end(); f++)
  {
    //Node *n = (*f)->getSourceNode();
    /* FIXME
    if (n->nodeType == PLACE)
    {
      Place *p = static_cast<Place *>((*f)->getSourceNode());
      if (m[p->marking_id] >= (*f)->getWeight())
      {
        result = static_cast<Transition *>((*f)->getTargetNode());
        break;
      }
    }
    */
  }

  return result;
}




}
