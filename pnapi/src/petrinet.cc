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

#include "config.h"
#include <cassert>
#include <iostream>

#ifndef NDEBUG
#include <fstream>
#include "myio.h"
using std::cout;
using std::cerr;
using std::endl;
using pnapi::io::operator<<;
using pnapi::io::util::operator<<;
#endif

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

  namespace util
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
    arc.getTargetNode().presetArcs_.insert(&arc);
    arc.getSourceNode().postset_.insert(&arc.getTargetNode());
    arc.getSourceNode().postsetArcs_.insert(&arc);

    // update transition type
    arc.getTransition().updateType();
  }

  /*
   * \pre Arc has to be removed from PetriNet::arcs_ before calling this
   */
  void ComponentObserver::updateArcRemoved(Arc & arc)
  {
    assert(&arc.getPetriNet() == &net_);
    assert(net_.arcs_.find(&arc) == net_.arcs_.end());

    // update pre- and postsets
    arc.getTargetNode().preset_.erase(&arc.getSourceNode());
    arc.getTargetNode().presetArcs_.erase(&arc);
    arc.getSourceNode().postset_.erase(&arc.getTargetNode());
    arc.getSourceNode().postsetArcs_.erase(&arc);

    // update transition type
    arc.getTransition().updateType();
  }

  /*
   * \brief Only purpose of this function is erasing the original arc
   *        by mirroring so that ComponentObserver::updateArcRemoved
   *        can be called.
   */
  void ComponentObserver::updateArcMirror(Arc & arc)
  {
    // you could check place type and pre-/postset here
    // assert(...)

    net_.arcs_.erase(&arc);
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
    updateTransitionLabels(trans);

    //net_.labels_.clear();
    net_.transitionsByLabel_.clear();
    
    for (set<Transition *>::iterator t = net_.synchronizedTransitions_.begin(); 
                 t != net_.synchronizedTransitions_.end(); ++t)
    {
      for (set<string>::iterator l = (*t)->getSynchronizeLabels().begin(); 
                   l != (*t)->getSynchronizeLabels().end(); ++l)
      {
        //net_.labels_.insert(*l);
        net_.transitionsByLabel_[*l].insert(*t);
      }
    }
  }
  

  void ComponentObserver::updateTransitionLabels(Transition & trans)
  {
    if (trans.isSynchronized())
    {
      net_.synchronizedTransitions_.insert(&trans);
    }
    else  
    {
      net_.synchronizedTransitions_.erase(&trans);
    }
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
    /*
    deque<string> history = node.getNameHistory();
    deque<string>::iterator it = history.begin();
    //for (deque<string>::iterator it = history.begin(); it != history.end();
    //     ++it)
    {
      assert((net_.nodesByName_.find(*it))->second == &node ||
	     net_.nodesByName_.find(*it) == net_.nodesByName_.end());
      
      net_.nodesByName_[*it] = &node;
    }
    //*/
    
    string name = node.getName();
    assert( (net_.nodesByName_.find(name) == net_.nodesByName_.end()) ||
            ((net_.nodesByName_.find(name))->second == &node) );
    
    net_.nodesByName_[name] = &node;
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

  } /* namespace util */



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
    labels_(net.labels_),
    observer_(*this),
    condition_(net.condition_, copyStructure(net)),
    meta_(net.meta_)
  {
    setConstraintLabels(net.constraints_);
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
    labels_.clear();
    meta_.clear();
    constraints_.clear();
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
   */
  map<const Place *, const Place *>
  PetriNet::copyPlaces(const PetriNet & net, const string & prefix)
  {
    map<const Place *, const Place *> placeMapping;
    for (set<Place *>::iterator it = net.places_.begin();
	 it != net.places_.end(); ++it)
      {
	assert(!containsNode((*it)->getName()));
	placeMapping[*it] = new Place(*this, observer_, **it, prefix);
      }
    return placeMapping;
  }


  /*!
   * Adds all nodes and arcs of the second net.
   *
   * \pre   the node names are unique (disjoint name sets); use
   *        prefixNodeNames() on <em>both</em> nets to achieve this
   */
  map<const Place *, const Place *>
  PetriNet::copyStructure(const PetriNet & net, const string & prefix)
  {
    // add all transitions of the net
    for (set<Transition *>::iterator it = net.transitions_.begin();
	 it != net.transitions_.end(); ++it)
      {
	assert(!containsNode((*it)->getName()));
	new Transition(*this, observer_, **it, prefix);
      }

    // add all places
    const map<const Place *, const Place *> & placeMapping =
      copyPlaces(net, prefix);

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
   * \brief Merges this net with a second net.
   * 
   * Given a second Petri net #net, a resulting net is created
   * by applying the following steps:
   * 
   * 1.: Internal places of this net and #net are prefixed and copied
   *     in the resulting net
   * 2.: Input and output places are connected appropriatly 
   *     (if an input and an output place name of the two nets match)
   *     or copied (without a prefix!).
   * 3.: Transitions are arranged by the following rules:
   *     a) Labels, appearing in both this and the other net (from now
   *     called "common labels"), will be synchronized; each transition 
   *     not containing such a common label will be prefixed and copied.
   *     b) If a transition t1 of this net and a transition t2 of the other
   *     net share at least one label, a transition t' will be created in
   *     the resulting net. The preset/postset of t' is the union of the
   *     presets/postsets of t1 and t2. The Labels of t' is the difference
   *     of the union of the labels of t1 and t2 and the intersection of 
   *     their labels. Transitions, labeled with a "common label" but
   *     without a matching transition in the other net are copied.
   *     c) If there remains transitions in the resulting net with
   *     "common labels", the will be "killed" by adding an new, unmarked
   *     place to their preset. 
   * 
   */
  void PetriNet::compose(const PetriNet & net, const string & prefix,
			 const string & netPrefix)
  {
    PetriNet result;
    // mapping from source nets' places to result net's places
    map<const Place *, const Place *> placeMap;

    // ------------ STEP 1 -----------------------------
    
    // sets of internalplaces
    set<Place *> thisPlaces = getInternalPlaces();
    set<Place *> netPlaces = net.getInternalPlaces();
    
    // copy internal places of this net
    for (set<Place *>::iterator p = thisPlaces.begin(); 
           p != thisPlaces.end(); ++p)
    {
      placeMap[*p] = &result.createPlace(prefix+(*p)->getName(), Node::INTERNAL, (*p)->getTokenCount());
    }
    
    // copy internal places of #net
    for (set<Place *>::iterator p = netPlaces.begin(); 
            p != netPlaces.end(); ++p)
    {
      placeMap[*p] = &result.createPlace(netPrefix+(*p)->getName(), Node::INTERNAL, (*p)->getTokenCount());
    }
    
    // ------------ STEP 2 -----------------------------
    
    // sets of interface places
    set<Place *> thisInput = getInputPlaces();
    set<Place *> thisOutput = getOutputPlaces();
    set<Place *> netInput = net.getInputPlaces();
    set<Place *> netOutput = net.getOutputPlaces();

    // iterate through this nets input places
    for (set<Place *>::iterator p = thisInput.begin(); 
            p != thisInput.end(); ++p)
    {
      Place *rp = NULL; // place in result net
      Place *opponent = net.findPlace((*p)->getName()); // place in #net

      if ( (opponent == NULL) || // if this place doesn't occur in #net
           (opponent->getType() != Node::OUTPUT) ) // if the found place is no matching output place
      {
        // then the resulting place remains an interface place
        rp = &result.createPlace((*p)->getName(), Node::INPUT);
      }
      else // if there is a matching output place
      {
        // the resulting place becomes an internal place
        rp = &result.createPlace((*p)->getName());
        
        /* places are already merged, so the corresponding
         * output place can be removed from #net's output set. */ 
        netOutput.erase(opponent);
        
        placeMap[opponent] = rp;
      }
      
      placeMap[*p] = rp;
    }

    // iterate through this net's output places
    for (set<Place *>::iterator p = thisOutput.begin(); 
            p != thisOutput.end(); ++p)
    {
      Place *rp = NULL; // place in result net
      Place *opponent = net.findPlace((*p)->getName()); // place in #net

      if ( (opponent == NULL) || // if this place doesn't occur in #net
           (opponent->getType() != Node::INPUT) ) // if the found place is no matching input place
      {
        // then the resulting place remains an interface place
        rp = &result.createPlace((*p)->getName(), Node::OUTPUT);
      }
      else // if there is a matching input place
      {
        // the resulting place becomes an internal place
        rp = &result.createPlace((*p)->getName());
        
        /* places are already merged, so the corresponding
         * input place can be removed from #net's input set. */ 
        netInput.erase(opponent);
        
        placeMap[opponent] = rp;
      }
      
      placeMap[*p] = rp;
    }

    /* 
     * all matching interface places have been merged, so the remaining
     * places are also interface places in the resulting net
     */
    
    // iterate #net's input places
    for (set<Place *>::iterator p = netInput.begin(); 
            p != netInput.end(); ++p)
    {
      Place *np = &result.createPlace((*p)->getName(), Node::INPUT);
      placeMap[*p] = np;
    }

    // iterate #net's output places
    for (set<Place *>::iterator p = netOutput.begin(); 
            p != netOutput.end(); ++p)
    {
      Place *np = &result.createPlace((*p)->getName(), Node::OUTPUT);
      placeMap[*p] = np;
    }

    // ------------ STEP 3 -----------------------------
    
    // arrange label sets
    const set<string> & labels1 = labels_;
    const set<string> & labels2 = net.getSynchronousLabels();
    set<string> sharedLabels = util::setIntersection(labels1,labels2);
    result.labels_ = util::setDifference(util::setUnion(labels1,labels2),sharedLabels);
    
    // sets of transitions to be merged
    set<Transition*> mergeThis;
    set<Transition*> mergeOther;
    
    // iterate through this net's transitions and copy transitions
    for (set<Transition *>::iterator t = getTransitions().begin(); 
            t != getTransitions().end(); ++t)
    {
      if ( (!(*t)->isSynchronized()) || // this transition is not labeled
           (util::setIntersection((*t)->getSynchronizeLabels(), sharedLabels).empty()) ) // transition has no shared label
      {
        // create a prefixed transition in the resulting net
        Transition & rt = result.createTransition(prefix+(*t)->getName(), (*t)->getSynchronizeLabels());
        
        // copy preset arcs
        for (set<Arc *>::iterator f = (*t)->getPresetArcs().begin(); 
                f != (*t)->getPresetArcs().end(); ++f)
        {
          result.createArc(*const_cast<Place*>(placeMap[(Place*)&(*f)->getSourceNode()]), rt, (*f)->getWeight());
        }
        
        // copy postset arcs
        for (set<Arc *>::iterator f = (*t)->getPostsetArcs().begin(); 
                f != (*t)->getPostsetArcs().end(); ++f)
        {
          result.createArc(rt, *const_cast<Place*>(placeMap[(Place*)&(*f)->getTargetNode()]), (*f)->getWeight());
        }
      }
      else
      {
        mergeThis.insert(*t);
      }
    }
    
    // iterate through #net's transitions and copy transitions
    for (set<Transition *>::iterator t = net.getTransitions().begin(); 
            t != net.getTransitions().end(); ++t)
    {
      if ( (!(*t)->isSynchronized()) || // this transition is not labeled
          (util::setIntersection((*t)->getSynchronizeLabels(), sharedLabels).empty()) ) // transition has no shared label
      {
        // create a prefixed transition in the resulting net
        Transition & rt = result.createTransition(netPrefix+(*t)->getName(), (*t)->getSynchronizeLabels());
        
        // copy preset arcs
        for (set<Arc *>::iterator f = (*t)->getPresetArcs().begin(); 
                f != (*t)->getPresetArcs().end(); ++f)
        {
          result.createArc(*const_cast<Place*>(placeMap[(Place*)&(*f)->getSourceNode()]), rt, (*f)->getWeight());
        }
        
        // copy postset arcs
        for (set<Arc *>::iterator f = (*t)->getPostsetArcs().begin(); 
                f != (*t)->getPostsetArcs().end(); ++f)
        {
          result.createArc(rt, *const_cast<Place*>(placeMap[(Place*)&(*f)->getTargetNode()]), (*f)->getWeight());
        }
      }
      else
      {
        mergeOther.insert(*t);
      }
    }
    
    /// transitions without a partner
    set<Transition*> lonelyTransitions1;
    set<Transition*> lonelyTransitions2 = mergeOther;
    
    /* 
     * Transitions of the other net are assumed as "lonely".
     * If such a transition gets a partner, it is removed from this set.
     * Own transitions are assumed as not "lonely". If such a transition
     * remains without a partner, it is added to this set.
     */ 
    
    /// transitions that must be killed
    set<Transition*> doomedTransitions;
    
    /// merge matching synchronous transitions
    for(set<Transition*>::iterator t1 = mergeThis.begin();
          t1 != mergeThis.end(); ++t1)
    {
      bool stayLonely = true; // if t1 finds a partner
      for(set<Transition*>::iterator t2 = mergeOther.begin();
            t2 != mergeOther.end(); ++t2)
      {
        set<string> t1Labels = (*t1)->getSynchronizeLabels();
        set<string> t2Labels = (*t2)->getSynchronizeLabels();
        set<string> t12Labels = util::setIntersection(t1Labels, t2Labels); 
        
        if(!t12Labels.empty()) // if t1 and t2 share at least one label
        {
          set<string> labelUnion = util::setUnion(t1Labels, t2Labels);
          set<string> rtLabels = util::setDifference(labelUnion, sharedLabels);
          
          // create new transition by merging
          Transition & rt = result.createTransition((*t1)->getName() + netPrefix + (*t2)->getName(), rtLabels);
          
          // copy preset arcs
          for (set<Arc *>::iterator f = (*t1)->getPresetArcs().begin(); 
                  f != (*t1)->getPresetArcs().end(); ++f)
          {
            result.createArc(*const_cast<Place*>(placeMap[(Place*)&(*f)->getSourceNode()]), rt, (*f)->getWeight());
          }
          
          // copy postset arcs
          for (set<Arc *>::iterator f = (*t1)->getPostsetArcs().begin(); 
                  f != (*t1)->getPostsetArcs().end(); ++f)
          {
            result.createArc(rt, *const_cast<Place*>(placeMap[(Place*)&(*f)->getTargetNode()]), (*f)->getWeight());
          }
          
          // copy #net's preset arcs
          for (set<Arc *>::iterator f = (*t2)->getPresetArcs().begin(); 
                  f != (*t2)->getPresetArcs().end(); ++f)
          {
            result.createArc(*const_cast<Place*>(placeMap[(Place*)&(*f)->getSourceNode()]), rt, (*f)->getWeight());
          }
          
          // copy #net's postset arcs
          for (set<Arc *>::iterator f = (*t2)->getPostsetArcs().begin(); 
                  f != (*t2)->getPostsetArcs().end(); ++f)
          {
            result.createArc(rt, *const_cast<Place*>(placeMap[(Place*)&(*f)->getTargetNode()]), (*f)->getWeight());
          }
          
          // both transitions found a partner
          stayLonely = false;
          lonelyTransitions2.erase(*t2);
          
          // if one transition keeps a "common label"
          if(util::setDifference(labelUnion, t12Labels) != rtLabels)
          {
            // then this transition has to be killed
            doomedTransitions.insert(&rt);
            
            //TODO: partial matching may be handled different
          }
        }
      }
      if(stayLonely)
        lonelyTransitions1.insert(*t1);
    }
    
    // copy lonely transitions
    for(set<Transition*>::iterator t = lonelyTransitions1.begin();
          t != lonelyTransitions1.end(); ++t)
    {
      set<string> rtLabels = util::setDifference((*t)->getSynchronizeLabels(), sharedLabels);
                
      // create new transition by merging
      Transition & rt = result.createTransition(prefix + (*t)->getName(), rtLabels);
      
      // copy preset arcs
      for (set<Arc *>::iterator f = (*t)->getPresetArcs().begin(); 
              f != (*t)->getPresetArcs().end(); ++f)
      {
        result.createArc(*const_cast<Place*>(placeMap[(Place*)&(*f)->getSourceNode()]), rt, (*f)->getWeight());
      }
      
      // copy postset arcs
      for (set<Arc *>::iterator f = (*t)->getPostsetArcs().begin(); 
              f != (*t)->getPostsetArcs().end(); ++f)
      {
        result.createArc(rt, *const_cast<Place*>(placeMap[(Place*)&(*f)->getTargetNode()]), (*f)->getWeight());
      }
      
      // this transition is dead
      doomedTransitions.insert(&rt);
    }
    for(set<Transition*>::iterator t = lonelyTransitions2.begin();
              t != lonelyTransitions2.end(); ++t)
    {
      set<string> rtLabels = util::setDifference((*t)->getSynchronizeLabels(), sharedLabels);
                
      // create new transition by merging
      Transition & rt = result.createTransition(netPrefix + (*t)->getName(), rtLabels);
      
      // copy preset arcs
      for (set<Arc *>::iterator f = (*t)->getPresetArcs().begin(); 
              f != (*t)->getPresetArcs().end(); ++f)
      {
        result.createArc(*const_cast<Place*>(placeMap[(Place*)&(*f)->getSourceNode()]), rt, (*f)->getWeight());
      }
      
      // copy postset arcs
      for (set<Arc *>::iterator f = (*t)->getPostsetArcs().begin(); 
              f != (*t)->getPostsetArcs().end(); ++f)
      {
        result.createArc(rt, *const_cast<Place*>(placeMap[(Place*)&(*f)->getTargetNode()]), (*f)->getWeight());
      }
      
      // this transition is dead
      doomedTransitions.insert(&rt);
    }
    
    // kill doomed transitions
    for(set<Transition*>::iterator t = doomedTransitions.begin();
          t != doomedTransitions.end(); ++t)
    {
      Place & p = result.createPlace();
      result.createArc(p, **t);
    }
    

    /*!
     * \todo check me!
     */
    result.finalCondition().merge(finalCondition(), placeMap);
    result.finalCondition().merge(net.finalCondition(), placeMap);

    // overwrite this net with the resulting net
    *this = result;
    
    /*assert(prefix != netPrefix);

    map<string, PetriNet *> nets;
    nets[prefix] = this;
    nets[netPrefix] = const_cast<PetriNet *>(&net);

    *this = compose(nets);*/


    /* below the old implementation
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
    */
  }


  /*!
   */
  PetriNet PetriNet::composeByWiring(const map<string, PetriNet *> & nets)
  {
    PetriNet result;

    // create instance map
    typedef map<string, vector<PetriNet> > Instances;
    Instances instances;
    
    for (map<string, PetriNet *>::const_iterator it = nets.begin();
           it != nets.end(); ++it)
    {
      instances[it->first].push_back(*it->second);
    }

    // create wiring
    typedef map<Place *, LinkNode *> Wiring;
    Wiring wiring;
    
    for (Instances::iterator it1 = instances.begin();
           it1 != instances.end(); ++it1)
    {
      string prefix = it1->first; assert(!prefix.empty());
      PetriNet & net1 = *it1->second.begin();

      for (Instances::iterator it2 = instances.begin();
            it2 != instances.end(); ++it2)
      {
        PetriNet & net2 = *it2->second.begin();
        wire(net1, net2, wiring);
      }
    }

    // create result net
    return result.createFromWiring(instances, wiring);
  }


  /*!
   */
  void PetriNet::wire(const PetriNet & net1, const PetriNet & net2,
		      map<Place *, LinkNode *> & wiring)
  {
    set<Place *> interface = net1.getInterfacePlaces();
    
    for (set<Place *>::iterator it = interface.begin();
          it != interface.end(); ++it)
    {
      Place * p1 = *it;
      Place * p2 = net2.findPlace(p1->getName());
      
      if ( (p2 != NULL) && 
           (p2->isComplementType(p1->getType())) )
      {
        LinkNode * node1 = wiring[p1];
        if (node1 == NULL)
          wiring[p1] = node1 = new LinkNode(*p1, LinkNode::ANY);
        
        LinkNode * node2 = wiring[p2];
        if (node2 == NULL)
          wiring[p2] = node2 = new LinkNode(*p2, LinkNode::ANY);
        
        node1->addLink(*node2);
      }
    }
  }


  /*!
   */
  PetriNet &
  PetriNet::createFromWiring(map<string, vector<PetriNet> > & instances,
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
      delete (*it); // clean up
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
      delete *it;
      expanded.erase(it);
      delete node;
      expanded.erase(node);
    }

    return *this;
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
  Transition & PetriNet::createTransition(const string & name,
					  const std::set<std::string> & labels)
  {
    return *new Transition(*this, observer_,
			   name.empty() ? getUniqueNodeName("t") : name,
			   labels);
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
    for (set<Arc*>::iterator it = source.getPostsetArcs().begin();
          it != source.getPostsetArcs().end(); ++it)
    {
      if ( &((*it)->getTargetNode()) == &target )
        return *it;
    }

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
   *
   */
  const set<Arc *> & PetriNet::getArcs() const
  {
    return arcs_;
  }


  /*!
   */
  const set<Transition *> & PetriNet::getSynchronizedTransitions() const
  {
    return synchronizedTransitions_;
  }
  
  
  /*!
   * \brief Get synchronized Transitions to a given label
   * 
   * \param label specifies the label in question
   * 
   * \return a set of transitions that are synchronized with this label 
   * 
   * \note  Throws an UnknownTransitionError if no such set exists.
   */
  const set<Transition *> & PetriNet::getSynchronizedTransitions(const string & label) const
  {
    map<string, set<Transition*> >::const_iterator t = transitionsByLabel_.find(label);
    
    if(t == transitionsByLabel_.end())
      throw exceptions::UnknownTransitionError(); 
    
    return t->second;
  }
  

  /*!
   */
  std::set<std::string> PetriNet::getSynchronousLabels() const
  {
    return labels_;
  }


  /*!
   * \todo  Maybe make private again.
   */
  void PetriNet::setSynchronousLabels(const std::set<std::string> & ls)
  {
    labels_ = ls;
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
   * \return  map<Transition *, string> a mapping representing the edge labels
   *          of the later service automaton.
   */
  const std::map<Transition *, string> PetriNet::normalize()
  {
    if (!getSynchronizedTransitions().empty())
    {
      normalize_classical();
    }
    else
    {
      normalize_rules();
    }

    std::map<Transition *, string> edgeLabels; ///< returning map
    
    // get input labels
    for(set<Place*>::iterator p = inputPlaces_.begin();
          p != inputPlaces_.end(); ++p)
    {
      for(set<Node*>::iterator t = (*p)->getPostset().begin();
            t != (*p)->getPostset().end(); ++t)
      {
        edgeLabels[static_cast<Transition*>(*t)] = (*p)->getName();
      }
    }
    
    // get output labels
    for(set<Place*>::iterator p = outputPlaces_.begin();
          p != outputPlaces_.end(); ++p)
    {
      for(set<Node*>::iterator t = (*p)->getPreset().begin();
            t != (*p)->getPreset().end(); ++t)
      {
        edgeLabels[static_cast<Transition*>(*t)] = (*p)->getName();
      }
    }
    
    // get synchronous and TAU labels
    for (set<Transition*>::iterator t = transitions_.begin();
          t != transitions_.end(); ++t)
    {
      if((*t)->getType() != Node::INTERNAL)
        continue;
        
      edgeLabels[*t] = ((*t)->isSynchronized()) ? (*(*t)->getSynchronizeLabels().begin()) : "TAU";
    }

    return edgeLabels;
  }


  /*!
   * Makes the inner structure of the net, which means to delete all
   * interface places to simplify the firing rules when creating a
   * service automaton.
   */
  void PetriNet::makeInnerStructure()
  {
    std::set<Place *> interface;
    while (!(interface = getInterfacePlaces()).empty())
      deletePlace(**interface.begin());
  }


  /*!
   * described in "Behavioral Constraints for Services" (BPM 2007)
   * (http://dx.doi.org/10.1007/978-3-540-75183-0_20)
   *
   * \note  There is an error in definition 5: The arcs of the transitions
   *        with empty label are missing.
   */
  void PetriNet::produce(const PetriNet & net, const string & aPrefix,
			 const string & aNetPrefix) throw (io::InputError)
  {
    typedef set<Transition *> Transitions;
    typedef map<Transition *, Transitions> Labels;
    typedef map<const Place *, const Place *> PlaceMapping;

    assert(!aPrefix.empty());
    assert(!aNetPrefix.empty());
    assert(aPrefix != aNetPrefix);
    assert(net.inputPlaces_.empty());
    assert(net.outputPlaces_.empty());

    string prefix = aPrefix + ".";
    string netPrefix = aNetPrefix + ".";
    Labels labels = translateConstraintLabels(net);


    // add (internal) places
    prefixNodeNames(prefix, true); // prefix only internal nodes
    PlaceMapping placeMapping = copyPlaces(net, netPrefix);

    // merge final conditions
    condition_.merge(net.condition_, placeMapping);

    // handle transitions with empty label
    for (Transitions::iterator it = net.transitions_.begin();
	 it != net.transitions_.end(); ++it)
      {
	Transition & netTrans = **it; // t'
	Transitions label;
	if (labels.find(&netTrans) != labels.end())
	  label = labels.find(&netTrans)->second;
	if (label.empty())
	  {
	    Transition & trans = createTransition(netTrans, netPrefix);
	    createArcs(trans, netTrans, &placeMapping);
	  }
      }

    // create product transitions
    Transitions labelTransitions;
    for (Labels::const_iterator it1 = labels.begin();
	 it1 != labels.end(); ++it1)
      {
	assert(it1->first != NULL);
	Transition & netTrans = *it1->first;             // t'
	assert(net.containsNode(netTrans));
	Transitions ts = it1->second;
	for (Transitions::const_iterator it2 = ts.begin();
	     it2 != ts.end(); ++it2)
	  {
	    assert(*it2 != NULL);
	    Transition & trans = **it2;                  // t
	    assert(containsNode(trans));
	    labelTransitions.insert(&trans);
	    Transition & prodTrans = createTransition(); // (t, t')
	    createArcs(prodTrans, trans);
	    createArcs(prodTrans, netTrans, &placeMapping);
	  }
      }

    // remove label transitions
    for (Transitions::iterator it = labelTransitions.begin();
	 it != labelTransitions.end(); ++it)
      deleteTransition(**it);
  }


  /*
   * \todo  maybe private again
   */
  void PetriNet::setConstraintLabels(const map<Transition *, set<string> > & labels)
  {
    constraints_.clear();
    for (map<Transition *, set<string> >::const_iterator it = labels.begin();
	 it != labels.end(); ++it)
      {
	Transition * t = findTransition(it->first->getName());
	assert(t != NULL);
	constraints_[t] = it->second;
      }
  }


  map<Transition *, set<Transition *> >
  PetriNet::translateConstraintLabels(const PetriNet & net)
    throw (io::InputError)
  {
    typedef set<string> Labels;
    typedef set<Transition *> Transitions;
    typedef map<Transition *, Labels> OriginalLabels;
    typedef map<Transition *, Transitions> ResultLabels;

    OriginalLabels labels = net.constraints_;
    ResultLabels result;
    for (OriginalLabels::iterator it1 = labels.begin();
	 it1 != labels.end(); ++it1)
      {
	assert(it1->first != NULL);
	Transition & t = *it1->first;
	for (Labels::iterator it2 = it1->second.begin();
	     it2 != it1->second.end(); ++it2)
	  {
	    Transition * labelTrans = findTransition(*it2);
	    if (labelTrans != NULL)
	      result[&t].insert(labelTrans);
	    else
	      {
		string filename =
		  net.meta_.find(io::INPUTFILE) != net.meta_.end()
		  ? net.meta_.find(io::INPUTFILE)->second : "";
		throw io::InputError(io::InputError::SEMANTIC_ERROR, filename,
				     0, *it2, "unknown transition");
	      }
	  }
      }
    return result;
  }


  /*!
   */
  Transition & PetriNet::createTransition(const Transition & t,
					  const std::string & prefix)
  {
    return *new Transition(*this, observer_, t, prefix);
  }


  /*!
   */
  void PetriNet::createArcs(Transition & trans, Transition & otherTrans,
			 const map<const Place *, const Place *> * placeMapping)
  {
    typedef set<Arc *> Arcs;

    // add preset arcs of t' to t
    Arcs preset = otherTrans.getPresetArcs();
    for (Arcs::iterator it = preset.begin(); it != preset.end(); ++it)
      {
	Arc & arc = **it;
	Place & place = placeMapping == NULL ? arc.getPlace() :
	  *const_cast<Place *>(placeMapping->find(&arc.getPlace())->second);
	new Arc(*this, observer_, arc, place, trans);
      }

    // add postset arcs of t' to t
    Arcs postset = otherTrans.getPostsetArcs();
    for (Arcs::iterator it = postset.begin(); it != postset.end(); ++it)
      {
	Arc & arc = **it;
	Place & place = placeMapping == NULL ? arc.getPlace() :
	  *const_cast<Place *>(placeMapping->find(&arc.getPlace())->second);
	new Arc(*this, observer_, arc, trans, place);
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


  /*!
   */
  void PetriNet::mirror()
  {
    typedef set<Place *> Places;
    Places interface = getInterfacePlaces();

    for (Places::iterator it = interface.begin(); it != interface.end(); ++it)
      (*it)->mirror();
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
   * \param   noInterface  whether interface places should not be prefixed
   */
  PetriNet & PetriNet::prefixNodeNames(const string & prefix, bool noInterface)
  {
    if(noInterface)
    {
      for (set<Node *>::iterator it = nodes_.begin(); 
            it != nodes_.end(); ++it)
      {
        Place * p = dynamic_cast<Place*>(*it);
        if( (p != NULL) && // recent node is a place
            ((p->getType() == Node::INPUT) || (p->getType() == Node::OUTPUT)) ) // and its an interface place
          continue; // do not prefix this node
        
        (*it)->prefixNameHistory(prefix);
      }
    }
    else
    {
      for (set<Node *>::iterator it = nodes_.begin(); 
            it != nodes_.end(); ++it)
      {
        (*it)->prefixNameHistory(prefix);
      }
    }
    return *this;
  }


  void PetriNet::deletePlace(Place & place)
  {
    if (finalCondition().concerningPlaces().count(&place) > 0)
    {
      std::cerr << PACKAGE_STRING << ": place '" << place.getName()
        << "' occurs in the final condition. Please rewrite the final condition first."
        << " (the place was not deleted)" << std::endl;
      return;
    }
    observer_.finalizePlaceType(place, place.getType());
    places_.erase(&place);
    deleteNode(place);
  }


  void PetriNet::deleteTransition(Transition & trans)
  {
    if (trans.isSynchronized())
      synchronizedTransitions_.erase(&trans);
    transitions_.erase(&trans);

    labels_.clear();
    for (std::set<Transition *>::iterator t = transitions_.begin(); t != transitions_.end(); t++)
      for (std::set<std::string>::iterator l = (*t)->getSynchronizeLabels().begin(); l != (*t)->getSynchronizeLabels().end(); l++)
        labels_.insert(*l);

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
   * \pre Net and Arc have the same observer
   */
  void PetriNet::deleteArc(Arc & arc)
  {
    assert(arcs_.find(&arc) != arcs_.end());

    arcs_.erase(&arc);

    observer_.updateArcRemoved(arc);
    
    delete &arc;
  }


  /*!
   */
  void PetriNet::normalize_classical()
  {
    // transitions may to synchronize
    set<Transition*> candidates = synchronizedTransitions_;
    // complement places generated during normalisation
    set<Place*> complementPlaces;
    
    // fill candidates
    for(set<Place*>::iterator p = inputPlaces_.begin();
         p != inputPlaces_.end(); ++p)
    {
      for(set<Node*>::iterator t = (*p)->getPostset().begin();
           t != (*p)->getPostset().end(); ++t)
      {
        candidates.insert(static_cast<Transition*>(*t));
      }
    }
    for(set<Place*>::iterator p = outputPlaces_.begin();
         p != outputPlaces_.end(); ++p)
    {
      for(set<Node*>::iterator t = (*p)->getPreset().begin();
           t != (*p)->getPreset().end(); ++t)
      {
        candidates.insert(static_cast<Transition*>(*t));
      }
    }
    
    for(set<Transition*>::iterator t = candidates.begin();
         t != candidates.end(); ++t)
    {
      if( ((*t)->isNormal()) || // already normal; nothing to do
          ((*t)->getSynchronizeLabels().size() > 1) ) // can't normalize this transition
        continue;
      
      // check neighbor arcs
      set<Arc*> possiblyEvilArcs = util::setUnion((*t)->getPresetArcs(),(*t)->getPostsetArcs());
      for(set<Arc*>::iterator a = possiblyEvilArcs.begin(); 
           a != possiblyEvilArcs.end(); ++a)
      {
        if((*a)->getPlace().getType() == Node::INTERNAL)
          continue;
        // arcs reaching this line are evil
        
        Place & p = (*a)->getPlace();
        if(p.getType() == Node::INPUT)
        {
          for(unsigned int i = 0; i < (*a)->getWeight(); ++i) // reduce arc weights
          {
            stringstream ss;
            ss << i;
            
            Place & np = createPlace("normal" + ss.str() + "_" + (*t)->getName() + "_" + p.getName()); // new place
            Place & cp = createPlace("complement" + ss.str() + "_" + (*t)->getName() + "_" + p.getName()); // complement place
            cp.mark(1);
            complementPlaces.insert(&cp);
            Transition & nt = createTransition("t_in" + ss.str() + "_" + (*t)->getName() + "_" + p.getName());
            
            createArc(p, nt);
            createArc(nt, np);
            createArc(cp, nt);
            createArc(np, **t);
            createArc(**t, cp);
          }
        }
        else
        {
          for(unsigned int i = 0; i < (*a)->getWeight(); ++i) // reduce arc weights
          {
            stringstream ss;
            ss << i;
            
            Place & np = createPlace("p" + ss.str() + "_" + (*t)->getName() + "_" + p.getName()); // new place
            Transition & nt = createTransition("t_out" + ss.str() + "_" + (*t)->getName() + "_" + p.getName());
            
            createArc(nt, p);
            createArc(np, nt);
            createArc(**t, np);
          }
        }
        
        // remove arc
        deleteArc(**a);
      }
    }

    for(set<Place*>::iterator p = complementPlaces.begin();
         p != complementPlaces.end(); ++p)
    {
      condition_ = condition_.formula() && ((**p) == 1);
    }
    
    condition_ = condition_.formula() && formula::ALL_OTHER_PLACES_EMPTY;
  }


  /*!
   * This method detects sequences of transitions with interface
   * communication. Then, it tries to normalize the net through
   * the rules from [Aalst07].
   *
   * \todo     The rules from [Aalst07] will follow soon!
   */
  void PetriNet::normalize_rules()
  {
    normalize_classical();
  }

}
