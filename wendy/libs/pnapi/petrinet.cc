// -*- C++ -*-

/*!
 * \file    petrinet.cc
 *
 * \brief   Petri net basics
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          Christian Gierds <gierds@informatik.hu-berlin.de>,
 *          Martin Znamirowski <znamirow@informatik.hu-berlin.de>,
 *          Robert Waltemath <robert.waltemath@uni-rostock.de>,
 *          last changes of: $Author: cas $
 *
 * \since   2005-10-18
 *
 * \date    $Date: 2010-03-14 13:44:15 +0100 (Sun, 14 Mar 2010) $
 *
 * \version $Revision: 5513 $
 */

#include "config.h"

#include "petrinet.h"
#include "util.h"

#include <iostream>
#include <sstream>

#ifndef NDEBUG
using std::cout;
using std::cerr;
using std::endl;
using pnapi::io::operator<<;
using pnapi::io::util::operator<<;
#endif /* NDEBUG */

using std::deque;
using std::map;
using std::multimap;
using std::ostringstream;
using std::pair;
using std::set;
using std::stack;
using std::string;
using std::stringstream;
using std::vector;

namespace pnapi
{

namespace util
{

/****************************************************************************
 *** Class ComponentObserver Function Defintions
 ***************************************************************************/

/*!
 * \brief constructor
 */
ComponentObserver::ComponentObserver(PetriNet & net) :
  net_(net)
{
  assert(&net.observer_ == this); 
}


/*!
 * \brief get the net this observer belongs to
 */
PetriNet & ComponentObserver::getPetriNet() const
{
  return net_;
}


/*!
 * \brief inform the net about a new history of a node
 */
void ComponentObserver::updateNodeNameHistory(Node & node,
                                              const std::deque<std::string> & oldHistory)
{
  assert(net_.containsNode(node));

  
  // remove access to nodes by their former names
  net_.nodesByName_.erase(*oldHistory.begin());
  
  initializeNodeNameHistory(node);
}


/*!
 * \brief make a node accessable in a net by its name
 */
void ComponentObserver::initializeNodeNameHistory(Node & node)
{
  string name = node.getName();
  assert( (net_.nodesByName_.find(name) == net_.nodesByName_.end()) ||
          ((net_.nodesByName_.find(name))->second == &node) );

  net_.nodesByName_[name] = &node;
}


/*!
 * \brief inform the net about a new arc
 */
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
 * \brief inform nodes abput removed arc between them
 * 
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
 * \brief inform the net about a mirrored arc
 * 
 * Only purpose of this function is erasing the original arc
 * by mirroring so that ComponentObserver::updateArcRemoved
 * can be called.
 */
void ComponentObserver::updateArcMirror(Arc & arc)
{
  net_.arcs_.erase(&arc);
}

/*!
 * \brief inform the net about new place
 */
void ComponentObserver::updatePlaces(Place & place)
{
  updateNodes(place);
  net_.places_.insert(&place);
}

/*!
 * \brief inform the net about new transition
 */
void ComponentObserver::updateTransitions(Transition & trans)
{
  updateNodes(trans);
  net_.transitions_.insert(&trans);
  updateTransitionLabels(trans);
}

/*!
 * \brief inform the net about changes in a transitions label set
 */
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

/*!
 * \brief inform net about new node
 */
void ComponentObserver::updateNodes(Node & node)
{
  assert(&node.getPetriNet() == &net_);
  assert(!net_.containsNode(node));
  assert(net_.nodesByName_.find(node.getName()) == net_.nodesByName_.end());

  net_.nodes_.insert(&node);
  initializeNodeNameHistory(node);
}

} /* namespace util */


/****************************************************************************
 *** Static PetriNet Variables
 ***************************************************************************/

/// path to petrify
std::string PetriNet::pathToPetrify_ = CONFIG_PETRIFY;

/// path to genet
std::string PetriNet::pathToGenet_ = CONFIG_GENET;

/// converter Automaton => PetriNet
PetriNet::AutomatonConverter PetriNet::automatonConverter_ = PETRIFY;

/// capacity for genet
uint8_t PetriNet::genetCapacity_ = 2;

/****************************************************************************
 *** Class PetriNet Function Defintions
 ***************************************************************************/

/*** constructors and destructors ***/

/*!
 * \brief constructor
 * 
 * \note The condition is set to True by default.
 */
PetriNet::PetriNet() :
  observer_(*this), interface_(*this),
  warnings_(0), reducablePlaces_(NULL)
{
}

/*!
 * \brief compose constructor
 */
PetriNet::PetriNet(const Interface & interface1, const Interface & interface2, std::map<Label *, Label *> & label2label,
         std::map<Label *, Place *> & label2place, std::set<Label *> & commonLabels) :
  observer_(*this), interface_(*this, interface1, interface2, label2label, label2place, commonLabels),
  warnings_(0), reducablePlaces_(NULL)
{
}


/*!
 * \brief The copy constructor with deep copy.
 *
 * \note copyStructure has to be called BEFORE interface is copied
 */
PetriNet::PetriNet(const PetriNet & net) :
  observer_(*this), interface_(*this),
  roles_(net.roles_), meta_(net.meta_),
  warnings_(net.warnings_), reducablePlaces_(NULL),
  finalCondition_(net.finalCondition_, copyStructure(net))
{
  setConstraintLabels(net.constraints_);
}


/*!
 * \brief The destructor of the PetriNet class.
 */
PetriNet::~PetriNet()
{
  clear();

  assert(nodes_.empty());
  assert(nodesByName_.empty());
  assert(transitions_.empty());
  assert(places_.empty());
  assert(arcs_.empty());
  assert(roles_.empty());
}

/*!
 * \brief clears the net
 */
void PetriNet::clear()
{
  meta_.clear();
  constraints_.clear();
  finalCondition_ = true;
  warnings_ = 0;
  roles_.clear();
  interface_.clear();
  
  // delete all places
  set<Place *> places = places_;
  PNAPI_FOREACH(it, places)
  {
    deletePlace(**it);
  }

  // delete all transitions
  set<Transition *> transitions = transitions_;
  PNAPI_FOREACH(it, transitions)
  {
    deleteTransition(**it);
  }
}

/*!
 * \brief The assignment operator.
 */
PetriNet & PetriNet::operator=(const PetriNet & net)
{
  assert(this != &net);

  this->~PetriNet();
  return *new (this) PetriNet(net);
}


/*!
 * \brief Adds all nodes and arcs of the second net.
 *
 * \pre   the node names are unique (disjoint name sets); use
 *        prefixNodeNames() on <em>both</em> nets to achieve this
 */
std::map<const Place *, const Place *>
PetriNet::copyStructure(const PetriNet & net, const std::string & prefix)
{
  // copy interface
  map<Label *, Label *> labelMap;
  PNAPI_FOREACH(port, net.interface_.getPorts())
  {
    Port & p = ((port->first == "") ? (*interface_.getPort()) : interface_.addPort(port->first));
    PNAPI_FOREACH(label, port->second->getAllLabels())
    {
      Label * l = new Label(*this, p, (*label)->getName(), (*label)->getType());
      labelMap[*label] = l;
      p.addLabel(*l);
    }
  }
  
  // add all transitions of the net
  PNAPI_FOREACH(it, net.transitions_)
  {
    assert(!containsNode((*it)->getName()));
    Transition & t = createTransition(**it, prefix);
    PNAPI_FOREACH(l, (*it)->getLabels())
    {
      t.addLabel(*labelMap[l->first], l->second);
    }
  }

  // add all places
  const map<const Place *, const Place *> & placeMapping = copyPlaces(net, prefix);

  // create arcs according to the arcs in the net
  PNAPI_FOREACH(it, net.arcs_)
  {
    new Arc(*this, observer_, **it);
  }

  return placeMapping;
}


/*!
 * \brief adds the places of a second net
 */
std::map<const Place *, const Place *>
PetriNet::copyPlaces(const PetriNet & net, const std::string & prefix)
{
  map<const Place *, const Place *> placeMapping;
  PNAPI_FOREACH(it, net.places_)
  {
    assert(!containsNode((*it)->getName()));
    placeMapping[*it] = new Place(*this, observer_, **it, prefix);
  }
  return placeMapping;
}


/*!
 * \brief get the final condition
 */
Condition & PetriNet::getFinalCondition()
{
  return finalCondition_;
}


/*!
 * \brief get the final condition
 */
const Condition & PetriNet::getFinalCondition() const
{
  return finalCondition_;
}


/*!
 * \brief Compose this net to a second net.
 * 
 * Given a second Petri net "net", a resulting net is created
 * by applying the following steps:
 * 
 * 1.: The interfaces will be composed, gaining new places
 *     and a set of common synchronous labels.
 * 2.: Internal places of this net and "net" are prefixed and copied
 *     in the resulting net
 * 3.: Transitions are arranged by the following rules:
 *     a) Transitions without common labels will be prefixed and copied.
 *     b) If two transitions t1 and t2 share at least one
 *     synchronous label, a transition t' will be created. The pre- and
 *     postset of t' is the union of the pre- or postsets
 *     of t1 and t2 appropriately. The labels of t' are the difference
 *     of the union of the labels of t1 and t2, and the common labels.
 *     c) If there is a transition with a common label but without a
 *     partner transition, it will be copied and "killed" by adding
 *     an empty pre-place.
 *     d) If a label has to be copied that was replaced by a place in
 *     step 1, an arc to this place will be created instead.  
 * 4.: Final conditions will be merged.
 * 
 * \todo  review me!
 * \todo  Think about synchronous transitions!!!
 * 
 */
void PetriNet::compose(const PetriNet & net, const std::string & myPrefix,
                       const std::string & netPrefix)
{
  // ------------ STEP 1 -----------------------------
  
  // mapping from old interface to new interface and new places
  map<Label *, Label *> label2label;
  map<Label *, Place *> label2place;
  // common synchronous labels
  set<Label *> commonLabels;
  
  // compose interface
  PetriNet result(interface_, net.interface_, label2label, label2place, commonLabels);

  // ------------ STEP 2 -----------------------------
  
  // mapping from source nets' places to result net's places
  map<const Place *, const Place *> placeMap;

  // copy internal places of this net
  PNAPI_FOREACH(p, places_)
  {
    placeMap[*p] = &result.createPlace(myPrefix+(*p)->getName(), (*p)->getTokenCount());
  }

  // copy internal places of "net"
  PNAPI_FOREACH(p, net.places_)
  {
    placeMap[*p] = &result.createPlace(netPrefix+(*p)->getName(), (*p)->getTokenCount());
  }

  // ------------ STEP 3 -----------------------------

  // sets of transitions to be merged
  set<Transition *> mergeThis;
  set<Transition *> mergeOther;

  // iterate through this net's transitions and copy transitions
  PNAPI_FOREACH(t, transitions_)
  {
    if( (!(*t)->isSynchronized()) || // this transition is not synchronized
        (util::setIntersection((*t)->getSynchronousLabels(), commonLabels).empty()) ) // transition has no shared label
    {
      // create a prefixed transition in the resulting net
      Transition & rt = result.createTransition(myPrefix+(*t)->getName());
      rt.setCost((*t)->getCost()); // copy transition costs

      // copy preset arcs
      PNAPI_FOREACH(f, (*t)->getPresetArcs())
      {
        result.createArc(*const_cast<Place *>(placeMap[static_cast<Place *>(&(*f)->getSourceNode())]), rt, (*f)->getWeight());
      }

      // copy postset arcs
      PNAPI_FOREACH(f, (*t)->getPostsetArcs())
      {
        result.createArc(rt, *const_cast<Place *>(placeMap[static_cast<Place *>(&(*f)->getTargetNode())]), (*f)->getWeight());
      }
      
      // copy labels
      PNAPI_FOREACH(l, (*t)->getLabels())
      {
        Label * rl = label2label[l->first]; // label in result net
        if(rl == NULL) // if no resulting label was found
        {
          // this label is now a place
          if(l->first->getType() == Label::INPUT) // if transition was consuming
          {
            result.createArc(*label2place[l->first], rt, l->second);
          }
          else // transition was producing
          {
            result.createArc(rt, *label2place[l->first], l->second);
          }
        }
        else
        {
          rt.addLabel(*rl, l->second);
        }
      }
    }
    else
    {
      mergeThis.insert(*t);
    }
  }
  
  // iterate through other net's transitions and copy transitions
  PNAPI_FOREACH(t, net.transitions_)
  {
    if( (!(*t)->isSynchronized()) || // this transition is not synchronized
        (util::setIntersection((*t)->getSynchronousLabels(), commonLabels).empty()) ) // transition has no shared label
    {
      // create a prefixed transition in the resulting net
      Transition & rt = result.createTransition(netPrefix+(*t)->getName());
      rt.setCost((*t)->getCost()); // copy transition costs

      // copy preset arcs
      PNAPI_FOREACH(f, (*t)->getPresetArcs())
      {
        result.createArc(*const_cast<Place *>(placeMap[static_cast<Place *>(&(*f)->getSourceNode())]), rt, (*f)->getWeight());
      }

      // copy postset arcs
      PNAPI_FOREACH(f, (*t)->getPostsetArcs())
      {
        result.createArc(rt, *const_cast<Place *>(placeMap[static_cast<Place *>(&(*f)->getTargetNode())]), (*f)->getWeight());
      }
      
      // copy labels
      PNAPI_FOREACH(l, (*t)->getLabels())
      {
        Label * rl = label2label[l->first]; // label in result net
        if(rl == NULL) // if no resulting label was found
        {
          // this label is now a place
          if(l->first->getType() == Label::INPUT) // if transition was consuming
          {
            result.createArc(*label2place[l->first], rt, l->second);
          }
          else // transition was producing
          {
            result.createArc(rt, *label2place[l->first], l->second);
          }
        }
        else
        {
          rt.addLabel(*rl, l->second);
        }
      }
    }
    else
    {
      mergeOther.insert(*t);
    }
  }
  

  /*
   * transitions without a partner
   *  
   * Transitions of the other net are assumed as "lonely".
   * If such a transition gets a partner, it is removed from this set.
   * Own transitions are assumed as not "lonely". If such a transition
   * remains without a partner, it is added to this set.
   */
  set<Transition *> lonelyTransitions1;
  set<Transition *> lonelyTransitions2 = mergeOther;

  /// transitions that must be killed
  set<Transition *> doomedTransitions;

  /// merge matching synchronous transitions
  PNAPI_FOREACH(t1, mergeThis)
  {
    bool stayLonely = true; // if t1 finds a partner
    PNAPI_FOREACH(t2, mergeOther)
    {
      set<string> t1Labels = (*t1)->getSynchronousLabelNames();
      set<string> t2Labels = (*t2)->getSynchronousLabelNames();
      set<string> t12Labels = util::setIntersection(t1Labels, t2Labels); 

      if(!t12Labels.empty()) // if t1 and t2 share at least one label
      {
        // create new transition by merging
        Transition & rt = result.createTransition((*t1)->getName() + netPrefix + (*t2)->getName());
        /// TODO: maybe calculate other costs for merged transitions
        rt.setCost((*t1)->getCost() + (*t2)->getCost()); // copy transition costs

        // copy preset arcs
        PNAPI_FOREACH(f, (*t1)->getPresetArcs())
        {
          result.createArc(*const_cast<Place *>(placeMap[static_cast<Place *>(&(*f)->getSourceNode())]), rt, (*f)->getWeight());
        }

        // copy postset arcs
        PNAPI_FOREACH(f, (*t1)->getPostsetArcs())
        {
          result.createArc(rt, *const_cast<Place *>(placeMap[static_cast<Place *>(&(*f)->getTargetNode())]), (*f)->getWeight());
        }

        // copy "net"'s preset arcs
        PNAPI_FOREACH(f, (*t2)->getPresetArcs())
        {
          result.createArc(*const_cast<Place *>(placeMap[static_cast<Place *>(&(*f)->getSourceNode())]), rt, (*f)->getWeight());
        }

        // copy "net"'s postset arcs
        PNAPI_FOREACH(f, (*t2)->getPostsetArcs())
        {
          result.createArc(rt, *const_cast<Place *>(placeMap[static_cast<Place *>(&(*f)->getTargetNode())]), (*f)->getWeight());
        }

        // copy labels
        bool doomed = false;
        PNAPI_FOREACH(l, (*t1)->getLabels())
        {
          Label * rl = label2label[l->first]; // label in resulting net
          if(rl == NULL) // label is a common label or now a place
          {
            Place * p = label2place[l->first]; // former interface label
            if(p == NULL)
            {
              // this label is a common label
              if(t12Labels.find(l->first->getName()) == t12Labels.end())
              {
                // this transition holds a common label that the other one doesn't
                doomed = true;
                //TODO: partial matching may be handled differently
              }
            }
            else
            {
              if(l->first->getType() == Label::INPUT) // transition was consuming
              {
                result.createArc(*p, rt, l->second);
              }
              else // transition was producing
              {
                result.createArc(rt, *p, l->second);
              }
            }
          }
          else
          {
            rt.addLabel(*rl, l->second);
          }
        }
        PNAPI_FOREACH(l, (*t2)->getLabels())
        {
          Label * rl = label2label[l->first]; // label in resulting net
          if(rl == NULL) // label is a common label or now a place
          {
            Place * p = label2place[l->first]; // former interface label
            if(p == NULL)
            {
              // this label is a common label
              if(t12Labels.find(l->first->getName()) == t12Labels.end())
              {
                // this transition holds a common label that the other one doesn't
                doomed = true;
                //TODO: partial matching may be handled differently
              }
            }
            else
            {
              if(l->first->getType() == Label::INPUT) // transition was consuming
              {
                result.createArc(*p, rt, l->second);
              }
              else // transition was producing
              {
                result.createArc(rt, *p, l->second);
              }
            }
          }
          else
          {
            rt.addLabel(*rl, l->second);
          }
        }
        
        if(doomed)
        {
          doomedTransitions.insert(&rt);
        }
        
        // both transitions found a partner
        stayLonely = false;
        lonelyTransitions2.erase(*t2);
      }
    }
    if(stayLonely)
    {
      lonelyTransitions1.insert(*t1);
    }
  }

  // copy lonely transitions
  PNAPI_FOREACH(t, lonelyTransitions1)
  {
    // create new transition by merging
    Transition & rt = result.createTransition(myPrefix + (*t)->getName());
    rt.setCost((*t)->getCost()); // copy transition costs

    // copy preset arcs
    PNAPI_FOREACH(f, (*t)->getPresetArcs())
    {
      result.createArc(*const_cast<Place *>(placeMap[static_cast<Place *>(&(*f)->getSourceNode())]), rt, (*f)->getWeight());
    }
  
    // copy postset arcs
    PNAPI_FOREACH(f, (*t)->getPostsetArcs())
    {
      result.createArc(rt, *const_cast<Place *>(placeMap[static_cast<Place *>(&(*f)->getTargetNode())]), (*f)->getWeight());
    }

    // copy labels
    PNAPI_FOREACH(l, (*t)->getLabels())
    {
      Label * rl = label2label[l->first]; // label in resulting net
      if(rl == NULL) // label not found in net
      {
        Place * p = label2place[l->first]; // maybe label is now a place
        if(p != NULL) // yes, it is
        {
          if(l->first->getType() == Label::INPUT) // transition was consuming
          {
            result.createArc(*p, rt, l->second);
          }
          else // transition was producing
          {
            result.createArc(rt, *p, l->second);
          }
        }
      }
      else
      {
        rt.addLabel(*rl, l->second);
      }
    }
    
    // this transition is dead
    doomedTransitions.insert(&rt);
  }
  PNAPI_FOREACH(t, lonelyTransitions2)
  {
    // create new transition by merging
    Transition & rt = result.createTransition(netPrefix + (*t)->getName());
    rt.setCost((*t)->getCost()); // copy transition costs

    // copy preset arcs
    PNAPI_FOREACH(f, (*t)->getPresetArcs())
    {
      result.createArc(*const_cast<Place *>(placeMap[static_cast<Place *>(&(*f)->getSourceNode())]), rt, (*f)->getWeight());
    }
  
    // copy postset arcs
    PNAPI_FOREACH(f, (*t)->getPostsetArcs())
    {
      result.createArc(rt, *const_cast<Place *>(placeMap[static_cast<Place *>(&(*f)->getTargetNode())]), (*f)->getWeight());
    }

    // copy labels
    PNAPI_FOREACH(l, (*t)->getLabels())
    {
      Label * rl = label2label[l->first]; // label in resulting net
      if(rl == NULL) // label not found in net
      {
        Place * p = label2place[l->first]; // maybe label is now a place
        if(p != NULL) // yes, it is
        {
          if(l->first->getType() == Label::INPUT) // transition was consuming
          {
            result.createArc(*p, rt, l->second);
          }
          else // transition was producing
          {
            result.createArc(rt, *p, l->second);
          }
        }
      }
      else
      {
        rt.addLabel(*rl, l->second);
      }
    }
    
    // this transition is dead
    doomedTransitions.insert(&rt);
  }
  

  // kill doomed transitions
  PNAPI_FOREACH(t, doomedTransitions)
  {
    Place & p = result.createPlace();
    result.createArc(p, **t);
  }

  // here be dragons
  result.finalCondition_.conjunct(finalCondition_, placeMap);
  result.finalCondition_.conjunct(net.finalCondition_, placeMap);

  // overwrite this net with the resulting net
  *this = result;
}


/*!
 * \brief create an arc between two nodes
 * 
 * \param   source  the source Node
 * \param   target  the target Node
 * \param   weight  weight of the Arc
 * 
 * \return  the newly created Arc
 */
Arc & PetriNet::createArc(Node & source, Node & target, unsigned int weight)
{
  return *new Arc(*this, observer_, source, target, weight);
}


/*!
 * \brief creates a place
 * 
 * If an empty name is given, one is generated using getUniqueNodeName().
 *
 * \param   name  the (initial) name of the place
 * \param   tokens initial marking of this place
 * \param   capacity capacity of this place
 * 
 * \return  the newly created place
 */
Place & PetriNet::createPlace(const std::string & name,
                              unsigned int tokens, unsigned int capacity)
{
  return *new Place(*this, observer_,
                    (name.empty() ? getUniqueNodeName("p") : name),
                    tokens, capacity);
}


/*!
 * \brief creates a transition
 * 
 * If an empty name is given, one is generated using getUniqueNodeName().
 */
Transition & PetriNet::createTransition(const std::string & name)
{
  return *new Transition(*this, observer_,
                         (name.empty() ? getUniqueNodeName("t") : name));
}


/*!
 * \brief Introduces a new port to the open net.
 */
Port & PetriNet::createPort(const std::string & name)
{
  return interface_.addPort(name);
}


/*!
 * \brief Creates an input label.
 */
Label & PetriNet::createInputLabel(const std::string & name, Port & port)
{
  return interface_.addInputLabel(name, port);
}


/*!
 * \brief Creates an input label
 */
Label & PetriNet::createInputLabel(const std::string & name, const std::string & port)
{
  return interface_.addInputLabel(name, port);
}


/*!
 * \brief Creates an output label
 */
Label & PetriNet::createOutputLabel(const std::string & name, Port & port)
{
  return interface_.addOutputLabel(name, port);
}


/*!
 * \brief Creates an output label
 */
Label & PetriNet::createOutputLabel(const std::string & name, const std::string & port)
{
  return interface_.addOutputLabel(name, port);
}


/*!
 * \brief Creates a synchronous label
 */
Label & PetriNet::createSynchronizeLabel(const std::string & name, Port & port)
{
  return interface_.addSynchronousLabel(name, port);
}


/*!
 * \brief Creates a synchronous label
 */
Label & PetriNet::createSynchronizeLabel(const std::string & name, const std::string & port)
{
  return interface_.addSynchronousLabel(name, port);
}


/*!
 * \brief whether net contains given node
 */
bool PetriNet::containsNode(const Node & node) const
{
  PNAPI_FOREACH(n, nodes_)
  {
    if(*n == &node)
    {
      return true;
    }
  }
  
  return false;
}


/*!
 * \brief whether net contains given node
 */
bool PetriNet::containsNode(const std::string & name) const
{
  return (findNode(name) != NULL);
}


/*!
 * \brief find a node by name
 */
Node * PetriNet::findNode(const std::string & name) const
{
  map<string, Node *>::const_iterator result = nodesByName_.find(name);

  if (result != nodesByName_.end())
    return result->second;

  return NULL;
}


/*!
 * \brief find a place by name
 * 
 * \return  a pointer to the place or a NULL pointer if the place was not
 *          found.
 */
Place * PetriNet::findPlace(const std::string & name) const
{
  // if we had a map placesByName we could avoid the dynamic cast here
  return dynamic_cast<Place *>(findNode(name));
}


/*!
 * \brief find a transition by name
 * 
 * \return  a pointer to the transition or a NULL pointer if the transition
 *          was not found.
 */
Transition * PetriNet::findTransition(const std::string & name) const
{
  // if we had a map transitionsByName we could avoid the dynamic cast here
  return dynamic_cast<Transition *>(findNode(name));
}


/*!
 * \brief find an arc by its connected nodes
 */
Arc * PetriNet::findArc(const Node & source, const Node & target) const
{
  PNAPI_FOREACH(it, source.getPostsetArcs())
  {
    if(&((*it)->getTargetNode()) == &target)
    {
      return (*it);
    }
  }

  return NULL;
}

/// get the interface
Interface & PetriNet::getInterface()
{
  return interface_;
}

/*!
 * \brief get all nodes
 */
const std::set<Node *> & PetriNet::getNodes() const
{
  return nodes_;
}


/*!
 * \brief get places
 */
const std::set<Place *> & PetriNet::getPlaces() const
{
  return places_;
}


/*!
 * \brief get transitions
 */
const std::set<Transition *> & PetriNet::getTransitions() const
{
  return transitions_;
}


/*!
 * \brief get arcs
 */
const std::set<Arc *> & PetriNet::getArcs() const
{
  return arcs_;
}

/*!
 * \brief get roles
 */
const std::set<std::string> & PetriNet::getRoles() const
{
  return roles_;
}

/*!
 * \brief get warnings
 */
unsigned int PetriNet::getWarnings() const
{
  return warnings_;
}

/*!
 * \brief set warnings
 */
void PetriNet::setWarnings(unsigned int warnings)
{
  warnings_ = warnings;
}

/*!
 * \brief get transitions with synchronous labels
 */
const std::set<Transition *> & PetriNet::getSynchronizedTransitions() const
{
  return synchronizedTransitions_;
}


/*!
 * \brief Adds a role
 */
void PetriNet::addRole(const std::string & roleName) 
{
  roles_.insert(roleName);
}

/*!
 * \brief Adds a set of roles
 */
void PetriNet::addRoles(const std::set<std::string> & roles)
{
  PNAPI_FOREACH(role, roles)
  {
    roles_.insert(*role);
  }
}


/*!
 * \brief get a unique node name
 * 
 * \param   base  base name
 * \return  a string to be used as a name for a new node
 */
std::string PetriNet::getUniqueNodeName(const std::string & base) const
{
  int i = 0;
  string name;

  // use a "mutable" cache to make this more efficient
  do
  {
    ostringstream str;
    str << base << ++i;
    name = str.str();
  }
  while(nodesByName_.find(name) != nodesByName_.end());

  return name;
}


/*!
 * \brief   checks whether the Petri net is free choice
 *
 *          A Petri net is free choice iff
 *          all two transitions have either disjoint or equal presets
 *
 * \return  true if the criterion is fulfilled and false if not
 */
bool PetriNet::isFreeChoice() const
{
  PNAPI_FOREACH(t, transitions_)
  {
    PNAPI_FOREACH(tt, transitions_)
    {
      set<Node *> t_pre  = (*t)->getPreset();
      set<Node *> tt_pre = (*tt)->getPreset();
      if((t_pre != tt_pre) &&
          !(util::setIntersection(t_pre, tt_pre).empty()))
      {
        return false;
      }
    }
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
  PNAPI_FOREACH(t, transitions_)
  {
    if(!(*t)->isNormal())
      return false;
  }

  return true;
}

/*!
 * \brief   Checks whether a transition role name is specified
 *
 * \return  true iff the transition role name has been specified
 */
bool PetriNet::isRoleSpecified(const std::string & roleName) const
{
  if(roles_.find(roleName) != roles_.end())
  {
    return true;
  }
  
  return false;
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
std::map<Transition *, std::string> PetriNet::normalize()
{
  if(interface_.getSynchronousLabels().empty())
  {
    normalize_rules();
  }
  else
  {
    normalize_classical();
  }

  std::map<Transition *, string> edgeLabels; ///< returning map

  // set default value and overwrite communicating transitions
  PNAPI_FOREACH(t, transitions_)
  {
    edgeLabels[*t] = "TAU";
  }
  
  // get input labels
  set<Label *> inputLabels = interface_.getInputLabels(); 
  PNAPI_FOREACH(label, inputLabels)
  {
    PNAPI_FOREACH(t, (*label)->getTransitions())
    {
      edgeLabels[*t] = (*label)->getName();
    }
  }

  // get output labels
  set<Label *> outputLabels = interface_.getOutputLabels(); 
  PNAPI_FOREACH(label, outputLabels)
  {
    PNAPI_FOREACH(t, (*label)->getTransitions())
    {
      edgeLabels[*t] = (*label)->getName();
    }
  }
  
  // get synchronous labels
  set<Label *> synLabels = interface_.getSynchronousLabels();
  PNAPI_FOREACH(label, synLabels)
  {
    PNAPI_FOREACH(t, (*label)->getTransitions())
    {
      edgeLabels[*t] = (*label)->getName();
    }
  }

  return edgeLabels;
}


/*!
 * \brief product with Constraint oWFN
 * 
 * described in "Behavioral Constraints for Services" (BPM 2007)
 * (http://dx.doi.org/10.1007/978-3-540-75183-0_20)
 *
 * \note  There is an error in definition 5: The arcs of the transitions
 *        with empty label are missing.
 */
void PetriNet::produce(const PetriNet & net, const std::string & aPrefix,
                       const std::string & aNetPrefix) throw (exception::InputError)
{
  assert(!aPrefix.empty());
  assert(!aNetPrefix.empty());
  assert(aPrefix != aNetPrefix);
  assert(net.interface_.getInputLabels().empty());
  assert(net.interface_.getOutputLabels().empty());

  string prefix = aPrefix + ".";
  string netPrefix = aNetPrefix + ".";
  map<Transition *, set<Transition *> > labels = translateConstraintLabels(net);

  // add places
  prefixNodeNames(prefix); // prefix nodes
  map<const Place *, const Place *> placeMapping = copyPlaces(net, netPrefix);

  // conjunct final conditions
  finalCondition_.conjunct(net.finalCondition_, placeMapping);

  // handle transitions with empty label
  PNAPI_FOREACH(it, net.transitions_)
  {
    Transition & netTrans = **it; // t'
    set<Transition *> label;
    if (labels.find(&netTrans) != labels.end())
    {
      label = labels.find(&netTrans)->second;
    }
    if (label.empty())
    {
      Transition & trans = createTransition(netTrans, netPrefix);
      createArcs(trans, netTrans, &placeMapping);
    }
  }

  // create product transitions
  set<Transition *> labelTransitions;
  PNAPI_FOREACH(it1, labels)
  {
    assert(it1->first != NULL);
    Transition & netTrans = *it1->first;             // t'
    assert(net.containsNode(netTrans));
    set<Transition *> ts = it1->second;
    PNAPI_FOREACH(it2, ts)
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
  PNAPI_FOREACH(it, labelTransitions)
  {
    deleteTransition(**it);
  }
}


/*!
 * \brief sets labels (and translates references)
 */
void PetriNet::setConstraintLabels(const std::map<Transition *, std::set<std::string> > & labels)
{
  constraints_.clear();
  PNAPI_FOREACH(it, labels)
  {
    Transition * t = findTransition(it->first->getName());
    assert(t != NULL);
    constraints_[t] = it->second;
  }
}

/*!
 * \brief translates constraint labels to transitions
 */
std::map<Transition *, std::set<Transition *> >
PetriNet::translateConstraintLabels(const PetriNet & net) throw (exception::InputError)
{
  map<Transition *, set<string> > labels = net.constraints_;
  map<Transition *, set<Transition *> > result;
  PNAPI_FOREACH(it1, labels)
  {
    assert(it1->first != NULL);
    Transition & t = *it1->first;
    
    PNAPI_FOREACH(it2, it1->second)
    {
      Transition * labelTrans = findTransition(*it2);
      if (labelTrans != NULL)
      {
        result[&t].insert(labelTrans);
      }
      else
      {
        string filename =
          ((net.meta_.find(io::INPUTFILE) != net.meta_.end())
          ? net.meta_.find(io::INPUTFILE)->second : "");
        
        throw exception::InputError(exception::InputError::SEMANTIC_ERROR, filename,
                                    0, *it2, "unknown transition");
      }
    }
  }
  return result;
}


/*!
 * \brief creates a transition as a copy of another one
 */
Transition & PetriNet::createTransition(const Transition & t,
                                        const std::string & prefix)
{
  return *new Transition(*this, observer_, t, prefix);
}


/*!
 * \brief creates arcs for a transition based on the arcs of another one
 */
void PetriNet::createArcs(Transition & trans, Transition & otherTrans,
                          const std::map<const Place *, const Place *> * placeMapping)
{
  typedef set<Arc *> Arcs;

  // add preset arcs of t' to t
  Arcs preset = otherTrans.getPresetArcs();
  PNAPI_FOREACH(it, preset)
  {
    Arc & arc = **it;
    Place & place = ((placeMapping == NULL) ? arc.getPlace() :
                     (*const_cast<Place *>(placeMapping->find(&arc.getPlace())->second)));
      new Arc(*this, observer_, arc, place, trans);
  }

  // add postset arcs of t' to t
  Arcs postset = otherTrans.getPostsetArcs();
  PNAPI_FOREACH(it, postset)
  {
    Arc & arc = **it;
    Place & place = ((placeMapping == NULL) ? arc.getPlace() :
                     (*const_cast<Place *>(placeMapping->find(&arc.getPlace())->second)));
    
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
 * 
 * \todo    Erkennung der SZK ohne Hinzufuegen der Transition
 *          Daniela nach Alorithmus fragen; Funktion const machen;
 *          anschließend methode const machen
 */
bool PetriNet::isWorkflow()
{
  Place * first = NULL;
  Place * last  = NULL;

  // finding places described in condition (1) & (2)
  PNAPI_FOREACH(p, places_)
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
  
  if((first == NULL) || (last == NULL))
    return false;

  // insert new transition which consumes from last and produces on first to form a cycle
  Transition & tarjan = createTransition("tarjan");
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
  PNAPI_FOREACH(p, places_)
  {
    index[*p] = (-1);
  }
  PNAPI_FOREACH(t, transitions_)
  {
    index[*t] = (-1);
  }

  // getting the number of strongly connected components reachable from first
  unsigned int sscCount = util::dfsTarjan<Node *>(first, S, stacked, i, index, lowlink);

  deleteTransition(tarjan);

  // check set $P \cup T$
  set<Node *> nodeSet;
  PNAPI_FOREACH(p, places_)
  {
    nodeSet.insert(*p);
  }
  PNAPI_FOREACH(t, transitions_)
  {
    nodeSet.insert(*t);
  }

  /*
   * true iff only one strongly connected component found from first and all nodes
   * of $\mathcal{N}$ are members of this component
   */
  if((sscCount == 1) && (util::setDifference(nodeSet, stacked).empty()))
    return true;
  else
    return false;
}


/*!
 * \brief swaps input and output labels
 */
void PetriNet::mirror()
{
  interface_.mirror();
}


/****************************************************************************
 *** Private PetriNet Function Definitions
 ***************************************************************************/

/*!
 * \brief returns the meta information if available
 */
std::string PetriNet::getMetaInformation(std::ios_base & ios,
                                         io::MetaInformation i,
                                         const std::string & def) const
{
  map<io::MetaInformation, string> & streamMeta = io::util::MetaData::data(ios);
  if(streamMeta.find(i) != streamMeta.end())
    return streamMeta.find(i)->second;
  
  if(meta_.find(i) != meta_.end())
    return meta_.find(i)->second;
  
  return def;
}


/*!
 * \brief All Places and Transitions of the net are prefixed.
 *
 * \param   prefix  the prefix to be added
 */
PetriNet & PetriNet::prefixNodeNames(const std::string & prefix)
{
  PNAPI_FOREACH(it, nodes_)
  {
    (*it)->prefixNameHistory(prefix);
  }
  
  return *this;
}

/*!
 * \brief deletes a place
 */
void PetriNet::deletePlace(Place & place)
{
  finalCondition_.removePlace(place);
  
  places_.erase(&place);
  deleteNode(place);
}

/*!
 * \brief deletes a transition
 */
void PetriNet::deleteTransition(Transition & trans)
{
  if (trans.isSynchronized())
  {
    synchronizedTransitions_.erase(&trans);
  }
  transitions_.erase(&trans);

  deleteNode(trans);
}


/*!
 * \brief deletes a node
 * 
 * \note  Never call this method directly! Use deletePlace() or
 *        deleteTransition() instead!
 */
void PetriNet::deleteNode(Node & node)
{
  assert(containsNode(node));
  assert((dynamic_cast<Place *>(&node) == NULL) ? true :
         (places_.find(dynamic_cast<Place *>(&node)) == places_.end()));
  assert((dynamic_cast<Transition *>(&node) == NULL) ? true :
         (transitions_.find(dynamic_cast<Transition *>(&node)) == transitions_.end()));

  while(!node.getPreset().empty())
    deleteArc(*findArc(**node.getPreset().begin(), node));
  
  while (!node.getPostset().empty())
    deleteArc(*findArc(node, **node.getPostset().begin()));

  // remove access to this nodes
  nodesByName_.erase(*node.getNameHistory().begin());
  
  nodes_.erase(&node);

  delete &node;
}


/*!
 * \brief deletes an arc
 * 
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
 * \brief normalizes a net according to [LohmannMW_2007_icatpn], Figure 4
 *        with fix for arc weights
 * 
 * Each interface place connected with an unnormal transition 
 * will be wrapped by a n-bounded place, where n is the maximum
 * weight of all arcs connected to the interface place.
 */
void PetriNet::normalize_classical()
{
  // iterate through input labels
  set<Label *> inputs = interface_.getInputLabels();
  PNAPI_FOREACH(label, inputs)
  {
    bool needToWrap = false;
    // check the postset
    PNAPI_FOREACH(t, (*label)->getTransitions())
    {
      if(!((*t)->isNormal()))
      {
        needToWrap = true;
        break;
      }
    }
    
    // if a transition was not normal
    if(!needToWrap)
    {
      continue;
    }
    
    unsigned int complementMarking = 0;
    // then we create a wrapper place
    Place & np = createPlace();
    // and a complement place
    Place & cp = createPlace();
    
    set<Transition *> transitions = (*label)->getTransitions();
    
    // connect postset of the interface place to the wrapper place
    PNAPI_FOREACH(t, transitions)
    {
      unsigned int weight = (*t)->getLabels().find(*label)->second;
      // consume tokens from the wrapper place
      createArc(np, **t, weight);
      // produce same amount of tokens to the complement place
      createArc(**t, cp, weight);
      // adjust maximum arc weight
      complementMarking = (complementMarking < weight) ? weight : complementMarking;
      // remove original "arc"
      (*t)->removeLabel(**label);
    }
    
    // connect wrapper place with actual place
    Transition & t = createTransition();
    t.addLabel(**label);
    createArc(t ,np);
    createArc(cp, t);

    // adjust complement place
    cp.setTokenCount(complementMarking);
    // adjust final condition
    finalCondition_ = (finalCondition_.getFormula() && (np == 0) && (cp == complementMarking));  
  }
  
  // iterate through output places
  set<Label *> outputs = interface_.getOutputLabels();
  PNAPI_FOREACH(label, outputs)
  {
    bool needToWrap = false;
    // check the preset
    PNAPI_FOREACH(t, (*label)->getTransitions())
    {
      if(!((*t)->isNormal()))
      {
        needToWrap = true;
        break;
      }
    }
    
    // if a transition was not normal
    if(!needToWrap)
    {
      continue;
    }
    
    unsigned int complementMarking = 0;
    // then we create a wrapper place
    Place & np = createPlace();
    // and a complement place
    Place & cp = createPlace();
    
    set<Transition *> transitions = (*label)->getTransitions();
    // connect preset of the interface place to the wrapper place
    PNAPI_FOREACH(t, transitions)
    {
      unsigned int weight = (*t)->getLabels().find(*label)->second;
      // consume tokens from the wrapper place
      createArc(**t, np, weight);
      // produce same amount of tokens to the complement place
      createArc(cp, **t, weight);
      // adjust maximum arc weight
      complementMarking = (complementMarking < weight) ? weight : complementMarking;
      // remove original "arc"
      (*t)->removeLabel(**label);
    }
    
    // connect wrapper place with actual place
    Transition & t = createTransition();
    t.addLabel(**label);
    createArc(np, t);
    createArc(t, cp);

    // adjust complement place
    cp.setTokenCount(complementMarking);
    // adjust final condition
    finalCondition_ = (finalCondition_.getFormula() && (np == 0) && (cp == complementMarking));
  }
}


/*!
 * \brief normalization after [Aalst07]
 * 
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

} /* namespace pnapi */

/*!
 * This auxiliary function has the sole purpose to allow other tools to
 * check whether they can link agains this PNAPI library using Autoconf's
 * command "AC_CHECK_LIB(pnapi, libpnapi_is_present)".
 *
 * \return Always 0 to indicate successful termination.
 */
extern "C" {
  char libpnapi_is_present() {
      pnapi::PetriNet a();
      return 0;
  }
}
