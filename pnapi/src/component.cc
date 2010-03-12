/*!
 * \file    component.cc
 */

#include "config.h"

#include <set>

#include "util.h"
#include "petrinet.h"
#include "component.h"
#include "port.h"

using std::set;
using std::map;
using std::string;

namespace pnapi {

using util::ComponentObserver;


/****************************************************************************
 *** Class Node Function Definitions
 ***************************************************************************/


/*!
 * \brief constructor
 */
Node::Node(PetriNet & net, util::ComponentObserver & observer,
           const std::string & name) :
  net_(net), observer_(observer)
{
  assert(&observer.getPetriNet() == &net);
  history_.push_back(name);
}


/*!
 * \brief copy constructor
 */
Node::Node(PetriNet & net, util::ComponentObserver & observer,
           const Node & node, const std::string & prefix) :
  net_(net), observer_(observer), history_(node.history_)
{
  assert(&observer.getPetriNet() == &net);
  if (!prefix.empty())
  {
    PNAPI_FOREACH(it, history_)
    {
      *it = prefix + *it;
    }
  }
}


/*!
 * \brief destructor
 * 
 * Do not delete a node directly but ask the net instead:
 * PetriNet::deleteNode()
 */
Node::~Node()
{
  assert(!net_.containsNode(*this));
}

/*!
 * \brief Checks if two Nodes are parallel to each other.
 *
 * According to [STA90] (see petrinet-reduction.cc):
 * If there exist two distinct nodes n1 (this) and n2 (precondition 1)
 * with identical preset (precondition 2)
 * and identical postset (precondition 3)
 * and the arc weight for each node of the pre- and postset to/from p1
 * equals the arc weiht to/from n2 appropriate (precondition 4)
 * and, if both nodes are transitions, if they are connected to the same
 * interface labels with the same weight (precondition 5),
 * then these nodes are parallel to each other.
 *
 * This function is needed by PetriNet::reduce_rule_3p()
 * and PetriNet::reduce_rule_3t().
 *
 * \param n2 the other node to which parallelism will be checked.
 */
bool Node::isParallel(const Node & n2) const
{
  if (
      (this != &n2) &&   // precondition 1
      (getPreset() == n2.getPreset()) && // precondition 2
      (getPostset() == n2.getPostset()) // precondition 3
     )
  {
    // precondition 4 - preset
    PNAPI_FOREACH(preArc_, presetArcs_)
    {
      if((*preArc_)->getWeight() != net_.findArc(((*preArc_)->getSourceNode()),n2)->getWeight() )
        return false;
    }

    // precondition 4 - postset
    PNAPI_FOREACH(postArc_, postsetArcs_)
    {
      if((*postArc_)->getWeight() != net_.findArc(n2,(*postArc_)->getTargetNode())->getWeight() )
        return false;
    }
    
    // precondition 5
    Transition * t1 = const_cast<Transition *>(dynamic_cast<const Transition *>(this));
    Transition * t2 = const_cast<Transition *>(dynamic_cast<const Transition *>(&n2));
    if((t1 != NULL) && (t2 != NULL))
    {
      return t1->equalLabels(*t2);
    }

    // all preconditions fullfilled
    return true;
  }
  return false;
}


/*!
 * \brief returns the name of the node
 */
std::string Node::getName() const
{
  assert(!history_.empty());
  return *history_.begin();
}


/*!
 * \brief adds a prefix to all names
 */
void Node::prefixNameHistory(const std::string & prefix)
{
  assert(!prefix.empty());
  std::deque<string> oldHistory = history_;
  PNAPI_FOREACH(it, history_)
  {
    *it = prefix + *it;
  }
  
  observer_.updateNodeNameHistory(*this, oldHistory);
}


/*!
 * \brief returns the name history
 */
std::deque<std::string> Node::getNameHistory() const
{
  return history_;
}

/*!
 * \brief set new name of the node
 */
void Node::setName(const std::string & newName)
{
  // add history of node to this
  std::deque<string> oldHistory = history_;
  history_.push_front(newName);
  observer_.updateNodeNameHistory(*this, oldHistory);
}

/*!
 * \brief returns the petri net this node belongs to
 */
PetriNet & Node::getPetriNet() const
{
  return net_;
}


/*!
 * \brief returns the node's preset
 */
const std::set<Node *> & Node::getPreset() const
{
  return preset_;
}


/*!
 * \brief returns the node's postset
 */
const std::set<Node *> & Node::getPostset() const
{
  return postset_;
}


/*!
 * \brief returns the arcs to the node's preset
 */
const std::set<Arc *> & Node::getPresetArcs() const
{
  return presetArcs_;
}


/*!
 * \brief returns the arcs to the node's postset
 */
const std::set<Arc *> & Node::getPostsetArcs() const
{
  return postsetArcs_;
}


/*!
 * \brief merges another node into this one
 * 
 * \param node the other node
 * \param addArcWeights whether arc weights shall be added
 * 
 * \post other node's history is merges with this node's history
 * \post other node will be deleted
 */
void Node::merge(Node & node, bool addArcWeights)
{
  assert(&net_ == &node.net_);

  mergeNameHistory(node);

  mergeArcs(node, addArcWeights, false);
  mergeArcs(node, addArcWeights, true);

  // delete other node
  Place * place = dynamic_cast<Place *>(&node);
  if (place != NULL)
    net_.deletePlace(*place);
  else
    net_.deleteTransition(*static_cast<Transition *>(&node));
}


/*!
 * \brief merges the histories of two nodes
 */
void Node::mergeNameHistory(const Node & node)
{
  std::deque<string> nodeHistory = node.history_;

  // add history of node to this
  std::deque<string> oldHistory = history_;
  history_.insert(history_.end(), nodeHistory.begin(), nodeHistory.end());
  observer_.updateNodeNameHistory(*this, oldHistory);
}


/*!
 * \brief merges the pre-/postsets of two nodes
 * 
 * \todo review adding of arc weights
 *       maybe parameter addWeighs is obsolete - when are weights not to be added?
 */
void Node::mergeArcs(const Node & node, bool addWeights, bool isPostset)
{
  const set<Arc *> & arcSet = (isPostset ? (node.postsetArcs_) : (node.presetArcs_));
  PNAPI_FOREACH(arc2, arcSet)
  {
    Node & arc1Source = isPostset ? (*this) : ((*arc2)->getSourceNode());
    Node & arc1Target = isPostset ? ((*arc2)->getTargetNode()) : (*this);

    Arc * arc1 = net_.findArc(arc1Source, arc1Target);

    if (arc1 == NULL)
    {
      net_.createArc(arc1Source, arc1Target, (*arc2)->getWeight());
    }
    else
    {
      if(addWeights)
      {
        arc1->merge(**arc2);
      }
      else
      {
        assert(arc1->getWeight() == (*arc2)->getWeight());
      }
    }
  }
}


/****************************************************************************
 *** Class Place Function Definitions
 ***************************************************************************/


/*!
 * \brief constructor
 */
Place::Place(PetriNet & net, util::ComponentObserver & observer,
             const std::string & name, unsigned int tokens,
             unsigned int capacity) :
  Node(net, observer, name), tokens_(tokens), capacity_(capacity),
  wasInterface_(false), maxOccurrence_(-1)
{
  observer_.updatePlaces(*this);
}


/*!
 * \brief copy constructor
 */
Place::Place(PetriNet & net, util::ComponentObserver & observer,
             const Place & place, const std::string & prefix) :
  Node(net, observer, place, prefix), tokens_(place.tokens_),
  capacity_(place.capacity_), wasInterface_(place.wasInterface_),
  maxOccurrence_(place.maxOccurrence_)
{
  observer_.updatePlaces(*this);
}


/*!
 * \brief returns the number of tokens lying on this place
 */
unsigned int Place::getTokenCount() const
{
  return tokens_;
}


/*!
 * \brief sets the number of tokens lying on this place
 */
void Place::setTokenCount(unsigned int tokens)
{
  tokens_ = tokens;
}


/*!
 * \brief returns the capacity
 */
unsigned int Place::getCapacity() const
{
  return capacity_;
}


/*!
 * \brief if the place was an interface label
 */
bool Place::wasInterface() const
{
  return wasInterface_;
}


/*!
 * Merges the properties of the given node into this one.
 *
 * The following properties are merged:
 * - NameHistory (concatenation)
 * - WasInterface (disjunction)
 * - Capacity (maximum)
 * - TokenCount (maximum)
 * - Pre-/Postset (union)
 *
 * \pre   the places must reside in the same PetriNet instance
 * \post  the place is internal after merging
 * 
 * \todo review me!
 */
Place & Place::merge(Place & place, bool addArcWeights)
{
  // merge place properties
  wasInterface_ = wasInterface_ || place.wasInterface_;
  capacity_ = (capacity_ > place.capacity_) ? capacity_ : place.capacity_;
  tokens_ = (tokens_ > place.tokens_) ? tokens_ : place.tokens_;

  // Node::merge does all the rest
  Node::merge(place, addArcWeights);

  return *this;
}


/*!
 * \brief set the maximum occurrence
 */
void Place::setMaxOccurrence(int maxOccurrence)
{
  maxOccurrence_ = maxOccurrence;
}


/*!
 * \brief get the maximum occurrence
 */
int Place::getMaxOccurrence()
{
  return maxOccurrence_;
}

/*!
 * \brief set wasInterface flag
 */
void Place::setWasInterface(bool wasInterface)
{
  wasInterface_ = wasInterface;
}


/****************************************************************************
 *** Class Transition Function Definitions
 ***************************************************************************/


/*!
 * \brief constructor
 */
Transition::Transition(PetriNet & net, util::ComponentObserver & observer,
                       const std::string & name) :
  Node(net, observer, name), cost_(0), type_(INTERNAL)
{
  observer_.updateTransitions(*this);
}


/*!
 * \brief copy constructor
 */
Transition::Transition(PetriNet & net, util::ComponentObserver & observer,
                       const Transition & trans, const std::string & prefix) :
  Node(net, observer, trans, prefix), roles_(trans.roles_),
  type_(trans.type_), cost_(trans.cost_)
{
  observer_.updateTransitions(*this);
}

/*!
 * \brief destructor
 */
Transition::~Transition()
{
  PNAPI_FOREACH(label, labels_)
  {
    label->first->removeTransition(*this);
  }
}

/*!
 * \brief retrieves the communication type
 */
Transition::Type Transition::getType() const
{
  return type_;
}


/*!
 * \brief changes the type of this transition
 */
void Transition::setType(Transition::Type type)
{
  type_ = type;
}


/*!
 * \brief compares the type with another transition's type
 */
bool Transition::isComplementType(Transition::Type type) const
{
  return ( ((type_ == INPUT) && (type == OUTPUT)) ||
           ((type_ == OUTPUT) && (type == INPUT)) );
}


/*!
 * \brief set transition cost
 */
void Transition::setCost(int cost)
{
  cost_ = cost;
}

/*!
 * \brief get transition cost
 */
int Transition::getCost() const
{
  return cost_;
}

/*!
 * \brief add role to transition
 * 
 * \todo move changing of net's roles to net::addRole(role, transition)
 */
void Transition::addRole(const std::string & roleName)
{
  roles_.insert(roleName);
  net_.addRole(roleName);
}

/*!
 * \brief add set of roles to transition
 * 
 * \todo move changing of net's roles to net::addRole(roles, transition)
 */
void Transition::addRoles(const std::set<std::string> & roles)
{
  PNAPI_FOREACH(role, roles)
  {
    roles_.insert(*role);
  }
  net_.addRoles(roles);
}

/*!
 * \brief get transition roles
 */
const std::set<std::string> & Transition::getRoles() const
{
  return roles_;
}

/*!
 * \brief   checks if the transition is normal
 *
 * A transition is normal iff its connected
 * with at most one interface label
 * by a weight of at most one. 
 * 
 * \note    This is a help method for normalize method
 *          in class PetriNet.
 */
bool Transition::isNormal() const
{
  return ( (labels_.size() == 0) ||
           ( (labels_.size() == 1) &&
             (labels_.begin()->second == 1) ) );
}


/*!
 * Merges the properties of the given node into this one.
 *
 * The following properties are merged:
 * - NameHistory (concatenation)
 * - Pre-/Postset (union)
 * - interface labels (union)
 *
 * \pre  the transitions must reside in the same PetriNet instance
 * \post type will be recalculated
 */
void Transition::merge(Transition & trans)
{
  PNAPI_FOREACH(l, trans.labels_)
  {
    labels_[l->first] += l->second;
  }
  
  // Node::merge does all the remaining work
  Node::merge(trans, true);
  
  updateType();
}


/*!
 * \brief updates the type
 */
void Transition::updateType()
{
  setType(INTERNAL);
  
  PNAPI_FOREACH(l, labels_)
  {
    updateType(*(l->first));
    
    if (type_ == INOUT)
      break;
  }
}

/*!
 * \brief updates the type
 */
void Transition::updateType(const Label & l)
{
  if(type_ == INOUT)
    return;
  
  switch(l.getType())
  {
  case Label::INPUT:
  {
    if(type_ == OUTPUT)
    {
      setType(INOUT);
    }
    else
    {
      setType(INPUT);
    }
    break;
  }
  case Label::OUTPUT:
  {
    if(type_ == INPUT)
    {
      setType(INOUT);
    }
    else
    {
      setType(OUTPUT);
    }
    break;
  }
  default: /* do nothing */ ;
  }
}


/*!
 * \brief returns true, if the transition is associated to at least one synchronous label
 */
bool Transition::isSynchronized() const
{
  PNAPI_FOREACH(l, labels_)
  {
    if(l->first->getType() == Label::SYNCHRONOUS)
      return true;
  }
  
  return false;
}

/*!
 * \brief the set of synchronous labels
 */
std::set<Label *> Transition::getInputLabels() const
{
  set<Label *> result;
  
  PNAPI_FOREACH(l, labels_)
  {
    if(l->first->getType() == Label::INPUT)
    {
      result.insert(l->first);
    }
  }
  
  return result;
}

/*!
 * \brief the set of synchronous labels
 */
std::set<Label *> Transition::getOutputLabels() const
{
  set<Label *> result;
  
  PNAPI_FOREACH(l, labels_)
  {
    if(l->first->getType() == Label::OUTPUT)
    {
      result.insert(l->first);
    }
  }
  
  return result;
}

/*!
 * \brief the set of synchronous labels
 */
std::set<Label *> Transition::getSynchronousLabels() const
{
  set<Label *> result;
  
  PNAPI_FOREACH(l, labels_)
  {
    if(l->first->getType() == Label::SYNCHRONOUS)
    {
      result.insert(l->first);
    }
  }
  
  return result;
}

/*! 
 * \brief the names of synchronous labels
 */
std::set<std::string> Transition::getSynchronousLabelNames() const
{
  set<string> result;
  
  PNAPI_FOREACH(l, labels_)
  {
    if(l->first->getType() == Label::SYNCHRONOUS)
    {
      result.insert(l->first->getName());
    }
  }
  
  return result;
}

/*!
 * \brief add an interface label
 */
void Transition::addLabel(Label & label, unsigned int weight)
{
  labels_[const_cast<Label *>(&label)] += weight;
  label.addTransition(*this);
  updateType(label);
}

/*!
 * \brief remove an interface label
 */
void Transition::removeLabel(const Label & label)
{
  labels_.erase(const_cast<Label *>(&label));
}

/*!
 * \brief get mapping of interface labels to their weight
 */
const std::map<Label *, unsigned int> & Transition::getLabels() const
{
  return labels_;
}

/*!
 * \brief compare interface labels with given transition
 * 
 * \pre there is no label with "arc weight" 0
 * 
 * \return true iff both transitions are connected with the same
 *         interface labels by the same weights.
 */
bool Transition::equalLabels(const Transition & t) const
{
  if(labels_.size() != t.labels_.size())
    return false;
  
  PNAPI_FOREACH(l, labels_)
  {
    map<Label *, unsigned int>::const_iterator l2 = t.labels_.find(l->first);
    if((l2 == t.labels_.end()) || (l->second != l2->second))
      return false;
  }
  
  return true;
}


/****************************************************************************
 *** Class Arc Function Definitions
 ***************************************************************************/


/*!
 * \brief constructor
 */
Arc::Arc(PetriNet & net, util::ComponentObserver & observer,
         Node & source, Node & target, unsigned int weight) :
  net_(net), observer_(observer),
  source_(&source), target_(&target), weight_(weight)
{
  assert(&observer.getPetriNet() == &net);

  observer_.updateArcCreated(*this);
}


/*!
 * \brief copy constructor
 */
Arc::Arc(PetriNet & net, util::ComponentObserver & observer, const Arc & arc) :
  net_(net), observer_(observer),
  source_(net.findNode(arc.source_->getName())),
  target_(net.findNode(arc.target_->getName())), weight_(arc.weight_)
{
  assert(&observer.getPetriNet() == &net);
  assert(source_ != NULL);
  assert(target_ != NULL);

  observer_.updateArcCreated(*this);
}


/*!
 * \brief copy constructor
 */
Arc::Arc(PetriNet & net, util::ComponentObserver & observer,
         const Arc & arc, Node & source, Node & target) :
  net_(net), observer_(observer),
  source_(&source), target_(&target), weight_(arc.weight_)
{
  assert(&observer.getPetriNet() == &net);
  assert(net.containsNode(source));
  assert(net.containsNode(target));

  observer_.updateArcCreated(*this);
}


/*!
 * \brief destructor
 * 
 * You must not destroy an Arc directly.
 */
Arc::~Arc()
{
  assert(net_.findArc(*source_, *target_) == NULL);
}


/*!
 * \brief returns the Petri net this arc belongs to
 */
PetriNet & Arc::getPetriNet() const
{
  return net_;
}


/*!
 * \brief source node
 */
Node & Arc::getSourceNode() const
{
  return *source_;
}


/*!
 * \brief target node
 */
Node & Arc::getTargetNode() const
{
  return *target_;
}


/*!
 * \brief transition
 */
Transition & Arc::getTransition() const
{
  Transition * t = dynamic_cast<Transition *>(source_);
  if (t != NULL)
    return *t;

  t = dynamic_cast<Transition *>(target_);
  assert(t != NULL);
  return *t;
}


/*!
 * \brief place
 */
Place & Arc::getPlace() const
{
  Place * t = dynamic_cast<Place *>(source_);
  if (t != NULL)
    return *t;

  t = dynamic_cast<Place *>(target_);
  assert(t != NULL);
  return *t;
}


/*!
 * \brief returns weight
 */
unsigned int Arc::getWeight() const
{
  return weight_;
}

/*!
 * \brief set the weight
 */
void Arc::setWeight(unsigned int weight)
{
  weight_ = weight;
}

/*!
 * \brief merges another arc into this one
 */
void Arc::merge(Arc & arc)
{
  weight_ += arc.weight_;
}


/*!
 * \brief swaps source and target node
 * 
 * \todo review me and observer methods!
 */
void Arc::mirror()
{
  observer_.updateArcMirror(*this);
  observer_.updateArcRemoved(*this);
  Node * oldSource = source_;
  source_ = target_;
  target_ = oldSource;
  observer_.updateArcCreated(*this);
}

} /* namespace pnapi */
