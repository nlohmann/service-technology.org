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

#ifndef NDEBUG
#include <iostream>
#include <fstream>
using std::cout;
using std::cerr;
using std::endl;
#endif

#include <cassert>
#include <sstream>

#include "link.h"
#include "util.h"
#include "petrinet.h"

using std::pair;
using std::multimap;
using std::vector;
using std::stringstream;
using std::ostringstream;
using std::deque;

namespace pnapi
{

  /****************************************************************************
   *** Class ComponentObserver Function Defintions
   ***************************************************************************/

  /*!
   */
  ComponentObserver::ComponentObserver(PetriNet & net) :
    net_(net)
  {
    assert(&net.observer_ == this);
  }


  /*!
   */
  PetriNet & ComponentObserver::getPetriNet() const
  {
    return net_;
  }


  /*!
   */
  void ComponentObserver::updateNodeNameHistory(Node & node,
					  const std::deque<string> & oldHistory)
  {
    assert(net_.containsNode(node));

    finalizeNodeNameHistory(node, oldHistory);
    initializeNodeNameHistory(node);
  }


  /*!
   */
  void ComponentObserver::updatePlaceType(Place & place, Node::Type type)
  {
    assert(net_.containsNode(place));

    finalizePlaceType(place, type);
    initializePlaceType(place);
  }


  void ComponentObserver::updateArcCreated(Arc & arc)
  {
    assert(&arc.getPetriNet() == &net_);
    assert(net_.arcs_.find(&arc) == net_.arcs_.end());
    assert(net_.findArc(arc.getSourceNode(), arc.getTargetNode()) == NULL);

    // update pre- and postsets
    net_.arcs_.insert(&arc);
    arc.getTargetNode().preset_.insert(&arc.getSourceNode());
    arc.getSourceNode().postset_.insert(&arc.getTargetNode());

    // update transition type
    arc.getTransition().updateType();
  }


  void ComponentObserver::updateArcRemoved(Arc & arc)
  {
    assert(&arc.getPetriNet() == &net_);
    assert(net_.arcs_.find(&arc) == net_.arcs_.end());
    assert(net_.findArc(arc.getSourceNode(), arc.getTargetNode()) == NULL);

    // update pre- and postsets
    net_.arcs_.erase(&arc);
    arc.getTargetNode().preset_.erase(&arc.getSourceNode());
    arc.getSourceNode().postset_.erase(&arc.getTargetNode());

    // update transition type
    arc.getTransition().updateType();
  }


  void ComponentObserver::updatePlaces(Place & place)
  {
    updateNodes(place);
    net_.places_.insert(&place);
    initializePlaceType(place);
  }


  void ComponentObserver::updateTransitions(Transition & trans)
  {
    updateNodes(trans);
    net_.transitions_.insert(&trans);
  }


  void ComponentObserver::updateNodes(Node & node)
  {
    assert(&node.getPetriNet() == &net_);
    assert(!net_.containsNode(node));
    assert(net_.nodesByName_.find(node.getName()) == net_.nodesByName_.end());

    net_.nodes_.insert(&node);
    initializeNodeNameHistory(node);
  }


  void ComponentObserver::updateNodesMerged(Node & node1, Node & node2)
  {
    //assert(node2.getNameHistory().empty());

    // delete node2 from net
    Place * place = dynamic_cast<Place *>(&node2);
    if (place != NULL)
      net_.deletePlace(*place);
    else
      net_.deleteTransition(*dynamic_cast<Transition *>(&node2));
  }


  void ComponentObserver::initializeNodeNameHistory(Node & node)
  {
    deque<string> history = node.getNameHistory();
    deque<string>::iterator it = history.begin();
    //for (deque<string>::iterator it = history.begin(); it != history.end();
    //     ++it)
    {
      assert((net_.nodesByName_.find(*it))->second == &node ||
	     net_.nodesByName_.find(*it) == net_.nodesByName_.end());
      net_.nodesByName_[*it] = &node;
    }
  }


  void ComponentObserver::finalizeNodeNameHistory(Node & node,
						  const deque<string> & history)
  {
    deque<string>::const_iterator it = history.begin();
    //for (deque<string>::const_iterator it = history.begin();
    //     it != history.end(); ++it)
    net_.nodesByName_.erase(*it);
  }


  void ComponentObserver::initializePlaceType(Place & place)
  {
    string port = place.getPort();

    switch (place.getType())
      {
      case Node::INTERNAL:
	assert(port.empty());
	net_.internalPlaces_.insert(&place);
	break;
      case Node::INPUT:
	net_.inputPlaces_.insert(&place);
	net_.interfacePlaces_.insert(&place);
	if (!port.empty())
	  net_.interfacePlacesByPort_
	    .insert(pair<string, Place *>(port, &place));
	break;
      case Node::OUTPUT:
	net_.outputPlaces_.insert(&place);
	net_.interfacePlaces_.insert(&place);
	if (!port.empty())
	  net_.interfacePlacesByPort_
	    .insert(pair<string, Place *>(port, &place));
	break;
      default: break;
      }
  }


  void ComponentObserver::finalizePlaceType(Place & place, Node::Type type)
  {
    pair<multimap<string, Place *>::iterator,
      multimap<string, Place *>::iterator> portRange =
      net_.interfacePlacesByPort_.equal_range(place.getPort());

    switch (type)
      {
      case Node::INTERNAL:
	net_.internalPlaces_.erase(&place);
	break;
      case Node::INPUT:
	net_.inputPlaces_.erase(&place);
	net_.interfacePlaces_.erase(&place);
	for (multimap<string, Place *>::iterator it = portRange.first;
	     it != portRange.second; ++it)
	  if (it->second == &place)
	    {
	      net_.interfacePlacesByPort_.erase(it);
	      break;
	    }
	break;
      case Node::OUTPUT:
	net_.outputPlaces_.erase(&place);
	net_.interfacePlaces_.erase(&place);
	for (multimap<string, Place *>::iterator it = portRange.first;
	     it != portRange.second; ++it)
	  if (it->second == &place)
	    {
	      net_.interfacePlacesByPort_.erase(it);
	      break;
	    }
	break;
      default: break;
      }
  }



  /****************************************************************************
   *** Class PetriNet Function Defintions
   ***************************************************************************/

  /*!
   * \note    The condition is standardly set to True.
   */
  PetriNet::PetriNet() :
    observer_(*this)
  {
  }


  /*!
   * The copy constructor with deep copy.
   *
   * \note    The condition is standardly set to True.
   */
  PetriNet::PetriNet(const PetriNet & net) :
    observer_(*this),
    condition_(net.condition_, copyStructure(net)),
    meta_(net.meta_)
  {
  }


  /*!
   * The destructor of the PetriNet class.
   */
  PetriNet::~PetriNet()
  {
    clear();

    assert(nodes_.empty());
    assert(nodesByName_.empty());
    assert(transitions_.empty());
    assert(places_.empty());
    assert(internalPlaces_.empty());
    assert(inputPlaces_.empty());
    assert(outputPlaces_.empty());
    assert(interfacePlaces_.empty());
    assert(interfacePlacesByPort_.empty());
    assert(arcs_.empty());
  }


  void PetriNet::clear()
  {
    meta_.clear();
    condition_ = true;

    // delete all places
    set<Place *> places = places_;
    for (set<Place *>::iterator it = places.begin(); it != places.end(); ++it)
      deletePlace(**it);

    // delete all transitions
    set<Transition *> transitions = transitions_;
    for (set<Transition *>::iterator it = transitions.begin();
	 it != transitions.end(); ++it)
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
   * Adds all nodes and arcs of the second net.
   *
   * \pre   the node names are unique (disjoint name sets); use
   *        prefixNodeNames() on <em>both</em> nets to achieve this
   */
  map<const Place *, const Place *>
  PetriNet::copyStructure(const PetriNet & net)
  {
    // add all transitions of the net
    for (set<Transition *>::iterator it = net.transitions_.begin();
	 it != net.transitions_.end(); ++it)
      {
	assert(!containsNode((*it)->getName()));
	new Transition(*this, observer_, **it);
      }

    // add all places
    map<const Place *, const Place *> placeMapping;
    for (set<Place *>::iterator it = net.places_.begin();
	 it != net.places_.end(); ++it)
      {
	assert(!containsNode((*it)->getName()));
	placeMapping[*it] = new Place(*this, observer_, **it);
      }

    // create arcs according to the arcs in the net
    for (set<Arc *>::iterator it = net.arcs_.begin(); it != net.arcs_.end();
	 ++it)
      new Arc(*this, observer_, **it);

    return placeMapping;
  }


  /*!
   */
  Condition & PetriNet::finalCondition()
  {
    return condition_;
  }


  /*!
   */
  const Condition & PetriNet::finalCondition() const
  {
    return condition_;
  }


  /*!
   * Given a second Petri net #net, the internal structure is added and input
   * and output places are connected appropriatly (if an input and an output
   * place name of the two nets match).
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
	if (place != NULL && place->isComplementType((*it)->getType()))
	  mergePlaces.push_back(pair<Place *, Place *>(place, *it));
      }

    // prefix and combine the nets
    prefixNodeNames(prefix);
    tmpNet.prefixNodeNames(netPrefix);
    const map<const Place *, const Place *> & placeMapping =
      copyStructure(tmpNet);

    // translate references and merge places
    for (vector<pair<Place *, Place *> >::iterator it = mergePlaces.begin();
	 it != mergePlaces.end(); ++it)
      {
	if (&it->first->getPetriNet() != this)
	  it->first = findPlace(it->first->getName());
	else if (&it->second->getPetriNet() != this)
	  it->second = findPlace(it->second->getName());
	it->first->merge(*it->second);
	//deletePlace(*it->second);
      }

    // merge final conditions
    condition_.merge(net.condition_, placeMapping);
  }


  /*!
   */
  void PetriNet::createFromWiring(map<string, vector<PetriNet> > & instances,
				  const map<Place *, LinkNode *> & wiring)
  {
    vector<LinkNode *> wiredNodes;
    wiredNodes.reserve(wiring.size());

    // clean up old net
    clear();

    // add structure of nets
    for (map<string, vector<PetriNet> >::iterator it = instances.begin();
	 it != instances.end(); ++it)
      {
	assert(!it->first.empty());

	for (unsigned int i = 0; i < it->second.size(); i++)
	  {
	    PetriNet & net = it->second[i];
	    stringstream ss; ss << i + 1;
	    map<const Place *, const Place *> placeMapping =
	      copyStructure(net.prefixNodeNames(it->first + "[" + ss.str() +
						"]."));
	    condition_.merge(net.condition_, placeMapping);

	    // translate references in wiring
	    for (map<Place *, LinkNode *>::const_iterator it = wiring.begin();
		 it != wiring.end(); ++it)
	      if (placeMapping.find(it->first) != placeMapping.end())
		wiredNodes.push_back(&wiring.find(it->first)->second
		 ->replacePlace(const_cast<Place &>(*placeMapping
						    .find(it->first)->second)));
	  }
      }

    set<LinkNode *> expanded;
    vector<LinkNode *> result;

    // expand all nodes
    for (vector<LinkNode *>::iterator it = wiredNodes.begin();
	 it != wiredNodes.end(); ++it)
      {
	result = (*it)->expand();
	expanded.insert(result.begin(), result.end());
	delete (*it);  // clean up
      }

    // join one-to-one links
    LinkNode * node;
    set<LinkNode *>::iterator it;
    while (!expanded.empty())
      {
	it = expanded.begin();
	(*it)->joinPlaces();

	// clean up
	node = &(*it)->getPartner();
	expanded.erase(it); delete *it;
	expanded.erase(node); delete node;
      }
  }


  /*!
   * \param   source  the source Node
   * \param   target  the target Node
   * \param   weight  weight of the Arc
   * \return  the newly created Arc
   */
  Arc & PetriNet::createArc(Node & source, Node & target, unsigned int weight)
  {
    return *new Arc(*this, observer_, source, target, weight);
  }


  /*!
   * If an empty name is given, one is generated using getUniqueNodeName().
   *
   * \param   name  the (initial) name of the place
   * \param   type  communication type of the place (internal or interface)
   * \return  the newly created place
   */
  Place & PetriNet::createPlace(const string & name, Node::Type type,
				unsigned int tokens, unsigned int capacity,
				const string & port)
  {
    return *new Place(*this, observer_,
		      name.empty() ? getUniqueNodeName("p") : name, type,
		      tokens, capacity, port);
  }


  /*!
   * If an empty name is given, one is generated using getUniqueNodeName().
   */
  Transition & PetriNet::createTransition(const string & name)
  {
    return *new Transition(*this, observer_, name.empty() ? getUniqueNodeName("t") : name);
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
    // use an arc cache to make this more efficient
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
  const set<Place *> & PetriNet::getInternalPlaces() const
  {
    return internalPlaces_;
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
  set<Place *> PetriNet::getInterfacePlaces(const string & port) const
  {
    set<Place *> places;
    pair<multimap<string, Place *>::const_iterator,
      multimap<string, Place *>::const_iterator> portRange =
      interfacePlacesByPort_.equal_range(port);
    for (multimap<string, Place *>::const_iterator it = portRange.first;
	 it != portRange.second; ++it)
      places.insert(it->second);
    return places;
  }


  /*!
   */
  const set<Transition *> & PetriNet::getTransitions() const
  {
    return transitions_;
  }


  /*!
   * \param   base  base name
   * \return  a string to be used as a name for a new node
   */
  string PetriNet::getUniqueNodeName(const string & base) const
  {
    int i = 1;
    string name;

    // use a "mutable" cache to make this more efficient
    do { ostringstream str; str << base << i++; name = str.str(); }
    while (nodesByName_.find(name) != nodesByName_.end());

    return name;
  }


  /*!
   * \brief   checks whether the Petri net is free choice
   *
   *          A Petri net is free choice iff
   *            all two transitions have either disjoint or equal presets
   *
   * \return  true if the criterion is fulfilled and false if not
   */
  bool PetriNet::isFreeChoice() const
  {
    for (set<Transition *>::iterator t = transitions_.begin();
        t != transitions_.end(); t++)
      for (set<Transition *>::iterator tt = transitions_.begin();
          tt != transitions_.end(); tt++)
      {
        set<Node *> t_pre  = (*t)->getPreset();
        set<Node *> tt_pre = (*tt)->getPreset();
        if ((t_pre != tt_pre) &&
            !(util::setIntersection(t_pre, tt_pre).empty()))
          return false;
      }
    return true;
  }


  /*!
   * \brief   Checks if the net is normal
   *
   * A PetriNet N is called normal iff each transition t has only one
   * Place \f[p \in (P_{in} \cup P_{out}) : p \in ^*t \cup t^*\f]
   *
   * \return  true iff the net is normal
   */
  bool PetriNet::isNormal() const
  {
    for (set<Transition *>::const_iterator t = transitions_.begin();
        t != transitions_.end(); t++)
      if (!(*t)->isNormal())
        return false;

    return true;
  }


  /*!
   * \brief   normalizes the given Petri net
   *
   * A Petri net (resp. open net) is called normal if
   * each transition has only one adjacent interface
   * place.
   *
   * For input places complementary places are introduced.
   */
  const std::map<Transition *, string> PetriNet::normalize(bool makeInnerStructure)
  {
    static int in = 1;
    static int on = 1;
    std::map<Transition *, string> edgeLabels;

    if (!isNormal())
    {
      set<Place *> temp;

      for (set<Transition *>::const_iterator t = transitions_.begin();
          t != transitions_.end(); t++)
      {
        if (edgeLabels[*t] != "")
          continue;

        /// adjacent input places of t
        set<Place *> t_inp;
        for (set<Node *>::const_iterator p = (*t)->getPreset().begin();
            p != (*t)->getPreset().end(); p++)
          if ((*p)->getType() == Node::INPUT)
            t_inp.insert(static_cast<Place *>(*p));

        for (set<Place *>::const_iterator ip = t_inp.begin();
            ip != t_inp.end(); ip++)
        {
          char c = (in++ % 10) + 48;
          string nname = (**ip).getName();
          nname.push_back(c);
          /// new internal place
          Place &nint = createPlace(nname+"_normalized");
          /// complementary place
          Place &ncomp = createPlace("comp_"+nname+"_normalized");
          ncomp.mark(1);
          /// new transition
          Transition &ntrans = createTransition("t_"+nname);
          /// creating arcs
          createArc(**ip, ntrans);
          createArc(ntrans, nint);
          createArc(nint, **t);
          createArc(ncomp, ntrans);
          createArc(*(*t), ncomp);

          condition_ = condition_.formula() && nint == 0;

          edgeLabels[&ntrans] = (**ip).getName();

          deleteArc(*findArc(**ip, **t));
        }

        /// adjacent output places of t
        set<Place *> t_out;
        for (set<Node *>::const_iterator p = (*t)->getPostset().begin();
            p != (*t)->getPostset().end(); p++)
          if ((*p)->getType() == Node::OUTPUT)
            t_out.insert(static_cast<Place *>(*p));

        for (set<Place *>::const_iterator op = t_out.begin();
            op != t_out.end(); op++)
        {
          char c = (on++ % 10) + 48;
          string nname = (**op).getName();
          nname.push_back(c);
          /// new internal place
          Place &nint = createPlace(nname+"_normalized");
          /// new transition
          Transition &ntrans = createTransition("t_"+nname);
          /// creating arcs
          createArc(*(*t), nint);
          createArc(nint, ntrans);
          createArc(ntrans, **op);

          condition_ = condition_.formula() && nint == 0;

          edgeLabels[&ntrans] = (**op).getName();

          deleteArc(*findArc(**t, **op));
        }

        edgeLabels[*t] = "tau";
      }
    }
    else
    {
      for (set<Transition *>::const_iterator t = transitions_.begin(); t != transitions_.end(); t++)
      {
        switch ((*t)->getType())
        {
        case Node::INPUT:
        {
          set<Arc *> preset = (*t)->getPresetArcs();
          for (set<Arc *>::const_iterator f = preset.begin(); f != preset.end(); f++)
          {
            if ((*f)->getPlace().getType() == Node::INPUT)
            {
              edgeLabels[*t] = (*f)->getPlace().getName();
              break;
            }
          }
          break;
        }
        case Node::OUTPUT:
        {
          set<Arc *> postset = (*t)->getPostsetArcs();
          for (set<Arc *>::const_iterator f = postset.begin(); f != postset.end(); f++)
          {
            if ((*f)->getPlace().getType() == Node::OUTPUT)
            {
              edgeLabels[*t] = (*f)->getPlace().getName();
              break;
            }
          }
          break;
        }
        case Node::INTERNAL:
        {
          edgeLabels[*t] = "tau";
          break;
        }
        default: assert(false);
        }
      }
    }

    if (makeInnerStructure)
      for (set<Place *>::const_iterator p = interfacePlaces_.begin();
          p != interfacePlaces_.end(); p++)
        deletePlace(**p);

    return edgeLabels;
  }


  /*!
   * \brief   produces a second constraint oWFN
   *
   * \param   a constraint oWFN
   */
  void PetriNet::produce(const PetriNet &net)
  {
    /*
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
   * \brief   checks whether the Petri net is a workflow net or not
   *
   *          A Petri net N is a workflow net iff
   *            (1) there is exactly one place with empty preset
   *            (2) there is exactly one place with empty postset
   *            (3) all other nodes are situated on a path between the places described in (1) and (2)
   *
   * \return  true iff (1), (2) and (3) are fulfilled
   * \return  false in any other case
   */
  bool PetriNet::isWorkflow()
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
          return false;
      }
      if ((*p)->getPostset().empty())
      {
        if (last == NULL)
          last = *p;
        else
          return false;
      }
    }
    if (first == NULL || last == NULL)
      return false;

    // insert new transition which consumes from last and produces on first to form a cycle
    Transition &tarjan = createTransition("tarjan");
    createArc(*last, tarjan);
    createArc(tarjan, *first);

    // each 2 nodes \f$x,y \in P \cup T\f$ are in a directed cycle
    // (strongly connected net using tarjan's algorithm)
    unsigned int i = 0; ///< count index
    map<Node *, int> index; ///< index property for nodes
    map<Node *, unsigned int> lowlink; ///< lowlink property for nodes
    set<Node *> stacked; ///< the stack indication for nodes
    stack<Node *> S; ///< stack used by Tarjan's algorithm

    // set all nodes' index values to ``undefined''
    for (set<Place *>::const_iterator p = places_.begin();
        p != places_.end(); p++)
      index[*p] = (-1);
    for (set<Transition *>::const_iterator t = transitions_.begin();
        t != transitions_.end(); t++)
      index[*t] = (-1);

    // getting the number of strongly connected components reachable from first
    unsigned int sscCount = util::dfsTarjan<Node *>(first, S,
        stacked, i, index, lowlink);

    deleteTransition(tarjan);

    // check set $P \cup T$
    set<Node *> nodeSet;
    for (set<Place *>::const_iterator p = places_.begin(); p != places_.end(); p++)
      nodeSet.insert(*p);
    for (set<Transition *>::const_iterator t = transitions_.begin(); t != transitions_.end(); t++)
      nodeSet.insert(*t);

    /*
     * true iff only one strongly connected component found from first and all nodes
     * of $\mathcal{N}$ are members of this component
     */
    if (sscCount == 1 && util::setDifference(nodeSet, stacked).empty())
      return true;
    else
      return false;
  }



  /****************************************************************************
   *** Private PetriNet Function Definitions
   ***************************************************************************/

  string PetriNet::getMetaInformation(std::ios_base & ios,
				      io::MetaInformation i,
				      const string & def) const
  {
    map<io::MetaInformation, string> & streamMeta =
      io::util::MetaData::data(ios);
    if (streamMeta.find(i) != streamMeta.end())
      return streamMeta.find(i)->second;
    if (meta_.find(i) != meta_.end())
      return meta_.find(i)->second;
    return def;
  }


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


  void PetriNet::deletePlace(Place & place)
  {
    observer_.finalizePlaceType(place, place.getType());
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

    while (!node.getPreset().empty())
      deleteArc(*findArc(**node.getPreset().begin(), node));
    while (!node.getPostset().empty())
      deleteArc(*findArc(node, **node.getPostset().begin()));

    observer_.finalizeNodeNameHistory(node, node.getNameHistory());
    nodes_.erase(&node);

    delete &node;
  }


  /*!
   */
  void PetriNet::deleteArc(Arc & arc)
  {
    assert(arcs_.find(&arc) != arcs_.end());

    arcs_.erase(&arc);
    delete &arc;
  }


  /*!
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

}
